/**
  ******************************************************************************
  * @file    LCD.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.28
  * @date    2014-01-03
  * @brief   Code  for UG-2864KSWLG01 WiseChip 焊接接口，IC:SSD1306
  * 				 Hardware	: STM32F103(101) C8T6,CBT6
	* 				 Platform	: IAR6.40,Keil-MDK5.01,LIB:V3.5.0(11/03/2011),JLINK V6-V7
	******************************************************************************
  * @attention	2014.1.3
	******************************************************************************
  * @note	2014.4.7
	周成磊 2011.5.9
	1. 液晶RAM数据LSB在上，MSB在下
	2. 进行智晶OLED液晶1.3寸(UG-2864KSWLG01  WiseChip 焊接接口)显示操作的底层函数
	3. LCD12864_Num3()来显示数值，不同板子显示画面不一样，这个函数可能不一样。 ZCL 2018.5.19
	4. 高压电机温度可以显示负号		//ZCL 2019.10.18 显示个负号
	*/

/* Includes ------------------------------------------------------------------*/
#include "LCD.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include "DZ_Demo.h"	   //林戈智能液晶演示程序自带点阵   6x8 8X16 16x16 图片
#include "DZ_Cat_off.h"	   //猫闭眼点阵
#include "DZ_Cat_on.h"	   //猫睁眼点阵
#include "DZ_Number1607.h" //0123456789>+KPa .-   16*7点阵  16*8点阵
#include "DZ_Number1632.h" //0--9  16*32点阵
// #include <intrins.h>					//_crol_ 测试不好用

#define LCD_DELAY_TIME 25

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 周成磊 2014.4.23 选择不同的SEGBEGIN，决定不同的液晶屏
// #define	SEGBEGIN	0			//SSD1306  128*64
#define SEGBEGIN 2 // SH1106G  132*64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile unsigned long sysTickValue;
u8 Remainder; // 余数
u8 Quotient;  // 商数

u8 B_DispZero;

/* Private variables extern --------------------------------------------------*/
extern u8 S_ModParStatus;
extern u16 Lw_PageNo; // 显示的画面序号
extern u8 F_ModPar1;  // 修改参数标志1	 用来指定本页面那个参数要修改
/* Private function prototypes -----------------------------------------------*/
void IWDG_Feed(void); // 独立看门狗喂狗	 2017.6.19
/* Private functions ---------------------------------------------------------*/

/*********************LCD初始化************************************/

/********************************************************************************
							   变频电机液晶显示DCM200_SCR驱动程序
									   copyright @ 2014

编程: ZCL  ZYX
日期: 2014-09-25
*********************************************************************************/

/*******************************************************************************
函  数:	void LCD12864_Init(void)
参  数:	初始化液晶模块.
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-09-09
*********************************************************************************/
void LCD12864_Init(void)
{
	LCD12864_RST_L;			 // 复位：低电平
	LCD12864_DelayUS(20000); // 延时

	LCD12864_RST_H;			 // 结束复位：高电平。复位液晶
	LCD12864_DelayUS(20000); // 延时
	LCD12864_PSB_H;			 // LCD并口模式
	LCD12864_DelayUS(10000);

	LCD12864_WriteCommand(0x30); // 基本指令集
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x01); // 清零屏幕
	LCD12864_DelayUS(10000);

	LCD12864_WriteCommand(0x06); // 光标的移动方向
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x0C); // 开显示,关游标
	LCD12864_DelayUS(1000);
}

/********************************************************************************
函  数:	void LCD12864_DelayUS(unsigned int Dly)
参  数:
返  回:

编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
void LCD12864_DelayUS(unsigned int Dly)
{
	while (Dly--)
#ifdef GD32			 // ZCL 2021.12.22
		IWDG_Feed(); // ZCL 2021.10.25
#else
		;
#endif
	// KickDog();
	;
}

/********************************************************************************
函  数:	void LCD12864_WriteOneByte(u8 Data)
参  数:
返  回:

编  程: ZCL  ZYX
日  期: 2013-09-09
*********************************************************************************/
// ZCL 2015.8.30 按位写
void LCD12864_WriteOneByte(u8 Data)
{
	if (Data & 0x01)
		LCD12864_DATA0_H;
	else
		LCD12864_DATA0_L;

	if (Data & 0x02)
		LCD12864_DATA1_H;
	else
		LCD12864_DATA1_L;

	if (Data & 0x04)
		LCD12864_DATA2_H;
	else
		LCD12864_DATA2_L;

	if (Data & 0x08)
		LCD12864_DATA3_H;
	else
		LCD12864_DATA3_L;

	if (Data & 0x10)
		LCD12864_DATA4_H;
	else
		LCD12864_DATA4_L;

	if (Data & 0x20)
		LCD12864_DATA5_H;
	else
		LCD12864_DATA5_L;

	if (Data & 0x40)
		LCD12864_DATA6_H;
	else
		LCD12864_DATA6_L;

	if (Data & 0x80)
		LCD12864_DATA7_H;
	else
		LCD12864_DATA7_L;

	LCD12864_DelayUS(10);
}

