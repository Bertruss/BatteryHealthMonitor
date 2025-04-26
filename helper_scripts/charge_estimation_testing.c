#include <stdio.h>
#include "../include/bhmADC.h"
#include <inttypes.h>
//Q6.26
#define fixed_point 0x04000000
#define fixed_point_convert(x) (x << 26)
#define fixed_point_mult(x, y) ((x>>7)*(y>>7)) >> 12
#define fixed_point_div(x, y)  (x << 26)/y
#define e (uint32_t)0xADF8545
#define INV_LOG2_E_Q1DOT31  UINT64_C(0x58b90bfc) // Inverse log base 2 of e

struct fraction{
	uint32_t N;
	uint32_t D;
};

div_fix(uint64_t x, uint64_t y){
	uint64_t a = x;
	uint64_t b = y;
	while(b != 1 && a != b){
		if(a > b){
			a = a - b;
		}else{
			b = b - a;
		}
	}	
	x /= a;
	y /= a;
	x = x << 26; 	
	y = y << 26;
	return fixed_point_div(x, y);
}

uint64_t root(uint64_t val, uint32_t p);
uint64_t pow_fractional(uint64_t val, struct fraction p);
uint64_t power(uint64_t val, struct fraction pow);

float TESTING;

uint64_t abs(int64_t val){
	uint64_t temp = val >> 31;     // make a mask of the sign bit
  	val ^= temp;                   // toggle the bits if value is negative
  	val += temp & 1;
	return val; 
}

uint32_t least_factor(uint32_t val){
    if(val == 1){
        return 1;
	}else{
        uint32_t test_val = 2;
        if(val%test_val != 0){
            test_val = test_val+1;
            while(val%test_val != 0){
                test_val = test_val+2;
			}
		}
        return test_val;
	}
}

//fixed point power calculation
uint64_t pow(uint64_t val, uint64_t p){
	printf("decimal power\n");
	if(p == fixed_point)
		return val;
	// separate integer and fractional power
	uint64_t D;
	uint64_t N;
	//printf("N = %u\n",val);
	// if fractional, determine numerator and denominator of fractional power
	uint8_t shift = 0;
	while(p%fixed_point !=0 && shift < 11){
		shift++;
		p = p << 1; // essentially, shifting out the fractional value
		printf("test\n");
	}
	D = (uint32_t)1 << shift;
	struct fraction power;
	power.N = p/fixed_point;
	power.D = D;

	return pow_fractional(val, power);
}

uint64_t pow_fractional(uint64_t val, struct fraction p){
	printf("fractional power\n");
	uint64_t accum = val; //large variable for multiplication accumulation
	
	//stupid edge case
	if(p.N == 0){
		return 1;	
	} else{
		uint64_t accum = val;
	}

	//Simplify fraction using GCF
	uint32_t a = p.N;
	uint32_t b = p.D;
	while(b != 1 && a != b){
		if(a > b){
			a = a - b;
		}else{
			b = b - a;
		}
	}	

	p.D = p.D/b;
	p.N = p.N/b;
	
	printf("N = %u\n",p.N);
	printf("D = %u\n",p.D);
	
	accum = power(val, p);
	return accum;
}

