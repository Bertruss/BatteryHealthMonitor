#include <stdio.h>
#include "../include/bhmADC.h"

uint8_t basic_charge_estimate(uint32_t voltage){
	uint16_t tmpval = voltage / (fixed_point_1e8/100);
	int i = 0;
	for(;i < 18;i++){
		if(cellV_LUT[i] < tmpval)
		return (100-5*i);
	}
}

uint8_t adv_charge_estimate(uint32_t voltage, uint32_t current, uint64_t *estimated_energy){
	uint8_t rough_percentage_est = 0;
	uint64_t tmpEst;
	if(current > 1 * (fixed_point_1e8/10)){
		
	}else{
		rough_percentage_est = basic_charge_estimate(voltage);
	}
	// estimate remaining energy from basic charge estimate
	tmpEst = Ah/fixed_point_1e8*rough_percentage_est; 
	
	// refine estimate
	return rough_percentage_est;
}

int main(){
    uint32_t voltage = 42 * fixed_point_1e8 / 10;
    uint32_t current = 110 * fixed_point_1e8 / 100;
    uint64_t estimated_energy = Ah;
    
    while(1){

        estimated_energy
        printf("%u", )
    }

}