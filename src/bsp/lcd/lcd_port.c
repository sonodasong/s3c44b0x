#include "lcd_port.h"

__inline static uint16 portRead(void);
__inline static void portWrite(uint16 data);

void lcdPinInit(void)
{
	/* RESET */
	portControl(3, 0, 1);
	portPullUp(3, 0, 0);
	LCD_RST_HIGH();
	/* CS */
	portControl(3, 2, 1);
	portPullUp(3, 2, 0);
	LCD_CS_HIGH();
	/* RS */
	portControl(3, 1, 1);
	portPullUp(3, 2, 0);
	LCD_RS_LOW();
	/* RD */
	portControl(3, 3, 1);
	portPullUp(3, 3, 0);
	LCD_RD_HIGH();
	/* WR */
	portControl(3, 5, 1);
	portPullUp(3, 5, 0);
	LCD_WR_HIGH();
	/* Data */
	rPCONC = 0x00;
	rPUPC = 0x00;
}

void lcdIndexW(uint16 index)
{
	LCD_CS_LOW();
	LCD_RS_LOW();
	LCD_WR_LOW();
	portWrite(index);
	LCD_WR_HIGH();
	LCD_CS_HIGH();
}

uint16 lcdDataR(void)
{
	uint16 data;

	LCD_CS_LOW();
	LCD_RS_HIGH();
	LCD_RD_LOW();
	data = portRead();
	LCD_RD_HIGH();
	LCD_CS_HIGH();
	return data;
}

void lcdDataW(uint16 data)
{
	LCD_CS_LOW();
	LCD_RS_HIGH();
	LCD_WR_LOW();
	portWrite(data);
	LCD_WR_HIGH();
	LCD_CS_HIGH();
}

__inline static uint16 portRead(void)
{
	rPCONC = 0x00;
	return (uint16) rPDATC;
}

__inline static void portWrite(uint16 data)
{
	rPCONC = 0x55555555;
	rPDATC = data;
}
