#include <stdio.h>
#include "../include/bhmADC.h"
#include <inttypes.h>

#define fixed_point 0x04000000
#define fix(x) ((uint64_t)x << 26)
#define fixed_point_div(x, y)  (x << 26)/y
#define fixed_point_mult(x, y) ((x>>8)*(y>>8)) >> 10
#define INV_LOG2_E_Q1DOT31  UINT64_C(0x58b90bfc) // Inverse log base 2 of e

// exponential
uint64_t exp(uint64_t val){
	// Maclaurin series expansion for e^x
	uint64_t mod = val;
	uint64_t accum = (uint64_t)1*fixed_point + mod;
	for(uint8_t i = 2; i < 100; i++){
		mod = (fixed_point_mult( mod, val))/i;
		accum += mod;
	}
	return accum;
}

// clay s. turner log base 2
int32_t log2fix (uint64_t x){
	int32_t b = (uint32_t)1 << 25;
	int32_t y = 0;

	if (x == 0) {
		return INT32_MIN; // represents negative infinity
	}
	while (x < ((uint32_t)1 << 26)) { // normalizing to 1 <= x
		x <<= 1;
		y -= 1U << 26;
	}
	while (x >= ((uint32_t)2 << 26)) { // normalizing to x < 2
		x >>= 1;
		y += 1U << 26;
	}

	uint64_t z = x;

	for (size_t i = 0; i < 26; i++) {
		z = z * z >> 26;
		if (z >= 2U << (uint64_t)26) {
			z >>= 1;
			y += b;
		}
		b >>= 1;
	}
	return y;
}

int32_t logfix (uint64_t x){
    return (uint64_t)(log2fix(x) * INV_LOG2_E_Q1DOT31) >> 31;
}

int64_t power(uint64_t val, uint64_t power){
    return exp(fixed_point_mult((uint64_t)logfix(val), power));
}

//charge_per = 123 - 123./(1+(v./3.7).^80).^0.165
uint64_t calculate_charge(uint64_t voltage){
    uint64_t charge_percent = (uint64_t)fixed_point_div(voltage,(fix(37)/10));
    charge_percent = power(charge_percent, fix(80));
    charge_percent = power((uint64_t)(fix(1)+charge_percent),(uint32_t)(fix(165)/1000));
    charge_percent = (uint64_t)fix(123) - fixed_point_div((uint64_t)fix(123),(uint32_t)charge_percent);
    return charge_percent;
}

uint32_t calculate_charge_simple(uint32_t voltage){
	uint64_t charge_percent = (uint64_t)fixed_point_div(((uint64_t)voltage/2),(fix(37)/10));
	charge_percent = power(charge_percent, fix(80));
	charge_percent = power((uint64_t)(fix(1)+charge_percent),(uint32_t)(fix(165)/1000));
	charge_percent = (uint64_t)fix(123) - fixed_point_div((uint64_t)fix(123),(uint32_t)charge_percent);
	return (uint8_t)(charge_percent/fixed_point + 1);
}

int main(){
    uint8_t test = calculate_charge_simple((uint32_t)(fix(840)/100)); //calculate_charge((fix(42)/10));
	printf("8.4v charge percentage = %u\n",test);
    test = calculate_charge_simple((uint32_t)(fix(800)/100)); //calculate_charge((fix(42)/10));
	printf("8.0v charge percentage = %u\n",test);
    test = calculate_charge_simple((uint32_t)(fix(740)/100)); //calculate_charge((fix(42)/10));
	printf("7.4v charge percentage = %u\n",test);
}