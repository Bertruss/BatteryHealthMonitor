#ifndef TINY_ADC_H
#define TINY_ADC_H
#include <stdint.h>
// interface for ease of use of the ADC

void adc_init();
void adc_enable();
void adc_pause();
uint16_t adc_read_async();
uint16_t adc_read_sync();
#endif