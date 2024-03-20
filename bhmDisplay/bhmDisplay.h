//bhmDisplay are a collection of graphics drawing functions specific to the battery health monitor use case.

uint8_t graphics[16] = {
    0xff, 0xff, // endcap
    0x40, 0x01, // empty ind
    0x4f, 0xf1, // full ind
    0x51, 0x00, // begin - lightning bolt
    0x05, 0x10,
    0x11, 0x40,
    0x01, 0x04 // end - lightning bolt
};

char charge_txt[6] = {'C', 'H', 'A', 'R', 'G', 'E'}; 
char time_txt[9] = {'E', 'S', 'T', '.', ' ', 'T', 'I', 'M', 'E'}; // might be too ambitious
char warning_txt[24] = {'B', 'A', 'T', 'T', 'E', 'R', 'Y', ' ', 'L','E','V','E','L',' ','C','R','I','T','I','C','A','L'};
