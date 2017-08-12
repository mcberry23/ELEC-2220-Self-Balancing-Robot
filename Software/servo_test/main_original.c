#include <stm32f4xx.h>
//#include "stm32f4xx_tim.h"

#define  RCC_APB1ENR_TIM3EN          ((uint32_t)0x00000002)


void delay (void)                   //create simple delay loop
{
	int d;
	for (d=0; d<10000000; d++);
}

int main (void) {
	RCC->APB1ENR     = RCC_APB1ENR_TIM3EN;  		//enable timer 3
	TIM3->CR1       |= 0x81;                    //enable timer 1 = 10000001
	//TIM3->CR2     |= 0x40;                    //  = 01000000
	TIM3->PSC        = 0x48;                    //set prescale to 72
	TIM3->ARR        = 0x4E20;                  //set auto reload to 20000
	TIM3->CCER    	|= 0x1000;                  //set timer to output
	TIM3->CCMR1     |= 0x68;                    //Set PWM mode 1 = 01101000


	//timer 3 now set to 50hz

	RCC->AHB1ENR   |= 0x05;                //IO Port A and C clock enable  = 00000101

	GPIOC->MODER   |= 0x400;               //set PC6 as alternate function = 0000 0100 0000 0000 ISSUE:this is output, should be input?
	GPIOC->AFR[0]   = 0x02000000;          //Set AF to timer 3 = 0000 0010 0000 0000 0000 0000 0000 0000
	GPIOC->OTYPER   = 0;                   //Set output as Push-Pull mode
	GPIOC->OSPEEDR	= 0;                   //Set output speed 2MHz low speed
	GPIOC->PUPDR		= 0;                   //Set no pull up and pull down
	
	GPIOA->MODER   &= 0xfffffffc;          // Set Port a Bit 0 Active input
	GPIOA->OTYPER   = 0;                   //Set output as Push-Pull mode
	GPIOA->OSPEEDR  = 0;                   //Set output speed 2MHz low speed
	GPIOA->PUPDR    = 0;                   //Set no pull up and pull down

while(1) {
		TIM3->CCR1 |= 0x28A;            //650us pulses
		delay();
		TIM3->CCR1 |= 0x73A;                //1850us pulses
		delay();
	}
}
