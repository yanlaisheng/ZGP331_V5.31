/**
  ******************************************************************************
  * @file    LCD.h
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.28
  * @date    2014-01-03
  * @brief   Code  for UG-2864KSWLG01 WiseChip 焊接接口，IC:SSD1306
  * 				 Hardware	: STM32F103(101) C8T6,CBT6
	* 				 Platform	: IAR6.40,Keil-MDK5.01,LIB:V3.5.0(11/03/2011),JLINK V6-V7	
	******************************************************************************
	*/
	
#ifndef __LCD_H__
#define __LCD_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LCD_DLY_ms(u32 time);
void LCD_WrDat(unsigned char dat);																//LCD写数据
void LCD_WrCmd(unsigned char cmd);																//LCD写命令
void LCD_Set_Pos(unsigned char x, unsigned char y);								//LCD 设置坐标
void LCD_Fill(unsigned char bmp_dat);															//LCD全屏
void LCD_CLS(void);																								//LCD复位
void LCD_Init(void);																							//LCD初始化
void LCD_P6x8Str(u8 x, u8 y, u8 ch[]);					//功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7
void LCD_P8x16Str(u8 x, u8 y, u8 ch[]);				//功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～7
void LCD_P16x16Ch(u8 x, u8 y, u8 N);													//功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7
void Draw_BMP(u8 x0, u8 y0, u8 x1, u8 y1, uc8 BMP[]);	//功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7

void Oled_display(uc8 picture[],u8 startpage,u8 endpage);		//可指定起始页，结束页
void Clear_screen(void);            //清屏幕 128*64
void Clear_page(u8 page); 	  			//清指定页函数
void Zhenkongdu_SelfCheck(void);		//真空度监测计时+数值显示  效果图见文件
void Show_cat(uc8 cat[]);     		  //被cat_offon调用
void Cat_offon(void);								//显示猫关，猫开(两幅图，猫动)
void Setcontrast(void);							//设置对比度 淡出
void Setcontrast2(void);						//设置对比度 淡入

//p:第*页  从指定页.指定行写指定汉字 16*16点阵 32字节
void HZ_1616(u8 p,u8 h,u8 l,u8 a);  //16*16点阵
//p:第*页  从指定页.指定行写指定长度的数组（数组16点阵，需要写2页，每页的长度为数组长度/2）
void HZ_16x(u8 p,u8 h,u8 l,uc8 *pic,u16 Arraylength);   //write 任意长度的字符图形（可指定页和位置）
//在制定页写.指定行写 24*24汉字 24*24点阵 72字节
void HZ_2424(u8 p,u8 h,u8 l,u8 a);    //24*24汉字    已经调用 p必须<=5
//在制定页写.指定行写 24*24汉字 24*24点阵 72字节  激光扫描书写
void HZ_2424Laser(u8 p,u8 h,u8 l,u8 a);    //24*24汉字    未调用 p必须<=5
//p:第*页  从指定页.指定行写指定数字 16*7点阵 14字节
void Num_1607(u8 p,u8 h,u8 l,u8 a);   //write 数字0123456789>+ 16*7点阵 14字节
//p:第*页  从指定页.指定行写指定字母 16*8点阵 16字节
void Num_1608(u8 p,u8 h,u8 l,u8 a);   //write 字母KPa     16*8点阵 16字节
/*write word p-page h-column high l-column lower a-word*/
//w_word p:第*页  从指定页.指定行写指定大数字 16*32点阵 64字节
void Num_1632(u8 p,u8 h,u8 l,u8 a);   


//LCD12864液晶屏控制线
//LCD_RS	 --	  PB11
//LCD_RW	 --	  PB12
//LCD_En	 --	  PB13
//LCD_PSB	 --	  PA12
//LCD_RST	 --	  PA13

#define  LCD12864_RS_H			GPIO_SetBits(GPIOB, GPIO_Pin_9)		//数据指令控制 RS  GPIO_Pin_11  ZCL 2018.10.11
#define  LCD12864_RS_L			GPIO_ResetBits(GPIOB, GPIO_Pin_9)		//

#define  LCD12864_RW_H			GPIO_SetBits(GPIOC, GPIO_Pin_4)		//读写控制 RW  ZCL 2018.12.3
#define  LCD12864_RW_L			GPIO_ResetBits(GPIOC, GPIO_Pin_4)		//

#define  LCD12864_EN_H			GPIO_SetBits(GPIOA, GPIO_Pin_7)		//使能引脚 EN ZCL 2018.11.30
#define  LCD12864_EN_L			GPIO_ResetBits(GPIOA, GPIO_Pin_7)		//

