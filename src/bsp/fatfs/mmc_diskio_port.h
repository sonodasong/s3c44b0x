#ifndef __MMC_DISKIO_PORT_H__
#define __MMC_DISKIO_PORT_H__

#include <driver.h>
#include "diskio.h"

#define CS_LOW()					rPDATE &= rex(5)
#define CS_HIGH()					rPDATE |= ex(5)
#define MMC_CD()					TRUE
#define MMC_WP()					FALSE
#define FCLK_SLOW()					spiClockSlow()
#define FCLK_FAST()					spiClockFast()
#define rcvr_spi_multi(p, cnt)		spiBufR(p, cnt, 0xFF)
#define xmit_spi_multi(p, cnt);		spiBufW((uint8 *)p, cnt)

void power_on(void);
void power_off(void);
BYTE xchg_spi(BYTE dat);

#endif
