#include <stdint.h>
#define cell_num 2 // change to reflect specific battery parameters
uint32_t maxV = cell_num*4.2*100000000; // C*4.2*10^8
uint32_t minV = cell_num*3.2*100000000; // C*3.2*10^8
uint32_t qaunta = 5*100000000/1024; // 5*10^8
