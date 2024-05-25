#include "bhmUtils.h"
#include "ADCscale.h"
#include "..\TinyADC\TinyADC.h"
#include "bhmDisplay.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Note: to avoid floating point numbers, using fixed point math. 32-bit unsigned int was used to preserve accuracy (and conserve memory)
// TODO: consider uint64_t, operations had to have their accuracy pre-halved to avoid overflows. (using 10^4 instead of 10^8)
// though it should be noted, this does seem to give at least 2 decimal places free of quantization error.

uint32_t measure_battery_voltage(){
    // measuring Vcc to apply Vref correction (should be ~5v but this step corrects for some innaccuracy)
    uint16_t Vcc_ref = adc_measure_ref();
	test_fnct(Vcc_ref, 0); // DEBUG
	// NOTE: not sure there is a better way to do this to limit quantization error
    uint32_t Vcc = 1.1*10000/Vcc_ref*1024; // measure Vcc, so error can be determined 
   
    // Apply error correction
    // Note: quanta is just hard-coded 5/1024
    // add vcc offset to get 
	ADMUX &= 0x00;
	ADMUX |= (1 << MUX1) | (1 << MUX0);
	uint16_t batt_val = adc_read(WAKE);
	test_fnct(batt_val, 2); // DEBUG
	batt_val = adc_read(WAKE);
	test_fnct(batt_val,6); // DEBUG
    uint32_t val = (batt_val*qaunta)/10000*Vcc/5 + Vcc;
    return val;
}

// TODO: placeholder linear approximation of charge from voltage. very innaccurate
uint8_t battery_charge(uint32_t voltage){
    // "open circuit voltage" battery charge
    //4.2 - 3.6 90%
    //3.6 - 3.2 10%
    return ((((voltage - minV) >> 8)*100)/((maxV-minV) >> 8) >> 24); // :P goofy, will replace
}