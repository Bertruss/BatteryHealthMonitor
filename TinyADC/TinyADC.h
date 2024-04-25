#ifndef TINY_ADC_H
#define TINY_ADC_H
#include <stdint.h>
// interface for ease of use of the ADC
enum adc_mode{
	WAKE = 0x00,
	SLEEP = 0x01
}adc_mode;

void adc_init();
void adc_enable();
void adc_pause();
uint16_t adc_read(enum adc_mode mode);
uint16_t adc_measure_ref();
#endif