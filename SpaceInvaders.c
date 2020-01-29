// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 11/20/2018 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2018

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2018

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 

// up button connected to PE0								green
// down button connected to PE1							yellow
// left button connected to PE2							blue
// right button connected to PE3						orange

// 8*R resistor DAC bit 0 on PB0 (least significant bit)				green
// 4*R resistor DAC bit 1 on PB1																yellow
// 2*R resistor DAC bit 2 on PB2																blue
// 1*R resistor DAC bit 3 on PB3 (most significant bit)					orange

// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V																								red
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)																								blue
// MOSI (pin 7) connected to PA5 (SSI0Tx)																								orange
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)																						yellow
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command					green
// RESET (pin 3) connected to PA7 (GPIO)																								blue
// VCC (pin 2) connected to +3.3 V																											red
// Gnd (pin 1) connected to ground																											black

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "DAC.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "PortE.h"
#include "Random.h"

#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

void DisableInterrupts(void); 	// Disable interrupts
void EnableInterrupts(void);  	// Enable interrupts
void Delay1msGAME(uint32_t count); 	// time delay in 0.1 seconds
void SysTick_Init(void);				// Enable Systick (used to get data from slide pot)


void gameInit(void);
void Draw(void);
void Input(void);
void Logic(void);
void checkFoodandPoison(void);
void EndGame(void);

void WaitForInterrupt(void);

//uint32_t Random_FoodX(void);									//generates a random, valid X coordinate for the food to spawn at										
//uint32_t Random_FoodY(void);									//generates a random, valid Y coordinate for the food to spawn at	
//uint32_t Random_PoisonX(void);								//generates a random, valid X coordinate for the poison to spawn at
//uint32_t Random_PoisonY(void);								//generates a random, valid Y coordinate for the poison to spawn at

//int getDirection(void);

int headx;
int heady;
uint32_t foodx;
uint32_t foody;
uint32_t poisonx;
uint32_t poisony;
int score;

int counter;

int menu = 1;							//menu = true(1), false(0)
int tailx[1000];
int taily[1000];
int length;
extern int direction;
int keyFlag;
uint32_t key;
volatile int gameOver;
uint32_t ADCMail;
uint32_t ADCStatus;


#define UPdeg 90;
#define DOWNdeg 270;
#define LEFTdeg 180;
#define RIGHTdeg 360;

const int width = 120;
const int height = 160;

//int getGameOver()
//{
//	return gameOver;
//}

void PortF_Init(void)
{
	SYSCTL_RCGCGPIO_R |= 0x20;
	volatile int temp = 0;
	temp++;
	temp++;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
	PF1 = 0x02;
	PF2 = 0x00;
}

int main(void){
  PLL_Init(Bus80MHz);       						// Bus clock is 80 MHz 
	
	ST7735_InitR(INITR_REDTAB);						// initialize the LCD screen
  ST7735_FillScreen(0x0000);            // set screen to black
	
	PortF_Init();													// hearbeat init
	EdgeCounter_Init();										// buttons init (PortE -- edge triggered)
	ADC_Init();														// slide pot init
	DAC_Init();														// DAC init
	SysTick_Init();												// SysTick_Init 
	Timer0_Init(&Sound_Play, 16000);		// initialize timer for sound
	EnableInterrupts();
	
	Random_Init(NVIC_ST_CURRENT_R);

	while(1)
	{
		while(menu)
		{
					ST7735_FillScreen(0x0000);
					ST7735_DrawBitmap(0, 160, SnekMenu1, 128, 160);
					Delay1msGAME(4000);
					ST7735_FillScreen(0x0000);
					ST7735_DrawBitmap(0, 160, SnekMenu2, 128, 160);
					Delay1msGAME(4000);
					if (direction == RIGHT)
					{
						menu = 0;
					}
		}
		gameInit();
		counter = 0;	
		while(!gameOver)
		{
			if (counter == 10)
			{
				poisonx = Random_PoisonX();
				poisony = Random_PoisonY();
				if (((poisonx == foodx) && (poisony == foody)) || ((poisonx == headx) && (poisony == heady)))
				{
					poisonx = Random_PoisonX();
					poisony = Random_PoisonY();
				}
				counter = 0;
			}
			checkFoodandPoison();
			Draw();
			Logic();
			PF1 ^= 0x02;
			Delay1msGAME(Convert(ADCMail));			//delay
			ADCStatus = 0;
			counter++;
		}
		EndGame();
		Delay1msGAME(15600);
		gameOver = 0;
		Delay1msGAME(30000);
		menu = 1;
	}
}	

