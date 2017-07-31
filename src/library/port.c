#include "port.h"

void portControl(uint8 port, uint8 pin, uint8 func)
{
	uint32 address;
	uint32 mask;

	address = 0x01D1FFF8 + 0x0C * port;
	mask = 0x03;
	mask = ~(mask << (pin << 1));
	*((uint32 *)address) &= mask;
	mask = (uint32)func;
	mask = mask << (pin << 1);
	*((uint32 *)address) |= mask;
}

void portPullUp(uint8 port, uint8 pin, uint8 func)
{
	uint32 address;

	address =  0x01D20000 + 0x0C * port;
	if (func == 0x00) {
		*((uint32 *)address) &= rex(pin);
	} else {
		*((uint32 *)address) |= ex(pin);
	}
}
