#include "port.h"
#include "uart0.h"

static char uart0RxBuf[ex(UART0_RX_Q_SIZE)][ex(UART0_RX_SIZE)];
static uint8 uart0RxCnt;
static uint8 uart0RxQCnt;
static void *uart0RxQPtr[ex(UART0_RX_Q_SIZE)];
static OS_EVENT *uart0RxQ;

static char *uart0TxPtr;
static uint8 uart0TxCnt;
static OS_EVENT *uart0TxRdy;

static void uart0RXDHandler(void);
static void uart0TXDHandler(void);

void uart0Init(void)
{
	/* enable clock */
	rCLKCON |= ex(8);

	/* RxD0 configuration */
	portControl(4, 2, 2);
	portPullUp(4, 2, 0);

	/* TxD0 configuration */
	portControl(4, 1, 2);
	portPullUp(4, 1, 0);

	/* line control, 8-bit, 1 stop bit, no parity check */
	rULCON0 = 0x03;

	/* baud rate 115200 Hz */
	rUBRDIV0 = 40;

	/* interrupt mode */
	rUCON0 = 0x85;

	/* enable fifo */
	rUFCON0 = 0x17;

	/* semaphore and queue */
	uart0RxQ = OSQCreate(uart0RxQPtr, ex(UART0_RX_Q_SIZE));
	uart0RxCnt = 0;
	uart0RxQCnt = 0;
	uart0TxRdy = OSSemCreate(0);

	/* set up interrupt */
	pISR_URXD0 = (uint32)uart0RXDHandler;
	rI_ISPC = BIT_URXD0;
	rINTMSK &= ~BIT_URXD0;
	pISR_UTXD0 = (uint32)uart0TXDHandler;
	rI_ISPC = BIT_UTXD0;
	rINTMSK &= ~BIT_UTXD0;
}

INT8U uart0Read(char **str)
{
	INT8U err;

	*str = (char *)OSQPend(uart0RxQ, UART0_RX_TIMEOUT, &err);
	return err;
}

INT8U uart0Print(char *str)
{
	INT8U err;
	char temp;

	temp = str[0];
	if (temp == '\0') {
		return OS_ERR_NONE;
	}
	uart0TxPtr = str;
	uart0TxCnt = 0;
	do {
		rUTXH0 = temp;
		uart0TxCnt++;
		temp = str[uart0TxCnt];
	} while ((temp != '\0') && uart0TxCnt < 16);
	OSSemPend(uart0TxRdy, UART0_TX_TIMEOUT, &err);
	return err;
}

#if UART0_ENABLE_PRINTF

#include <stdio.h>
#include <stdarg.h>

static char uart0TxBuf[64];

void uart0Printf(char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vsprintf(uart0TxBuf, fmt, ap);
	va_end(ap);
	uart0Print(uart0TxBuf);
}

#endif

static void uart0RXDHandler(void)
{
	char temp;
	boolean osCalled;

	osCalled = FALSE;
	while (rUFSTAT0 & 0x0F) {
		temp = rURXH0;;
		if (temp == UART0_RX_EOF) {
			uart0RxBuf[uart0RxQCnt][uart0RxCnt] = '\0';
			uart0RxCnt = 0;
			if (osCalled == FALSE) {
				osCalled = TRUE;
				OSIntEnter();
			}
			OSQPost(uart0RxQ, uart0RxBuf[uart0RxQCnt]);
			uart0RxQCnt++;
			uart0RxQCnt &= ex(UART0_RX_Q_SIZE) - 1;
		} else {
			uart0RxBuf[uart0RxQCnt][uart0RxCnt] = temp;
			uart0RxCnt++;
			uart0RxCnt &= ex(UART0_RX_SIZE) - 1;
		}
	}
	if (osCalled == TRUE) {
		OSIntExit();
	}
}

static void uart0TXDHandler(void)
{
	char temp;
	uint8 fifoCnt;

	temp = uart0TxPtr[uart0TxCnt];
	if (temp == '\0') {
		OSIntEnter();
		OSSemPost(uart0TxRdy);
		OSIntExit();
	} else {
		fifoCnt = 0;
		while ((temp != '\0') && (fifoCnt < 16)) {
			rUTXH0 = temp;
			fifoCnt++;
			uart0TxCnt++;
			temp = uart0TxPtr[uart0TxCnt];
		}
	}
}
