#include "user.h"

static OS_STK stack0[128];
static OS_STK stack1[128];
static OS_STK stack2[128];
static OS_STK stack3[128];

static void systemInit(void)
{
	rINTCON = 0x05;
	rINTMOD = 0x00;
	rI_ISPC = 0x03FFFFFF;
	rINTMSK &= ~(BIT_GLOBAL);
}

static void sysTickInit(void)
{
	pISR_WDT = (uint32)OSTimeTick;
	rWTCON = 0x7C04;
	rWTDAT = 375;
	rWTCNT = 375;
	rWTCON |= ex(5);
	rI_ISPC = BIT_WDT;
	rINTMSK &= ~(BIT_WDT);
}

int Main(void)
{
	ARMDisableInt();
	systemInit();
	sysTickInit();

	OSInit();

	uart0PendingInit();

	OSTaskCreate(blink1, (void *)0, &stack0[127], 0);
	OSTaskCreate(blink2, (void *)0, &stack1[127], 1);
	OSTaskCreate(blink3, (void *)0, &stack2[127], 2);
	OSTaskCreate(uart0Demo, (void *)0, &stack3[127], 3);

	OSStart();

	return 0;
}
