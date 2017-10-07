#include "util.h"

void ledInit(uint8 num)
{
	portControl(2, num, 1);
	portPullUp(2, num, 0);
	rPDATC &= rex(num);
}

void ledOn(uint8 num)
{
	rPDATC |= ex(num);
}

void ledOff(uint8 num)
{
	rPDATC &= rex(num);
}

void ledToggle(uint8 num)
{
	if (rPDATC & ex(num)) {
		rPDATC &= rex(num);
	} else {
		rPDATC |= ex(num);
	}
}
