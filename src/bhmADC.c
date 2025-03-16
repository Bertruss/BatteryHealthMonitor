#include <stdint.h>
#include "../include/bhmADC.h"
uint32_t qaunta = 5*fixed_point/1024; // Volts per step,  with 5v Vcc
uint64_t Ah = 2*fixed_point*3600; // should be set to match the storage capacity of the battery, in Ah, converted to As
// N*10^-2. 100%, down in %5increments.

//https://blog.ampow.com/lipo-voltage-chart/
uint16_t cellV_LUT[21] = {
	420*cell_num, 415*cell_num, 411*cell_num, 408*cell_num, 
	402*cell_num, 389*cell_num, 395*cell_num, 391*cell_num, 
	387*cell_num, 385*cell_num, 384*cell_num, 382*cell_num, 
	380*cell_num, 379*cell_num, 377*cell_num, 375*cell_num, 
	373*cell_num, 371*cell_num, 369*cell_num, 361*cell_num,
	327*cell_num};
	
	// sense resistor gain is 11, sense resistor is .01R
	// V = 11*.01*I
	// I = V/(11*.01)
uint32_t amp_per_v = (fixed_point)/11*100;