// note: this is only compliant with the port defs of the attiny25/45/85
#ifndef TINYTWI
#define TINYTWI 

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

enum xfer_type{
    WRITE = 0x00, 
    READ = 0x01
}xfer_type;

#define DDR_TWI DDRB // data direction register
#define PORT_TWI PORTB //write register
#define PIN_TWI PINB //read register
#define SDA 0x00
#define SCL 0x02
// TIMSK_I2C TIMSK
// TIMER_VECT_I2C TIMER0_COMPA_vect

void twi_init(void);
void twi_start(void);
void twi_stop(void);
inline bool read_ack(void);
uint8_t twi_byte_transfer(uint8_t buff, bool bit, enum xfer_type mode); 
bool twi_transmission (uint8_t addr, uint8_t* buff, uint16_t length, enum xfer_type mode);
#endif