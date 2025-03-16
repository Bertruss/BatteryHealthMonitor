#include "../include/fcpu.h"
#include "../include/TinyTWI.h"
#include "../include/TinyADC.h"
#include "../include/bhmDisplay.h"
#include "../include/bhmUtils.h"
#include "../include/SSD1306_driver.h"
#include <stdbool.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "../include/bhmADC.h"

ISR(WDT_vect){ // toggle off the interrupt
	// watchdog timer interrupt disables automatically
    // sleep is disabled on wake via watchdog
}

int main(){
    cli(); // global interrupt disable
	//low_speed();
    twi_init(); // Initialize i2c
    adc_init(); // Configure ADC
	bhm_init(); // bhm specific configurations
    SSD1306_init(); // Setup the screen
	SSD1306_clear(); // Clear the screen mem
	configure_WDT_interrupt(); // configure timer based interrupt to wake sleep
	sei();
	
	// Battery charge estimation 
	uint64_t total_charge_estimate = Ah; // assumption on bootup is that we start from full charge
	
	while(1){
        uint32_t voltage = measure_battery_voltage();
		uint32_t current = measure_current_draw();
        uint8_t percent = basic_charge_estimate(voltage);
		//uint8_t percent = adv_charge_estimate(voltage, current, &total_charge_estimate);
        bool batt_warn = percent > 15 ? false : true; // below 15% trigger battery warning
		//calculate battery percentage estimate
        //calculate time-to-empty
        //update screen
		// ideas:
        // measure cell voltage O
        // transmit "bat low" if needed O
        // low power shut-off? X
        // low power beep? X
        // sleep for 500ms? update screen O
		update_display(percent, batt_warn, voltage, current);
		//sleep_pause(); // 2s sleep pause
	}
}

void WDT_interrupt_enable(){
	 // enable the watchdog timer interrupt for sleep wake-up
	 WDTCR |= (1 << WDIE);
}				

void WDT_interrupt_disable(){
	WDTCR &= ~(1 << WDIE); // enable the watchdog timer interrupt for sleep wake-up
}

void configure_WDT_interrupt(){
	// TODO: consider configurable sleep cycle
    // TODO: PRR |= () //consider further power reduction
	// WDP3 WDP2 WDP1 WDP0
    WDTCR |= (1 << WDP1) | // 2 seconds
			(1 << WDP2);
}

void low_speed(){
	CLKPR = 0x80; // enable clk prescaler manipulation
	//(1 << CLKPCE) & !(1 << CLKPS3) & !(1 << CLKPS2) & !(1 << CLKPS1) & !(1 << CLKPS0);
	CLKPR |= (1 << CLKPS2); // | (1 << CLKPS0);
}

// triggers sleep mode with WDT triggered wake 
void sleep_pause(){
	cli();
	WDT_interrupt_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei();
	sleep_enable();
	sleep_cpu(); //start slept conversion
	sleep_disable(); // wake
	WDT_interrupt_disable();
	cli();
}
