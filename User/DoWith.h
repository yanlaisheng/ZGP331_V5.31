/**
  ******************************************************************************
  * @file    DoWith.h
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.27
  * @date    2014-01-03
  * @brief   数字量输入检测，数字量输出,模拟量输入检测，模拟量输出,其他本机操作
	******************************************************************************
	*/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __DOWITH_H
#define __DOWITH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Variable_Init(void);			// 变量初始化
void ParLst_Init (void);			// RAM中参数表列 初始化 (读出)
void ParArrayWrite(u16 *p_Top,u16 *p_Base, uint w_WriteSize);
void ParArrayRead(uint *p_Top,uc16 *p_Base, uint w_ReadSize);
void GprsArrayRead(uchar *p_Top,uc8 *p_Base, uint w_ReadSize);		//ZCL 2019.4.4 uc16
void Boot_ParLst (void);			// 初始化设定参数
void SavePar_Prompt (void);		// 保存参数+状态提示
void ForceSavePar(void);			// 强制保存参数
void DoWith(void);						// 一些数据,记录的处理
void ParLimit(void);					// 参数限制
void DealPlayVoice(void);			// 放音 循环报警
void SPI_FLASH_Record (void);	// SPI-FLASH 记录函数
void Time_Output(void);				// 软件时钟输出	 2008.10.21
void GetLockCode(void);				// 2013.10.26 得到锁定密码
void Formula_12(u8 *D,u8 *Result);
void Formula_113(u8 *D,u8 *Result);

void Test(void); 		//输出 
void	ADC_DMA_Init(void); 

u32 FtoU32(u16 w1,u16 w2); 						//四字节的浮点数据转换为乘了10的长整形数据
#endif /* __DOWITH_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


