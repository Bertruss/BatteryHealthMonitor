#ifndef BHM_DISPLAY_H
#define BHM_DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
//bhmDisplay is a collection of graphics drawing functions specific to the battery health monitor use case.

void update_display(uint8_t percent, bool warn, uint32_t voltage, uint32_t current);
void write_char(char val);
void draw_graphic(uint8_t addr);
void draw_percent_bar(uint8_t percent);
void render_symbol(uint8_t address);
void clear_warning();
void display_warning();
void display_percent_charge(uint8_t val);
void display_4digit(uint32_t value, char unit);
void display_voltage(uint32_t value);
void display_current(uint32_t value);
void display_delete();
#endif