#ifndef FNT_H
#define FNT_H
// NOTE: consider using an Enum?
// index in list is the same as index in eeprom, multiplied by the width of the character bitmap
uint8_t char_width = 5;
char fnt_lut[51] = {
'!', '%', '&', '\'', '+', ',', '-', '.', '/', '0', 
'1', '2', '3', '4', '5', '6', '7', '8', '9', ':', 
';', '<', '=', '>', '?', 'A', 'B', 'C', 'D', 'E', 
'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

uint8_t characterLookup(char val){
    uint8_t i = 0;
    while(fnt_lut[i++] != val )
    return i*char_width;
}
#endif