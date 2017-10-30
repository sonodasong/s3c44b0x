#include "port.h"
#include "util.h"

void strClr(char *str, uint8 size)
{
	while (size != 0) {
		str[size - 1] = '\0';
		size--;
	}
}

uint8 strCpy(char *src, char *dst, uint8 size)
{
	uint8 i;

	i = 0;
	while (src[i] != '\0' && i < size) {
		dst[i] = src[i];
		i++;
	}
	return i;
}

void ledInit(uint8 num)
{
	rPDATC &= rex(num);
	portControl(2, num, 1);
	portPullUp(2, num, 0);
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
