// PortE.c
// This file is used to handle the edge-triggered interrupts
// on port E. There are four buttons interfaced on pins E0 - E3.
// These buttons will determine the direction of the user's 
// snake.

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "Random.h"

#define UP 10
#define DOWN 10
#define LEFT 10
#define RIGHT 10

void EnableInterrupts(void);

// Initialization for PortE interrupts
volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init(void)
{                          
SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTE_DIR_R &= ~0x0F;    // (c) make PE0-3 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x0F;  //     disable alt funct on PF4
  GPIO_PORTE_DEN_R |= 0x0F;     //     enable digital I/O on PF4   
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // configure PF4 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
  //GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x0F;     // (d) PF4 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x0F;    //     PF4 is not both edges
  GPIO_PORTE_IEV_R = 0x0F;    //     PF4 falling edge event
  GPIO_PORTE_ICR_R = 0x0F;      // (e) clear flag4
  GPIO_PORTE_IM_R = 0x0F;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  //NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x000000A0;
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 30 in NVIC
//  EnableInterrupts();           // (i) Clears the I bit
}

volatile uint32_t randomValPoison = 500;
volatile uint32_t randomValFood = 0;
volatile int direction = 1;

void GPIOPortE_Handler(void)
{
	direction = GPIO_PORTE_DATA_R;
	GPIO_PORTE_ICR_R |= 0x0F;      // acknowledge flag4
	uint32_t temp = NVIC_ST_CURRENT_R;
	//Random_Init(NVIC_ST_CURRENT_R);
}
