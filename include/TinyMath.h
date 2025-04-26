#ifndef TINYMATH_H_
#define TINYMATH_H_

#define fixed_point 0x04000000
#define fix(x) ((uint64_t)x << 26)
#define fix32(x) ((uint32_t)x << 26) 
#define fixed_point_div(x, y)  (x << 26)/y
#define fixed_point_mult(x, y) ((x>>8)*(y>>8)) >> 10
#define INV_LOG2_E_Q1DOT31  UINT64_C(0x58b90bfc) // Inverse log base 2 of e

uint64_t abs(int64_t val);
uint64_t expfix(uint64_t val);
int32_t log2fix (uint64_t x);
int32_t logfix (uint64_t x);
int64_t powerfix(uint64_t val, uint64_t power);
uint32_t power_simple(uint32_t val, uint8_t p);
#endif /* INCFILE1_H_ */