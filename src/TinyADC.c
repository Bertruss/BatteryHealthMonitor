#include "../include/TinyADC.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

enum adc_pin selected_input;

ISR(ADC_vect){
}

//TODO: this mostly seems unnecessary. disable digital input buffers.
void digital_input_buffer_toggle(enum adc_pin input_pin){
	uint8_t in_buffer_lut[4] = {0x20, 0x04, 0x10, 0x08}; // LUT from admux val to data input buffer disable	
}

void adc_pin_select(enum adc_pin input_pin){
    // clear settings for prior adc input selected, mux enable and input buffer disable
    ADMUX &= 0xF0;
	ADMUX |= input_pin;
	selected_input = input_pin;
}

void adc_init(){
    /*
    REFS[7,6,4] -> 000 
    Vcc selected as VREF
    */
    ADCSRA |= (1 << ADEN) | // enable the ADC
            (1 << ADPS2) | // set the prescaler to 128. 8mhz/128 -> 62.5 khz
            (1 << ADPS1) |
            (1 << ADPS0);
	ADCSRA |= (1 << ADIF);
	DDRB |= 0x00; // set all adc pins to input
}

void adc_enable(){ 
    ADCSRA |= (1 << ADEN);
}

void adc_pause(){ 
    ADCSRA &= ~(1 << ADEN);
}

uint16_t adc_read(enum adc_mode mode){ 
    // conversion started by writing a one to the ADC start conversion bit ADSC.
    // this is held high until the conversion is completed.
    if(mode == WAKE){
		// normal waking read
		ADCSRA |= (1 << ADSC);	
		while((ADCSRA & (1 << ADIF)) == 0); // wait for the conversion to complete, just to be sure
    }else{
		// sleep cycle
		sei();
		set_sleep_mode(SLEEP_MODE_ADC);
		sleep_enable();
		ADCSRA |= (1 << ADSC) | (1 << ADIE);
		sleep_cpu(); //start slept conversion
		sleep_disable (); //wake
		while((ADCSRA & (1 << ADSC)) == 1);
		ADCSRA &= ~(1 << ADIE);
    }
    // ensure interrupt flag is cleared
	ADCSRA |= (1 << ADIF);
	
    // The ADC generates a 10-bit result which is presented in the ADC Data Registers, ADCH and ADCL
    // ADCL must be read first, then ADCH
    // but can optionally be presented left adjusted by setting the ADLAR bit in
    // ADMUX
    uint16_t result = 0x0000;
    // read the result, copying first the low byte then the high bits    
    result |= ADCL;
    result |= ADCH << 8;
    return result;
}
