#include "SSD1306_driver.h"
#include "fnt.h"
#include "TinyTWI.h"
#include "TinyEeprom.h"

void display_write_char(char val){
	//retrieve address of character bitmap from eeprom
	uint8_t addr = characterLookup(val);
	uint8_t char_buffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	if(val != ' '){
		eeprom_read(&char_buffer, addr, 5);
	}
	uint8_t write_buffer[2] = {0x40, 0x00};
	// this thing is d u m b.
	for(uint8_t i = 0; i < 5; i++){
		cursor_pg = 0;
		//pull first column
		write_buffer[1] =
		((char_buffer[i] & 0x80) >> 7) +
		((char_buffer[i] & 0x40) >> 4) +
		((char_buffer[i] & 0x20) >> 1) +
		((char_buffer[i] & 0x10) << 2);
		//write slice
		twi_transmission (SCREEN_ADDR, write_buffer, 2, WRITE);
		SSD1306_set_cursor(++cursor_pg, cursor_col);
		write_buffer[1] =
		((char_buffer[i] & 0x08) >> 3) +
		((char_buffer[i] & 0x04)) +
		((char_buffer[i] & 0x02) << 3) +
		((char_buffer[i] & 0x01) << 6);
		twi_transmission (SCREEN_ADDR, write_buffer, 2, WRITE);
		SSD1306_set_cursor(--cursor_pg, ++cursor_col);
	}
}
