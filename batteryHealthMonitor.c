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
        uint32_t voltage = read_battery_voltage();

		for(uint8_t i = 0; i < 101; i++){
			update_display(i, false, i);
		}		
        //calculate battery percentage estimate
        //calculate time-to-empty
        //update screen
        
        // measure cell voltage
        // transmit "bat low" if needed
        // low power shut-off?
        // low power beep?
        // sleep for 500ms? update screen
    }
}

