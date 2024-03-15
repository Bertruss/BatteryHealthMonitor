#include <avr/io.h>

uint8_t* eeprom_read(uint8_t start_addr, uint8_t length); 
uint8_t eeprom_read_byte(uint8_t add);