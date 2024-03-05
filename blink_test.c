/*
 * main.c
 *
 * Created: 2/21/2024 10:50:15 PM
 *  Author: Michael
 */ 

void delay(volatile long time){
	while (time > 0){
		time--;
	}
}

#define DORB *((volatile uint8_t*) 0x37) 
#define PORB *((volatile uint8_t*) 0x38) 

int main(void)
{
	DORB |= (1 << 3);
    while(1)
    {
		PORB ^= (1 << 3);
		delay(1000000);
		    
    }
}