#include "task.h"

void blink1(void *pdata)
{
	(void)pdata;
	ledInit(1);
	while (1) {
		ledOn(1);
		OSTimeDly(1);
		ledOff(1);
		OSTimeDly(29);
	}
}

void blink2(void *pdata)
{
	(void)pdata;
	ledInit(2);
	while (1) {
		ledOn(2);
		OSTimeDly(1);
		ledOff(2);
		OSTimeDly(39);
	}
}

void blink3(void *pdata)
{
	(void)pdata;
	ledInit(3);
	while (1) {
		ledOn(3);
		OSTimeDly(1);
		ledOff(3);
		OSTimeDly(49);
	}
}

void uart0Demo(void *pdata)
{
	(void)pdata;
	while (1) {
		uart0PendingPrint("abcdefgh\r\n");
		OSTimeDly(50);
		uart0PendingPrint("12345678\r\n");
		OSTimeDly(50);
		uart0PendingPrint("hgfedcba\r\n");
		OSTimeDly(50);
		uart0PendingPrint("87654321\r\n");
		OSTimeDly(50);
	}
}
