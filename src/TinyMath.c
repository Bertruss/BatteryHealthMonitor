#include <stdio.h>
#include "../include/bhmADC.h"
#include "../include/TinyMath.h"
#include <inttypes.h>

uint64_t abs(int64_t val){
	uint64_t temp = val >> 31;     // make a mask of the sign bit
	val ^= temp;                   // toggle the bits if value is negative
	val += temp & 1;
	return val;
}

uint32_t fixy(uint8_t x){
	return (uint32_t)x << 26;
}

uint64_t expfix(uint64_t val){
	// Maclaurin  series expansion for e^x
	uint8_t N = (10 + (val >> 26)*5); // iteration scaling
	uint64_t mod = val;
	uint64_t accum = (uint64_t)1*fixed_point + mod;
	for(uint8_t i = 2; i < N; i++){
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
	while (x < fixy(1)) { // normalizing to 1 <= x
		x <<= 1;
		y -= fixy(1);
	}
	while (x >= fixy(2)) { // normalizing to x < 2
		x >>= 1;
		y += fixy(1);
	}

	uint64_t z = x;

	for (size_t i = 0; i < 26; i++) {
		z = z * z >> 26;
		if (z >= fixy(2)) {
			z >>= 1;
			y += b;
		}
		b >>= 1;
	}
	return y;
}

int32_t logfix (uint64_t x){
	uint64_t t;
	t = log2fix(x) * INV_LOG2_E_Q1DOT31;
	return t >> 31;
}

int64_t powerfix(uint64_t val, uint64_t power){
	uint64_t temp = (uint64_t)logfix(val);
	temp = fixed_point_mult(temp, power);
	temp = expfix(temp);
	return temp;
}


uint32_t power_simple(uint32_t val, uint8_t p){
	uint32_t out = 1;
	while (p > 0){
		out = val*out;
		p--;
	}
	return out;
}
