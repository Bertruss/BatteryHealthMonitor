#include "../include/bhmUtils.h"
#include "../include/bhmADC.h"
#include "../include/TinyADC.h"
#include "../include/TinyTWI.h"
#include "../include/bhmDisplay.h"
#include "../include/TinyMath.h"
#include "../include/timer.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Note: to avoid floating point numbers, using fixed point math. 32-bit unsigned int was used to preserve accuracy (and conserve memory)

uint8_t reps = 44; //number of measurements taken for smoothing
uint32_t voltage_mem;

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
	
	uint32_t comp = fix32(26)/1000;
	val = val > comp? val - comp : 0; //compensating for relatively constant offset
	
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
	adjust_clock(20); // tuned for 44 rep 
	
	if(select == ADC_REF){ // Measure Reference
		// Note: VREF is still Vcc, so the measured value of the 1.1 bandgap ref will inform on Vcc's deviation from 5v
		// After switching to internal voltage reference the ADC requires a settling time of 1ms before measurements are stable.
		// Conversions starting before this may not be reliable. The ADC must be enabled during the settling time.
		return 11*(fixed_point/10)/(accum+1)*1024; // 1.1*1e8 with 5v Vcc compared to measured 1.1v value, so error can be determined
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

// charge_per = 123 - 123./(1+(v./3.7).^80).^0.165
// based on a best fit approximation of the nonlinear charge/voltage relationship
uint8_t calculate_charge(uint64_t voltage){
	uint64_t charge_percent = powerfix((uint64_t)fixed_point_div((voltage),(fix(37)/10*cell_num)), fix(80));
	charge_percent = powerfix((uint64_t)(fix(1)+charge_percent),(uint32_t)(fix(165)/1000));
	charge_percent = (uint64_t)fix(123) - fixed_point_div((uint64_t)fix(123),(uint32_t)charge_percent);
	return (uint8_t)(charge_percent/fixed_point + 1);
}


uint8_t adv_charge_estimate(uint32_t *voltage, uint32_t current, uint64_t *estimated_energy){
	uint8_t rough_percentage_est = 0;
	int64_t tmpEst = estimated_energy;
	
	// if there's more than 100mA draw
	if(current > (fix32(15)/100)){
		*voltage = voltage_mem; // preserve last reading
		//1.50s
	}else{
		voltage_mem = *voltage;
		rough_percentage_est = calculate_charge(voltage_mem);
		
		// estimate remaining energy from basic charge estimate
		tmpEst = Ah/100*rough_percentage_est;
		
		//if close, keep current estimate
		if(abs(tmpEst - *estimated_energy)){
			
		}
		
		
		//if far (greater than 2.5% distance) readjust current estimated energy based on voltage
	}
	
	
	// compare current energy estimate with estimate based on  
	
	// refine estimate
	return rough_percentage_est;
}