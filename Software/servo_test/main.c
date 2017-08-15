#include <stm32f4xx.h>
const int MINSERVO = 16;
const int MIDSERVO = 24;
const int MAXSERVO = 32;
const int LEFTSERVO =	0;
const int RIGHTSERVO = 1;	

//simple delay loop
void delay (void)
{
	int d;
	for (d=0; d<4000000; d++);
}

void initLeftServo() {
		//----Init Timer 4----
		RCC->APB1ENR  |=  RCC_APB1ENR_TIM4EN; //enable timer clock
		TIM4->PSC			 = 1000;									//prescale = 63
		TIM4->ARR			 = 320-1;								//up-counting: from 0 to ARR
		//OC1M = 110 for PWM Mode 1 output on channel 1
		TIM4->CCMR1		|=	TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
		TIM4->CCMR1		|=	TIM_CCMR1_OC1PE;		//Output 1 preload enable
		TIM4->CR1			|= 	TIM_CR1_ARPE;				//Auto-reload preload enable
		TIM4->CCER		|=  TIM_CCER_CC1E;			//ENABLE output for channel 1
		TIM4->EGR			|=	TIM_EGR_UG;					//Force updates
		TIM4->SR			&=	~TIM_SR_UIF;				//Clear the update flag
		TIM4->DIER		|=	TIM_DIER_UIE;				//ENABLE interrupt on update
		TIM4->CR1		 	 =	TIM_CR1_CEN;		//Enable timer 4
		//----Init PB6----
		RCC->AHB1ENR  |= 	0x02;     	//IO Port B clock enable  = 00000010
		GPIOB->MODER 	&= ~0x00003000; //clear PB6 mode 
		GPIOB->MODER  |= 	0x00002000; //PB6 = AF mode 
		GPIOB->AFR[0] |=  0x02000000; //PB6 = AF2
}
void initRightServo() {
		//----Init Timer 4----
		RCC->APB1ENR  |=  RCC_APB1ENR_TIM4EN; //enable timer clock
		TIM4->PSC			 = 1000;									//prescale = 63
		TIM4->ARR			 = 320-1;								//up-counting: from 0 to ARR
		//OC2M = 110 for PWM Mode 1 output on channel 2
		TIM4->CCMR2		|=	TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
		TIM4->CCMR2		|=	TIM_CCMR2_OC3PE;		//Output 2 preload enable
		TIM4->CR1			|= 	TIM_CR1_ARPE;				//Auto-reload preload enable
		TIM4->CCER		|=  TIM_CCER_CC3E;			//ENABLE output for channel 2
		TIM4->EGR			|=	TIM_EGR_UG;					//Force updates
		TIM4->SR			&=	~TIM_SR_UIF;				//Clear the update flag
		TIM4->DIER		|=	TIM_DIER_UIE;				//ENABLE interrupt on update
		TIM4->CR1		 	 =	TIM_CR1_CEN;		//Enable timer 4
		//----Init PB8----
		RCC->AHB1ENR  |= 	0x02;     	//IO Port B clock enable  = 00000010
		GPIOB->MODER 	&= ~0x00030000; //clear PB8 mode 
		GPIOB->MODER  |= 	0x00020000; //PB8 = AF mode 
		GPIOB->AFR[1] |=  0x00000002; //PB8 = AF1
}
// Sets servo at position from 16 to 32
void setServo(int servo,int position) {
		if (servo == RIGHTSERVO) { //Right Servo
			TIM4->CCR3 = position;
		}
		else {	//Left Servo
			TIM4->CCR1 = position;
		}
}
// Stop both servos
void stopServos() {
		setServo(LEFTSERVO,MIDSERVO);
		setServo(RIGHTSERVO,MIDSERVO);
}
// Move both servos forward by speed 0 - 8
void moveForward(int speed) {
		setServo(LEFTSERVO,MIDSERVO+speed);
		setServo(RIGHTSERVO,MIDSERVO-speed);
}
// Move both servos backward by speed 0 - 8
void moveBackward(int speed) {
		setServo(LEFTSERVO,MIDSERVO-speed);
		setServo(RIGHTSERVO,MIDSERVO+speed);
}
// Main routine that moves the bot forward and backwards with pauses
int main (void) {	
	initLeftServo();
	initRightServo();
	while(1) {
			moveForward(8);
			delay();
			stopServos();
			delay();
			moveBackward(8);
			delay();
			stopServos();
			delay();
	}
}
