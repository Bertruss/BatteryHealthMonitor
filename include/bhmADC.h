#ifndef ADC_SCALE_H
#define ADC_SCALE_H
#include <stdint.h>
#define cell_num 2 // change to reflect specific battery parameters
// fixed point set at 27th bit. 6-bits of integer, 26-bits for fractional values. Minimum real resolution of ~1.5e-8. Integer range from 63-0.
// also, multiply and divide can be replaced with bitwise shift
#define fixed_point (uint32_t) 0x04000000
#define fixed_point_div(x) (x >> 26);
#define fixed_point_mult(x)  (x << 26);

extern const uint32_t qaunta;
extern const uint64_t Ah;
extern const uint16_t cellV_LUT[21];
extern const uint32_t amp_per_v;
#endif