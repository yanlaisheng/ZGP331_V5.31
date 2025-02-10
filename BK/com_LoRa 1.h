/**
  ******************************************************************************
  * @file    com_LoRa.h
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.10
  * @date    2015-04-13
  * @brief   Com1 USE USART2
	******************************************************************************
	*/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __com_LoRa_H
#define __com_LoRa_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LoRa_Init(void);							//LoRa参数Init
void LoRa(void);
void LoRaRcv(void);		// LoRa 接收


#endif /* __COM2_485_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


