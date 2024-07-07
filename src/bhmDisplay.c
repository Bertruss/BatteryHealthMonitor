#include "../include/bhmDisplay.h"
#include "../include/SSD1306_driver.h"
#include "../include/fnt.h"
#include "../include/TinyTWI.h"
#include "../include/TinyEeprom.h"
#include "../include/ADCscale.h"

// bhmDisplay is a collection of graphics drawing functions specific to the battery health monitor use case.

// width = 25;
// height = 4;
/*
|	Z |~~~~~~~~~~~~~~~~~| 100%
|	VOLTAGE :  	
|	EST. TIME : ____  	
| 			[warning]
*/

uint8_t graphics[16] = {
	0xff, 0xff,  // endcap
	0x01, 0x40,  // empty ind
	0xf1, 0x4f,  // full ind
	0x04, 0x01,  // begin - lightning bolt
	0x10, 0x05,
	0x40, 0x11,
	0x00, 0x41	// end - lightning bolt
};

char charge_txt[6] = {'C', 'H', 'A', 'R', 'G', 'E'};
char time_txt[9] = {'E', 'S', 'T', '.', ' ', 'T', 'I', 'M', 'E'}; // might be too ambitious
char warning_txt[24] = {'B', 'A', 'T', 'T', 'E', 'R', 'Y', ' ', 'L','E','V','E','L',' ','C','R','I','T','I','C','A','L'};
char voltage_txt[13] = {'C','E','L','L',' ','V','O','L','T','A','G','E', ':'};

void update_display(uint8_t percent, bool warn, uint32_t voltage){
	draw_percent_bar(percent);
	display_voltage(voltage);
	// draw time estimate?
	if(warn){
		display_warning();
	}else{
		clear_warning();
	}
}

void write_char(char val){
	// retrieve address of character bitmap from eeprom
	if(val == ' '){
		render_symbol(255);
	}else{
		render_symbol(characterLookup(val));
	}
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
	uint8_t char_buffer[5] = {0x00,0x00,0x00,0x00,0x00};
	if(address != 255){
		eeprom_read(char_buffer, address, 5);
	}
	uint8_t i = 0;
	uint8_t ii = 0;
	while(i != 0x15){
		ii = (i & 0x0F);
		buff = 
		((char_buffer[ii] & 0x80) >> 7) +
		((char_buffer[ii] & 0x40) >> 4) +
		((char_buffer[ii] & 0x20) >> 1) +
		((char_buffer[ii] & 0x10) << 2);
		char_buffer[ii] = (0x0F & char_buffer[ii]) << 4;
		SSD1306_draw(buff);
		i++;
		if(i == 5){
			i = 0x10;
			SSD1306_set_cursor(cursor_pg+1, cursor_col);
		}
	}
	SSD1306_set_cursor(cursor_pg-1, cursor_col+5);
}

void draw_graphic(uint8_t addr){
	// draw graphic selected from library 
	SSD1306_draw(graphics[addr]);
	SSD1306_set_cursor(cursor_pg+1, cursor_col);
	SSD1306_draw(graphics[addr+1]);
	SSD1306_set_cursor(cursor_pg-1, cursor_col+1);
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
	draw_graphic(2);
	int i = 0;
	for(; i < 96; i++){
		if(i < percent - 5){
			draw_graphic(4);
		}else{
			draw_graphic(2);
		}
	}
	//draw the endcap
	draw_graphic(0);
	
	// line spacer 
	SSD1306_set_cursor(cursor_pg, ++cursor_col);
	display_percent_charge(percent);	
}

void clear_warning(){
	SSD1306_clear_segment(6, 0, 127);
	SSD1306_clear_segment(7, 0, 127);
}

void display_warning(){
	SSD1306_set_cursor(6, 0);
	for(int i = 0; i < 22 ; i++){
		write_char(warning_txt[i]);		
	}
}

void display_percent_charge(uint8_t value){
	// writes out battery percentage estimate
	uint8_t numstart = 0x2d;
	uint8_t temp = 0;
	// ascii numbers start with address 0x2D in the font
	if(value == 0x64){
		render_symbol(numstart + 6);
		SSD1306_set_cursor(cursor_pg, cursor_col-2); //adjusting weird kerning
		render_symbol(numstart);
		render_symbol(numstart);
	}else{
		temp = value/10;
		render_symbol(temp*5 + numstart);
		
		value = value % 10;
		render_symbol(value*5 + numstart);
	}
	write_char('%');
}

void display_voltage(uint32_t value){
	// convert val to display voltage
	// TODO: rework with some bitwise operations. this may take a lot of cycles
	// determine expected voltage range (8 - 6.4 for 2 cell?)
	
	uint8_t v = value/100000000;
	value = value % 100000000;
	
	// pull the .1's place
	uint8_t cv = value/10000000;
	value = value % 10000000;

	// pull the .01's
	uint8_t mv = value/1000000;
  
	SSD1306_set_cursor(2, 0);

	for(int i = 0; i < 13; i++){
		write_char(voltage_txt[i]);
	}
	render_symbol(0x2d + v*5);
	write_char('.');
	render_symbol(0x2d + cv*5);
	render_symbol(0x2d + mv*5);
	write_char('V');
}

void test_fnct16(uint16_t val, uint8_t position, uint8_t col){
	// convert val to display voltage
	// TODO: rework with some bitwise operations. this may take a lot of cycles
	// determine expected voltage range (8 - 6.4 for 2 cell?)
	
	uint8_t thousand = val/1000;
	val = val % 1000;
	
	// pull the .1's place
	uint8_t hundred = val/100;
	val = val % 100;

	// pull the .1's place
	uint8_t tens = val/10;
	val = val % 10;

	// pull the .01's
	uint8_t ones = val;
	
	SSD1306_set_cursor(position, col);
	render_symbol(0x2d + thousand*5);
	render_symbol(0x2d + hundred*5);
	render_symbol(0x2d + tens*5);
	render_symbol(0x2d + ones*5);
}

void test_fnct32(uint32_t val, uint8_t position, uint8_t col){
	// convert val to display voltage
	// TODO: rework with some bitwise operations. this may take a lot of cycles
	// determine expected voltage range (8 - 6.4 for 2 cell?)
	// max = 4,294,967,295
	uint8_t billion = val/1000000000;
	val = val % 1000000000;
	
	// pull the .1's place
	uint8_t hundredmillion = val/100000000;
	val = val % 100000000;

	// pull the .1's place
	uint8_t tenmillion = val/10000000;
	val = val % 10000000;
	
	uint8_t million = val/1000000;
	val = val % 1000000;
	
	// pull the .1's place
	uint8_t hundredthousand = val/100000;
	val = val % 100000;

	// pull the .1's place
	uint8_t tenthousand = val/10000;
	val = val % 10000;

	uint8_t thousand = val/1000;
	val = val % 1000;
	
	// pull the .1's place
	uint8_t hundred = val/100;
	val = val % 100;

	// pull the .1's place
	uint8_t tens = val/10;
	val = val % 10;

	// pull the .01's
	uint8_t ones = val;
	
	SSD1306_set_cursor(position, col);
	render_symbol(0x2d + billion*5);
	render_symbol(0x2d + hundredmillion*5);
	render_symbol(0x2d + tenmillion*5);
	render_symbol(0x2d + million*5);
	render_symbol(0x2d + hundredthousand*5);
	render_symbol(0x2d + tenthousand*5);
	render_symbol(0x2d + thousand*5);
	render_symbol(0x2d + hundred*5);
	render_symbol(0x2d + tens*5);
	render_symbol(0x2d + ones*5);
}