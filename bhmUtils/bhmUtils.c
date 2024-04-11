#include "bhmUtils.h"
#include "ADCscale.h"
#include "..\TinyADC\TinyADC.h"
#include <stdint.h>

// Note: to avoid floating point numbers, 32 bit unsigned int was used to preserve accuracy.
// TODO: consider uint64_t, operations had to have their accuracy pre-halved to avoid overflows. (using 10^4 instead of 10^8)
// though it should be noted, this does seem to give at least 2 decimal places free of quantization error.

uint32_t measure_battery_voltage(){
    // measuring Vcc to apply Vref correction (should be ~5v but this step corrects for some innaccuracy)
    uint32_t ref = 1.1*10000/adc_measure_ref()*1024;
    
    // Apply error correction and reference offset

    // Note: quanta is just hard-coded 5/1024
    // ref SHOULD equal current Vcc, so the error correction modifier is the ref/5 
    uint32_t val = (adc_read_sync()*qaunta)/10000/5*ref + ref;

    return val;
}

uint8_t battery_percentage(uint32_t voltage){
    // TODO: battery discharge is only linearish down to 3.6v... 
    return (maxV/voltage);
}