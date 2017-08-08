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

void serial(void *pdata)
{
	char *str;

	(void)pdata;
	while (1) {
		uart0Read(&str);
		uart0Printf("%s\r\n", str);
	}
}

#define BUFF_SIZE		16

void fatfsTask(void *pdata)
{
	FATFS FatFs;
	FIL fil;
	char line[BUFF_SIZE];
	UINT rc, i;
	char *str;

	(void)pdata;
	f_mount(&FatFs, "", 0);
	while (1) {
		strBufClr(line, BUFF_SIZE);
		f_open(&fil, "test.txt", FA_READ);
		f_read(&fil, line, BUFF_SIZE - 1, &rc);
		f_close(&fil);
		line[rc] = '\0';
		uart0Printf("%s\r\n", line);
		uart0Read(&str);
		i = strCpy(str, line, BUFF_SIZE - 1);
		if (i < rc) {
			i = rc;
		}
		f_open(&fil, "test.txt", FA_WRITE);
		f_write(&fil, line, i, &rc);
		f_close(&fil);
	}
}
