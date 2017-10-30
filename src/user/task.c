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

static uint8 buffer[3 * LCD_WIDTH * LCD_HEIGHT];

static void bmp24To16(void);
static void printbuffer(uint8 *buf);

void lcdDisplayBmpTask(void *pdata)
{
	FATFS FatFs;
	FIL fil;
	UINT rc;
	char *str;

	(void)pdata;
	lcdInitFromTask();
	f_mount(&FatFs, "", 0);
	while (1) {
		uart0Read(&str);
		uart0Printf("%s\r\n", str);
		lcdDisplaySingleColor(0xFFE0);
		f_open(&fil, "demo.bmp", FA_READ);
		f_lseek(&fil, 54);
		f_read(&fil, buffer, 3 * LCD_WIDTH * LCD_HEIGHT, &rc);
		f_close(&fil);
		bmp24To16();
		lcdDisplayBmp((uint16 *)buffer);
	}
}

void lcdGramReadTask(void *pdata)
{
	FATFS FatFs;
	FIL fil;
	UINT rc;
	char *str;
	uint8 i;

	(void)pdata;
	lcdInitFromTask();
	f_mount(&FatFs, "", 0);
	while (1) {
		uart0Read(&str);
		uart0Printf("%s\r\n", str);
		lcdDisplaySingleColor(0x07FF);
		f_open(&fil, "demo.bmp", FA_READ);
		f_lseek(&fil, 54);
		f_read(&fil, buffer, 3 * LCD_WIDTH * LCD_HEIGHT, &rc);
		f_close(&fil);
		bmp24To16();
		lcdDisplayBmp((uint16 *)buffer);

		uart0Printf("buffer data:\r\n");
		printbuffer(buffer);
		strClr((char *)(buffer + 128), 128);
		for (i = 0; i < 64; i++) {
			((uint16 *)(buffer + 128))[i] = lcdGramRead(i, 0);
		}
		uart0Printf("GRAM data:\r\n");
		printbuffer(buffer + 128);
		if (strCmp((char *)buffer, (char *)(buffer + 128), 128)) {
			uart0Printf("GRAM read success\r\n");
		} else {
			uart0Printf("GRAM read fail\r\n");
		}
	}
}

void fatfsTimerTask(void *pdata)
{
	extern void disk_timerproc(void);

	(void)pdata;
	while (1) {
		disk_timerproc();
		OSTimeDly(1);
	}
}

static void bmp24To16(void)
{
	uint8 red, green, blue;
	uint32 i24, i16;

	i16 = 0;
	for (i24 = 0; i24 < 3 * LCD_WIDTH * LCD_HEIGHT;) {
		blue = buffer[i24++];
		green = buffer[i24++];
		red = buffer[i24++];
		buffer[i16++] = ((green << 3) & 0xE0) | (red >> 3);
		buffer[i16++] = (blue & 0xF8) | (green >> 5);
	}
}

static void printbuffer(uint8 *buf)
{
	uint8 i;
	uint8 j;

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			uart0Printf("%04x ", ((uint16 *)buf)[j * 8 + i]);
		}
		uart0Printf("\r\n");
	}
}
