#include "TinyTWI.h"
#include "SSD1306_driver.h"
#include "fnt.h"
#include "TinyEeprom.h"

uint8_t cursor_pg = 0;
uint8_t cursor_col = 0;

uint8_t initialization[] = {
	0x80, 0x8d, /* Enable charge pump regulator (RESET = ) */
	0x80, 0x14, /* ^ */
	0x80, 0xaf, /* Display On (RESET = ) */
	0x80, 0x20, /* Set Memory Addressing Mode to Horizontal Addressing Mode (RESET = Page Addressing Mode) */
	0x80, 0x00, /* ^ horizontal addressing mode */
	0x80, 0x21, /* Reset Column Address (for horizontal addressing) */
	0x80, 0x00, /* ^ start: col 0 */
	0x80, 0x7F, /* ^ end: col 127 */
	0x80, 0x22, /* Reset Page Address (for horizontal addressing) */
	0x80, 0x00, /* ^ start page 0 */
	0x80, 0x07  /* ^ end page 7 */
};

uint8_t cursor_cmd[] = {
	0x80, 0x20, /* Set Memory Addressing Mode to Horizontal Addressing Mode (RESET = Page Addressing Mode) */
	0x80, 0x00, /* ^ horizontal addressing mode */
    0x80, 0xb0,  // page start address: 0xb0 | (y >> 3)
    0x80, 0x00,  // lower nibble of column: 0x00 | (x & 0x0f)
    0x80, 0x10,  // upper nibble of column: 0x10 | ((x >> 4) & 0x0f)
};

void SSD1306_init(){
    twi_transmission(SCREEN_ADDR, &initialization, 22, WRITE);
}

void SSD1306_write_char(char val){
    //retrieve address of character bitmap from eeprom
    uint8_t addr = characterLookup(val);
    char *char_buffer = eeprom_read(addr, 5);
    uint8_t *write_buffer = malloc(2);
    write_buffer[0] = 0x40;
	// this thing is d u m b. 
    for(uint8_t i = 0; i < 5; i++){
		cursor_pg = 0;
        //pull first column
        write_buffer[1] = 
        ((buffer[i] & 0x80) >> 7) +
        ((buffer[i] & 0x40) >> 4) +
        ((buffer[i] & 0x20) >> 1) +
        ((buffer[i] & 0x10) << 2);
        //write slice
        twi_transmission (SCREEN_ADDR, write_buffer, 2, WRITE);
        SSD1306_set_cursor(++cursor_pg, cursor_col);
		write_buffer[1] = 
        ((buffer[i] & 0x08) >> 3) +
        ((buffer[i] & 0x04)) +
        ((buffer[i] & 0x02) << 3) +
        ((buffer[i] & 0x01) << 6);
		twi_transmission (SCREEN_ADDR, write_buffer, 2, WRITE);
        SSD1306_set_cursor(--cursor_pg, ++cursor_col);
    }
    free(write_buffer);
    free(char_buffer);
}

void SSD1306_set_cursor(uint8_t page, uint8_t col){
    // set position of write operation
    // page must be less than 8
    // col must be less than 128
    cursor_pg = page;
    cursor_col = col;
    cursor_cmd[5] = 0xb0 | page;
    cursor_cmd[7] = 0x00 | (col & 0x0f);
    cursor_cmd[9] = 0x10 | ((col >> 4) & 0x0f);
    SSD1306_reset_cursor();
}

void SSD1306_reset_cursor(){
    twi_transmission (SCREEN_ADDR, cursor_cmd, 10, WRITE);
}

void SSD1306_clear(){
	//clears the whole screen
    SSD1306_set_cursor(0,0);
    twi_start();
	twi_byte_transfer((SCREEN_ADDR << 1), false, WRITE);
	if(twi_byte_transfer(0x00, true, READ) == 0x00){// read ack
		twi_byte_transfer(0x40, false, WRITE);
		twi_byte_transfer(0x00, true, READ); // read ack
		for(int i = 0; i < 1024; i++){
			twi_byte_transfer(0x00, false, WRITE);
			twi_byte_transfer(0x00, true, READ); // read ack
		}
		twi_stop();
	}
	SSD1306_reset_cursor();
}