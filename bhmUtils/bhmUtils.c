#include "bhmUtils.h"
#include "ADCscale.h"
#include "..\TinyADC\TinyADC.h"
#include <stdint.h>

// Note: to avoid floating point numbers, using fixed point math. 32-bit unsigned int was used to preserve accuracy (and conserve memory)
// TODO: consider uint64_t, operations had to have their accuracy pre-halved to avoid overflows. (using 10^4 instead of 10^8)
// though it should be noted, this does seem to give at least 2 decimal places free of quantization error.

uint32_t measure_battery_voltage(){
    // measuring Vcc to apply Vref correction (should be ~5v but this step corrects for some innaccuracy)
    if(adc_active_pin != ADC3_PB3){
        adc_pin_select(ADC3_PB3);
    }
	
	// NOTE: not sure there is a better way to do this to limit quantization error
    uint32_t ref = 1.1*10000/adc_measure_ref()*1024; // measure Vcc error from 5v. 
    
    // Apply error correction
    // Note: quanta is just hard-coded 5/1024
    // add vcc offset to get 
    uint32_t val = (adc_read(WAKE)*qaunta)/10000*ref/5 + ref;
    return val;
}

uint8_t battery_charge(uint32_t voltage){
    // "open circuit voltage" battery charge
    //4.2 - 3.6 90%
    //3.6 - 3.2 10%
    return 0; //yeah we're not doing this yet
}