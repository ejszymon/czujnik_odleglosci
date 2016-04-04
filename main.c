#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
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
#define wszystkie GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15
void czujnik(void)
{

}
int main(void)
{
	SystemInit();

	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	TM_HCSR04_t czujnik;

	TM_DELAY_Init(); // funkcje opozniajace

	  if (!TM_HCSR04_Init(&czujnik, GPIOD, GPIO_PIN_0, GPIOC, GPIO_PIN_1)) //jak nie udalo sie podlaczyc
	  	  {																   //ECHO PD0, TRIGGER PC1
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
		        Delayms(100); //opoznienie
		        GPIO_ResetBits(GPIOD, wszystkie);
	}
}

