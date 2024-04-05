#ifndef BHM_UTILS_H
#define BHM_UTILS_H
#include <stdint.h>
uint32_t measure_battery_voltage();
uint8_t calculate_battery_percentage(uint32_t voltage);
#endif