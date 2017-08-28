#include <stdio.h>
#include <stdarg.h>
#include "port.h"
#include "uart0_pending.h"

char hexTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static char uart0Buf[256];

void uart0PendingInit(void)
{
	/* enable clock */
	rCLKCON |= ex(8);

	/* TxD0 configuration */
	portControl(4, 1, 2);
	portPullUp(4, 1, 0);

	/* RxD0 configuration */
	portControl(4, 2, 2);
	portPullUp(4, 2, 0);

	/* line control, 8-bit, 1 stop bit, no parity check */
	rULCON0 = 0x03;

	/* baud rate 115200 Hz */
	rUBRDIV0 = 40;

	/* polling mode */
	rUCON0 = 0x05;

	/* fifo enable */
	rUFCON0 = 0x17;
}

void uart0PendingByte(uint8 c)
{
	while (rUFSTAT0 & ex(9));
	rUTXH0 = c;
}

void uart0PendingPrint(char *str)
{
	char temp;
	uint8 cnt;

	cnt = 0;
	temp = str[0];
	while (temp != '\0') {
		rUTXH0 = temp;
		cnt++;
		temp = str[cnt];
		while (rUFSTAT0 & ex(9));
	}
}

void uart0PendingPrintf(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsprintf(uart0Buf, fmt, ap);
	va_end(ap);
	uart0PendingPrint(uart0Buf);
}

void uart0PendingHex8(uint8 hex8)
{
	char buffer[5];

	buffer[0] = '0';
	buffer[1] = 'x';
	buffer[2] = hexTable[(hex8>>4)&0x0F];
	buffer[3] = hexTable[hex8&0x0F];
	buffer[4] = '\0';
	uart0PendingPrint(buffer);
}

void uart0PendingHex16(uint16 hex16)
{
	char buffer[7];

	buffer[0] = '0';
	buffer[1] = 'x';
	buffer[2] = hexTable[(hex16>>12)&0x0F];
	buffer[3] = hexTable[(hex16>>8)&0x0F];
	buffer[4] = hexTable[(hex16>>4)&0x0F];
	buffer[5] = hexTable[hex16&0x0F];
	buffer[6] = '\0';
	uart0PendingPrint(buffer);
}

void uart0PendingHex32(uint32 hex32)
{
	char buffer[11];

	buffer[0] = '0';
	buffer[1] = 'x';
	buffer[2] = hexTable[(hex32>>28)&0x0F];
	buffer[3] = hexTable[(hex32>>24)&0x0F];
	buffer[4] = hexTable[(hex32>>20)&0x0F];
	buffer[5] = hexTable[(hex32>>16)&0x0F];
	buffer[6] = hexTable[(hex32>>12)&0x0F];
	buffer[7] = hexTable[(hex32>>8)&0x0F];
	buffer[8] = hexTable[(hex32>>4)&0x0F];
	buffer[9] = hexTable[hex32&0x0F];
	buffer[10] = '\0';
	uart0PendingPrint(buffer);
}

void uart0PendingLongInt(uint32 x)
{
	char digit[11];
	uint8 i;

	digit[10] = '\0';
	i = 9;
	do {
		digit[i] = '0' + x%10;
		x /= 10;
		i--;
	} while (x != 0);
	i++;
	uart0PendingPrint(&digit[i]);
}
