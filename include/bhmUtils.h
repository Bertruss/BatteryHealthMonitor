#ifndef BHM_UTILS_H
#define BHM_UTILS_H
#include <stdint.h>
#include "TinyADC.h"
void bhm_init();
uint32_t measure_vcc_voltage();
uint32_t measure_battery_voltage();
uint32_t measure_current_draw();
uint32_t bhm_adc_read(enum adc_pin select);
uint8_t basic_charge_estimate(uint32_t voltage);
uint8_t adv_charge_estimate(uint32_t voltage, uint32_t current, uint64_t *estimated_energy);
#endif