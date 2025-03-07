#ifndef ADC_SCALE_H
#define ADC_SCALE_H
#include <stdint.h>
#define cell_num 2 // change to reflect specific battery parameters
#define fixed_point_1e8 (uint32_t)100000000
extern uint32_t qaunta;
extern uint64_t Ah;
extern uint16_t cellV_LUT[18];

#endif