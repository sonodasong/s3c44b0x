#include "mmc_diskio_port.h"

void power_on(void)
{
	/* CS pin configuration */
	rPDATE |= ex(5);
	portControl(4, 5, 1);
	portPullUp(4, 5, 0);
	OSTimeDly(100 / (1000 /OS_TICKS_PER_SEC));
}

void power_off(void)
{
}

BYTE xchg_spi(BYTE dat)
{
	spiBufWR(&dat, &dat, 1);
	return dat;
}
