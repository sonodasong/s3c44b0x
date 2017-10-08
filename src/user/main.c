#include "user.h"

static OS_STK stack0[512];
static OS_STK stack1[128];

static void systemInit(void)
{
	rINTCON = 0x05;
	rINTMOD = 0x00;
	rI_ISPC = 0x03FFFFFF;
	rINTMSK &= ~BIT_GLOBAL;
}

static void sysTickInit(void)
{
	pISR_WDT = (uint32)OSTickISR;
	rWTCON = 0x7C04;
	rWTDAT = 375;
	rWTCNT = 375;
	rWTCON |= ex(5);
	rI_ISPC = BIT_WDT;
	rINTMSK &= ~BIT_WDT;
}

int Main(void)
{
	ARMDisableInt();
	systemInit();
	sysTickInit();

	OSInit();

	spiInit();
	uart0Init();

	OSTaskCreate(lcdDisplayBmpTask, (void *)0, &stack0[511], 0);
	OSTaskCreate(fatfsTimerTask, (void *)0, &stack1[127], 1);

	OSStart();

	return 0;
}
