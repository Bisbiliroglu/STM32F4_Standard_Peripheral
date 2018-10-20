#include "stm32f4xx.h"                  // Device header

//A0 ADC bacagini okuyarak D12 , D13 , D14 ,D15 Ledlerini Kontrol eder,
//A0 Bacagina potansyometre baglidir.  

uint8_t Read_ADC(void)
{
	//A0 Bacagindaki ADC verisini okur
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_56Cycles);
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}

int main(void)
{
	GPIO_InitTypeDef 			GPIO_InitStructure;
	ADC_InitTypeDef 			ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	uint8_t 							ADC_Data = 0;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin		=	GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Resolution 			= ADC_Resolution_8b;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
	ADC_CommonInitStructure.ADC_Mode 			= ADC_Mode_Independent;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
	
	while(1)
	{
		ADC_Data = Read_ADC();
		//8 bit veri max 256 4 led icin 64 luk parçalara boleriz
		if(ADC_Data > 0 && ADC_Data < 64)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_12);
			GPIO_ResetBits(GPIOD,GPIO_Pin_13);
			GPIO_ResetBits(GPIOD,GPIO_Pin_14);
			GPIO_ResetBits(GPIOD,GPIO_Pin_15);

		}
		else if(ADC_Data >= 64 && ADC_Data < 128)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_12);
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
			GPIO_ResetBits(GPIOD,GPIO_Pin_14);
			GPIO_ResetBits(GPIOD,GPIO_Pin_15);			
		}
		else if(ADC_Data >= 128 && ADC_Data < 192)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_12);
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
			GPIO_SetBits(GPIOD,GPIO_Pin_14);
			GPIO_ResetBits(GPIOD,GPIO_Pin_15);			
		}	
		else if(ADC_Data >= 192 && ADC_Data < 256)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_12);
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
			GPIO_SetBits(GPIOD,GPIO_Pin_14);
			GPIO_SetBits(GPIOD,GPIO_Pin_15);		
		}
		else
		{
			GPIO_ResetBits(GPIOD,GPIO_Pin_12);
			GPIO_ResetBits(GPIOD,GPIO_Pin_13);
			GPIO_ResetBits(GPIOD,GPIO_Pin_14);
			GPIO_ResetBits(GPIOD,GPIO_Pin_15);			
		}
	}
}
