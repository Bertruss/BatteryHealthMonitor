// ONLY FOR 8mhz clock, so 1/8000000 seconds per clock
// configure clock prescaler for Timer/counter1 for div/8?

#include  "../include/fcpu.h"
#include  "../include/timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint32_t timerMillis;

// Initializes the use of the timer functions by setting up the TCA timer.
void timer_init()
{
	DDRB |= (1 << PB1);//debug
	
	TCCR1 = 0; // stop timer
	TCNT1 = 0; // reset counter
	GTCCR |= (1 << PSR1); // reset prescaler
	
	OCR1A = 249; // set compare register to 250 - 1? well see about the -1. ((8mhz/32)/250)
	OCR1C = 249;
	// set Timer/Counter1 Output Compare RegisterA
	// configure clock prescaler for Timer/counter1 to div/32 also enable clear timer on compare match
	TCCR1 |= (1 << CS12) | (1 << CS11) | (1 << CTC1); // (1 << COM1A0) |  //debug
	TIMSK |= (1 << OCIE1A);// Enable compare A interrupt
}

// TCA overflow handler, called every millisecond.
ISR(TIMER1_COMPA_vect) // compare match A interrupt service routine
{
	//PORTB ^= (1 << PB1);
	//clear timer
	timerMillis++;
}

// clock adjust
uint32_t adjust_clock(uint32_t nudge){
	cli();
	timerMillis += nudge;
	sei();
}

// Gets the milliseconds of the current time.
uint32_t tinymillis()
{
	uint32_t copy;
	cli();
	copy = timerMillis;
	sei();
	return copy;
}
