#include <avr/io.h>

void adc_init(){
/*
REFS[7,6,4] -> 000 
Vcc selected as VREF
TODO: consider alternative VREF
*/
ADMUX = (1 << MUX1) | // select ADC3 
        (1 << MUX0); 

ADCSRA = (1 << ADEN) | // enable the ADC
        (1 << ADPS2) | // set the prescaler to 128. 8mhz/128 -> 62.5 khz
        (1 << ADPS1) |
        (1 << ADPS0) |

DIDR0 |= (1 << ADC3D) 

}

void adc_enable(){ 
    ADCSRA |= (1 << ADEN);
}

void adc_pause(){ 
    ADCSRA &= !(1 << ADEN);
}

uint16_t adc_read_async(){ 
    //TODO
}

uint16_t adc_read_sync(){ 
    // conversion started by writing a one to the ADC start conversion bit ADSC.
    // this is held high until the conversion is completed.
    ADCSRA |= (1 << ADSC);
    while((ADCSRA & (1 << ADIF)) == 0); // wait for the conversion to complete
    ADCSRA |= (1 << ADIF); // reset the conversion-finished flag
    

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