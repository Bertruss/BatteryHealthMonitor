#ifndef ADC_SCALE_H
#define ADC_SCALE_H
#include <stdint.h>
#define cell_num 2 // change to reflect specific battery parameters
// fixed point set at 27th bit. 6-bits of integer, 26-bits for fractional values. Minimum real resolution of ~1.5e-8. Integer range from 63-0.
#define fixed_point (uint32_t) 0x04000000
extern uint32_t qaunta;
extern uint64_t Ah;
extern uint16_t cellV_LUT[21];
extern uint32_t amp_per_v;
#endif