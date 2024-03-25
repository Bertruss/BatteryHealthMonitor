#include "SSD1306_driver.h"
#include "TinyTWI.h"
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
	0x80, 0xb0,  // page start address: 0xb0 | (y >> 3)
	0x80, 0x00,  // lower nibble of column: 0x00 | (x & 0x0f)
	0x80, 0x10,  // upper nibble of column: 0x10 | ((x >> 4) & 0x0f)
};

void SSD1306_init(){
	twi_transmission(SCREEN_ADDR, &initialization, 22, WRITE);
}

void SSD1306_set_cursor(uint8_t page, uint8_t col){
	// set position of write operation
	// page must be less than 8
	// col must be less than 128
	cursor_pg = page;
	cursor_col = col;
	cursor_cmd[1] = 0xb0 | page;
	cursor_cmd[3] = 0x00 | (col & 0x0f);
	cursor_cmd[5] = 0x10 | ((col >> 4) & 0x0f);
	SSD1306_reset_cursor();
}

void SSD1306_reset_cursor(){
	twi_transmission (SCREEN_ADDR, cursor_cmd, 6, WRITE);
}

void SSD1306_clear(){
	//clears the whole screen
	for(int i = 0; i < 8; i++){
        SSD1306_clear_segment(i, 0, 127);
    }
}

void SSD1306_clear_segment(uint8_t pg, uint8_t start_col, uint8_t end_col){
	//clears section of screen
	SSD1306_set_cursor(pg, start_col);
	twi_start();
	twi_byte_transfer((SCREEN_ADDR << 1), false, WRITE);
    uint8_t len = end_col - start_col + 1;
	if(twi_byte_transfer(0x00, true, READ) == 0x00){// read ack
		twi_byte_transfer(0x40, false, WRITE);
		twi_byte_transfer(0x00, true, READ); // read ack
		for(int i = 0; i < len; i++){
			twi_byte_transfer(0x00, false, WRITE);
			twi_byte_transfer(0x00, true, READ); // read ack
		}
		twi_stop();
	}
	SSD1306_reset_cursor();
}

void SSD1306_draw(uint8_t byte){
		uint8_t write_buff[2] = {0x40, byte};
		twi_transmission (SCREEN_ADDR, write_buff, 2, WRITE);
}