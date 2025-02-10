/**
  ******************************************************************************
  * @file    com_LoRa.h
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.20
  * @date    2018-12-14
  * @brief   LoRa USE SPI3
  ******************************************************************************
  * @note	2015.04.13

  */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __COM_LORA_H
#define __COM_LORA_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void LoRa_Init(void);
void LoRa_RcvProcess(void);
void LoRa_SlaveSend(void);  // 串口0从机发送
void LoRa_MasterSend(void); // 串口0主发送程序　

void LoRaSendTest(void);   // LoRa 发送测试
void LoRaRcvTest(void);    // LoRa 接收测试
void LoRaSendToDone(void); // LoRa 发送，并检测是否完成

void Jude_VVVF_Comm(void); // 检测变频器通信故障

#endif /* __COM_LORA_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
