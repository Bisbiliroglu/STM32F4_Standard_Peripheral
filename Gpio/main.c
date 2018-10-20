#include "stm32f4xx.h"                  // Device header
/*
Discovery ledlerine bagli D portu 12, 13 ,14 ve 15 nolu bacaklari kontrol eder
Discovery butonuna bagli A portu 0 numarali bacagi kontrol eder.
*/

void delay(uint32_t millis)
	{
		millis = 1000 * millis * 42;
		while (millis--);
  }
int main()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t counter = 0, reverse = 0;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	while(1)
		{
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
			{
				if(reverse == 0)
				{
					counter++;
				}
				else
				{
					counter--;
				}
				while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
				if(counter > 4){
					reverse = 1;
					counter = 3;
				}
				else if(counter < 1)
				{
					reverse = 0;
					counter = 0;
				}
			}
			
			if(counter == 0)
			{
				GPIO_ResetBits(GPIOD,GPIO_Pin_12);
				GPIO_ResetBits(GPIOD,GPIO_Pin_13);
				GPIO_ResetBits(GPIOD,GPIO_Pin_14);
				GPIO_ResetBits(GPIOD,GPIO_Pin_15);
			}
			else if(counter == 1)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_12);
				GPIO_ResetBits(GPIOD,GPIO_Pin_13);
				GPIO_ResetBits(GPIOD,GPIO_Pin_14);
				GPIO_ResetBits(GPIOD,GPIO_Pin_15);
			}
			else if(counter == 2)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_12);
				GPIO_SetBits(GPIOD,GPIO_Pin_13);
				GPIO_ResetBits(GPIOD,GPIO_Pin_14);
				GPIO_ResetBits(GPIOD,GPIO_Pin_15);
			}
			else if(counter == 3)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_12);
				GPIO_SetBits(GPIOD,GPIO_Pin_13);
				GPIO_SetBits(GPIOD,GPIO_Pin_14);
				GPIO_ResetBits(GPIOD,GPIO_Pin_15);
			}
			else if(counter == 4)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_12);
				GPIO_SetBits(GPIOD,GPIO_Pin_13);
				GPIO_SetBits(GPIOD,GPIO_Pin_14);
				GPIO_SetBits(GPIOD,GPIO_Pin_15);	
			}
			delay(25);

		}
	
	
}
