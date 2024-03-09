#include "TinyTWI.h"

#define __dwell_low()  _delay_us(4.7)
#define __dwell_high() _delay_us(4)

// refer to section 15 (pg.108) of the ATtiny25/45/85 Datasheet for more information
void tinyTWI_init(){
    //intialize Data Direction on sda and scl as output val
    DDR_TWI |= (1 << SCL) | (1 << SDA)

    //initialize sda and scl to "high", default state
    PORT_TWI  |= (1 << SCL) | (1 << SDA)

    // Set the data register on the USI port
    USIDR = 0xFF;

    // Set the Control reg 
    USICR = (1 << USIWM1)| //Set the wire mode to TWI or I2C
            (1 << USICS1)| //set "external, pos-edge" clock source
            (1 << USICLK); //and set the 4-bit counter clk to software gen, via the (USITC) register
    // NOTE: for the sake of the 4-bit counter, this is an "external clock", 
    // so the counter is triggered on both edges. Hence a 4-bit (0-15 value) counter to count
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


// Write
{
    PORT_I2C &= ~(1 << SCL_I2C); //  set clk low
    while ((PIN_I2C & (1 << SCL_I2C)) != 0); // spin wait for pin to pull low ?
    xfer(data, false); //transfer full byte
    DDR_I2C &= ~(1 << SDA_I2C); //switch to read
    return (xfer(0xff, true) & (1 << 0)) == 0; // return ACK or NACK
}

void twi_start(){
    // next 2 lines should not be necessary ??
    //PORT_TWI |= (1 << SCL); // set clock high 
    //while ((PIN_TWI & (1 << SCL)) == 0); // wait for clock to read high
    //__dwell_high();  
    PORT_TWI &= ~(1 << SDA); // set SDA low
    __dwell_low();
    PORT_TWI &= ~(1 << SCL); // set clock low
    while ((PIN_TWI & (1 << SCL)) != 0); // wait for clock to go low
    PORT_TWI |= (1 << SDA); // set SDA high
    return USISR & (1 << USISIF); // check for the start condition interrupt flag. in this mode it doesn't trigger an interrupt though?
}

void twi_stop(){

}

bool twi_transfer(char buff, bool bit, xfer_type mode){
    if(mode == WRITE){
        DDR_TWI |= (1 << SDA) // data direction : write
        USIDR = buff; // move the data to the USI data register
    }else{ // READ
        DDR_TWI &= ~(1 << SDA) // data direction : read
        //USIDR = 0xff; // doesn't matter
    }

    if(bit){ //only read or write single bit
        USISR |= (0xE << USICNT0); //overflow with 1 bit
    }   
    

    // following loop with shift the data register onto the sda line by toggling the clock
    do {
        __dwell_low();
        USICR |= (1 << USITC); // toggle the clock
        while ((PIN_I2C & (1 << SCL_I2C)) == 0); //wait for set
        __dwell_high();

        USICR |= (1 << USITC); // toggle the clock
        while ((PIN_I2C & (1 << SCL_I2C)) != 0); //wait for set
    }
    while((USISR & (1 << USIOIF)) == 0); // Check for counter overflow
}

int twi_transmission (char addr, char* buff, char mode){
    //sda brought low
    //scl brought low
    //delay one clk

    //write address [0:6]
    //write "" bit [7]
    // Concatenate the address and transmit mode
    addr |= (addr << 1) | (mode);
    twi_transfer(addr, false, WRITE);

    //// ** data loop **
    if(mode){ //DATA READ
        //nope
    }
    else{   //DATA WRITE
        do{
            // write next byte [0:7]
            twi_write();
            // read ack
            // check for end of buffer (for our purposes it will be null terminated)
            // ** end **
        }while(!*++buff)
    }

    //scl brought high
    //sda brought high
}
