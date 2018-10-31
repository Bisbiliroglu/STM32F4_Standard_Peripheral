#include "stm32f4xx.h"                  // Device header

/*
Discovery üzerindeki ledleri pwm ile kontrol eder 
1Hz sinyal ile PWM üretir. bu sayede PWM Degerleri osiloskop olmadan ledler ile gözlenebilir. 
Discovery board üzerindeki 4 lede 4 farkli Duty cycle degeribde PWM üretir. 
*/


int main(void)
{
	GPIO_InitTypeDef 				GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructre;						//Timer Type Def
	TIM_OCInitTypeDef				TIM_OCInitStructure;						//PWM Type Def
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 	 	//Ledler için GPIOD RCC aktif hale getiriliyor
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,	ENABLE);		//Timer 4 için RCC Aktif hale getiriliyor

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

	
	//GPIO AYARLARI
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin 	=	GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	//TIMER AYARLARI
	/*
		TIM_Prescaler (RCC_Clocks.PCLK2_Frequency/1000) ifadesi ile 1Khz lik  frekansi üretilir
		10-1 periyod ifadesi ile 1KhZ interrupt frekansi 1000 defade bir interrupta girmesi belirtilir
		Bu sayede 1Hz frekans üretilmnis olur. 
		Bu 1Hz frekans TIM_OCInitStructure.TIM_Pulse degeri ile bölünerek duty cycle degistirilebilir. 
		Örnegin %25 Duty cycle için direkt (1000 % 25 = 250) 250 - 1  degeri verilebilir. 
		pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1
	*/
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);  //Sistem frekans çalisma hizi almak için kullanilir.

	TIM_TimeBaseStructre.TIM_Prescaler 						= (RCC_Clocks.PCLK2_Frequency/1000) - 1;;  									
	TIM_TimeBaseStructre.TIM_Period								=	1000 - 1;											// arttikça yanip sönme hizi azaliyor
	//TIM_TimeBaseStructre.TIM_RepetitionCounter 	= 0;											// asagi sayici kaç kere  a ulasilinca reset olacagini gösterir
	//TIM_TimeBaseStructre.TIM_CounterMode				=	TIM_CounterMode_Up; 		// Sayici fonksiyon asagi yukari degeri
	TIM_TimeBaseStructre.TIM_ClockDivision				= TIM_CKD_DIV2 ; 						// Rakam yükseldikçe elektrik tüketimi ve hiz düser
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructre);
	TIM_Cmd(TIM4,ENABLE);
	
	//PWM AYARLARI
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		
	TIM_ARRPreloadConfig(TIM4, ENABLE); 

	TIM_OCInitStructure.TIM_Pulse = 250 - 1; /* 25% duty cycle */ //OC1 Yesil Led D12
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse = 500 - 1; /* 50% duty cycle */  //OC2 Turuncu Led D13
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_Pulse = 750 - 1; /* 75% duty cycle */ //OC3 Kirmizi Led  D14
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse = 1000 - 1; /* 100% duty cycle */ //OC4 Mavi Led D15
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

}

