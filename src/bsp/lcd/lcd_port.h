#ifndef __LCD_PORT_H__
#define __LCD_PORT_H__

#include <driver.h>

#define LCD_RST_LOW()		rPDATD &= rex(0)
#define LCD_RST_HIGH()		rPDATD |= ex(0)
#define LCD_CS_LOW()		rPDATD &= rex(2)
#define LCD_CS_HIGH()		rPDATD |= ex(2)
#define LCD_RS_LOW()		rPDATD &= rex(1)
#define LCD_RS_HIGH()		rPDATD |= ex(1)
#define LCD_RD_LOW()		rPDATD &= rex(3)
#define LCD_RD_HIGH()		rPDATD |= ex(3)
#define LCD_WR_LOW()		rPDATD &= rex(5)
#define LCD_WR_HIGH()		rPDATD |= ex(5)


void lcdPinInit(void);

__inline void lcdPortIn(void)
{
	rPCONC = 0x00;
}

__inline void lcdPortOut(void)
{
	rPCONC = 0x55555555;
}

__inline uint16 lcdPortRead(void)
{
	return (uint16) rPDATC;
}

__inline void lcdPortWrite(uint16 data)
{
	rPDATC = data;
}

__inline uint16 lcdStatusR(void)
{
	uint16 status;

	LCD_CS_LOW();
	LCD_RS_LOW();
	LCD_RD_LOW();
	status = lcdPortRead();
	LCD_RD_HIGH();
	LCD_CS_HIGH();
	return status;
}

__inline void lcdIndexW(uint16 index)
{
	LCD_CS_LOW();
	LCD_RS_LOW();
	lcdPortWrite(index);
	LCD_WR_LOW();
	LCD_WR_HIGH();
	LCD_CS_HIGH();
}

__inline uint16 lcdDataR(void)
{
	uint16 data;

	LCD_CS_LOW();
	LCD_RS_HIGH();
	LCD_RD_LOW();
	data = lcdPortRead();
	LCD_RD_HIGH();
	LCD_CS_HIGH();
	return data;
}

__inline void lcdDataW(uint16 data)
{
	LCD_CS_LOW();
	LCD_RS_HIGH();
	lcdPortWrite(data);
	LCD_WR_LOW();
	LCD_WR_HIGH();
	LCD_CS_HIGH();
}

#endif
