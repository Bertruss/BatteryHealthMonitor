#ifndef BHM_UTILS_H
#define BHM_UTILS_H
#include <stdint.h>
void bhm_init();
uint32_t measure_battery_voltage();
uint8_t battery_charge(uint32_t voltage);
#endif