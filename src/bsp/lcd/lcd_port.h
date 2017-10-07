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
void lcdIndexW(uint16 index);
uint16 lcdDataR(void);
void lcdDataW(uint16 data);

#endif
