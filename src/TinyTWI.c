#include "../include/TinyTWI.h"
#include "../include/fcpu.h"
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

// fast mode, delays shortened because of other overhead 
#define __dwell_long() _delay_us(1.3) //low 1.3ns
#define __dwell_short() _delay_us(.8) //high .6ns

// refer to section 15 (pg.108) of the ATtiny25/45/85 Datasheet for more information

void twi_init(void){
    //initialize USI for I2C/TWI
	// Set the data register on the USI port
    USIDR = 0xFF;
    
    //initialize sda and scl to "high", default state
    PORT_TWI  |= (1 << SCL) | (1 << SDA);

	//intialize Data Direction on sda and scl as output val
	DDR_TWI |= (1 << SCL) | (1 << SDA);

    // Set the Control reg 
    USICR = (1 << USIWM1)| //Set the wire mode to TWI or I2C
            (1 << USICS1)| //set "external, pos-edge" clock source
            (1 << USICLK); //and set the 4-bit counter clk to software gen, via the (USITC) register
    // NOTE: for the sake of the 4-bit counter, this is an "external clock", 
    // the counter is triggered on both edges. Hence a 4-bit (0-15 value) counter to count
    // shifting in and out an 8-bit value. The counter rolls over to 16 and triggers the overflow flag, 
    // which you can check to see if a full byte has been r/w

    // Set the status register
    /* TODO: experiment with pruning this.
    / USIPF and USIDC flags should never be set such that they need clearing in
    / this application. supposedly these are useful for multi-master arbitration but 
    / that won't appear in this programs intended use case.
    */
    USISR = (1 << USISIF) | //Clear the Start Condition Interrupt Flag
            (1 << USIOIF) | //Clear the Counter Overflow Interrupt Flag
            (1 << USIPF) | //Clear the Stop Condition Flag
            (1 << USIDC); //Clear Data Output Collision
}


void twi_disable(void){
	USICR &= !(1 << USIWM1); // disable TWI
}

void twi_enable(void){
	USICR |= (1 << USIWM1); // enable TWI
}

void twi_start(void){
    // outputs i2c start condition
    PORT_TWI &= ~(1 << SDA); // set SDA low
	__dwell_long();
    PORT_TWI &= ~(1 << SCL); // set clock low
	while ((PIN_TWI & (1 << SCL)) != 0); // wait for clock to pull low
	// other devices on the bus may hold clock high until they are ready, '
	// hence this wait.
	__dwell_long();
	//potentially, return status of start condition interrupt flag, start condition detection
}

void twi_stop(void){
    // outputs i2c stop condition
	USIDR = 0xff; //pulls SDA high?
	DDR_TWI |= (1 << SDA); //set SDA write
	PORT_TWI &= ~(1 << SDA); // set SDA low
	PORT_TWI |= (1 << SCL); // set SCL high
    while ((PIN_TWI & (1 << SCL)) == 0); //wait for SCL high
    __dwell_short();
    PORT_TWI |= (1 << SDA); // pull SDA high
    __dwell_long(); // just in case time-padding
}

uint8_t twi_byte_transfer(uint8_t buff, bool bit, enum xfer_type mode){
    //params:
    // buff - potential byte to write
    // bit - r/w single bit?
    // mode - transfer type is READ or WRITE
	USISR = (1 << USISIF) |
			(1 << USIOIF) |
			(1 << USIPF) |
			(1 << USIDC);
	USIDR = buff;
	
    if(mode == WRITE){
        DDR_TWI |= (1 << SDA); // data direction : write
		USIDR = buff; // move the data to the USI data register
    }
	else{ // READ
        DDR_TWI &= ~(1 << SDA); // data direction : read
    }

    if(bit){ //only read or write single bit
        USISR |= (0xE << USICNT0); //overflow with 1 bit
    }   
	
    // following loop will shift the data register onto the sda line (or vice versa depending on ddr setting) by toggling the clock
	PORT_TWI |= (1 << SDA); // "Release" sda
    do {
		__dwell_short();
		USICR = (1 << USIWM1) | (1 << USICS1) | (1 << USICLK) | (1 << USITC); // toggle the clock
		while ((PIN_TWI & (1 << SCL)) == 0); //wait for the clock to pull high
        __dwell_long();
        USICR = (1 << USIWM1) | (1 << USICS1) | (1 << USICLK) | (1 << USITC); // toggle the clock /* USICR = (1 << USIWM1) | (1 << USICS1) | (1 << USICLK) | (1 << USITC); */	
	}
    while((USISR & (1 << USIOIF)) == 0); // Check for counter overflow
	
    //__dwell_long();
    uint8_t data = USIDR;
    return data;
}

bool twi_transmission (uint8_t addr, uint8_t* buff, uint16_t length, enum xfer_type mode){
    twi_start();
    // Transmission Setup
    // Concatenate the address and transmit mode
    uint8_t ack;
	addr = (addr << 1) | mode;
    twi_byte_transfer(addr, false, WRITE);
	if(twi_byte_transfer(0x00, true, READ) != 0x00){ // check ack
		twi_stop();
		return false; // ACK failed
    }
	
    // Read or Write loop
    do{
        // read/write next byte
        *buff = twi_byte_transfer(*buff, false, mode);
		ack = twi_byte_transfer(0x00, true, 0x01^mode); // read/write ack bit
        buff++; // shift buffer pointer/index forward
		length--;
    }while(length && (ack == 0x00));
	
    //if read mode, end with NACK
    if(mode == READ){
        twi_byte_transfer(0x01, true, WRITE);
    }
	
    twi_stop();
	return (ack == 0x00);
}