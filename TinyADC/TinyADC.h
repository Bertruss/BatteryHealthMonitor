#ifndef TINY_ADC_H
#define TINY_ADC_H
#include <stdint.h>
// interface for ease of use of the ADC

extern uint8_t adc_active_pin;

enum adc_mode{
	WAKE = 0x00,
	SLEEP = 0x01
}adc_mode;

enum adc_pin{
	ADC1_PB2 = 0x01,
	ADC3_PB3 = 0x03,
	ADC2_PB4 = 0x02,
	ADC0_PB5 = 0x00,
	ADC_REF = 0x0C
}adc_pin;

void adc_init();
void adc_pin_select(enum adc_pin input_pin);
void adc_enable();
void adc_pause();
uint16_t adc_read(enum adc_mode mode);
uint16_t adc_measure_ref();
#endif