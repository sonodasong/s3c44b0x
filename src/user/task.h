#ifndef __TASK_H__
#define __TASH_H__

#include <bsp.h>

void blink1(void *pdata);
void blink2(void *pdata);
void blink3(void *pdata);
void serial(void *pdata);
void lcdDisplayBmpTask(void *pdata);
void lcdGramReadTask(void *pdata);
void fatfsTimerTask(void *pdata);

#endif
