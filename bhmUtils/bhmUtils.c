#include "bhmUtils.h"
#include "ADCscale.h"
#include "..\TinyADC\TinyADC.h"
#include "..\TinyTWI\TinyTWI.h"
#include "bhmDisplay.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Note: to avoid floating point numbers, using fixed point math. 32-bit unsigned int was used to preserve accuracy (and conserve memory)
// TODO: consider uint64_t, operations had to have their accuracy pre-halved to avoid overflows. (using 10^4 instead of 10^8)
// though it should be noted, this does seem to give at least 2 decimal places free of quantization error.

void bhm_init(){
	DIDR0 |= (1 << ADC3D);
}

// Averaging?
uint32_t last_measure = 0;
uint32_t measure_battery_voltage(){
    // UINT32_MAX = 4,294,967,295
	// first measuring Vbg, to calculate vref and apply correction (should be ~5v but this step corrects for some inaccuracy)
	adc_pin_select(ADC_REF);
	_delay_ms(3);

	// Note: VREF is still vcc, so the measured value of the 1.1 ref will inform on Vcc's deviation from 5v
	// After switching to internal voltage reference the ADC requires a settling time of 1ms before measurements are stable.
	// Conversions starting before this may not be reliable. The ADC must be enabled during the settling time.	
	
	uint32_t accum = 0; // accumulator for averaging
	uint8_t reps = 100; 
	uint8_t i = reps;
	while(i){
		accum += adc_read(SLEEP);
		i--;
	}
	accum /= reps;
	uint32_t Vcc = 1100000/(accum+1)*1024; // 1.1*10e7 measure Vcc, so error can be determined
	
	// measure test point
	adc_pin_select(ADC3_PB3);
	_delay_ms(1);
	accum = 0; // accumulator for averaging
	i = reps;
	while(i){
		accum += adc_read(SLEEP);
		i--;
	}
	accum /= reps;
	
	// Apply error correction
	uint32_t val = ((accum+1)*qaunta)/1000000*Vcc/5;
	return val;
}

// TODO: placeholder linear approximation of charge from voltage. very innaccurate
uint8_t battery_charge(uint32_t voltage){
    // "open circuit voltage" battery charge
    //4.2 - 3.6 90%
    //3.6 - 3.2 10%
    return ((((voltage - minV) >> 8)*100)/((maxV-minV) >> 8) >> 24); // :P goofy, will replace
}