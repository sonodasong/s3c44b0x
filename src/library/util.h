#ifndef __UTIL_H__
#define __UTIL_H__

#include "define.h"

void strBufClr(char *buf, uint8 size);
uint8 strCpy(char *src, char *dst, uint8 size);
void ledInit(uint8 num);
void ledOn(uint8 num);
void ledOff(uint8 num);
void ledToggle(uint8 num);

#endif
