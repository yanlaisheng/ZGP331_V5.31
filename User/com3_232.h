/**
 ******************************************************************************
 * @file    com3_232.h
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.27
 * @date    2014-01-03
 * @brief   USE USRAT3
 ******************************************************************************
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __COM3_232_H
#define __COM3_232_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Com3_Init(void);
void Com3_RcvProcess(void);
void Com3_SlaveSend(void);  // 串口0从机发送
void Com3_MasterSend(void); // 串口0主发送程序　

void At_QISENDAccordingLength(u8 LinkNo, uint Length);
void Gprs_TX_Byte(u8 i);
void Gprs_TX_String(u8 *String);
void Gprs_TX_Fill(uc8 *String, u8 Length);
void Gprs_TX_Start(void);
uchar Gprs_StrCmdLen(u8 *String);
uchar Bcd2Str(uint Val);
uint Str2Bcd(u8 *String);
uint Str2Hex(u8 *String);
uchar FindFirstValue(uc8 *Buffer, u8 FindValue, u8 EndValue);
void SavePar_Prompt(void);      // 保存参数+状态提示
void M35_LOOP(void);            // M35循环状态
void M35_Init(void);            // M35初始化
void Gprs_Send_ZhuCe(void);     // DTU 内容发送
void Gprs_HeartDelaySend(void); // Gprs心跳包的延时发送　2012.8.25
void Link_Function(void);
void Sms_Function(void);
void SmsAndPC_SetupInq(void); // 短信和PC设置和查询GPRS参数
void M35_Power_ONOFF(void);
u8 GPRSNetConfig(u8 link_num);
void SIM7600_LOOP(void);

uint8_t power_init(void);
uint8_t lte_init(void);
uint8_t lte_connect(void);
uint8_t lte_net_config(void);

void err_process(uint8_t err);
void lte_reset(void);
uint8_t find_string(char *p);
void lte_receive_process(void);
uint8_t lte_send_process(void);
uint8_t heard_beat_process(void);

void Com3_ReceiveData(void);
void DomainNameResolution(void);
#endif /* __COM3_232_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
