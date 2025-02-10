/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       spi.c
 * \brief      SPI hardware driver
 *
 * \version    1.0
 * \date       Feb 12 2010
 * \author     Miguel Luis
 */
 
//#include "stm32f10x_spi.h"

#include "spi.h"


//修改成SPI3
#define SPI_INTERFACE                               SPI3				//ZCL 2018.11.9
#define SPI_CLK                                     RCC_APB1Periph_SPI3		//RCC_APB2Periph_SPI1

#define SPI_PIN_SCK_PORT                            GPIOB
#define SPI_PIN_SCK_PORT_CLK                        RCC_APB2Periph_GPIOB
#define SPI_PIN_SCK                                 GPIO_Pin_3

#define SPI_PIN_MISO_PORT                           GPIOB
#define SPI_PIN_MISO_PORT_CLK                       RCC_APB2Periph_GPIOB
#define SPI_PIN_MISO                                GPIO_Pin_4

#define SPI_PIN_MOSI_PORT                           GPIOB
#define SPI_PIN_MOSI_PORT_CLK                       RCC_APB2Periph_GPIOB
#define SPI_PIN_MOSI                                GPIO_Pin_5	//ZCL 2018.11.9


void SpiInit( void )
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

		//禁用JTAG
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );
    GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );
		
    /* Enable peripheral clocks --------------------------------------------------*/
    /* Enable SPIy clock and GPIO clock for SPIy */
    RCC_APB2PeriphClockCmd( SPI_PIN_MISO_PORT_CLK | SPI_PIN_MOSI_PORT_CLK |
                            SPI_PIN_SCK_PORT_CLK, ENABLE );
    //RCC_APB2PeriphClockCmd( SPI_CLK, ENABLE );  //ZCL 2018.11.9
		RCC_APB1PeriphClockCmd( SPI_CLK, ENABLE );		//ZCL 2018.11.9 注意SPI3使用APB1时钟

    /* GPIO configuration ------------------------------------------------------*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = SPI_PIN_SCK;
    GPIO_Init( SPI_PIN_SCK_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = SPI_PIN_MOSI;
    GPIO_Init( SPI_PIN_MOSI_PORT, &GPIO_InitStructure );

	
#ifdef GD32 
		GPIO_InitStructure.GPIO_Pin = SPI_PIN_MISO;		//MISO
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//ZCL 2021.5.4
		//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SPI_PIN_MISO_PORT, &GPIO_InitStructure);
		//GD32 ZCL 2021.5.4	
#else
    GPIO_InitStructure.GPIO_Pin = SPI_PIN_MISO;
    GPIO_Init( SPI_PIN_MISO_PORT, &GPIO_InitStructure );	
#endif



    /* SPI_INTERFACE Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		
#ifdef GD32 
		//ZCL 2021.5.4  在 GD32 的 SPI 的时钟信号，空闲状态需要配置成高电平，以保证数据的稳定性。 STM32的也是这样，没有改变！
		//ZCL 2021.10.26
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 		//SPI_CPOL_High;  0,0(模式0); 1,1(模式3)
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		//SPI_CPHA_2Edge;
#else
		//SPI_CPOL  SPI_CPHA  0,0(模式0); 1,1(模式3)
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;				//LORA
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
#endif

	
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 72/8 MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init( SPI_INTERFACE, &SPI_InitStructure );
    SPI_Cmd( SPI_INTERFACE, ENABLE );
}

uint8_t SpiInOut( uint8_t outData )
{
    /* Send SPIy data */
    SPI_I2S_SendData( SPI_INTERFACE, outData );
    while( SPI_I2S_GetFlagStatus( SPI_INTERFACE, SPI_I2S_FLAG_RXNE ) == RESET );
    return SPI_I2S_ReceiveData( SPI_INTERFACE );
}

