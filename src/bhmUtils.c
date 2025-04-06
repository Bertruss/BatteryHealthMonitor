#include "../include/bhmUtils.h"
#include "../include/bhmADC.h"
#include "../include/TinyADC.h"
#include "../include/TinyTWI.h"
#include "../include/bhmDisplay.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Note: to avoid floating point numbers, using fixed point math. 32-bit unsigned int was used to preserve accuracy (and conserve memory)

const uint8_t reps = 50; //number of measurements taken for smoothing
const uint32_t losses_offset = 5;

void bhm_init(){
	// Digital Input Buffer Disable. important, as these will be used as analog inputs. Apparently this mostly just reduces power consumption. 
	DIDR0 |= (1 << ADC3D);
	DIDR0 |= (1 << ADC2D);
}

uint32_t measure_vcc_voltage(){
	return bhm_adc_read(ADC_REF);
}

uint32_t measure_battery_voltage(){
	return bhm_adc_read(ADC3_PB3);
}

uint32_t measure_current_draw(){
	uint32_t val = bhm_adc_read(ADC2_PB4);
	val = val >= 24*fixed_point/1000? val - 30*fixed_point/1000 : 0; //compensating for relatively constant offset
	
	//uint64_t test = (uint32_t)((val >> 4)*(amp_per_v >> 4));
	uint32_t test = (((uint64_t)val*(uint64_t)amp_per_v)/(uint64_t)fixed_point);
	return test;
}

uint32_t bhm_adc_read(enum adc_pin select){
	adc_pin_select(select);
	if(select == ADC_REF)
		_delay_ms(3);
	else
		_delay_ms(1);
	
	uint32_t accum = 0; // accumulator for averaging
	uint8_t i = reps;
	while(i){
		accum += adc_read(SLEEP);
		i--;
	}
	accum /= reps;
	
	if(select == ADC_REF){ // Measure Reference
		// Note: VREF is still Vcc, so the measured value of the 1.1 bandgap ref will inform on Vcc's deviation from 5v
		// After switching to internal voltage reference the ADC requires a settling time of 1ms before measurements are stable.
		// Conversions starting before this may not be reliable. The ADC must be enabled during the settling time.
		return 11*(fixed_point/10)/(accum)*1024; // 1.1*1e8 with 5v Vcc compared to measured 1.1v value, so error can be determined
	}
	else{
		// measure reference Vcc
		uint32_t Vcc = bhm_adc_read(ADC_REF);
		
		// Apply error correction
		uint32_t val = ((accum+1)*qaunta)/(fixed_point/0x80)*(Vcc/0x80)/5;
		
		if(select == ADC3_PB3){ // Measure Voltage
			return val + Vcc;
		}
		return val;
	} 
}

//fixed point power calculation
uint32_t pow(uint32_t val, uint32_t p){
	uint64_t accum; //large variable for multiplication accumulation
	if(p == 0){
		return 1;	
	} else{
		uint64_t accum = val;
	}
	
	// separate integer and fractional power
	uint32_t frac =  p%fixed_point;
	uint32_t power = p-frac;
	
	if(frac != 0){ // if fractional, determine numerator and denominator of fractional power
		const uint16_t scaler = 100000; // scaling to increase accuracy of fractional approximation of power
		power =  (power/frac + fixed_point)*scaler; //numerator
		frac = ((fixed_point*scaler)/frac); //denominator
	}
	
	// 
	for(uint8_t i = 1; i < power; i++){
		accum += fixed_point_div(accum*val);
	}
	
	if(frac != 0){
		accum = root(accum, frac);
	}
	return accum;
}

//fixed point root calculation
uint32_t root(uint64_t val, uint32_t p){
	//unfortunately, newtons method
	uint32_t root_val = 0;
	uint64_t error_val = 0; //difference
	uint64_t test_val = 0; //
	uint32_t guess_val = fixed_point; //initial guess of 1
	
	while(error_val > fixed_point/10000){
				test_val = 
	}
	}
}

//
uint8_t basic_charge_estimate(uint32_t voltage){
	uint16_t tmpval = voltage / (fixed_point/100);
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
	if(current > 1 * (fixed_point/10)){
		(15 * (fixed_point/10)); //
	}else{
		rough_percentage_est = basic_charge_estimate(voltage);
		
		// estimate remaining energy from basic charge estimate
		tmpEst = Ah/fixed_point*rough_percentage_est;
		
		//if close, keep current estimate
		
		//if far (greater than 2.5% distance) readjust current estimated energy based on voltage
	}
	
	
	// compare current energy estimate with estimate based on  
	
	// refine estimate
	return rough_percentage_est;
}