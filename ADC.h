// put prototypes for public functions, explain what it does
// Tyler Anson, Ethan Shen
// 5/5/19

#include <stdint.h>

void ADC_Init(void);													// Initialize slide pot
uint32_t ADC_In(void); 												// Read data from slide

uint32_t Convert(uint32_t input);							// Convert data from slide pot to be used to make the game faster/slower

uint32_t Random_FoodX(void);									//gets random, valid X coord for food
uint32_t Random_FoodY(void);									//gets random, valid Y coord for food
uint32_t Random_PoisonX(void);								//gets random, valid X coord for poison
uint32_t Random_PoisonY(void);								//gets random, valid Y coord for poison
