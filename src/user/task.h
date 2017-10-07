#ifndef __TASK_H__
#define __TASH_H__

#include <library.h>
#include <ff.h>

void blink1(void *pdata);
void blink2(void *pdata);
void blink3(void *pdata);
void serial(void *pdata);
void fatfsTask(void *pdata);
void fatfsTimerTask(void *pdata);

#endif
