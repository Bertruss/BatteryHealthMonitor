#include "SSD1306_driver.h"
#include "fnt.h"
#include "TinyTWI.h"
#include "TinyEeprom.h"

// width = 25;
// height = 4;
/*
|	Z |~~~~~~~~~~~~~~~~~| 100%
|	VOLTAGE :  	
|	EST. TIME : ____  	
| 			[warning]
*/

void update_display(uint8_t percent, bool warn, uint16_t voltage){
	draw_percent_bar(percent);
	draw_voltage(voltage);
	// draw time estimate?
	if(warn){
		display_warning();
	}else{
		clear_warning();
	}
}

void write_char(char val){
	// retrieve address of character bitmap from eeprom
	uint8_t addr;
	if(val != ' '){
		addr = characterLookup(val);
	}else{
		addr = 255;
	}
	render_symbol(addr);
}

void render_symbol(uint8_t address){// 
	/* 
	/ this thing is d u m b.
	/ let me explain:
	/ the SSD1306 with 128x32 pixel oled screen technically has screen buffer large enough for a screen twice that size;
	/ the same chip drives a 128x64 oled. So, you might assume (like me), that must mean that only half the addressable buffer is used. 
	/ You would be wrong.
	/ No, instead, for reasons that are murky to me, they decided that it made more sense to map full bytes to nibbles, 
	/ only drawing every other bit to the screen in this form 01010101b. So you either have to waste space pre-encoding your data
	/ to expand every byte to two bytes, or you have to waste cycles splitting every byte into two bytes. leading to the monstrosity below.
	/ admittedly this is compounded by the font needing to be flipped from its orientation in memory, the screen is written LSB first.
	*/
	uint8_t buff;
	uint8_t char_buffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	if(address != 255){
		eeprom_read(&char_buffer, addr, 5);
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
	else{
		for(uint8_t i = 0; i < 5; i++){
			SD1306_draw(0x00);
			SSD1306_set_cursor(++cursor_pg, cursor_col);
			SD1306_draw(0x00);
			SSD1306_set_cursor(--cursor_pg, ++cursor_col);
		}
	}
}

void render_symbol_x2(char val){
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
	// draw graphic selected from library 
	SD1306_draw(graphics[addr]);
	SSD1306_set_cursor(++cursor_pg, cursor_col);
	SD1306_draw(graphics[addr+1]);
	SSD1306_set_cursor(--cursor_pg, ++cursor_col);
}

void draw_percent_bar(uint8_t percent){
	// draw percent bar 0-100
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
	for(; i < percent - 5; i++){
		draw_graphic(4);
	}
	for(; i < 95; i++){
		draw_graphic(2);
	}
	//draw the endcap
	draw_graphic(1);
	draw_graphic(0);
	
	// line spacer 
	SSD1306_set_cursor(cursor_pg, ++cursor_col);
	display_percent(percent);	
}

void clear_warning(){
	SSD1306_clear_segment(6, 0, 127);
	SSD1306_clear_segment(7, 0, 127);
}

void display_warning(){
	SSD1306_set_cursor(6, 0);
	for(int i = 0; i < 24 ; i++){
		write_char(warning_txt[i]);		
	}
}

void display_percent(uint8_t val){
	// writes out battery percentage estimate
	uint8_t out[3] = {0x00, 0x00, 0x00}; 
	uint8_t len = 0;
	uint8_t temp = 0;
	// ascii numbers start with address 0x0A in the font
	if(val == 100){
		out[0] = 0x0A + 1; "1"
		out[1] = 0x0A; "0"
		out[2] = 0x0A; "0"
		len = 3;
	}
	else{
		if (val>10){
			temp = val / 10;
			out[len++] = temp + 0x0A;
			val -= temp;
		}
		out[len++] = val/1 + 0x0A;
	}
	
	for(int i = 0; i < len; i++){
		render_symbol(out[i]);
	}
	write_char('%');
}

void display_voltage(uint16_t val){
	// convert val to display voltage
	// TODO: rework with some bitwise operations. this may take a lot of cycles
	// determine expected voltage range (8 - 6.4 for 2 cell?)
	
	uint8_t millivolt_quanta = 0x00; //TODO: unknown scaling
	uint8_t quanta_num;
	quanta_num = val/millivolt_quanta;
	uint8_t v = quanta_num/100;
	quanta_num -= v * 100;  
	uint8_t cv = quanta_num/10
	uint8_t mv -= cv * 10;  
	SSD1306_set_cursor(2, 0);

	for(int i = 0; i < 13; i++){
		write_char(voltage_txt[i]);
	}
	render_symbol(0x0A + v)
	write_char('.');
	render_symbol(0x0A + cv);
	render_symbol(0x0A + mv);
}