#include "lcd_port.h"

void lcdPinInit(void)
{
	/* RESET */
	LCD_RST_HIGH();
	portControl(3, 0, 1);
	portPullUp(3, 0, 0);

	/* CS */
	LCD_CS_HIGH();
	portControl(3, 2, 1);
	portPullUp(3, 2, 0);

	/* RS */
	LCD_RS_LOW();
	portControl(3, 1, 1);
	portPullUp(3, 1, 0);

	/* RD */
	LCD_RD_HIGH();
	portControl(3, 3, 1);
	portPullUp(3, 3, 0);

	/* WR */
	LCD_WR_HIGH();
	portControl(3, 5, 1);
	portPullUp(3, 5, 0);

	/* Data */
	rPCONC = 0x55555555;
	rPUPC = 0x00;
}
