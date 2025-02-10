/**
  ******************************************************************************
  * @file    com2_232.h
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.10
  * @date    2015-04-13
  * @brief   Com1 USE USART2
	******************************************************************************
	*/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __COM2_232_H
#define __COM2_232_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void GPIO_Com2_Configuration(void);							//GPIO配置 
void Com2_config(void);
void Com2_Init(void);
void Com2_RcvProcess(void);
void Com2_SlaveSend(void);			// 串口2从机发送
void Com2_MasterSend(void);			// 串口2主发送程序　



#endif /* __COM2_232_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


