#ifndef __LCD_H__
#define __LCD_H__

#include <driver.h>

#define LCD_WIDTH		240
#define LCD_HEIGHT		320

void lcdInitFromTask(void);
uint16 lcdGramRead(uint16 x, uint16 y);
void lcdDisplaySingleColor(uint16 color);
void lcdDisplayBmp(uint16 *buf);

#endif