void gameInit()
{
	gameOver = 0;							// 0 means game is not over
	headx = 60;								// spawn head in middle of screen
	heady = 70;
	foodx = Random_FoodX();								// generate random location for first food
	foody = Random_FoodY();
	poisonx = Random_PoisonX();							// generate random location for first poison
	poisony = Random_PoisonY();
	length = 0;
	score = 0;
	keyFlag = 0;
	key = 0;
	direction = 1;
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Score: ");
}

void checkFoodandPoison(void)
{
	if ((foodx == poisonx) && (foody == poisony))
	{
		foodx = Random_FoodX();
		foody = Random_FoodY();
	}
//	if ((foodx <= 0) || (foodx >= 120))
//	{
//		foodx = Random_FoodX();
//	}
//	if ((foody <= 10) || (foody >= 160))
//	{
//		foody = Random_FoodY();
//	}
//	if ((poisonx <= 0) || (poisonx >= 120))
//	{
//		poisonx = Random_PoisonX();
//	}
//	if ((poisony <= 10) || (poisony >= 160))
//	{
//		poisony = Random_PoisonY();
//	}
}	

void Draw()
{
	ST7735_DrawBitmap(0, 160, black, 128, 150);
	ST7735_SetCursor(7, 0);
	LCD_OutDec(score);
	ST7735_DrawFastHLine(0, 9, 127, ST7735_CYAN);
	
	for(int i = 15; i <= height; i=i+5)							//i = ycoord
	{
		for (int j = 0; j <= width; j=j+5)							//j = xcoord
		{
			if((j == headx) && (i == heady))
			{
				ST7735_DrawBitmap(headx, heady, Snake, 5, 5);
			}
			else if((j == foodx) && (i == foody))
			{
				ST7735_DrawBitmap(foodx, foody, Food, 5, 5);
			}
			else if((j == poisonx) && (i == poisony))
			{
				ST7735_DrawBitmap(poisonx, poisony, Poison, 5, 5);
			}
			else
			{
				for(int a = 0; a < length; a++)
				{
					if(tailx[a] == j && taily[a] == i)
					{
						ST7735_DrawBitmap(tailx[a], taily[a], Tail, 5, 5);
					}
				}
			}
		}
	}
}

void Logic()
{
	int prevx = tailx[0];
	int prevy = taily[0];
	int prev2x;
	int prev2y;
	tailx[0] = headx;
	taily[0] = heady;
	
	for(int i = 1; i < length; i++)
	{
		prev2x = tailx[i];
		prev2y = taily[i];
		tailx[i] = prevx;
		taily[i] = prevy;
		prevx = prev2x;
		prevy = prev2y;
	}
	
	
	switch(direction)
	{
		case UP:
			heady -= 5;	
			break;
		case DOWN:
			heady += 5;
			break;
		case LEFT:
			headx -= 5;
			break;
		case RIGHT:
			headx += 5;
			break;
	}
	
	for(int i = 0; i < length; i++)	//check if the snake head hits any part of the tail
	{
		if((tailx[i] == headx) && (taily[i] == heady))
		{
			gameOver = 1;
		}
	}
	
	if(headx > width)
	{
		gameOver = 1;
	}
	else if(headx < 0)
	{
		gameOver = 1;
	}
	
	if(heady >= height)
	{
		gameOver = 1;
	}
	else if(heady <= 10)
	{
		gameOver = 1;
	}
	
	if((headx == poisonx) && (heady == poisony))//check if snake head hit poison
	{
		gameOver = 1;
	}
	
	if((headx == foodx) && (heady == foody))	//check if the snake ate the food
	{
		score++;														//increase score
		foodx = Random_FoodX();							//spawns a new food
		foody = Random_FoodY();
		length++;
	}
}

void EndGame(void)
{
	ST7735_SetCursor(6, 7);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(6, 8);
  ST7735_OutString("Nice try");
  ST7735_SetCursor(6, 9);
  ST7735_OutString("Score: ");
  ST7735_SetCursor(12, 9);
  LCD_OutDec(score);
}



// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none   unsigned long period
void SysTick_Init(void){
  //*** students write this ******
	NVIC_ST_RELOAD_R = 8000000;
  NVIC_ST_CURRENT_R = 0;  	   // any write will reload counter and clear count
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; 
  NVIC_ST_CTRL_R = 0x07;
}

void SysTick_Handler(void)
{ // every 16.67 ms
	ADCMail = ADC_In();
	ADCStatus = 1;
}

//This function is used to delay the speed between position updates
//It is passed a value taken from the slide pot
void Delay1msGAME(uint32_t count)	
{
	uint32_t time;
  while(count > 0)
	{
    time = 72724*2/91;  // 0.001 sec at 80 MHz
    while(time)
		{
	  	time--;
    }
    count--;
  }
}
