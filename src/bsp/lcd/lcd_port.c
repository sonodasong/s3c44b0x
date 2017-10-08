#include "lcd_port.h"

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
	rPCONC = 0x55555555;
	rPUPC = 0x00;
}