/********************************************************************************
函  数:	u8 LCD12864_ReadData(void)
参  数:
返  回:

编  程: ZCL  ZYX
日  期: 2013-09-09
*********************************************************************************/
u8 LCD12864_ReadOneByte(void)
{
	u8 RData;

	LCD12864_WriteOneByte(0xFF);
	LCD12864_DelayUS(10);

	RData = 0x00;

	if (LCD12864_RDATA0)
		RData |= 0x01;
	if (LCD12864_RDATA1)
		RData |= 0x02;
	if (LCD12864_RDATA2)
		RData |= 0x04;
	if (LCD12864_RDATA3)
		RData |= 0x08;
	if (LCD12864_RDATA4)
		RData |= 0x10;
	if (LCD12864_RDATA5)
		RData |= 0x20;
	if (LCD12864_RDATA6)
		RData |= 0x40;
	if (LCD12864_RDATA7)
		RData |= 0x80;

	return RData;
}

/********************************************************************************
函  数:	u8 LCD12864_CheckBusy(void)
参  数:	读取液晶忙碌状态.
返  回:	1--忙碌;0--不忙碌.

编  程: ZCL  ZYX
日  期: 2013-09-09
**********************************************************************************/
u8 LCD12864_CheckBusy(void)
{
	LCD12864_RS_L; // RS=0;	 L-CMD;H-Data.
	LCD12864_RW_H; // RW=1;	 L-Write;H-Read.
	LCD12864_DelayUS(10);

	LCD12864_EN_H;					  // EN=1;     L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); // 延时一小段时间,等待总线数据稳定

	// ZCL 2015.8.30 原先这里面有重大错误，已经修改
	//	while(LCD12864_RDATA7==1);

	LCD12864_EN_L;
	LCD12864_DelayUS(LCD_DELAY_TIME);

	return 1;
}

/********************************************************************************
函  数:	void LCD12864_WriteCommand(u8 Data)
参  数:	写一个字节的"命令字"到液晶屏.
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-09-09
*********************************************************************************/
void LCD12864_WriteCommand(u8 myCMD)
{
	LCD12864_CheckBusy(); // 检查忙碌状态,并且等待

	LCD12864_RS_L;					  // RS=0;	 L-CMD;H-Data.
	LCD12864_RW_L;					  // RW=0;	 L-Write;H-Read.
	LCD12864_EN_L;					  // EN=0;     L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); // 延时

	LCD12864_WriteOneByte(myCMD);	  //
	LCD12864_DelayUS(LCD_DELAY_TIME); // 延时一小段时间,等待总线数据稳定

	LCD12864_EN_H;					  // EN=1;     L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); // 延时一段时间,等待数据被液晶屏接收

	LCD12864_EN_L;					  // EN=1;     L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); //

	LCD12864_DelayUS(10000); // 延时
}

/********************************************************************************
函  数:	void LCD12864_WriteData(u8 myData)
参  数:	写一个字节的"数据字"到液晶屏.
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-09-09
*********************************************************************************/
void LCD12864_WriteData(u8 myData)
{
	LCD12864_CheckBusy(); // 检查忙碌状态,并且等待

	LCD12864_RS_H;					  // RS=1;	 L-CMD;H-Data.  这里与写指令不同
	LCD12864_RW_L;					  // RW=0;	 L-Write;H-Read.
	LCD12864_EN_L;					  // EN=0;  L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); //

	LCD12864_WriteOneByte(myData);	  //
	LCD12864_DelayUS(LCD_DELAY_TIME); // 延时一小段时间,等待总线数据稳定

	LCD12864_EN_H;					  // EN=1;     L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); // 延时一段时间,等待数据被液晶屏接收

	LCD12864_EN_L;					  // EN=1;     L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); //
}