//whole number root calculation
uint64_t root(uint64_t val, uint32_t p){
	printf("ROOT CALCULATION\n");
	//Newton-Raphson method
	uint64_t t_val = val;
	uint64_t p_val = p;
	int64_t test_val;
	int64_t derivative;
	int64_t guess_val = val/p; //initial guess of half the base + 1
	uint64_t temp_root;
	uint64_t temp_power;
	

	TESTING = (1.0*guess_val)/(1.0*fixed_point);
	printf("first guess = %f\n",TESTING);
	
	
	do{
		temp_root = least_factor(p);
		p = p/temp_root;
		temp_root = fixed_point_convert(temp_root);
		
		TESTING = (1.0*temp_root)/(1.0*fixed_point);
		printf("temp_root = %f\n",TESTING);

		printf("remaining = %u\n", p);
		uint8_t i = 0;
		do{
			i++;
			test_val = pow(guess_val, temp_root) - val;
			TESTING = (1.0*val)/fixed_point;
			printf("val = %f, ",TESTING);
			TESTING = (1.0*guess_val)/fixed_point;
			printf("guess_val = %f, ",TESTING);
			TESTING = (1.0*test_val)/(1.0*fixed_point);
			printf("error = %f\n",TESTING);

			derivative = pow(guess_val,temp_root-fixed_point);
			derivative = fixed_point_mult(temp_root, derivative);
			derivative = fixed_point_div(test_val,derivative);
			
			guess_val = guess_val - derivative;
			if(i > 3){
				printf("end in root\n");
				TESTING = (1.0*abs(test_val))/(1.0*fixed_point);
				printf("error = %f\n",TESTING);
				TESTING = (1.0*fixed_point/1000)/(1.0*fixed_point);
				printf("error = %f\n",TESTING);
				return 0;
			}
			}while(abs(test_val) > fixed_point/1000);
		val = guess_val;
		printf("ROOOOOOOOOOOOOOT\n");
		TESTING = (1.0*test_val)/(1.0*fixed_point);
		printf("error = %f\n",TESTING);
	}while(p != 1);
	
	TESTING = (1.0*t_val)/fixed_point;
	printf("original val = %f\n",TESTING);
	printf("root = %u\n",p_val);
	TESTING = (1.0*guess_val)/fixed_point;
	printf("guess_val = %f\n",TESTING);
	return guess_val;
}

//whole number root calculation
uint64_t power(uint64_t val, struct fraction powp){
	printf("POWER CALCULATION\n");
	//Newton-Raphson method
	uint64_t t_val = val;
	uint64_t temp;
	uint64_t p = powp.D;
	uint64_t numerator = powp.N;
	uint64_t p_val = powp.D;

	int64_t test_val;
	int64_t derivative;
	int64_t guess_val = val/p; //initial guess of half the base + 1
	uint64_t temp_root;
	uint64_t temp_power;
	

	TESTING = (1.0*val)/(1.0*fixed_point);
	printf("value = %f\n",TESTING);
	printf("initial root = %llu\n",p);
	printf("initial power= %llu\n",numerator);
	
	
	do{
		// I call this the power/root juggle
		if(numerator != 1 && (val < 2*fixed_point || p == 1)){
			printf("power raising **************\n");
			temp_power = least_factor(numerator);
			TESTING = (1.0*val)/(1.0*fixed_point); //debug
			printf("power raise: %f ^ %u = ",TESTING, temp_power); //debug
			temp = val;
			for(uint8_t i = 1; i < (temp_power); i++){
				val = fixed_point_mult(val,temp);
			}
			
			TESTING = (1.0*val)/(1.0*fixed_point); //debug
			printf("%f\n",TESTING); // debug
			numerator = numerator/temp_power;
			printf("new numerator = %llu\n",numerator); // debug
		}

		if(p != 1 && (val > fixed_point || numerator == 1)){
			printf("root testing + + + + + + + + + + + + + + + + + + + + + + +\n");
			temp_root = least_factor(p);
			p = p/temp_root;
			temp_root = fixed_point_convert(temp_root);
			
			printf("Unfactored ROOT= %llu : ", p); //debug
			TESTING = (1.0*temp_root)/(1.0*fixed_point); //debug
			printf("%f \n",TESTING); //debug

			uint8_t i = 0;
			do{
				i++;
				test_val = pow(guess_val, temp_root);
				
				TESTING = (1.0*guess_val)/fixed_point; //debug
				printf("root test: %f^",TESTING); //debug
				TESTING = (1.0*temp_root)/fixed_point; //debug
				printf("%f",TESTING); //debug
				TESTING = (1.0*test_val)/fixed_point; //debug
				printf(" = %f\n",TESTING); //debug
				
				test_val -= val; // debug compress^
				TESTING = (1.0*test_val)/(1.0*fixed_point); //debug
				printf("error = %f\n",TESTING); //debug

								
				derivative = pow(guess_val,temp_root-fixed_point);
				derivative = fixed_point_mult(temp_root, derivative);
				derivative = fixed_point_div(test_val,derivative);
				
				guess_val = guess_val - derivative;
				TESTING = (1.0*guess_val)/(1.0*fixed_point); //debug
				printf("new guess = %f\n",TESTING); //debug
				printf(" __ __ __ __ __ __ __ __ __ __ __ __ __ new iteration\n",TESTING); //debug

				if(i >100){
					printf("end in root\n");
					TESTING = (1.0*abs(test_val))/(1.0*fixed_point);
					printf("error = %f\n",TESTING);
					TESTING = (1.0*fixed_point/1000)/(1.0*fixed_point);
					printf("error = %f\n",TESTING);
					return 0;
				}
			}while(abs(test_val) > fixed_point/1000);
			val = guess_val;
			printf("ROOOOOOOOOOOOOOT\n");
			TESTING = (1.0*test_val)/(1.0*fixed_point);
			printf("error = %f\n",TESTING);
		}
		printf("%llu %llu\n",numerator, p);
	}while(p != 1 || numerator != 1);
	
	TESTING = (1.0*t_val)/fixed_point;
	printf("original val = %f\n",TESTING);
	printf("root = %u\n",p_val);
	TESTING = (1.0*guess_val)/fixed_point;
	printf("guess_val = %f\n",TESTING);
	TESTING = (1.0*val)/fixed_point;
	printf("returned val = %f\n",TESTING);
	return val;
}

