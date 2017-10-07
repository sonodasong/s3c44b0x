#include "lcd_port.h"
#include "lcd.h"

#define MS_10		(10 * OS_TICKS_PER_SEC / 1000)

static uint16 lcdRegR(uint16 reg);
static void lcdRegW(uint16 reg, uint16 data);
static void lcdSetRamAddr(uint16 x, uint16 y);

void lcdInitFromTask(void)
{
	lcdPinInit();
	LCD_RST_LOW();
	OSTimeDly(MS_10);
	LCD_RST_HIGH();
	lcdRegW(0x00,0x0000);OSTimeDly(MS_10);
	lcdRegW(0x07,0x0001);OSTimeDly(MS_10);//Display Control 1
	lcdRegW(0x08,0x0405);OSTimeDly(MS_10);//Display Control 2 0808
	lcdRegW(0x30,0x0707);OSTimeDly(MS_10);
	lcdRegW(0x31,0x0407);OSTimeDly(MS_10);
	lcdRegW(0x32,0x0203);OSTimeDly(MS_10);
	lcdRegW(0x33,0x0303);OSTimeDly(MS_10);
	lcdRegW(0x34,0x0303);OSTimeDly(MS_10);
	lcdRegW(0x35,0x0202);OSTimeDly(MS_10);
	lcdRegW(0x36,0x1007);OSTimeDly(MS_10);//0x1111
	lcdRegW(0x37,0x0707);OSTimeDly(MS_10);//0x0606
	lcdRegW(0x38,0x0407);OSTimeDly(MS_10);//0x0606
	lcdRegW(0x39,0x0203);OSTimeDly(MS_10);//0x0606
	lcdRegW(0x3a,0x0202);OSTimeDly(MS_10);
	lcdRegW(0x3b,0x0303);OSTimeDly(MS_10);
	lcdRegW(0x3c,0x0606);OSTimeDly(MS_10);
	lcdRegW(0x3d,0x1313);OSTimeDly(MS_10);
	lcdRegW(0x07,0x0101);OSTimeDly(MS_10);//Display Control 1
	lcdRegW(0x17,0x0001);OSTimeDly(MS_10);//Power Control 5
	lcdRegW(0x10,0x08b0);OSTimeDly(MS_10);//Power Control 1
	lcdRegW(0x11,0x0001);OSTimeDly(MS_10);//Power Control 2 0007
	lcdRegW(0x12,0x0118);OSTimeDly(MS_10);//Power Control 3
	lcdRegW(0x13,0x0B00);OSTimeDly(MS_10);//Power Control 4 0x1400
	lcdRegW(0x29,0x0004);OSTimeDly(MS_10);//VCOM High Voltage 0x0c
	lcdRegW(0x2a,0x0000);OSTimeDly(MS_10);//VCOM High Voltage 0x0080
	lcdRegW(0x12,0x01B8);OSTimeDly(MS_10);//Power Control 3 0x013e
	lcdRegW(0x01,0x0100);OSTimeDly(MS_10);//Driver Output Control
	lcdRegW(0x02,0x0300);OSTimeDly(MS_10);//LCD Driving Wave Control 0x0701
	lcdRegW(0x03,0x1030);OSTimeDly(MS_10);//Entry Mode 0x1030
	lcdRegW(0x04,0x0000);OSTimeDly(MS_10);//Resizing Control
	lcdRegW(0x09,0x0028);OSTimeDly(MS_10);//Display Control 3 0x0028
	lcdRegW(0x0c,0x0000);OSTimeDly(MS_10);//interface select
	lcdRegW(0x50,0x0000);OSTimeDly(MS_10);//Window Horizontal RAM Address Start
	lcdRegW(0x51,0x00ef);OSTimeDly(MS_10);//Window Horizontal RAM Address End
	lcdRegW(0x52,0x0000);OSTimeDly(MS_10);//Window Vertical RAM Address Start
	lcdRegW(0x53,0x013f);OSTimeDly(MS_10);//Window Vertical RAM Address End
	lcdRegW(0x60,0x2700);OSTimeDly(MS_10);//Driver Output Control
	lcdRegW(0x61,0x0001);OSTimeDly(MS_10);//Base Image Display Control
	lcdRegW(0x90,0x0015);OSTimeDly(MS_10);//Panel interface control 1 0010
	lcdRegW(0x92,0x0000);OSTimeDly(MS_10);//Panel interface control 2
	lcdRegW(0x93,0x0000);OSTimeDly(MS_10);//Panel interface control 3
	lcdRegW(0x20,0x00ef);OSTimeDly(MS_10);//RAM Address Set (Horizontal Address)
	lcdRegW(0x21,0x013f);OSTimeDly(MS_10);//RAM Address Set (Vertical Address)
	lcdRegW(0x07,0x0021);OSTimeDly(MS_10);//Display Control 1
	lcdRegW(0x07,0x0061);OSTimeDly(MS_10);//Display Control 1
	lcdRegW(0x07,0x0173);OSTimeDly(MS_10);//Display Control 1
	lcdRegW(0x10,0x16b0);OSTimeDly(MS_10);//10bo  0x18b0
	lcdRegW(0x20,0x0000);
	lcdRegW(0x21,0x0000);
}

void lcdDisplaySingleColor(uint16 color)
{
	uint16 x;
	uint16 y;
	lcdSetRamAddr(0, 0);
	lcdIndexW(0x22);
	for(y = 0; y < 320; y++) {
		for (x = 0; x < 240; x++) {
			lcdDataW(color);
		}
	}
}

static uint16 lcdRegR(uint16 reg)
{
	lcdIndexW(reg);
	return lcdDataR();
}

static void lcdRegW(uint16 reg, uint16 data)
{
	lcdIndexW(reg);
	lcdDataW(data);
}

static void lcdSetRamAddr(uint16 x, uint16 y)
{
	lcdRegW(0x20, x);
	lcdRegW(0x21, y);
}