/********************************************************************************
函  数:	u8 LCD12864_ReadData(void)
参  数:	从液晶屏12864读取一个字节的数据.
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-09-09
*********************************************************************************/
u8 LCD12864_ReadData(void)
{
	u8 RData;

	// ZCL 不能加，加上出错！
	// LCD12864_CheckBusy();			 	//检查忙碌状态,并且等待

	LCD12864_RS_H;					  // RS=1;	 L-CMD;H-Data.
	LCD12864_RW_H;					  // RW=1;	 L-Write;H-Read.		高电平读
	LCD12864_EN_L;					  // EN=0;  L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); //

	LCD12864_EN_H;					  // EN=1;     L-Disable;H-Enable		先使能：高电平
	LCD12864_DelayUS(LCD_DELAY_TIME); // 延时一段时间,等待数据稳定

	RData = LCD12864_ReadOneByte(); // 读取数据

	LCD12864_EN_L;					  // EN=1;     L-Disable;H-Enable
	LCD12864_DelayUS(LCD_DELAY_TIME); //

	return RData;
}

/********************************************************************************
函  数:	void LCD12864_Clear(void)
参  数:	在绘图模式下清空屏幕. 指令0x01不能作用在绘图模式,只能作用在文本模式.
返  回:	无

	DB7   DB6   DB5   DB4   DB3   DB2   DB1   DB0
		L     L     H     DL    x     RE    x     x

	DL --  1--8BIT控制接口; 0--4BIT控制接口
	RE --  1--扩充指令集;   0--基本指令集

	绘图显示RAM 提供64x32个字节的记忆空间(由扩充指令设定绘图 RAM  地址)，
最多可以控制256x64 点的二维绘图缓冲空间，在更改绘图RAM时，由扩充指令
设定GDRAM 地址先设垂直地址再设水平地址(连续写入两个字节的数据来完成
垂直与水平的坐标地址)，再写入两个8位的资料到绘图RAM，而地址计数器（AC）
会自动加一,整个写入绘图RAM 的步骤如下：

其中需要注意的是,绘图RAM(GDRAM)的写入过程是:
	1.先将垂直的字节坐标(Y)写入绘图RAM地址.
	2.在将水平的字节坐标(X)写入绘图RAM地址.
	3.将D15~D8写入RAM中(写入第一个Bytes).
	4.将D7~D0写入RAM中(写入第二个Bytes).


编  程: ZCL  ZYX
日  期: 2013-09-09
*********************************************************************************/
void LCD12864_Clear(void) // ZCL ? 2015.8.30
{
	u8 i, j;

	LCD12864_WriteCommand(0x36); // 扩展指令集

	for (i = 0; i < 32; i++) //
	{
		LCD12864_WriteCommand(0x80 + i); // Y坐标
		LCD12864_WriteCommand(0x80);	 // X坐标

		for (j = 0; j < 16; j++) //
		{
			LCD12864_WriteData(0x00); // 第一个字节数据
			LCD12864_WriteData(0x00); // 第二个字节数据
		}
	}
}

/*******************************************************************************
函  数:	void LCD12864_Cursor(u8 myDIR)
功  能: LCD12864的光标控制函数,控制光标左右移动
参  数:	myDIR   0--光标左移;1--光标右移
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-11-20
********************************************************************************/
void LCD12864_Cursor(u8 myDIR)
{
	if (myDIR == 0)
	{
		LCD12864_WriteCommand(0x10); // 光标左移,AC自动减少1
	}
	else
	{
		LCD12864_WriteCommand(0x14); // 光标右移,AC自动增加1
	}
}