/* 2015.3.28 LCD_RST改成PA15；PA13用成SWDIO 周成磊 */
// #ifdef SWD
#define  LCD12864_RST_H			GPIO_SetBits(GPIOC, GPIO_Pin_2)		//复位引脚 RST	GPIO_Pin_15  ZCL 2018.10.11
#define  LCD12864_RST_L			GPIO_ResetBits(GPIOC, GPIO_Pin_2)		//
// #else
// #define  LCD12864_RST_H			GPIO_SetBits(GPIOA, GPIO_Pin_13)		//复位引脚 RST
// #define  LCD12864_RST_L			GPIO_ResetBits(GPIOA, GPIO_Pin_13)		//	
// #endif

#define  LCD12864_PSB_H			GPIO_SetBits(GPIOA, GPIO_Pin_12)		//串行并线选择 PSB
#define  LCD12864_PSB_L			GPIO_ResetBits(GPIOA, GPIO_Pin_12)		//

#define  LCD12864_DATA0_H		GPIO_SetBits(GPIOB, GPIO_Pin_14)		//
#define  LCD12864_DATA0_L		GPIO_ResetBits(GPIOB, GPIO_Pin_14)		//

#define  LCD12864_DATA1_H		GPIO_SetBits(GPIOB, GPIO_Pin_15)		//
#define  LCD12864_DATA1_L		GPIO_ResetBits(GPIOB, GPIO_Pin_15)		//

#define  LCD12864_DATA2_H		GPIO_SetBits(GPIOC, GPIO_Pin_6)			//
#define  LCD12864_DATA2_L		GPIO_ResetBits(GPIOC, GPIO_Pin_6)		//

#define  LCD12864_DATA3_H		GPIO_SetBits(GPIOC, GPIO_Pin_7)			//
#define  LCD12864_DATA3_L		GPIO_ResetBits(GPIOC, GPIO_Pin_7)		//

#define  LCD12864_DATA4_H		GPIO_SetBits(GPIOC, GPIO_Pin_8)			//
#define  LCD12864_DATA4_L		GPIO_ResetBits(GPIOC, GPIO_Pin_8)		//

#define  LCD12864_DATA5_H		GPIO_SetBits(GPIOC, GPIO_Pin_9)			//
#define  LCD12864_DATA5_L		GPIO_ResetBits(GPIOC, GPIO_Pin_9)		//

#define  LCD12864_DATA6_H		GPIO_SetBits(GPIOA, GPIO_Pin_8)			//
#define  LCD12864_DATA6_L 		GPIO_ResetBits(GPIOA, GPIO_Pin_8)		//

#define  LCD12864_DATA7_H		GPIO_SetBits(GPIOA, GPIO_Pin_15)		//ZCL 2018.11.30
#define  LCD12864_DATA7_L		GPIO_ResetBits(GPIOA, GPIO_Pin_15)		//



//数据线读取操作
//LCD_DATA0	 --	  PB14
//LCD_DATA1	 --	  PB15
//LCD_DATA2	 --	  PC6
//LCD_DATA3	 --	  PC7
//LCD_DATA4	 --	  PC8
//LCD_DATA5	 --	  PC9
//LCD_DATA6	 --	  PA8
//LCD_DATA7	 --	  PA11-->PA15 //ZCL 2018.11.30

#define  LCD12864_RDATA0		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)	//
#define  LCD12864_RDATA1		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)	//
#define  LCD12864_RDATA2		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)	//
#define  LCD12864_RDATA3		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)	//
#define  LCD12864_RDATA4		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)	//
#define  LCD12864_RDATA5		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)	//
#define  LCD12864_RDATA6		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)	//
#define  LCD12864_RDATA7		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)	//ZCL 2018.11.30




void LCD12864_DelayUS(unsigned int Dly);											 //

void LCD12864_WriteOneByte(unsigned char Data);										 //
unsigned char LCD12864_ReadOneByte(void);											 //

unsigned char LCD12864_CheckBusy(void);												 //

void LCD12864_WriteCommand(unsigned char myCMD);									 //
void LCD12864_WriteData(unsigned char myData);										 //

unsigned char LCD12864_ReadData(void);												 //
void LCD12864_Clear(void);															 //

void LCD12864_SetPixelIndex(unsigned char x, unsigned char y, unsigned char Color);	 //
unsigned char LCD12864_GetPixelIndex(unsigned char x, unsigned char y);				 //

void LCD12864_Init(void);															 //

void LCD12864_Cursor(unsigned char myDIR);											 //

void LCD12864_TextMode(void);														 //
void LCD12864_GraphMode(void);														 //
void LCD12864_ClrText(void);														 //

void LCD12864_Char(unsigned int Add, char CH);										 //
void LCD12864_String(unsigned int Address, const char *Str);							 //
void LCD12864_Num(unsigned int Address, long Num);									 //
void LCD12864_Num2(unsigned int Address, char myLen, long Num);
//void LCD12864_Num3(unsigned int Address, char myLen, u8 XiaoShuBits, long Num); //ZCL
void LCD12864_Num3(unsigned int Address, char myLen, u8 XiaoShuBits, long Num, u8 mod);
#endif

/* This is the end line of LCD.h */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
