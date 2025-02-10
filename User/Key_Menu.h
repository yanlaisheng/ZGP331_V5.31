/**
  ******************************************************************************
  * @file    Key_Menu.h
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.28
  * @date    2014-01-03
  * @brief   Code  for UG-2864KSWLG01 WiseChip 焊接接口，IC:SSD1306
  * 				 Hardware	: STM32F103(101) C8T6,CBT6
	* 				 Platform	: IAR6.40,Keil-MDK5.01,LIB:V3.5.0(11/03/2011),JLINK V6-V7	
	******************************************************************************
  * @attention	2014.1.3	
	******************************************************************************	
  * @note	2014.4.8
	周成磊 2011.5.9
	1. 液晶RAM数据LSB在上，MSB在下
	2. 进行智晶OLED液晶1.3寸(UG-2864KSWLG01  WiseChip 焊接接口)显示操作的底层函数
	*/

#ifndef __Key_Menu_H__
#define __Key_Menu_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Menu_Demo(void) ;
void Menu_Welcome(void); 
void Menu_Dynamic_TXT(void); 
void Menu_Feature(void) ;
void Menu_LaserTxt(void); 
void Menu_ReadPara1(void);			//画面 读参数1
void Menu_ReadPara2(void);			//画面 读参数2
void Menu_ReadPara3(void);			//画面 读参数3
void Menu_SetPara1(void);				//画面 设定参数1
void Menu_SetPara2(void);				//画面 设定参数2
void Menu_SetPara3(void);				//画面 设定参数3
void Menu_SetPara4(void);				//画面 设定参数4
void Menu_SetPara5(void);				//画面 设定参数5
void Menu_SetPara6(void);				//画面 设定参数6
void Menu_SetPara7(void);				//画面 设定参数7
void Menu_SetPara8(void);				//画面 设定参数8
void GetKey(void);							//ZCL 得到按键的键值 2013.12.07
void Menu_Change(void);					//画面菜单改变
void ModParNum(u8 num);			//指定:修改参数的数量
void FaultDisplay(u16 FaultValue , u8 Addr);		//故障显示		故障值，显示地址
void DspFaultDisplay(u16 FaultValue , u8 Addr);		//ZCL 2018.5.18
void Menu_Delay(void);				//画面延时

//p:页  x:列总坐标(h l 合起来的)(最低位)  w:要显示的参数  length:参数显示长度
//void DispMod_Par(u8 p,u8 x,u16 *Address,u8 length,u8 f_modpar);
//void DispMod_Par(u8 x_pos,u8 length,u16 *Address, u8 ParType, u8 f_modpar);
void DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits, u16 *Address, u8 ParType, u8 nb_modpar);

void Menu_Dsp1ReadPar1(void);			//画面 DSP1读参数1		//ZCL 2019.3.8
void Menu_Dsp1ReadPar2(void);			//画面 DSP1读参数2
void Menu_Dsp1ReadPar3(void);			//画面 DSP1读参数3
void Menu_Dsp2ReadPar1(void);			//画面 DSP2读参数1
void Menu_Dsp2ReadPar2(void);			//画面 DSP2读参数2
void Menu_Dsp2ReadPar3(void);			//画面 DSP2读参数3
void Menu_Dsp3ReadPar1(void);			//画面 DSP3读参数1
void Menu_Dsp3ReadPar2(void);			//画面 DSP3读参数2
void Menu_Dsp3ReadPar3(void);			//画面 DSP3读参数3
void Menu_Dsp4ReadPar1(void);			//画面 DSP4读参数1
void Menu_Dsp4ReadPar2(void);			//画面 DSP4读参数2
void Menu_Dsp4ReadPar3(void);			//画面 DSP4读参数3
void Menu_LoRaMasterSetPar1(void);	//画面 LoRa主机 DSP设定参数1 
void Menu_GpsReadPar1(void);		

void ReadBPDJPar(void);		//ZCL 2019.3.7 读变频电机常规参数！
void ReadBPDJPar2(void);	//ZCL 2019.4.3
void DispMod_Par2_String(unsigned int Address, const char *Str);
//ZCL 2019.4.5
void DispMod_Par3_Num(unsigned int Address, long Num);

void DispMod_Par2(u8 x_pos,u8 length, u8 XiaoShuBits, u32 l_value, u8 ParType, u8 nb_modpar);		//ZCL 2022.4.23
#endif

/* This is the end line of LCD.h */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
