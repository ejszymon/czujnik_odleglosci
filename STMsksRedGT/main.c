#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
//-----//
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_hcsr04.h"
#include "tm_stm32f4_gpio.h"
#include <stdio.h>
#define zielona GPIO_Pin_12
#define pomaranczowa GPIO_Pin_13
#define czerwona GPIO_Pin_14
#define niebieska GPIO_Pin_15
#define wszystkie GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15 //dla wygody

void tim3_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 7200;
	TIM_TimeBaseStructure.TIM_Prescaler = 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);
}
void przerwania_tim3_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// numer przerwania
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	// priorytet g³ówny
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	// subpriorytet
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	// uruchom dany kana³
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// zapisz wype³nion¹ strukturê do rejestrów
	NVIC_Init(&NVIC_InitStructure);


	// wyczyszczenie przerwania od timera 3 (wyst¹pi³o przy konfiguracji timera)
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	// zezwolenie na przerwania od przepe³nienia dla timera 3
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

}
void TIM3_IRQHandler(void)
{
         	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
         	{
         		GPIO_ResetBits(GPIOD, wszystkie);
                	// wyzerowanie flagi wyzwolonego przerwania
                	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
         	}
}

void diody_init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		/* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
}
int main(void)
{
	SystemInit();

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	diody_init();

	TM_HCSR04_t czujnik;
	TM_DELAY_Init(); // funkcje opozniajace

	  if (!TM_HCSR04_Init(&czujnik, GPIOC, GPIO_PIN_13, GPIOE, GPIO_PIN_6)) //
	  	  {																   //bylo ECHO PD0, TRIGGER PC1
	        while (1)
	        {
	            GPIO_SetBits(GPIOD, czerwona);
	            Delayms(100);
	        }
	  	  }


	unsigned int i;


	while(1)
	{
		        TM_HCSR04_Read(&czujnik);

		        if (czujnik.Distance < 0) //blad
		        {
		        	GPIO_SetBits(GPIOD, czerwona);
		        }
		        else if (czujnik.Distance > 50) //ponad 50cm
		        {
		        	GPIO_SetBits(GPIOD, pomaranczowa);
		        }
		        else //miedzy 0 a 50 cm
		        {
		        	GPIO_SetBits(GPIOD, niebieska);
		        }
	}
}