/********************************************************************************
函  数:	void LCD12864_SetPixelIndex(u8 x,u8 y,u8 Color)
参  数:	在LCD12864中的特定坐标上显示一个点.
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
void LCD12864_SetPixelIndex(u8 x, u8 y, u8 Color)
{
	u8 X_Byte;			 // X坐标
	unsigned int X_Bits; // 标记当前点处于x方向字节中的哪一位.相应的位会被置1.
	u8 Y_Byte;			 // Y坐标
	unsigned int PixData;

	// 	LCD12864_WriteCommand(0x34);	    //8Bit控制接口; 扩充指令集

	//--------------------------------------------------------------------------------
	// 数据是下半屏的
	if (y >= 32) //
	{
		Y_Byte = 0x80 + (y - 32); // 等效于:Y_Byte = 0x80+y%32
		X_Byte = 0x88 + x / 16;
		X_Bits = 0x8000 >> (x % 16); // 计算出当前的x坐标属于字节中的哪一位
	}
	//--------------------------------------------------------------------------------
	// 数据是上半屏的
	else //
	{
		Y_Byte = 0x80 + y; // 等效于:Y_Byte = 0x80+y%32
		X_Byte = 0x80 + x / 16;
		X_Bits = 0x8000 >> (x % 16); // 计算出当前的x坐标属于字节中的哪一位
	}

	//--------------------------------------------------------------------------------
	// 读取数据并处理,最后送LCD显示
	LCD12864_WriteCommand(Y_Byte); // 数据格式规定先写入Y地址
	LCD12864_WriteCommand(X_Byte); // 再写入X地址

	LCD12864_ReadData();		   // 第一次读取数据无效
	PixData = LCD12864_ReadData(); // 第二次读取有效.高位.
	PixData <<= 8;
	PixData |= LCD12864_ReadData(); // 第三次读取.低位.

	if (Color == 0) // 指令为清空某一位
		PixData = X_Bits & (~PixData);
	else // 指令为显示某一位
		PixData = X_Bits | PixData;

	LCD12864_WriteCommand(Y_Byte);
	LCD12864_WriteCommand(X_Byte);

	LCD12864_WriteData(PixData / 256);
	LCD12864_WriteData(PixData % 256);

	LCD12864_WriteCommand(0x36); // 开启绘图开关
	LCD12864_DelayUS(50);		 // 必须有这个延时
}

/********************************************************************************
函  数:	u8 LCD12864_PixGet(u8 x,u8 y)
参  数:	获取指定点的颜色值.
返  回:	0--当前点没有显示;1-当前点有显示.

编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
u8 LCD12864_GetPixelIndex(u8 x, u8 y)
{
	u8 X_Byte;			 // X坐标
	unsigned int X_Bits; // 标记当前点处于x方向字节中的哪一位.相应的位会被置1.
	u8 Y_Byte;			 // Y坐标
	unsigned int PixData;

	// 	LCD12864_WriteCommand(0x34);	    //8Bit控制接口; 扩充指令集

	//--------------------------------------------------------------------------------
	// 数据是下半屏的
	if (y >= 32) //
	{
		Y_Byte = 0x80 + (y - 32); // 等效于:Y_Byte = 0x80+y%32
		X_Byte = 0x88 + x / 16;
		X_Bits = 0x8000 >> (x % 16); // 计算出当前的x坐标属于字节中的哪一位
	}
	//--------------------------------------------------------------------------------
	// 数据是上半屏的
	else //
	{
		Y_Byte = 0x80 + y; // 等效于:Y_Byte = 0x80+y%32
		X_Byte = 0x80 + x / 16;
		X_Bits = 0x8000 >> (x % 16); // 计算出当前的x坐标属于字节中的哪一位
	}

	//--------------------------------------------------------------------------------
	// 读取数据并处理,最后送LCD显示
	LCD12864_WriteCommand(Y_Byte); // 数据格式规定先写入Y地址
	LCD12864_WriteCommand(X_Byte); // 再写入X地址

	LCD12864_ReadData();		   // 第一次读取数据无效
	PixData = LCD12864_ReadData(); // 第二次读取有效.高位.
	PixData <<= 8;
	PixData |= LCD12864_ReadData(); // 第三次读取.低位.

	if (PixData & X_Bits)
		return 1; //
	else
		return 0; //
}

/********************************************************************************
函  数: void LCD12864_TextMode(void)
功  能:
参  数:
返  回:	无


编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
void LCD12864_TextMode(void)
{
	LCD12864_WriteCommand(0x30); // 基本指令集
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x06); // 光标的移动方向
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x0C); // 开显示,关光标,光标位置不反白闪烁
	LCD12864_DelayUS(1000);
}

/********************************************************************************
函  数: void LCD12864_GraphMode(void)
功  能:
参  数:
返  回:	无


编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
void LCD12864_GraphMode(void)
{
	LCD12864_WriteCommand(0x30); // 基本指令集
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x06); // 光标的移动方向
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x0C); // 开显示,关游标
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x34); // 扩展指令集,8Bit控制接口
	LCD12864_DelayUS(1000);
}

/********************************************************************************
函  数: void LCD12864_ClrText(void)
功  能:
参  数:
返  回:	无


编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
void LCD12864_ClrText(void)
{
	LCD12864_WriteCommand(0x30); // 基本指令集
	LCD12864_DelayUS(1000);

	LCD12864_WriteCommand(0x01); // 清零屏幕
	LCD_DLY_ms(3);
}

/********************************************************************************
函  数:	void LCD12864_Char(unsigned int Add, char CH)
参  数:
返  回:	无
		在液晶制定位置显示一个字符.(char类型,并非汉字)

编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
void LCD12864_Char(unsigned int Add, char CH)
{
	LCD12864_WriteCommand(0x30); // 基本指令集

	LCD12864_WriteCommand(Add);
	LCD12864_WriteData(CH);
}

/********************************************************************************
函  数: void LCD12864_String(unsigned int Address, const char *Str)
功  能:	在液晶上显示字符串的函数.
参  数:	Address  --字符串显示开头地址
				*Str     --字符串首地址
返  回:	无
		在液晶制定位置显示一串字符.(char类型,并非汉字)

编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
void LCD12864_String(unsigned int Address, const char *Str)
{
	u8 i;

	LCD12864_WriteCommand(0x30); // 基本指令集

	LCD12864_WriteCommand(Address);

	for (i = 0; Str[i] != '\0'; i++)
		LCD12864_WriteData(Str[i]);

	LCD12864_DelayUS(500);
}

/********************************************************************************
函  数:	void LCD12864_Num(unsigned int Address, long Num)
功  能: 向液晶屏特定地址输出数字.
参  数: Address--要显示的数字的位置.
				Num------要显示的数字.
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
// ZCL 2019.4.4 连续使用时，要加点延时，否则出现过数字重叠。 加1MS延时 OK
void LCD12864_Num(unsigned int Address, long Num)
{
	int BitLength = 0; // 要显示的数字的位数
	int Wei[32];	   // 要显示的数的每一位
	long NumTmp;
	int i;

	LCD12864_WriteCommand(0x30); // 基本指令集

	if (Num >= 0)
		NumTmp = Num; //
	else
		NumTmp = -Num;

	if (Num != 0) //
	{
		for (i = 0; i < 10; i++) // 比如：把一个整数进行分解：如5181,分成:5,1,8,1 BitLength=4
		{
			Wei[i] = NumTmp % 10;
			NumTmp /= 10;
			BitLength++;

			if (NumTmp == 0)
				break;
		}

		BitLength--; // BitLength 减一，因为位数从0开始的

		LCD12864_WriteCommand(Address);

		if (Num < 0) // 如果是负数，先写负号。
		{
			LCD12864_WriteData('-');
		}
		// ZCL 2015.8.30 这个意思是取数字的ASCII码，送出显示！！OK
		for (i = BitLength; i >= 0; i--)
		{
			LCD12864_WriteData(Wei[i] + '0');
		}
	}
	// =0，直接在指点地址写'0'		ZCL
	else if (Num == 0)
	{
		LCD12864_WriteCommand(Address);
		LCD12864_WriteData('0');
	}
}

/********************************************************************************
函  数:void LCD12864_Num2(unsigned int Address, char myLen, long Num)
功  能: 向液晶屏特定地址输出数字.
参  数: Address--要显示的数字的位置.
				Num------要显示的数字.
返  回:	无

编  程: ZCL  ZYX
日  期: 2013-09-09
********************************************************************************/
// 这个调用后必须加延时！！！ 否则显示错乱。 ZCL 2015.8.31
void LCD12864_Num2(unsigned int Address, char myLen, long Num)
{
	//	int BitLength=0;  			//要显示的数字的位数
	int Wei[32]; // 要显示的数的每一位
	long NumTmp;
	int i;

	if (myLen == 0)
		return; // 数据长度是0.

	for (i = 0; i < 32; i++)
		Wei[i] = 0; //

	LCD12864_WriteCommand(0x30); // 基本指令集

	if (Num >= 0)
		NumTmp = Num; //
	else
		NumTmp = -Num;

	if (Num != 0) //
	{
		for (i = 0; i < 10; i++)
		{
			Wei[i] = NumTmp % 10;
			NumTmp /= 10;

			if (NumTmp == 0)
				break;
		}

		LCD12864_WriteCommand(Address);

		if (Num < 0) // 如果是负数，先写负号。
		{
			LCD12864_WriteData('-');
		}
		// ZCL 2015.8.30 这个意思是取数字的ASCII码，送出显示！！OK
		for (i = myLen - 1; i >= 0; i--)
		{
			LCD12864_WriteData(Wei[i] + '0');
		}
	}
	else if (Num == 0) // 要显示的数字是0.
	{
		LCD12864_WriteCommand(Address); // 地址

		for (i = 0; i < myLen; i++) // 可以指定显示几个0 ZCL
		{
			LCD12864_WriteData('0');
		}
	}
}

