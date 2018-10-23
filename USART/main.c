#include "stm32f4xx.h"                  // Device header
#include <stdio.h>
#include <stdlib.h>

//USART 2 ve USB TTL Dönüstürücü kullanilarak PC ile haberlesme yapar
//TX = A2 RX = A3 bunlar ttl dönüstürücüde gerekli pinlere (rx -> tx ve tx ->rx olacak sekilde) baglanmuistir
//Receive islemi için interrupt kullanir
//Son gelen seri kanal bilgisini tutar

char string_rx[75], string_tx[75];
int rx_stopper = 0;

void USART_Puts(USART_TypeDef* USARTx, volatile char *string)
{
	while(*string)
		{
			while(!(USARTx->SR & 0X00000040)); 
			USART_SendData(USARTx, *string);
			*string++;
		}
}

void delay(void)
{
	int i = 30000000;
	while(i--);
}
 


int main(void)
{
	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef 	GPIO_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, 	ENABLE);
	
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	sprintf(string_tx,"\nHello World from project %.1f \n",4.0);
	USART_Puts(USART2,string_tx);
	while(1)
	{
		delay();		
		sprintf(string_tx,"Son gelen data: %s\n",string_rx);
		USART_Puts(USART2,string_tx);
	}
}

void USART2_IRQHandler(void)
{
	
	//Gelen her char için bu interrupta girer. 
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
		if((char)USART_ReceiveData(USART2) == '\n') // termite programinda gönderilen string bitince otomatik \n gönderiyor. Bununla stringin bittigini anlayabiliyoruz
		{
			rx_stopper = 1; // bu degisken gelen string verisinin bittigini gosterir
		}
		else
		{
			if(rx_stopper == 1) // eger daha önceden kayitli string_rx varsa temizleme kontrolü
			{
				sprintf(string_rx,"");
				rx_stopper = 0;
			}
			sprintf(string_rx,"%s%c",string_rx,(char)USART_ReceiveData(USART2)); // gelen her karakter daha önceden girilmis string_rx degerinin üstüne eklenr
			//USART_Puts(USART2,string_rx);
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }
}
