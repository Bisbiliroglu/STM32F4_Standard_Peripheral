#include "stm32f4xx.h"                  // Device header
#include <stdio.h>


/*
	SPI ile kart üzerinde bulunan motion sensor verisini okur.
	Veri bilgilerini USART ile pc ye gönderir. 
	Okunan veriye göre kart üzerinde bulunan 4 ledi harekete göre yakar.
*/

char string_tx[75]; //usart gonderilen veri

void USART_Puts(USART_TypeDef* USARTx, volatile char *string)
{
	while(*string)
		{
			while(!(USARTx->SR & 0X00000040)); 
			USART_SendData(USARTx, *string);
			*string++;
		}
}

uint8_t SPI_Rx(uint8_t adress)
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_3); //CS pini lojik 0 a çekildi
	adress=0x80|adress; // okuma islemi yapilacak 
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,adress);
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1);
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,0x00);
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	GPIO_SetBits(GPIOE,GPIO_Pin_3); //CS pini lojik 1 e çekildi
	return SPI_I2S_ReceiveData(SPI1);
}

void SPI_Tx(uint8_t adress, uint8_t data)
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_3);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,adress);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,data);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
  GPIO_SetBits(GPIOE,GPIO_Pin_3);  
}
 

void delay(void)
{
	int i = 3000000;
	while(i--);
}

int main(void)
{
	
	uint8_t x = 0, y = 0, z = 0, who_am_i = 0;
	
	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef  	SPI_InitStructure;
	USART_InitTypeDef USART_InitStructure;	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE , ENABLE);



	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	
	USART_InitStructure.USART_BaudRate 						= 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 								= USART_Mode_Tx ;
	USART_InitStructure.USART_Parity							= USART_Parity_No;
	USART_InitStructure.USART_StopBits						= USART_StopBits_1;
	USART_InitStructure.USART_WordLength					= USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin		=	GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	=	GPIO_OType_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		=	GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	=	GPIO_OType_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/* SPI SCK-MOSI-MISO pin Ayarlari */
	GPIO_InitStructure.GPIO_Pin		=	GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType	=	GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* GPIO AF pin Ayarlari */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); // sck pin
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); // mosi pin
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); // miso pin
	/* SPI Ayarlari */
	SPI_InitStructure.SPI_BaudRatePrescaler	=	SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_Direction					=	SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode							=	SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize					= SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit					=	SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CPOL							=	SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA							=	SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS								=	SPI_NSS_Soft|SPI_NSSInternalSoft_Set;
	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	GPIO_SetBits(GPIOE,GPIO_Pin_3); //CS pini lojik 1 seviyesine çekildi
	
	sprintf(string_tx,"\nHello World from project %.1f \n This project about SPI and Motion sensor \n",6.0);
	USART_Puts(USART2,string_tx);
	
	SPI_Tx(0x20,0x67);  	//100 Hz , 9.2 G
	//SPI_Tx(0x20,0x47);  //100 Hz , 2.3 G
	//SPI_Tx(0x24,0x48);


	while(1)
	{

		delay();

		who_am_i 	= SPI_Rx(0x0F);
		x				 	=	SPI_Rx(0x29);
		y					=	SPI_Rx(0x2B);
		z					=	SPI_Rx(0x2D);

		sprintf(string_tx,"who_am_i = %d, x = %d , y = %d  z = %d\n", who_am_i , x, y, z);
		USART_Puts(USART2,string_tx);
		
		if(y >= 5 && y < 30)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_13); 		// orange
			GPIO_ResetBits(GPIOD,GPIO_Pin_15);	// blue
		}
		else if (y <= 250 && y > 200) 
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_15);		// blue
			GPIO_ResetBits(GPIOD,GPIO_Pin_13); 	// orange
		}

		 if (x >= 5 && x <30  ) 
		{
			GPIO_ResetBits(GPIOD,GPIO_Pin_12);	// green
			GPIO_SetBits(GPIOD,GPIO_Pin_14); 		// red
		}
		else if (x <= 250 && x > 200 ) 
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_12);		// green
			GPIO_ResetBits(GPIOD,GPIO_Pin_14); 	// red
		}

		if(y < 5 || y > 255)
		{
			GPIO_ResetBits(GPIOD,GPIO_Pin_15);	// blue
			GPIO_ResetBits(GPIOD,GPIO_Pin_13); 	// orange
		}
		
			if(x < 5 || x > 255)
		{
			GPIO_ResetBits(GPIOD,GPIO_Pin_12);	// green
			GPIO_ResetBits(GPIOD,GPIO_Pin_14); 	// red
		}
		
	}
}
