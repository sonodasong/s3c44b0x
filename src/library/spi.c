#include "port.h"
#include "spi.h"

static uint8 *spiTxBuf;
static uint8 *spiRxBuf;
static uint16 spiSize;
static uint16 spiCnt;
static uint8 spiFill;
static OS_EVENT *spiRdy;

static void spiBufRHandler(void);
static void spiBufWHandler(void);
static void spiBufWRHandler(void);

void spiInit(void)
{
	/* MOSI pin configuration */
	rPCONF &= ~(0x07 << 10);
	rPCONF |= 0x03 << 10;
	portPullUp(5, 5, 0);

	/* MISO pin configuration */
	rPCONF &= ~(0x07 << 16);
	rPCONF |= 0x03 << 16;
	portPullUp(5, 7, 0);

	/* SCK pin configuration */
	rPCONF &= ~(0x07 << 19);
	rPCONF |= 0x03 << 19;
	portPullUp(5, 8, 0);

	/* SPI configuration */
	/* cannot understand why it is falling edge clock here */
	rSIOCON = 0x21;

	/* 18.75 MHz */
	rSBRDR = 1;

	/* set up semaphore */
	spiRdy = OSSemCreate(0);

	/* set up interrupt */
	rI_ISPC = BIT_SIO;
	rINTMSK &= ~BIT_SIO;
}

INT8U spiBufR(uint8 *buf, uint16 size, uint8 fill)
{
	INT8U err;

	spiRxBuf = buf;
	spiSize = size;
	spiCnt = 0;
	spiFill = fill;
	pISR_SIO = (uint32)spiBufRHandler;
	rSIODAT = fill;
	rSIOCON |= 0x08;
	OSSemPend(spiRdy, SPI_TIMEOUT, &err);
	return err;
}

INT8U spiBufW(uint8 *buf, uint16 size)
{
	INT8U err;

	spiTxBuf = buf;
	spiSize = size;
	spiCnt = 1;
	pISR_SIO = (uint32)spiBufWHandler;
	rSIODAT = buf[0];
	rSIOCON |= 0x08;
	OSSemPend(spiRdy, SPI_TIMEOUT, &err);
	return err;
}

INT8U spiBufWR(uint8 *txBuf, uint8 *rxBuf, uint16 size)
{
	INT8U err;

	spiTxBuf = txBuf;
	spiRxBuf = rxBuf;
	spiSize = size;
	spiCnt = 0;
	pISR_SIO = (uint32)spiBufWRHandler;
	rSIODAT = txBuf[0];
	rSIOCON |= 0x08;
	OSSemPend(spiRdy, SPI_TIMEOUT, &err);
	return err;
}

/* 250 kHz*/
void spiClockSlow(void)
{
	rSBRDR = 149;
}

/* 18.75 MHz */
void spiClockFast(void)
{
	rSBRDR = 1;
}

static void spiBufRHandler(void)
{
	spiRxBuf[spiCnt] = rSIODAT;
	spiCnt++;
	if (spiCnt < spiSize) {
		rSIODAT = spiFill;
		rSIOCON |= 0x08;
	} else {
		OSIntEnter();
		OSSemPost(spiRdy);
		OSIntExit();
	}
}

static void spiBufWHandler(void)
{
	if (spiCnt < spiSize) {
		rSIODAT = spiTxBuf[spiCnt];
		spiCnt++;
		rSIOCON |= 0x08;
	} else {
		OSIntEnter();
		OSSemPost(spiRdy);
		OSIntExit();
	}
}

static void spiBufWRHandler(void)
{
	spiRxBuf[spiCnt] = rSIODAT;
	spiCnt++;
	if (spiCnt < spiSize) {
		rSIODAT = spiTxBuf[spiCnt];
		rSIOCON |= 0x08;
	} else {
		OSIntEnter();
		OSSemPost(spiRdy);
		OSIntExit();
	}
}
