#include "stm32f4xx.h"                  // Device header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

/*
A0 vr A1 Bacagindan 2 kanalda ADC verisini 12 bit okur
Timer ile zaman ayarlai interrupta girer
USART ile okudugu veriyi timer interrupt frekansina göre gonderir
USART Gelen sayiyi ms cinsinden gönderim periyodu olarak girer (0 ile 100000 arasi)
Her veri gönderdiginde d12, d13, d14, d15 led durumunu degistirir
Standart ayarlarinda Matlab ile fonksiyon çizdirilecek sekilde çalisir
USART ile "log" yazisi gelirse matlab ile çalismaz ancak serial terminal ile okunakli yazi gonderir. 
USART ile log durumunda tekrar log yazisi dönerse standart çalisma sekline döner
*/


char string_rx[75], string_tx[75];
int rx_stopper = 0, period = 1000, log_status = 0;
uint16_t adc_Data1 = 0, adc_Data2 = 0;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructre;

uint16_t Read_ADC(int channel)
{
	ADC_RegularChannelConfig(ADC1,channel,1,ADC_SampleTime_56Cycles);

	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}


void USART_Puts(USART_TypeDef* USARTx, volatile char *string)
{
	while(*string)
		{
			while(!(USARTx->SR & 0X00000040)); 
			USART_SendData(USARTx, *string);
			*string++;
		}
}

int main(void)
{	
	
	GPIO_InitTypeDef 				GPIO_InitStructure;
	ADC_InitTypeDef 				ADC_InitStructure;
	ADC_CommonInitTypeDef 	ADC_CommonInitStructure;
	USART_InitTypeDef 			USART_InitStructure;
	NVIC_InitTypeDef 				NVIC_InitStructure;
	RCC_ClocksTypeDef 			RCC_Clocks;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,	ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,	ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,		ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,	ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,		ENABLE);
	
	
	RCC_GetClocksFreq(&RCC_Clocks); //Sistem frekans çalisma hizi almak için kullanilir.
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin		=	GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 

	USART_InitStructure.USART_BaudRate 						= 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 								= USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity							= USART_Parity_No;
	USART_InitStructure.USART_StopBits						= USART_StopBits_1;
	USART_InitStructure.USART_WordLength					= USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseStructre.TIM_Prescaler 						= (RCC_Clocks.PCLK2_Frequency/1000) - 1;;  									
	TIM_TimeBaseStructre.TIM_Period								=	period-1;											// arttikça yanip sönme hizi azaliyor
	//TIM_TimeBaseStructre.TIM_RepetitionCounter 	= 0;											// asagi sayici kaç kere  a ulasilinca reset olacagini gösterir
	//TIM_TimeBaseStructre.TIM_CounterMode				=	TIM_CounterMode_Up; 		// Sayici fonksiyon asagi yukari degeri
	TIM_TimeBaseStructre.TIM_ClockDivision				= TIM_CKD_DIV2 ; 						// Rakam yükseldikçe elektrik tüketimi ve hiz düser
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructre);
	TIM_Cmd(TIM2,ENABLE);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	//INTERRUPT AYARLARI
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0x01;					// KUCUK DEGERLER DAHA ONCELLIKLIDIR
	NVIC_InitStructure.NVIC_IRQChannelSubPriority					=	0x01;
	NVIC_InitStructure.NVIC_IRQChannel										=	TIM2_IRQn; 		//TIM2 INTERRUPT GOSTERILDI
	NVIC_InitStructure.NVIC_IRQChannelCmd									= ENABLE;     	//iNTERRUPTIN AKTIF MI PASIF MI OLDUGUNU GOSTERIYOR
	NVIC_Init(&NVIC_InitStructure);
	
	ADC_InitStructure.ADC_Resolution 			= ADC_Resolution_12b;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_Mode 			= ADC_Mode_Independent;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
	
	while(1)
	{
		adc_Data1 = Read_ADC(ADC_Channel_0);
		adc_Data2 = Read_ADC(ADC_Channel_1);

	}
}
//strtoint_n ve strtoint string degeri int e dönüstürmek için atoi fonksiyon çalistirilinca ADC degerleri 0 oluyor
int strtoint_n(char* str, int n)
{
    int sign = 1;
    int place = 1;
    int ret = 0;

    int i;
    for (i = n-1; i >= 0; i--, place *= 10)
    {
        int c = str[i];
        switch (c)
        {
            case '-':
                if (i == 0) sign = -1;
                else return -1;
                break;
            default:
                if (c >= '0' && c <= '9')   ret += (c - '0') * place;
                else return -1;
        }
    }

    return sign * ret;
}

int strtoint(char* str)
{
    char* temp = str;
    int n = 0;
    while (*temp != '\0')
    {
        n++;
        temp++;
    }
    return strtoint_n(str, n);
}

void USART2_IRQHandler(void)
{
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
		if((char)USART_ReceiveData(USART2) == '\n') // termite programinda gönderilen string bitince otomatik \n gönderiyor. Bununla stringin bittigini anlayabiliyoruz
		{
			rx_stopper = 1; // bu degisken gelen string verisinin bittigini gosterir
			//period = (int) strtol(string_rx, (char **)NULL, 10); // bu fonksiyonlar adc ile beraber çalisamiyorlar
			//period =  (int)atoi(string_rx); //
			if(strcmp(string_rx,"log") == 0){ //strcmp string karsilastirmak icin
				if(log_status == 0)
				{
					log_status = 1;
				}
				else
				{
					log_status = 0;
				}
			}
			else
			{
				period = strtoint(string_rx);
				if(period > 1 && period < 1000000)
				{
						TIM_TimeBaseStructre.TIM_Period		=	period-1;									
						TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructre);
				}
			}
	}
		else
		{
			if(rx_stopper == 1) // eger daha önceden kayitli string_rx varsa temizleme kontrolü
			{
				sprintf(string_rx,"");
				rx_stopper = 0;
			}
			sprintf(string_rx,"%s%c",string_rx,(char)USART_ReceiveData(USART2)); // gelen her karakter daha önceden girilmis string_rx degerinin üstüne eklenr
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }

}

void TIM2_IRQHandler()
{                      

	if(TIM_GetITStatus(TIM2,TIM_IT_Update != RESET)) //Timerin gerçeklesip gerçeklesmedigini denetler
		{
			if(log_status == 0)
			{
				sprintf(string_tx,"%d\n%d\n",adc_Data1,adc_Data2);
			}
			else
			{
				sprintf(string_tx,"periyot %d ms string_rx: %s ADC1: %d ADC2: %d\n",period,string_rx, adc_Data1,adc_Data2);
			}
			USART_Puts(USART2,string_tx);
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //TIM Flagi sifirlama 
			GPIO_ToggleBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		}
}
