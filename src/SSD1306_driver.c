#include "SSD1306_driver.h"
#include "../TinyTWI/TinyTWI.h"

/* 
 * 128x32 oled screen driver 
 * Uses tinyTWI library
*/
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
	0x80, 0x07, /* ^ end page 7 */
	0x80, 0x81, /* Set Brightness */
	0x80, 0x1F	/* ^ brightness val*/
};

uint8_t cursor_cmd[] = {
	0x80, 0xb0,  // page start address: 0xb0 | (y >> 3)
	0x80, 0x00,  // lower nibble of column: 0x00 | (x & 0x0f)
	0x80, 0x10,  // upper nibble of column: 0x10 | ((x >> 4) & 0x0f)
};

uint8_t brightness_cmd[] = {
	0x80, 0x81, // Brightness cmd
	0x80, 0x00 // value
};

void SSD1306_init(){
	twi_transmission(SCREEN_ADDR, &initialization, 26, WRITE);
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
	int i = 8;
	do{
		i--;
	    SSD1306_clear_segment(i, 0, 127);
    }while(i > 0);
	 SSD1306_set_cursor(0, 0); // put the cursor at the upper right hand corner
}

void SSD1306_clear_segment(uint8_t pg, uint8_t start_col, uint8_t end_col){
	//clears section of screen
	SSD1306_set_cursor(pg, start_col);
    uint8_t len = end_col - start_col + 1;
	while(len > 0){
		len--;
		SSD1306_draw(0x00);
	}
	SSD1306_reset_cursor();
}

void SSD1306_draw(uint8_t byte){
	// sends a byte with a draw command. Note, does not automatically increment the "cursor" var. 
	uint8_t write_buff[2] = {0x40, byte};
	twi_transmission (SCREEN_ADDR, write_buff, 2, WRITE);
}

void SSD1306_set_brightness(uint8_t val){
	brightness_cmd[3] = val;
	twi_transmission (SCREEN_ADDR, brightness_cmd, 4, WRITE);
}