#include <stdint.h>
#include "../include/bhmADC.h"
#include "../include/TinyMath.h"
uint32_t qaunta = fix(5)/1024; // Volts per step,  with 5v Vcc
uint64_t Ah = fix(2)*3600; // should be set to match the storage capacity of the battery, in Ah, converted to As
	// sense resistor gain is 11, sense resistor is .01R
	// V = 11*.01*I
	// I = V/(11*.01)
uint32_t amp_per_v = fix(1)/11*100;


