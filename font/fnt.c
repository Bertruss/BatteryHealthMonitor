#include "../include/fnt.h"
#include <stdint.h>

const uint8_t char_width = 5;

const char fnt_lut[51] = {
	'!', '%', '&', '\'', '+', ',', '-', '.', '/', '0',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', ':',
	';', '<', '=', '>', '?', 'A', 'B', 'C', 'D', 'E',
	'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

uint8_t characterLookup(char val){
	const char* i = fnt_lut;
	while(*(i++) != val);
	return (i - fnt_lut-1)*5;
}