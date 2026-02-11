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

// Current threshold settings for switching to coulomb counting
uint16_t mA_thresh = 500;

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
	adjust_clock(10); // tuned for 44 rep, 10ms lost from clock during ADC sleep calls
	
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
	voltage = fix(42)/10; //debug
	int64_t charge_percent = powerfix((uint64_t)fixed_point_div(voltage,(fix(37)/10*cell_num)), fix(80));
	charge_percent = powerfix((uint64_t)(fix(1)+charge_percent),(uint32_t)(fix(165)/1000));
	charge_percent = (uint64_t)fix(123) - fixed_point_div((uint64_t)fix(123),(uint32_t)charge_percent);
	charge_percent = (charge_percent/fixed_point + 1);
	if (charge_percent >= (int64_t)99 || charge_percent < (int64_t)0){
		return (uint8_t)100;
	} else {
		return (uint8_t)(charge_percent);
	}
}

// When current is near 0, assume that the typical discharge characteristics hold. When under sustained load, 
// estimate charge reduction based on discharged energy
uint8_t adv_charge_estimate(uint32_t *voltage, uint32_t *current, uint64_t *estimated_charge, uint32_t timestep){
	uint8_t rough_percentage_est = 0;
	uint64_t tmpEst = 0;
	uint64_t power = 0;
	// if there's more than 150mA draw
	if(current > (fix32(50)/100)){
		*voltage = voltage_mem; // restore last reading
		
		// timestep is recorded in ms, so *1000
		*estimated_charge = *estimated_charge - *current*timestep; //mAms calculation
		
		// calculate charge percentage
		rough_percentage_est = fixed_point_div(*estimated_charge, mAms_1percent);
	}else{
		voltage_mem = *voltage; // save voltage measurement
		rough_percentage_est = calculate_charge(voltage_mem);
		
		// estimate remaining energy from basic charge estimate
		tmpEst = mAms_1percent*rough_percentage_est;
		
		uint64_t temp;
		//if close, keep current estimate
		if(tmpEst > *estimated_charge){
			temp = (tmpEst - *estimated_charge);
		}else{
			temp = (*estimated_charge - tmpEst);
		}
		
		const uint64_t thresh = mAms/100*fix(1);
		//if far (greater than 2.5% distance) readjust current estimated mAhrs based on voltage
		if (temp > thresh){
		*estimated_charge = tmpEst;
		}
	}
	
	
	// compare current energy estimate with estimate based on  
	
	// refine estimate
	return rough_percentage_est;
}