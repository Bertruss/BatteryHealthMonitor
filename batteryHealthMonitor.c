#include "./TinyTWI/fcpu.h"
#include "./TinyTWI/TinyTWI.h"
#include "./bhmDisplay/bhmDisplay.h"
#include "./SSD1306_driver/SSD1306_driver.h"
#include <avr/io.h>
#include <util/delay.h>

// the setup function runs once when you press reset or power the board
int main(){
    //initialize i2c
    twi_init();
    adc_init();
	SSD1306_init();
	SSD1306_clear();
	
    while(1){
        //ADC
        // 10-bit
        // 1024 steps
        // resolution ~5mv (4.882813) at 5v vcc
        //measure battery voltage
        uint32_t voltage = read_battery_voltage();

		for(uint8_t i = 0; i < 101; i++){
			update_display(i, false, i);
		}		
        //calculate battery percentage estimate
        //calculate time-to-empty
        //update screen
        //** ads **
        // measure cell voltage
        // transmit "bat low" if needed
        // low power shut-off?
        // low power beep?
        // sleep for 500ms?update screen
        
        // testing the screen update
        
    }
}