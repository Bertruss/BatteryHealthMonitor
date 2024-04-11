#include "./TinyTWI/fcpu.h"
#include "./TinyTWI/TinyTWI.h"
#include "./bhmUtils/bhmDisplay.h"
#include "./bhmUtils/bhmUtils.h"
#include "./SSD1306_driver/SSD1306_driver.h"
#include <avr/io.h>
#include <util/delay.h>

int main(){
    twi_init(); // Initialize i2c
    adc_init(); // Configure ADC
    SSD1306_init(); // Setup the screen
	SSD1306_clear(); // Clear the screen mem
	
    while(1){
        uint32_t voltage = measure_battery_voltage();
        uint8_t percent_v = battery_percentage(voltage);
        bool batt_warn = percent_v > 15 ? false : true; // below 15% trigger battery warning
		bool batt_warn_display = false;
		update_display(percent_v, batt_warn, (uint16_t)(voltage >> 16));
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
    }
}