uint64_t ln(uint64_t val){
	TESTING = (1.0*val)/(1.0*fixed_point);
	printf("value = %f\n",TESTING);
	//Newton-Raphson method
	int64_t test_val;
	int64_t derivative;
	int64_t guess_val = val/10; //initial guess of half the base + 1
	uint64_t temp_root;

	int i = 0;
	do{
		i++;
		printf("ln test\n");
		test_val = pow(e, guess_val) - val;
		TESTING = (1.0*test_val)/(1.0*fixed_point);
		printf("error = %f\n",TESTING);
		derivative = pow(e,guess_val-fixed_point);
		derivative = fixed_point_mult(temp_root, derivative);
		derivative = fixed_point_div(test_val,derivative);
		
		guess_val = guess_val - derivative;
		TESTING = (1.0*guess_val)/fixed_point;
		printf("guess_val = %f\n",TESTING);
		if(i > 2){
			return 0;
		}
	}while(abs(test_val) > fixed_point/1000);
	val = guess_val;

	
	return guess_val;
}





#define fixed_point 0x04000000
#define fixed_point_convert(x) (x << 26)
#define fixed_point_mult(x, y) ((x>>7)*(y>>7)) >> 12
#define fixed_point_div(x, y)  (x << 26)/y
#define e (uint32_t)0xADF8545
#define INV_LOG2_E_Q1DOT31  UINT64_C(0x58b90bfc) // Inverse log base 2 of e

uint64_t exp(uint32_t val){
	// Maclauren series expansion e^x
	uint8_t N = 2 + (val >> 26)*5; // iteration check
	N = N < 10 ? 10 : N; 
	uint64_t mod = val;
	uint64_t accum = (uint64_t)1*fixed_point + mod;
	for(uint8_t i = 2; i < N; i++){
		mod = (fixed_point_mult( mod, val))/i;
		accum += mod;
	}
	return accum;
}

// clay s. turner
int32_t log2fix (uint32_t x)
{
    int32_t b = 1U << (25);
    int32_t y = 0;

    if (x == 0) {
        return INT32_MIN; // represents negative infinity
    }

	// normalizing to 1 <= x
    while (x < 1U << 26) {
        x <<= 1;
        y -= 1U << 26;
    }

	// normalizing to x < 2
    while (x >= 2U << 26) {
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


int32_t logfix (uint32_t x){
    uint64_t t;
    t = log2fix(x) * INV_LOG2_E_Q1DOT31;
    return t >> 31;
}


int main(){
	uint64_t val = ((uint64_t)7123*fixed_point)/1000;
	uint64_t val2 = ((uint64_t)3342430*fixed_point);
	
	struct fraction poer;//.165
	poer.N = 3;
	poer.D = 20;

	TESTING = (1.0*div_fix(val, val2))/(1.0*fixed_point);
	printf("div test 124352/334243 = %f\n",TESTING);
	TESTING = (1.0*val)/(1.0*fixed_point);
	printf("value = %f\n",TESTING);
	TESTING = (1.0*exp(val))/(1.0*fixed_point);
	printf("value = %f\n",TESTING);
	
	//val = pow_fractional(val, power);
    //printf("%u\n",val);
}