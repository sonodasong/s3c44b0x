# s3c44b0x_ucos-ii_lcd_sd_card

LCD driver is R61505U

FatFs with timer is faster than FatFs without timer.

lcdRegW(0x03,0x0030):
	W: (MSB) BGR (LSB)
	R: (MSB) BGR (LSB)

lcdRegW(0x03,0x1030):
	W: (MSB) RGB (LSB)
	R: (MSB) BGR (LSB)
