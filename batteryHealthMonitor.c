#include "./TinyTWI/fcpu.h"
#include "./TinyTWI/TinyTWI.h"
#include "./TinyADC/TinyADC.h"
#include "./bhmUtils/bhmDisplay.h"
#include "./bhmUtils/bhmUtils.h"
#include "./SSD1306_driver/SSD1306_driver.h"
#include <stdbool.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

ISR(WDT_vect){ // toggle off the interrupt
	// watchdog timer interrupt disables automatically
    // sleep is disabled on wake via watchdog
}

/*
Program cycle:
**start**
    [ADC low-power mode]
        |
        [measure voltage]
        |
        V
    # ADC complete flag interrupt #
    [calculate percentage]
    [check for low batt]
    [update screen]
    [Deep Sleep]
        |
        |
        V
    # ~1s timer interrupt #
^loop
*/

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
	while(1){
        uint32_t voltage = measure_battery_voltage();
        uint8_t percent = battery_charge(voltage);
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
		update_display(percent, batt_warn, voltage);
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
