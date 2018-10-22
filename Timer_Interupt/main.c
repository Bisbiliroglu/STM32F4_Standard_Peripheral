#include "stm32f4xx.h"                  // Device header

int main(void)
{
	GPIO_InitTypeDef 				GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructre;
	NVIC_InitTypeDef				NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);  //Ledler için GPIOD RCC aktif hale getiriliyor
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);		//Timer 2 için RCC Aktif hale getiriliyor
	
	
	//GPIO AYARLARI
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	=	GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	//TIMER AYARLARI
	/*
		TIM_Prescaler (RCC_Clocks.PCLK2_Frequency/1000) ifadesi ile 1Khz lik interrupt frekansi üretilir
		1000-1 periyod ifadesi ile 1KhZ interrupt frekansi 1000 defade bir interrupta girmesi belirtilir
		Bu sayede 1Hz frekansli TIM2_IRQHandler fonksiyonuna girilmis olur ve her saniyede çalisma saglanir
	*/
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);  //Sistem frekans çalisma hizi almak için kullanilir.
	
	TIM_TimeBaseStructre.TIM_Prescaler 						= (RCC_Clocks.PCLK2_Frequency/1000) - 1;;  									
	TIM_TimeBaseStructre.TIM_Period								=	1000-1;											// arttikça yanip sönme hizi azaliyor
	//TIM_TimeBaseStructre.TIM_RepetitionCounter 	= 0;											// asagi sayici kaç kere  a ulasilinca reset olacagini gösterir
	//TIM_TimeBaseStructre.TIM_CounterMode				=	TIM_CounterMode_Up; 		// Sayici fonksiyon asagi yukari degeri
	TIM_TimeBaseStructre.TIM_ClockDivision				= TIM_CKD_DIV2 ; 						// Rakam yükseldikçe elektrik tüketimi ve hiz düser
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructre);
	TIM_Cmd(TIM2,ENABLE);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	//INTERRUPT AYARLARI
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0x00;					// KUCUK DEGERLER DAHA ONCELLIKLIDIR
	NVIC_InitStructure.NVIC_IRQChannelSubPriority					=	0x01;
	NVIC_InitStructure.NVIC_IRQChannel										=	TIM2_IRQn; 		//TIM2 INTERRUPT GOSTERILDI
	NVIC_InitStructure.NVIC_IRQChannelCmd									= ENABLE;     	//iNTERRUPTIN AKTIF MI PASIF MI OLDUGUNU GOSTERIYOR
	NVIC_Init(&NVIC_InitStructure);
	while(1); 
}

void TIM2_IRQHandler()
{                      
	//Interrupt sirasinda çalisan fonksiyon
	//Bu fonksiyon ismi degistirilemez
	if(TIM_GetITStatus(TIM2,TIM_IT_Update != RESET)) //Timerin gerçeklesip gerçeklesmedigini denetler
		{
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //TIM Flagi sifirlama 
			GPIO_ToggleBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		}
}
