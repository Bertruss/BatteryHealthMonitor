#include "TinyEeprom.h"
#include <avr/io.h>

void eeprom_read(uint8_t *read_buff, uint8_t start_addr, uint8_t length){
    for(int i; i < length; i++){
        read_buff[i] = eeprom_read_byte(start_addr + i);
    } 
}

uint8_t eeprom_read_byte(uint8_t add){
    EEARL=add;
    /* Start eeprom read by writing EERE */
    EECR |= (1<<EERE);
    /* Return data from data register */
    return EEDR;
}