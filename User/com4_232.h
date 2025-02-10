/**
  ******************************************************************************
  * @file    com4_232.h
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.27
  * @date    2014-01-03
  * @brief   COM0 USE USART1
	******************************************************************************
	*/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __COM4_232_H
#define __COM4_232_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Com4_Init(void);
void Com4_RcvProcess(void);
void Com4_SlaveSend(void);			// 串口0从机发送
void Com4_MasterSend(void);			// 串口0主发送程序　

void GPIO_Com4_Configuration(void);							//GPIO配置 
void Com4_config(void);

u16 CalcCheck(u8* Bytes,u16 len);
void Judge_Reset_GPS(void);
void PowerOnOff_GPS(void);

#endif /* __COM0_232_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


