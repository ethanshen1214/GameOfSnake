// put implementations for functions, explain how it works
// Tyler Anson, Ethan Shen
// 5/1/2019
//
// These functions include the intialization for the ADC slide pot
// as well as the interrupt handler, which is used to determine the
// speed of the game. 

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "Random.h"

extern uint32_t randomValFood;
extern uint32_t randomValPoison;

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void)
{ 
		volatile int delay = 0;
		SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D
		delay++;
		delay++;
		GPIO_PORTD_DIR_R &= 0xFB;      	// 2) make PE4 input
		GPIO_PORTD_AFSEL_R |= 0x04;     // 3) enable alternate fun on PD2
		GPIO_PORTD_DEN_R &= 0xFB;      	// 4) disable digital I/O on PD2
		GPIO_PORTD_AMSEL_R |= 0x04;     // 5) enable analog fun on PD2
		SYSCTL_RCGCADC_R |= 0x01;       // 6) activate ADC0 
	
		delay++;
		delay++;
		delay++;
		delay++;

		ADC0_PC_R |= 0x01;
		ADC0_SSPRI_R = 0x0123;
		ADC0_ACTSS_R &= 0xF7;
		ADC0_EMUX_R &= 0xFFFF0FFF;
		ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5;  //PD2 IS INPUT
		ADC0_SSCTL3_R |= 0x06;
		ADC0_IM_R &= 0xF7;
		ADC0_ACTSS_R |= 0x08;
		ADC0_SAC_R = 0x06; 
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void)
{  
	uint32_t data;
  ADC0_PSSI_R = 0x0008;            
  while((ADC0_RIS_R&0x08)==0){};   
  data = ADC0_SSFIFO3_R&0xFFF; 
  ADC0_ISC_R = 0x0008; 
  return data;
}
//------------Convert--------------
// Converts data from the slide pot to a delay
// in milliseconds
// Input: 32-bit data 
// Output: Delay value in ms
// your function to convert ADC sample to a interrupt reload value
uint32_t Convert(uint32_t input)
{
  return ((input/2)+100);
}

int32_t data;
//------------Random_FoodX--------------
// Returns a random x coordinate for the food
int32_t Random_FoodX(void)
{
//	Random_Init(randomValFood);
	int32_t data = (((Random()%124)/5)*5);
	if ((data < 0) || (data>= 120))
	{
		data = Random_FoodX();
	}
	return data;
}
//------------Random_FoodY--------------
// Returns a random x coordinate for the food
int32_t Random_FoodY(void)
{
	int32_t data = (((Random()%160)/5)*5);
	if ((data <= 10) || (data >= 160))
	{
		data = Random_FoodY();
	}
	return data;
}


//------------Random_PoisonX--------------
//Returns a random x coordinate for the poison
int32_t Random_PoisonX(void)
{
	int32_t data = (((Random()%124)/5)*5);
	if ((data < 0) || (data>= 120))
	{
		data = Random_PoisonX();
	}
	return data;
}
//------------Random_PoisonY--------------
// Returns a random y coordinate for the poison
int32_t Random_PoisonY(void)
{
	int32_t data = (((Random()%160)/5)*5);
	if ((data <= 10) || (data >= 160))
	{
		data = Random_PoisonY();
	}
	return data;
}
