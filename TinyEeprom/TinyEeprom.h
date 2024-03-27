#ifndef TINY_EEPROM_H
#define TINY_EEPROM_H
#include <avr/io.h>

void eeprom_read(uint8_t *read_buff, uint8_t start_addr, uint8_t length);
uint8_t eeprom_read_byte(uint8_t add);
#endif