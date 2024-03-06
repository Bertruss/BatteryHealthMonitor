#include "TinyTWI.h"

void tinyTWI_init(){
    //intialize DDR val
    DDR_TWI |= (1 << SCL) | (1 << SDA)

    //initialize output to "high"
    PORT_TWI  |= (1 << SCL) | (1 << SDA)

    // Set the data register on the USI port
    USIDR = 0xFF;

    // Set the Control reg 
    USICR = (1 << USIWM1)| //Set the wire mode to  
            (1 << USICS1)| //set "external, pos-edge" clock source
            (1 << USICLK); //and set the 4-bit counter clk to software gen (USITC)
}


bool twi_write(char buff){
    

}

int transmission (char addr, char* buff, char mode){
    //sda brought low
    //scl brought low
    //delay one clk

    //write address [0:6]
    //write "" bit [7]
    // Concatenate the address and transmit mode
    addr |= (addr << 1) | (mode);

    //// ** data loop **
    if(mode){ //DATA READ

    }
    else{   //DATA WRITE
        do{
            // write next byte [0:7]
            // read ack
            // check for end of buffer (for our purposes it will be null terminated)
            // ** end **
        }while(!*++buff)
    }

    //scl brought high
    //sda brought high
}
