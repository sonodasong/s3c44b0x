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

static uint8 buf[3 * LCD_WIDTH * LCD_HEIGHT];

static void bmp24To16(void);
static void printBuf(void);

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
		f_read(&fil, buf, 3 * LCD_WIDTH * LCD_HEIGHT, &rc);
		f_close(&fil);
		bmp24To16();
		lcdDisplayBmp((uint16 *) buf);
	}
}

void lcdGramReadTask(void *pdata)
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
		f_read(&fil, buf, 3 * LCD_WIDTH * LCD_HEIGHT, &rc);
		f_close(&fil);
		bmp24To16();
		lcdDisplayBmp((uint16 *) buf);

		uart0Printf("buf data:\r\n");
		printBuf();
		strClr((char *)buf, 128);
		uart0Printf("buf cleared:\r\n");
		printBuf();
		lcdGramRead((uint16 *)buf, 0, 0, 64);
		uart0Printf("GRAM data:\r\n");
		printBuf();
	}
}

void fatfsTimerTask(void *pdata)
{
	extern void disk_timerproc (void);

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
		blue = buf[i24++];
		green = buf[i24++];
		red = buf[i24++];
		buf[i16++] = ((green << 3) & 0xE0) | (blue >> 3);
		buf[i16++] = (red & 0xF8) | (green >> 5);
	}
}

static void printBuf(void)
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