/********************************************************************************
函  数:void LCD12864_Num3(unsigned int Address, char AllLen, u8 XiaoShuBits, long Num, u8 mod)
功  能: 向液晶屏特定地址输出数字.
参  数: Address--要显示的数字的位置.
				Num------要显示的数字.
返  回:	无

编  程: ZCL
日  期: 2015
********************************************************************************/
// 这个调用后必须加延时！！！ 否则显示错乱。 ZCL 2015.8.31
void LCD12864_Num3(unsigned int Address, char AllLen, u8 XiaoShuBits, long Num, u8 mod)
{
	//	int BitLength=0;  			//要显示的数字的位数
	int Wei[32]; // 要显示的数的每一位
	long NumTmp;
	int i;
	// char m;
	char HighBit = 0;

	u8 nB_Sign; // ZCL 2019.10.18 符号标志

	if (mod >= 10)
	{
		mod = mod - 10;
		nB_Sign = 1;
	}
	else
		nB_Sign = 0;
	// ZCL 2019.10.18 符号标志
	if (AllLen == 0)
		return; // 数据长度是0.

	for (i = 0; i < 32; i++)
		Wei[i] = 0; //

	LCD12864_WriteCommand(0x30); // 基本指令集

	if (Num >= 0)
		NumTmp = Num; //
	else
		NumTmp = -Num;

	// if(Num != 0) 							//
	if (1) //
	{
		for (i = 0; i < 10; i++)
		{
			Wei[i] = NumTmp % 10;
			NumTmp /= 10;

			if (NumTmp == 0)
				break;
		}

		/* 		if(S_ModParStatus	)
				{
					LCD12864_WriteCommand(0x0f);	//开显示,开游标
				} */

		if (Pw_EquipmentType == 0) // 双驱泵
		{

			// ZCL Lw_PageNo==5，时间显示画面，特殊显示
			if (Lw_PageNo == TIMEPAGE || Lw_PageNo == DSP_TIMEPAGE) // 时间页
			{
				LCD12864_WriteCommand(Address - 1);
				if (mod)
				{
					LCD12864_WriteData(' ');
					LCD12864_WriteData('>');
				}
				else
				{
					LCD12864_WriteData(' ');
					LCD12864_WriteData(' ');
				}
			}
			else if (Lw_PageNo >= FAULTPAGE1 && Lw_PageNo <= FAULTPAGE1 + 6) // ZCL 2018.9.21
			{
				if (Lw_PageNo == FAULTPAGE1 + 6 && S_ModParStatus > 0 && F_ModPar1)
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, " >");
				}
				else if (Lw_PageNo == FAULTPAGE1 + 6 && F_ModPar1)
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, "  ");
				}
				else
					LCD12864_WriteCommand(Address); // Address--要显示的数字的位置.
			}

			// ZCL 2017.6.5 DCM100变频模式 DSP变频故障的页面
			else if (Lw_PageNo >= DSP_FAULTPAGE1 && Lw_PageNo <= DSP_FAULTPAGE1 + 6) // ZCL 2018.9.21
			{
				if (Lw_PageNo == DSP_FAULTPAGE1 + 6 && S_ModParStatus > 0 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, " >");
				}
				else if (Lw_PageNo == DSP_FAULTPAGE1 + 6 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, "  ");
				}
				else
					LCD12864_WriteCommand(Address);
			}

			// ZCL 2019.3.23 DCM220变频模式 DSP变频故障的页面
			else if (Lw_PageNo >= DSP_FAULTPAGE2 && Lw_PageNo <= DSP_FAULTPAGE2 + 6) // ZCL 2018.9.21
			{
				if (Lw_PageNo == DSP_FAULTPAGE2 + 6 && S_ModParStatus > 0 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, " >");
				}
				else if (Lw_PageNo == DSP_FAULTPAGE2 + 6 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, "  ");
				}
				else
					LCD12864_WriteCommand(Address);
			}

			// ZCL 2018.10.28  纬度和经度画面		LatLonPage
			else if (Lw_PageNo == LatLonPage_SQB || Lw_PageNo == LatLonPage2_SQB) // ZCL 2018.10.28
			{
				LCD12864_WriteCommand(Address);
				// if(mod)
				// LCD12864_WriteData('>');
				// else
				// LCD12864_WriteData(':');
			}

			// ZCL 2018.8.21 对画面显示内容进行限制  双水源   一个画面2个参数，参数占一行
			else if (Lw_PageNo >= TWO_WATER_SOURCE && Lw_PageNo <= TWO_WATER_SOURCE + 7)
			{
				LCD12864_WriteCommand(Address);
				if (mod)
					LCD12864_WriteData('>');
				else
					LCD12864_WriteData(' ');
			}

			// ZCL 2019.4.4 DTU ID 页面
			else if (Lw_PageNo == DTUIDPage_SQB)
			{
				LCD12864_WriteCommand(Address);
			}

			else
			{
				LCD12864_WriteCommand(Address);
				if (mod)
					LCD12864_WriteData('>');
				// ZCL 2019.10.18 显示个负号
				else if (nB_Sign == 1)
					LCD12864_WriteData('-');
				else
					LCD12864_WriteData(':');
			}

			if (Num < 0) // 如果是负数，先写负号。
			{
				LCD12864_WriteData('-');
			}

			// ZCL 2015.8.30 这个意思是取数字的ASCII码，送出显示！！OK
			// 限制作用
			if (AllLen < XiaoShuBits)
				AllLen = XiaoShuBits;

			// 显示长度==小数位数时的显示  如：0.300MPa
			if (AllLen == XiaoShuBits)
			{
				LCD12864_WriteData('0');
				LCD12864_WriteData('.');
				for (i = AllLen - 1; i >= 0; i--)
				{
					LCD12864_WriteData(Wei[i] + '0');
				}
			}
			// 显示长度>小数位数时的显示	  如：1.60MPa
			// 又分两种情况。XiaoShuBits==0  XiaoShuBits>0
			else if (XiaoShuBits == 0)
			{
				B_DispZero = 0;
				for (i = AllLen - 1; i >= 0; i--)
				{
					if (Wei[i] != 0 || i == 0)
						B_DispZero = 1;
					if (B_DispZero)
						LCD12864_WriteData(Wei[i] + '0');
					else
					{
						if (Lw_PageNo == DTUIDPage_SQB)
						{
							; // ZCL 2019.4.4 DTU ID的时候显示0
						}
						else
							LCD12864_WriteData(' '); // 不显示零
					}
				}
			}

			else if (XiaoShuBits > 0)
			{
				if (AllLen - XiaoShuBits > 1)
				{
					for (i = AllLen - 1; i >= XiaoShuBits; i--)
					{
						if (Wei[i] == 0 && HighBit == 0 && i + 1 - XiaoShuBits > 1)
							LCD12864_WriteData(' '); // 不显示零
						else
						{
							HighBit = 1;
							LCD12864_WriteData(Wei[i] + '0');
						}
					}
				}
				else
					LCD12864_WriteData(Wei[AllLen - 1] + '0');

				LCD12864_WriteData('.');
				for (i = XiaoShuBits - 1; i >= 0; i--)
				{
					LCD12864_WriteData(Wei[i] + '0');
				}
			}
		}
		else // 变频电机
		{

			// ZCL Lw_PageNo==5，时间显示画面，特殊显示
			if (Lw_PageNo == TIMEPAGE || Lw_PageNo == DSP_TIMEPAGE) // 时间页
			{
				LCD12864_WriteCommand(Address - 1);
				if (mod)
				{
					LCD12864_WriteData(' ');
					LCD12864_WriteData('>');
				}
				else
				{
					LCD12864_WriteData(' ');
					LCD12864_WriteData(' ');
				}
			}
			else if (Lw_PageNo >= FAULTPAGE1 && Lw_PageNo <= FAULTPAGE1 + 6) // ZCL 2018.9.21
			{
				if (Lw_PageNo == FAULTPAGE1 + 6 && S_ModParStatus > 0 && F_ModPar1)
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, " >");
				}
				else if (Lw_PageNo == FAULTPAGE1 + 6 && F_ModPar1)
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, "  ");
				}
				else
					LCD12864_WriteCommand(Address); // Address--要显示的数字的位置.
			}

			// ZCL 2017.6.5 DCM100变频模式 DSP变频故障的页面
			else if (Lw_PageNo >= DSP_FAULTPAGE1 && Lw_PageNo <= DSP_FAULTPAGE1 + 6) // ZCL 2018.9.21
			{
				if (Lw_PageNo == DSP_FAULTPAGE1 + 6 && S_ModParStatus > 0 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, " >");
				}
				else if (Lw_PageNo == DSP_FAULTPAGE1 + 6 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, "  ");
				}
				else
					LCD12864_WriteCommand(Address);
			}

			// ZCL 2019.3.23 DCM220变频模式 DSP变频故障的页面
			else if (Lw_PageNo >= DSP_FAULTPAGE2 && Lw_PageNo <= DSP_FAULTPAGE2 + 6) // ZCL 2018.9.21
			{
				if (Lw_PageNo == DSP_FAULTPAGE2 + 6 && S_ModParStatus > 0 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, " >");
				}
				else if (Lw_PageNo == DSP_FAULTPAGE2 + 6 && F_ModPar1) // ZCL 2018.9.21
				{
					LCD12864_WriteCommand(Address - 1);
					LCD12864_String(Address - 1, "  ");
				}
				else
					LCD12864_WriteCommand(Address);
			}

			// ZCL 2018.10.28  纬度和经度画面		LatLonPage
			else if (Lw_PageNo == LatLonPage_BPDJ || Lw_PageNo == LatLonPage2_BPDJ) // ZCL 2018.10.28
			{
				LCD12864_WriteCommand(Address);
				// if(mod)
				// LCD12864_WriteData('>');
				// else
				// LCD12864_WriteData(':');
			}

			// ZCL 2018.8.21 对画面显示内容进行限制  双水源   一个画面2个参数，参数占一行
			else if (Lw_PageNo >= TWO_WATER_SOURCE && Lw_PageNo <= TWO_WATER_SOURCE + 7)
			{
				LCD12864_WriteCommand(Address);
				if (mod)
					LCD12864_WriteData('>');
				else
					LCD12864_WriteData(' ');
			}

			// ZCL 2019.4.4 DTU ID 页面
			else if (Lw_PageNo == DTUIDPage_BPDJ)
			{
				LCD12864_WriteCommand(Address);
			}

			else
			{
				LCD12864_WriteCommand(Address);
				if (mod)
					LCD12864_WriteData('>');
				// ZCL 2019.10.18 显示个负号
				else if (nB_Sign == 1)
					LCD12864_WriteData('-');
				else
					LCD12864_WriteData(':');
			}

			if (Num < 0) // 如果是负数，先写负号。
			{
				LCD12864_WriteData('-');
			}

			// ZCL 2015.8.30 这个意思是取数字的ASCII码，送出显示！！OK
			// 限制作用
			if (AllLen < XiaoShuBits)
				AllLen = XiaoShuBits;

			// 显示长度==小数位数时的显示  如：0.300MPa
			if (AllLen == XiaoShuBits)
			{
				LCD12864_WriteData('0');
				LCD12864_WriteData('.');
				for (i = AllLen - 1; i >= 0; i--)
				{
					LCD12864_WriteData(Wei[i] + '0');
				}
			}
			// 显示长度>小数位数时的显示	  如：1.60MPa
			// 又分两种情况。XiaoShuBits==0  XiaoShuBits>0
			else if (XiaoShuBits == 0)
			{
				B_DispZero = 0;
				for (i = AllLen - 1; i >= 0; i--)
				{
					if (Wei[i] != 0 || i == 0)
						B_DispZero = 1;
					if (B_DispZero)
						LCD12864_WriteData(Wei[i] + '0');
					else
					{
						if (Lw_PageNo == DTUIDPage_BPDJ)
						{
							; // ZCL 2019.4.4 DTU ID的时候显示0
						}
						else
							LCD12864_WriteData(' '); // 不显示零
					}
				}
			}

			else if (XiaoShuBits > 0)
			{
				if (AllLen - XiaoShuBits > 1)
				{
					for (i = AllLen - 1; i >= XiaoShuBits; i--)
					{
						if (Wei[i] == 0 && HighBit == 0 && i + 1 - XiaoShuBits > 1)
							LCD12864_WriteData(' '); // 不显示零
						else
						{
							HighBit = 1;
							LCD12864_WriteData(Wei[i] + '0');
						}
					}
				}
				else
					LCD12864_WriteData(Wei[AllLen - 1] + '0');

				LCD12864_WriteData('.');
				for (i = XiaoShuBits - 1; i >= 0; i--)
				{
					LCD12864_WriteData(Wei[i] + '0');
				}
			}
		}
	}
}

void LCD_DLY_ms(u32 Time)
{
	u32 tick;
	tick = sysTickValue;
	/* Wait for specified Time */
	while ((u32)(sysTickValue - tick) < Time)
		;
}

/***************************************************************************/
