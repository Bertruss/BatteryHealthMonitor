// note: this is only compliant with the port defs of the attiny25/45/85
#ifndef TINYTWI
#define TINYTWI 

#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DDR_TWI DDRB // data direction register
#define PORT_TWI PORTB //write register
#define PIN_TWI PINB //read register
#define SDA 0x00
#define SCL 0x02
// TIMSK_I2C TIMSK
// TIMER_VECT_I2C TIMER0_COMPA_vect

void tinyTWI_init();
bool twi_write();
int transmission(char addr, char* buff, bool mode);
#endif