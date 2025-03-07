#include "../include/bhmUtils.h"
#include "../include/bhmADC.h"
#include "../include/TinyADC.h"
#include "../include/TinyTWI.h"
#include "../include/bhmDisplay.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Note: to avoid floating point numbers, using fixed point math. 32-bit unsigned int was used to preserve accuracy (and conserve memory)
// TODO: consider uint64_t, operations had to have their accuracy pre-halved to avoid overflows. (using 10^4 instead of 10^8)
// though it should be noted, this does seem to give at least 2 decimal places free of quantization error.

uint8_t reps = 100;

void bhm_init(){
	DIDR0 |= (1 << ADC3D);
}

uint32_t measure_vcc_voltage(){
	// UINT32_MAX = 4,294,967,295
	// first measuring Vbg, to calculate vref and apply correction (should be ~5v but this step corrects for some inaccuracy)
	adc_pin_select(ADC_REF);
	_delay_ms(3);

	// Note: VREF is still Vcc, so the measured value of the 1.1 bandgap ref will inform on Vcc's deviation from 5v
	// After switching to internal voltage reference the ADC requires a settling time of 1ms before measurements are stable.
	// Conversions starting before this may not be reliable. The ADC must be enabled during the settling time.
	
	uint32_t accum = 0; // accumulator for averaging
	uint8_t i = reps;
	while(i){
		accum += adc_read(SLEEP);
		i--;
	}
	accum /= reps;
	return 110000000/(accum-1)*1024; // 1.1*1e8 measure Vcc, so error can be determined
}

uint32_t measure_battery_voltage(){
    uint32_t Vcc = measure_vcc_voltage();
	uint8_t i = reps;
	// measure test point, ADC3_PB3
	adc_pin_select(ADC3_PB3);
	_delay_ms(1);
	uint32_t accum = 0; // accumulator for averaging
	i = reps;
	while(i){
		accum += adc_read(SLEEP);
		i--;
	}
	accum /= reps;
	
	// (1023+1)*(5*100000000/1024)/1000000*(1100000/(~225+1)*1024)/5
	// Apply error correction
	uint32_t val = ((accum+1)*qaunta)/1000000*(Vcc/100)/5 + Vcc;
	
	; //Vcc offset at the op-amp
	return val;
}

uint32_t measure_current_draw(){
	uint32_t Vcc = measure_vcc_voltage();
	uint8_t i = reps;
	// measure test point
	adc_pin_select(ADC2_PB4);
	_delay_ms(1);
	uint32_t accum = 0; // accumulator for averaging
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

uint8_t basic_charge_estimate(uint32_t voltage){
	uint16_t tmpval = voltage / (fixed_point_1e8/100);
	int i = 0;
	for(;i < 18;i++){
		if(cellV_LUT[i] <= tmpval)
		return (100-5*i);
	}
}

uint8_t adv_charge_estimate(uint32_t voltage, uint32_t current, uint64_t *estimated_energy){
	uint8_t rough_percentage_est = 0;
	uint64_t tmpEst = estimated_energy;
	
	// if there's more than 100mA draw
	if(current > 1 * (fixed_point_1e8/10)){
		
	}else{
		rough_percentage_est = basic_charge_estimate(voltage);
		
		// estimate remaining energy from basic charge estimate
		tmpEst = Ah/fixed_point_1e8*rough_percentage_est;
		
		//if close, keep current estimate
		
		//if far (greater than 2.5% distance) readjust current estimated energy based on voltage
	}
	
	
	// compare current energy estimate with estimate based on  
	
	// refine estimate
	return rough_percentage_est;
}