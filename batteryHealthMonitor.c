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
    //cli(); // global interrupt disable
    twi_init(); // Initialize i2c
    adc_init(); // Configure ADC
    SSD1306_init(); // Setup the screen
	SSD1306_clear(); // Clear the screen mem
	set_sleep_mode(SLEEP_MODE_IDLE);
	configure_WDT_interrupt(); // configure timer based interrupt to wake sleep
	adc_pin_select(ADC3_PB3);
	
	while(1){
        //sleep_cpu();
		//Use the ADC noise canceler?
        uint32_t voltage = measure_battery_voltage();
        //uint8_t percent = battery_charge(voltage);
        //bool batt_warn = percent > 15 ? false : true; // below 15% trigger battery warning
		//calculate battery percentage estimate
        //calculate time-to-empty
        //update screen
		// ideas:
        // measure cell voltage O
        // transmit "bat low" if needed O
        // low power shut-off? X
        // low power beep? X
        // sleep for 500ms? update screen O
		//update_display(100, false, voltage);
		_delay_ms(100);
	}
}

void WDT_interrupt_enable(){
	WDTCR |= (1 << WDIE); // enable the watchdog timer interrupt for sleep wake-up
}

void WDT_interrupt_disable(){
	WDTCR &= !(1 << WDIE); // enable the watchdog timer interrupt for sleep wake-up
}

void configure_WDT_interrupt(){
	// consider configurable sleep cycle
    // PRR |= () //consider further power reduction
    WDTCR |= (1 << WDP1) | 
			(1 << WDP2);
			
}

// triggers sleep mode with WDT triggered wake 
void sleep_pause(){
	sei();
	WDT_interrupt_enable();
	sleep_enable();
	sleep_mode();
}
