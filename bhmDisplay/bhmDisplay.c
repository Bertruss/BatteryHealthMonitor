#include "SSD1306_driver.h"
#include "fnt.h"
#include "TinyTWI.h"
#include "TinyEeprom.h"

// width = 25;
// height = 4;
/*
|	Z |~~~~~~~~~~~~~~~~~| 100%                            
|	est. time - ____  	
| 			[warning]
*/

void display_write_char(char val){
	//retrieve address of character bitmap from eeprom
	uint8_t addr = characterLookup(val);
	uint8_t char_buffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	if(val != ' '){
		eeprom_read(&char_buffer, addr, 5);
	}
	uint8_t buff;
	/* this thing is d u m b.
	/ let me explain:
	/ the SSD1306 with 128x32 pixel oled screen technically has screen buffer large enough for a screen twice that size;
	/ the same chip drives a 128x64 oled. So, you might assume, that must mean that only half the addressable buffer is used. 
	/ You would be wrong.
	/ No, instead, for reasons that are murky to me, they decided that it made more sense to map full bytes to nibbles, 
	/ only drawing every other bit to the screen in this form 01010101b. So you either have to waste space pre-encoding your data
	/ to expand every byte to two bytes, or you have to waste cycles splitting every byte into two bytes. leading to the monstrosity below.
	/ admittedly this is compounded by the font needing to be flipped from its orientation in memory, but still.
	*/
	for(uint8_t i = 0; i < 5; i++){
		cursor_pg = 0;
		//pull first column
		buff =
		((char_buffer[i] & 0x80) >> 7) +
		((char_buffer[i] & 0x40) >> 4) +
		((char_buffer[i] & 0x20) >> 1) +
		((char_buffer[i] & 0x10) << 2);
		//first write slice
		SD1306_draw(buff);
		SSD1306_set_cursor(++cursor_pg, cursor_col);
		buff =
		((char_buffer[i] & 0x08) >> 3) +
		((char_buffer[i] & 0x04)) +
		((char_buffer[i] & 0x02) << 3) +
		((char_buffer[i] & 0x01) << 6);
		SD1306_draw(buff);
		SSD1306_set_cursor(--cursor_pg, ++cursor_col);
	}
}

void display_write_char_2x(char val){
	// draw characters double size
	// retrieve address of character bitmap from eeprom
	uint8_t addr = characterLookup(val);
	uint8_t char_buffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	if(val != ' '){
		eeprom_read(&char_buffer, addr, 5);
	}
	uint8_t buff;

	for(uint8_t i = 0; i < 5; i++){
		cursor_pg = 0;
		//pull first column
		buff =
		((char_buffer[i] & 0x80) >> 7) +
		((char_buffer[i] & 0x80) >> 5) +
		((char_buffer[i] & 0x40) >> 2) +
		((char_buffer[i] & 0x40) >> 0);
		//first write slice
		SD1306_draw(buff);
		SD1306_draw(buff);
		SSD1306_set_cursor(++cursor_pg, cursor_col);
		buff =
		((char_buffer[i] & 0x20) >> 5) +
		((char_buffer[i] & 0x20) >> 3) +
		((char_buffer[i] & 0x10)) +
		((char_buffer[i] & 0x10) << 2);
		//first write slice
		SD1306_draw(buff);
		SD1306_draw(buff);
		SSD1306_set_cursor(++cursor_pg, cursor_col);
		buff =
		((char_buffer[i] & 0x08) >> 3) +
		((char_buffer[i] & 0x08) >> 1) +
		((char_buffer[i] & 0x04 << 2)) +
		((char_buffer[i] & 0x04) << 4);
		SD1306_draw(buff);
		SD1306_draw(buff);
		SSD1306_set_cursor(++cursor_pg, cursor_col);
		buff =
		((char_buffer[i] & 0x02) >> 1) +
		((char_buffer[i] & 0x02) << 1) +
		((char_buffer[i] & 0x01) << 4);
		((char_buffer[i] & 0x01) << 6);
		SD1306_draw(buff);
		SD1306_draw(buff);
		SSD1306_set_cursor(--cursor_pg, ++cursor_col);
	}
}

void draw_graphic(uint8_t addr){
	SD1306_draw(graphics[addr]);
	SSD1306_set_cursor(++cursor_pg, cursor_col);
	SD1306_draw(graphics[addr+1]);
	SSD1306_set_cursor(--cursor_pg, ++cursor_col);
}

void draw_percent_bar(uint8_t percent){
//draw percent bar 0-100
	//set cursor
	SSD1306_set_cursor(0, 0);

	//draw lightning bolt
	for(int i = 0; i < 4; i++){
		draw_graphic(2*i+6);
	}
	// add a line
	SSD1306_set_cursor(cursor_pg, ++cursor_col);
	
	//draw the start of the percent bar
	draw_graphic(0);
	draw_graphic(1);
	int i = 0;
	for(; i < percent - 5; i++)}{
		draw_graphic(4);
	}
	for(; i < 95; i++)}{
		draw_graphic(2);
	}
	//draw the endcap
	draw_graphic(1);
	draw_graphic(0);
	
	// line spacer 
	SSD1306_set_cursor(cursor_pg, ++cursor_col);
	// Convert percent to text :[ 
}

void clear_warning(){
	SSD1306_clear_segment(6, 0, 127);
	SSD1306_clear_segment(7, 0, 127);
}

void display_warning(){
	SSD1306_set_cursor(6, 0);
	for(int i = 0; i < 24 ; i++){
		display_write_char(warning_txt[i]);		
	}
}

void display_percent(uint8_t val){
	// Converts uint8_t to num string
	// Returns number of characters in return string

	uint8_t len = 0;
	uint8_t temp = 0;
	// ascii numbers start with address 0x0A in the font
	if(val == 100){
		out[0] = 0x0A + 1; "1"
		out[1] = 0x0A; "0"
		out[2] = 0x0A; "0"
	}
	else{
		if (val>10){
			temp = val / 10;
			out[len++] = temp + 0x0A;
			val -= temp;
		}
		out[len++] = val/1 + 0x0A;
	}
	
}