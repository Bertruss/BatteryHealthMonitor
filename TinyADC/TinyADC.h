#ifndef TINY_ADC_H
#define TINY_ADC_H
#include <stdint.h>
// interface for ease of use of the ADC

extern uint8_t active_pin;
extern enum adc_pin;
void adc_init();
void adc_pin_select(enum adc_pin input_pin);
void adc_enable();
void adc_pause();
uint16_t adc_read(enum adc_mode mode);
uint16_t adc_measure_ref();
#endif