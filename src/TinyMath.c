#include <stdio.h>
#include "../include/bhmADC.h"
#include "../include/TinyMath.h"
#include <inttypes.h>
#define EXP_MAX_INPUT  ((int64_t)43 * (1LL << 26))
#define LN2_FIX        ((int64_t)46516319)
#define INV_LN2_FIX    ((int64_t)96945817)

uint64_t abs(int64_t val){
	uint64_t temp = val >> 63;     // make a mask of the sign bit
	val ^= temp;                   // toggle the bits if value is negative
	val += temp & 1;
	return val;
}

uint32_t fact(uint8_t x){
	uint32_t accum = 1;
	while(x > 0){
		accum *= x;
		x--;
	}
	return accum;
}

int64_t expfix(int64_t val){
	if (val >= EXP_MAX_INPUT)  return INT64_MAX;
    if (val <= -EXP_MAX_INPUT) return 0; // underflows to ~0 in Q26
	
	// Maclaurin series expansion for e^x
	// --- Clamp to prevent guaranteed overflow ---	
	if (val < 0) { // handles negatives with 1/e^x identity
        int64_t pos = expfix(-val);
        if (pos == 0) return INT64_MAX; // guard div/0
        // 1/pos in Q26: fix(1)^2 / pos
        return ((int64_t)(1LL << 26) * (1LL << 26)) / pos;
    }
	
	volatile uint16_t N = (10 + (abs(val) >> 26)*5); // iteration scaling
	volatile int64_t mod = val;
	volatile int64_t accum = val + fix(1);
	for(uint16_t i = 2; i < N; i++){
		if (mod == 0){
			break;
		}
		mod = (mod*(val/i)) >> 26;
		accum += mod;
	}
	return accum;
}

// clay s. turner log base 2
int64_t log2fix (int64_t x){
	int32_t b = fix(1) >> 1; // should start shifted right 1 place of the base
	int64_t y = 0;

	if (x == 0) {
		return INT32_MIN; // represents negative infinity
	}
	while (x < fix(1)) { // normalizing to 1 <= x
		x <<= 1;
		y -= fix(1);
	}
	while (x >= fix(2)) { // normalizing to x < 2
		x >>= 1;
		y += fix(1);
	}

	int64_t z = x;

	for (size_t i = 0; i < 26; i++) {
		z = z * z >> 26;
		if (z >= fix(2)) {
			z >>= 1;
			y += b;
		}
		b >>= 1;
	}
	return y;
}

// natural log, fixed point
int64_t logfix (int64_t x){
	int64_t t;
	t = log2fix(x) * INV_LOG2_E_Q1DOT31; // gave this number more bit depth Q1.31
	return t >> 31;
}

int64_t powerfix(int64_t val, int64_t power){
 // Handle edge cases
 if (val <= 0) return 0;   // log undefined
 if (power == 0) return fix(1);
 volatile uint64_t temp = logfix(val);
 temp = fixed_point_mult(temp, power);
 temp = expfix(temp);
 return temp;
}