#include "../include/fcpu.h"
#include "../include/TinyTWI.h"
#include "../include/TinyADC.h"
#include "../include/bhmDisplay.h"
#include "../include/bhmUtils.h"
#include "../include/TinyMath.h"
#include "../include/SSD1306_driver.h"
#include "../include/bhmADC.h"
#include  "../include/timer.h"

#include <stdbool.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

volatile uint8_t wdtimer_int_trig;

ISR(WDT_vect){ // toggle off the interrupt
	// watchdog timer interrupt disables automatically
    // sleep is disabled on wake via watchdog
	wdtimer_int_trig = 1;
}

int main(){
	// DDRB |= (1 << PB1);//debug
	
    cli(); // global interrupt disable
	//low_speed();
	/*
    twi_init(); // Initialize i2c
	SSD1306_init(); // Setup the screen
	SSD1306_clear(); // Clear the screen memory
	timer_init(); // configure timekeeper
	configure_WDT_interrupt((uint8_t)5); // configure timer based interrupt to wake sleep
	*/
    adc_init(); // Configure ADC
	bhm_init(); // bhm specific configurations
	sei();
	
	// Battery charge estimation 
	uint64_t total_charge_estimate = mAms;
	uint8_t percent_charge;
	uint32_t current;
	uint32_t voltage;
	bool batt_warn;
	volatile uint32_t cycle_time = 0;
	volatile uint32_t last_cycle = 0;
	while(1){
        last_cycle = cycle_time;
		//cycle_time = tinymillis(); //Measure the length of each measurement cycle
		current = measure_current_draw();
		voltage = measure_battery_voltage();
		percent_charge = calculate_charge(voltage);
		//percent_charge = adv_charge_estimate(voltage, current, &total_charge_estimate, (cycle_time-last_cycle));
		//batt_warn = percent_charge > 15 ? false : true; // below 15% trigger battery warning
		//calculate time-to-empty
        //update screen
		// ideas:
        // measure cell voltage O
        // transmit "bat low" if needed O
        // low power shut-off? X
        // low power beep? X
        // sleep for 500ms? update screen O
		update_display(percent_charge, 0, voltage, current);
		sleep_pause();	
	}
}

void WDT_interrupt_enable(){
	 // enable the watchdog timer interrupt for sleep wake-up
	 WDTCR |= (1 << WDIE);
}				

void WDT_interrupt_disable(){
	WDTCR &= ~(1 << WDIE); // enable the watchdog timer interrupt for sleep wake-up
}

void configure_WDT_interrupt(uint8_t opt){
	// TODO: consider configurable sleep cycle
    // TODO: PRR |= () //consider further power reduction
	//WDT prescaler options
	//default (2048) cycles 16 ms
	switch(opt){
		case 0: //(4096) cycles 32 ms
		WDTCR = (1 << WDP0); break;
		case 1: //(8192) cycles 64 ms
		WDTCR = (1 << WDP1); break;
		case 2: //(16384) cycles 0.125 s
		WDTCR = (1 << WDP1) | (1 << WDP0); break;
		case 3: //(32764) cycles 0.25 s
		WDTCR = (1 << WDP2); break;
		case 4: //(65536) cycles 0.5 s
		WDTCR = (1 << WDP2) | (1 << WDP0); break;
		case 5: //(131072) cycles 1.0 s
		WDTCR = (1 << WDP2) | (1 << WDP1); break;
		case 6: //(262144) cycles 2.0 s
		WDTCR = (1 << WDP2) | (1 << WDP1) | (1 << WDP0); break;
		case 7: //(524288) cycles 4.0 s
		WDTCR = (1 << WDP3); break;
		case 8: //(1048576) cycles 8.0 s
		WDTCR = (1 << WDP3) | (1 << WDP0); break;
	}
	WDT_interrupt_enable();
}

void low_speed(){
	CLKPR = 0x80; // enable clk prescaler manipulation
	//(1 << CLKPCE) & !(1 << CLKPS3) & !(1 << CLKPS2) & !(1 << CLKPS1) & !(1 << CLKPS0);
	CLKPR |= (1 << CLKPS2); // | (1 << CLKPS0);
}

// triggers sleep mode with WDT triggered wake 
void sleep_pause(){
	WDT_interrupt_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);
	while(wdtimer_int_trig != 1){
		sleep_enable();
		sleep_cpu();
		sleep_disable();
	}
	wdtimer_int_trig = 0;
	WDT_interrupt_disable();
}
