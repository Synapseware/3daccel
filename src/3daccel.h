#ifndef _3DACCEL_H
#define _3DACCEL_H



#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>

#include <libs/utils.h>
#include <libs/events/events.h>
#include <libs/uart/uart.h>
#include <libs/drivers/mma7260/mma7260.h>



/* *******************************************************************
FUSES:
reading fuses:
	avrdude -c stk500v2 -P com6 -p attiny84 -B 2 -U hfuse:r:.\high.txt:h -U lfuse:r:.\low.txt:h -U efuse:r:.\ext.txt:h

writing fuses:

(external crystal)
	avrdude -c stk500v2 -P com6 -p attiny84 -B 2 -U lfuse:w:0xff:m -U hfuse:w:0xd7:m -U efuse:w:0xff:m

(internal 8mHz r/c)
	avrdude -c stk500v2 -P com6 -p attiny84 -B 2 -U lfuse:w:0xe2:m -U hfuse:w:0xd7:m -U efuse:w:0xff:m


******************************************************************* */





#endif
