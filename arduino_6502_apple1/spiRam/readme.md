# apple 1 using 23LCV512 spi ram

// This one is using spi ram 23LCV512 for ram memory.

// This appears under memtest factor 100-500 times slower!!

// In real world apps, like mandel3.bas app. 10-20 times slower.

// So it is at the etch to be usable?

I measured with stop watch the 'rendering' auf mandel3.bas:

Real memory on mega 2560 (4k):  8min 42 seconds

SPI Ram 23LCV512 (32k):        16min 55 seconds

So its app. 2 times slower in 'real world apps'.

![sch](https://github.com/petersieg/arduino/blob/master/arduino_6502_apple1/spiRam/SCH%2023LC512.jpg)

