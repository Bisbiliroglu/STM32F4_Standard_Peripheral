#include "stm32f4xx.h"                  // Device header
/*
EXTI ve Discovery board üzerindeki butonu kullanarak buton takibi yaapr
Button basilinca ledleri Toggle eder. 
Her gpio kendi pin numarasinda line a baglanir yani PA3 Line 3 e PD 12 Exti Line 12 ye baglanir
Exti Line 15 den sonrasi farkli fonksiyonlara (USB v.b.) ayarlidir. GPIO ile 0-15 arasi kullanilir.
*/

int main (void)
{
	EXTI_InitTypeDef   	EXTI_InitStructure;
	GPIO_InitTypeDef		GPIO_InitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, 	ENABLE);		//Button için RCC erisimini aç
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, 	ENABLE);		//Ledler için RCC erisimini aç
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,	ENABLE);	//EXTI için SYSCFG RCC erisimini aç

	/*GPIO Led ayarlari*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/*GPIO Button ayarlari*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//  EXTI Line0 i PA0 için baglar
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Exti Line 0 için ayarlar */
  EXTI_InitStructure.EXTI_Line 		= EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  	//Yükselen kenarda tetikleme yapar
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Exti Line 0 interrupt ayarlari */
  NVIC_InitStructure.NVIC_IRQChannel 										= EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	while(1);

}
/*Exti çalistiginda islemlerin yapilacagi fonksiyon*/
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
		GPIO_ToggleBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}


