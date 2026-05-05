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

const int32_t cutoffs[6] = {327, 369, 371, 379, 385, 398}; // 415

int8_t sign[24] = {
1,1,1,
-1,1,1,
1,-1,1,
-1,1,1,
1,-1,1,
-1,-1,1};

int32_t integ[24] = {
58,      31,     0,    0,
480008,  19200,  57,   10,
5886,    470,    250,  15,
44025,   3402,   287,  35,
2777,    873,    220,  55,
139,     73,     134,  75};


int32_t fract[24] = {
8290104, 9811580, 0, 
7873462, 3514938, 9964850,
1301369, 8904109, 0,      
7034544, 5772148, 6712328,
4414542, 8116006, 5029013,
5056201, 2317392, 1281669};

// based on a best fit approximation of the nonlinear charge/voltage relationship
// there was a sigmoid function here... it's gone now. 
uint8_t calculate_charge(uint64_t voltage){
	
	uint8_t row = 0;
	for(uint8_t i = 0; i < 6; i++){
		if(voltage > fix(cutoffs[i])/100){
			row = i;
		}	
	}
	
	int64_t V = voltage/cell_num-fix(cutoffs[row])/100; // correct voltage based on cell num. Curves are calculated based on 1 cell.
	uint8_t index = row*4;
	uint8_t indexD = row*3;
	int64_t A = sign[indexD]*(fix(integ[index]) + fix(fract[indexD])/10000000);
	int64_t B = sign[1+indexD]*(fix(integ[1+index]) + fix(fract[1+indexD])/10000000);
	int64_t C = sign[2+indexD]*(fix(integ[2+index]) + fix(fract[2+indexD])/10000000);
	int64_t D = fix(integ[3+index]);
	
	int64_t charge_percent = fixed_point_mult(A, fixed_point_mult(V,fixed_point_mult(V,V))); 
	charge_percent += fixed_point_mult(B, fixed_point_mult(V,V)); 
	charge_percent += fixed_point_mult(C, V); 
	charge_percent += (D);
	charge_percent = charge_percent/fixed_point + 1;
	
	if (charge_percent >= (int64_t)99 || charge_percent < (int64_t)0){
		return (uint8_t)100;
	} else {
		return (uint8_t)(charge_percent);
	}
}

// When current is near 0, assume that the typical discharge characteristics hold. 
// When under sustained load, estimate charge reduction based on discharged energy
uint8_t adv_charge_estimate(uint32_t *voltage, uint32_t *current, uint64_t *estimated_charge, uint32_t timestep){
	uint8_t rough_percentage_est = 0;
	uint64_t tmpEst = 0;
	uint64_t power = 0;
	
	
	// if there's more than 150mA draw
	if(*current > (fix32(150)/1000)){
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
		
		
		// Use new remaining charge estimate if there is notable difference between them
		uint64_t temp;
		if(tmpEst > *estimated_charge){
			temp = (tmpEst - *estimated_charge);
		}else{
			temp = (*estimated_charge - tmpEst);
		}
		
		const uint64_t thresh = mAms/100; // 1 percent threshold
		//if far (greater than 1% distance) readjust current estimated mAhrs based on voltage
		if (temp > thresh){
		*estimated_charge = tmpEst;
		}
	}
	
	
	// compare current energy estimate with estimate based on  voltage
	
	// refine estimate
	return rough_percentage_est;
}