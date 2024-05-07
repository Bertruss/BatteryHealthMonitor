#include "./TinyTWI/fcpu.h"
#include "./TinyTWI/TinyTWI.h"
#include "./bhmUtils/bhmDisplay.h"
#include "./bhmUtils/bhmUtils.h"
#include "./SSD1306_driver/SSD1306_driver.h"
#include <avr/io.h>
#include <util/delay.h>

ISR(TIMER0_COMPA){ // toggle off the interrupt
    TIMSK &= !(1 << OCIE0A); // output compare timer0 registerA interrupt disable
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
    # ~.5s timer interrupt #
^loop
*/

int main(){
    cli();
    twi_init(); // Initialize i2c
    adc_init(); // Configure ADC
    SSD1306_init(); // Setup the screen
	SSD1306_clear(); // Clear the screen mem
	sei(); // global interrupt enable

    while(1){
        // Use the ADC noise canceler?
        uint32_t voltage = measure_battery_voltage();
        uint8_t percent_v = battery_percentage(voltage);
        bool batt_warn = percent_v > 15 ? false : true; // below 15% trigger battery warning
		bool batt_warn_display = false;
		
		//calculate battery percentage estimate
        //calculate time-to-empty
        //update screen
        
		// ideas:
        // measure cell voltage O
        // transmit "bat low" if needed O
        // low power shut-off? X
        // low power beep? X
        // sleep for 500ms? update screen O
		//		- Set a timer based interrupt to kick off a measurement every 500ms
        update_display(percent_v, batt_warn, voltage);
    }
}

void configure_timer_interrupt(){
    MCUCR &= !(1 << SM1) &
            !(1 << SM0); // set idle interrupt

    // PRR |= () //consider further power reduction

    TCCR1 |= (1 << CTC1) | // set timer clear on compare match
        (1 << CS13) |
        (1 << CS12) |
        (1 << CS11) |
        (1 << CS10); // Divide by 16384    
    OCR1A = 0xF4; //244; at 8mhz, divided by 16384, roughly a 1/2 second
    TIMSK |= (1 << OCIE0A); // output compare timer0 registerA interrupt enable
}