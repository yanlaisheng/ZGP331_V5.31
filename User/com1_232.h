/**
 ******************************************************************************
 * @file    com1_232.h
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.10
 * @date    2015-04-13
 * @brief   Com1 USE USART2
 ******************************************************************************
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __COM1_232_H
#define __COM1_232_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void GPIO_Com1_Configuration(void); // GPIO配置
void Com1_config(void);
void Com1_Init(void);
void Com1_RcvProcess(void);
void Com1_SlaveSend(void);  // 串口0从机发送
void Com1_MasterSend(void); // 串口0主发送程序　
u16 CRC16(u8 *pCrcData, u8 CrcDataLen);

void Read_Stm32Data(u16 Reg, u16 Num);
void Write_Stm32Data(u16 Reg, u16 Data);
void Write_Stm32MulDatas(u16 Reg, u16 Num, u16 *Data);
u16 Position(u16 *p1, u16 *p2);
u16 Address(u16 *p, u16 Area);
u16 *AddressChange(void);        // ZCL 2018.12.14
u16 *AddressChange_2(u16 L_Reg); // YLS 2023.12.15
u16 *AddressConvert(u16 L_Reg);  // YLS 2023.12.15

void USART_putchar(USART_TypeDef *USART_x, unsigned char ch); // ZCL 2018.11.13

#endif /* __COM1_232_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
