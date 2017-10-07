#ifndef __PORT_H__
#define __PORT_H__

#include "define.h"

void portControl(uint8 port, uint8 pin, uint8 func);
void portPullUp(uint8 port, uint8 pin, uint8 func);

#endif
