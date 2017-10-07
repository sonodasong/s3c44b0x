#include "user.h"

static OS_STK stack0[512];

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

	uart0Init();

	OSTaskCreate(lcdDemo, (void *)0, &stack0[511], 0);

	OSStart();

	return 0;
}
