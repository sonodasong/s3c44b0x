# s3c44b0x_ucos-ii_fatfs_with_timer

SPI clock: CPOL = 0, CPHA = 0 <=> falling edge clock.

The maximum clock rate is 20MHz for MMC, 25MHz for SDC in most case.

At 18.75 MHz, SPI with polling is nearly no faster than SPI with OS. Let alone at 1 MHz. As a result, SPI with OS will be used.
