// Student names: Tyler Anson, Ethan Shen
// Last modification date: 5/5/19
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void)
{
	volatile int nop = 0;
	SYSCTL_RCGCGPIO_R |= 0x02;
	nop++;
	nop++;
	GPIO_PORTB_DIR_R = 0x0F;
	GPIO_PORTB_DEN_R = 0x0F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data)
{
	GPIO_PORTB_DATA_R = data;
}
