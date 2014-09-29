#include "3daccel.h"


static volatile char MODE = 'F';

#define ACCEL_CTL_DDRD	0x0A
#define ACCEL_CTL_PORTD	0x0B


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// toggle the LED pin
void blinkLED(eventState_t state)
{
	PINB |= (1<<PORTB0);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// converts the raw accelerometer data to degrees
void accelDataToDeltas(void)
{
	float x, y, z;
	accel_readDeltas(&x, &y, &z);

	uartSendBuff((char*)&x, sizeof(float));
	uartSendBuff((char*)&y, sizeof(float));
	uartSendBuff((char*)&z, sizeof(float));
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// writes the sample data as a set of angles to the serial port
void accelDataToAngles(void)
{
	float x, y, z;
	accel_readAngles(&x, &y, &z);

	uartSendBuff((char*)&x, sizeof(float));
	uartSendBuff((char*)&y, sizeof(float));
	uartSendBuff((char*)&z, sizeof(float));
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UART receive handler
void onUartReceive(char data)
{
	PORTB &= ~(1<<PORTB1);

	// convert to upper case
	MODE = data & 0x5F;

	switch (MODE)
	{
		// return the delta between this read and the previous read
		case 'D':
			accelDataToDeltas();
			break;
		// return acceleration as an angle
		case 'A':
			accelDataToAngles();
			break;
	}

	PORTB |= (1<<PORTB1);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// setup the chip
void init(void)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// establish event timer & handler
	// timer1, event timer
	// Set CTC mode (Clear Timer on Compare Match) (p.133)
	// Have to set OCR1A *before anything else*, otherwise it gets reset to 0!
	power_timer1_enable();
	OCR1A	=	(F_CPU / SAMPLE_RATE);
	TCCR1A	=	0;
	TCCR1B	=	(1<<WGM12) |	// CTC
				(1<<CS10);
	TIMSK1	=	(1<<OCIE1A);

	setTimeBase(SAMPLE_RATE);


	// setup timer2 which will be a millisecond timer for timing function calls
	power_timer2_enable();
	TCCR2A	=	(0<<COM2A1) |
				(0<<COM2A0)	|
				(0<<COM2B1)	|
				(0<<COM2B0)	|
				(1<<WGM21)	|		// CTC
				(0<<WGM20);

	TCCR2B	=	(0<<FOC2A)	|
				(0<<FOC2B)	|
				(0<<WGM22)	|
				(1<<CS22)	|		// Fcpu/1024
				(1<<CS21)	|
				(1<<CS20);

	OCR2A	=	143;				// 14,745,600/1024/144 = 100
									// 1/100 = 0.010s, or 10ms ticks


	// portb0 and portb1 will be used for diag LEDs
	// portb0 = heart beat LED
	// portb1 = debug LED
	DDRB |= (1<<PORTB0) | (1<<PORTB1);
	PORTB |= (1<<PORTB0) | (1<<PORTB1);

	accel_init();

	uart_init();

	uartBeginReceive(onUartReceive);

	sei();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main(void)
{
	init();

	// get most recent sample data
	registerEvent(blinkLED, SAMPLE_RATE / 2, 0);

	while(1)
	{
		eventsDoEvents();
	}

	return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Should be running @ 8.000kHz - this is the event sync driver method
ISR(TIMER1_COMPA_vect)
{
	eventSync();
}
