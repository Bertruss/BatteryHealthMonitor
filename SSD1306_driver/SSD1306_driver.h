#include "TinyTWI.h"

#define SSD1306_ADDR 0x3c
#define SCREEN_ADDR 0x3c

void SSD1306_init();
void SSD1306_set_cursor(uint8_t page, uint8_t col);
void SSD1306_reset_cursor();
void SSD1306_clear();
void SSD1306_clear_segment(uint8_t pg, uint8_t start_col, uint8_t end_col);