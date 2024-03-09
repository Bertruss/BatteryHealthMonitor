#include <TinyTWI.h>

// the setup function runs once when you press reset or power the board
int main(){
    char screen_addr = 0x3e; // TODO: replace with actual address
    //initialize i2c
    twi_init();


    while(1){
        //ADC
        // 10-bit
        // 1024 steps
        // resolution ~5mv (4.882813) at 5v vcc
        //measure battery voltage
        //adc sleep?

        //calculate battery percentage estimate
        //calculate time-to-empty
        //update screen
        //** ads **
        // measure cell voltage
        // transmit "bat low" if needed
        // low power shut-off?
        // low power beep?
        // sleep for 500ms?update screen
        
    }
}
