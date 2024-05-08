#include "TinyADC.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t adc_active_pin = 0xFF;

ISR(ADC_vect){
	ADCSRA |= (1 << ADIF); // reset the conversion-finished flag
}

void adc_pin_select(enum adc_pin input_pin){
    uint8_t in_buffer_lut[4] = {0x20, 0x04, 0x10, 0x08}; // LUT from admux val to data input buffer disable

    // clear settings for prior adc input selected, mux enable and input buffer disable
    ADMUX &= 0xF0;
    DIDR0 &= !0x3C;

    ADMUX |= input_pin;
    DIDR0 |= in_buffer_lut[input_pin]; // disable the digital input buffer on the input pin
    adc_active_pin = input_pin;
}

void adc_init(){
    /*
    REFS[7,6,4] -> 000 
    Vcc selected as VREF
    */

    ADCSRA = (1 << ADEN) | // enable the ADC
            (1 << ADPS2) | // set the prescaler to 128. 8mhz/128 -> 62.5 khz
            (1 << ADPS1) |
            (1 << ADPS0);
}

void adc_enable(){ 
    ADCSRA |= (1 << ADEN);
}

void adc_pause(){ 
    ADCSRA &= !(1 << ADEN);
}

uint16_t adc_read(enum adc_mode mode){ 
    // conversion started by writing a one to the ADC start conversion bit ADSC.
    // this is held high until the conversion is completed.

    if(mode == WAKE){
        ADCSRA |= (1 << ADSC);
        while((ADCSRA & (1 << ADIF)) == 1); // wait for the conversion to complete
    }else{
        MCUCR |= (1 << SE) | // sleep enable, ADC noise reduction
			(1 << SM0); 
    }
    
    // The ADC generates a 10-bit result which is presented in the ADC Data Registers, ADCH and ADCL
    // ADCL must be read first, then ADCH
    // but can optionally be presented left adjusted by setting the ADLAR bit in
    // ADMUX
    uint16_t result = 0x0000;
    // read the result, copying first the low byte then the high bits    
    result = ADCL |
            (ADCH << 8);
    
    return result;
}

uint16_t adc_measure_ref(){
    // Check Vcc by measuring 1.1v internal voltage reference, with Vcc as VREF. 
    
    // Set the input as the 1.1 ref, referred to as Vbg or 'Bandgap Voltage'
    uint8_t tmp_ADMUX = ADMUX;  // cache current setting 
    ADMUX &= 0xF0;      // clear setting
    ADMUX |= ADC_REF;   // set ADC reference

    // Note: VREF is still vcc, so the measured value of the 1.1 ref will inform on Vcc's deviation from 5v
    // After switching to internal voltage reference the ADC requires a settling time of 1ms before measurements are stable. 
    // Conversions starting before this may not be reliable. The ADC must be enabled during the settling time.
    _delay_ms(1);

    // normal value read
    uint16_t result = adc_read(SLEEP);
    
    // reset input selection
    ADMUX &= 0xF0;
    ADMUX = tmp_ADMUX;

    return result;
}
