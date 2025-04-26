#include <stdint.h>
#ifndef TIMER_H_
#define TIMER_H_

void initTimer();
uint32_t tinymillis();
uint32_t adjust_clock(uint32_t nudge);
#endif /* TIMER_H_ */