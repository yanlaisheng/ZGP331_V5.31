/**
 ******************************************************************************
 * @file    GlobalConst.h
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.27
 * @date    2014-01-03
 * @brief   全局变量声明
 ******************************************************************************
 */

#ifndef __GLOBALCONST_H
#define __GLOBALCONST_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private function prototypes -----------------------------------------------*/
void GPIO_PinReverse(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* 注意
	1. 所有的参数都是STM32 和 DSP 的， DCM200_SCR也已经有自己的分配参数区。（ZCL 2015.9.3 那是SCR没有）

	2. 随时注意数组的容量不能溢出，根据增加的变量增大，并修改相应的 ARM，DSP中容量！！！！！
	u16  	Pw_ParLst[500];				// 参数字列表区 设定
	u16  	w_ParLst[500];				// 参数字列表区 只读

	u16  	Pw_dspParLst[100];		// DSP参数字列表区 设定 2015.7.1
	u16  	w_dspParLst[150];			// DSP参数字列表区 只读 2015.7.1	ZCL 2018.9.21 数量100太少
	u16  	w_ScrParLst[30];			// ZCL 2018.8.3 SCR本身参数  			ZCL 2018.9.21 数量10太少

	2. ZCL 2019.4.3 备注
	DCM200_SCR_B 新程序，已经有了自己的参数区。w_ScrParLst

	3. ZCL 2019.4.4 备注
	DCM200_SCR_E(带LORA GPS GPRS) 新程序，已经有了自己的FM25L16，可以来保存自身参数。

	*/

/*
//ZCL 2015.06.09   已经在 stm32f10x.h中定义了。
typedef unsigned int 								u32;
typedef unsigned short int 					u16;
typedef unsigned char  							u8;

typedef const unsigned int 					uc32;
typedef const unsigned short int 		uc16;
typedef const unsigned char 					uc8;

typedef volatile unsigned short int vu16;
	*/

#define TRUE 1
#define FALSE 0
#define NULL 0
#define SPACE 0x20
#define TAB 0x09
#define QKEY 0x71

// #define	CRYSTAL	32

#define CS_FilterNum 32 //

#define FAULTPAGE1 10		 // 注意故障画面的键值定义
#define TIMEPAGE 17			 // 注意时间画面的键值定义
#define DSP_FAULTPAGE1 213	 // 注意故障画面的键值定义 DCM100 变频模式
#define DSP_FAULTPAGE2 255	 // 注意故障画面的键值定义 DCM220 变频模式
#define DSP_TIMEPAGE 220	 // 注意时间画面的键值定义
#define TWO_WATER_SOURCE 126 // 注意双水源画面的键值定义		ZCL 2018.8.21

#define LatLonPage_SQB 2000 // ZCL 2018.10.28 经纬度页面  218  2023.2.2
#define LatLonPage2_SQB 500 // ZCL 2018.10.28 经纬度页面2
#define DTUIDPage_SQB 551	// ZCL 2019.4.4 DTU ID 页面   519  2023.2.2

#define LatLonPage_BPDJ 218	 // ZCL 2018.10.28 经纬度页面
#define LatLonPage2_BPDJ 500 // ZCL 2018.10.28 经纬度页面2
#define DTUIDPage_BPDJ 531	 // ZCL 2019.4.4 DTU ID 页面 519

#define FLASH_PAR_SIZE 500 // NV参数数量

// 小密度，中密度型号FLASH每页 1K字节，高密度FLASH每页 2K字节
/*小容量产品 FLASH<=32K，			每个存储块划分为32个1K字节的页。  0x0800 0000-0x0800 7FFF
	中容量产品 64K<=FLASH<=128K，每个存储块划分为128个1K字节的页。0x0800 0000-0x0801 FFFF
	大容量产品 256K<=FLASH，		每个存储块划分为256个2K字节的页。 0x0800 0000-0x0807 FFFF
	互联型产品最大为32K×64位，每个存储块划分为128个2K字节的页。0x0800 0000-0x0803 FFFF */

/* //适用于STM32F10xCBT6 128K FLASH产品			2014.4.4
#define FLASH_PAR_ADDR		((u32)0x0801F000)		// NV参数地址 -0x0801F3FF		124K的位置
#define LOCKCODE_ADDR			((u32)0x0801F400)		// LOCKCODE参数地址 	125K的位置
				//0801F400:LOCKCODE起始地址		0801F410:滚码起始地址 */

/* 		//适用于STM32F10xC8T6 64K FLASH产品			2014.4.4
#define FLASH_PAR_ADDR		((u32)0x0800F000)		// NV参数地址 -0x0800F3FF		60K的位置
#define LOCKCODE_ADDR			((u32)0x0800F400)		// LOCKCODE参数地址 	61K的位置
				//0800F400:LOCKCODE起始地址		0800F410:滚码起始地址
#define SECCOUNT_ADDR					((u32)0x0800F800)		// 秒计数器地址 	62K的位置 */

#define LOCKCODE_ADDR ((u32)0x0803C000) // LOCKCODE参数地址 	240K的位置 -243K
// 0803C000:LOCKCODE起始地址		0803C010:滚码起始地址

#define FLASH_PAR_ADDR ((u32)0x0803D000) // ZCL 2018.12.10  SCR_E中没有使用
#define DTU_PAR_ADDR ((u32)0x0803D800)	 // ZCL 2018.12.10  SCR_E中没有使用

// ZCL 2018.8.3 定义FLASH参数保存的区域地址
/*大容量产品 256K<=FLASH，		每个存储块划分为256个2K字节的页。 0x0800 0000-0x0807 FFFF 	*/
// /* 0x0803F000 - 0x0803 FFFF		4K空间（252K-255K 位置）来保存 SCR的设定参数 */
// #define SCR_PAR_ADDR						((u32)0x0803F000)				// 地址-按键模式
// #define SCR_MENUMODE_ADDR			((u32)0x0803F002)				// 地址-菜单模式
// #define SCR_CURRENTBITS_ADDR		((u32)0x0803F004)				// 地址-电流小数

#define FORCESAVESECS 120 // 强制保存时间
#define NORCVMAXMS 5	  // 20 2007.7.5

#define TXD1_MAX 1536 // 最大发送数量
#define RCV1_MAX 2048 // 接收缓冲区长度 //256*8.5

#define TXD2_MAX 1536 // 最大发送数量
#define RCV2_MAX 2048 // 接收缓冲区长度 //256*8.5	数量很大，接收串口Xmodem设置参数用！ZCL 2019.3.22
#define TXD3_MAX 512  // 最大发送数量
#define RCV3_MAX 1536 // 接收缓冲区长度 //256*8.5	数量很大，接收GPRS网络数据用！ZCL 2019.3.14

#define TXD4_MAX 200 // 最大发送数量
#define RCV4_MAX 200 // 接收缓冲区长度 //256*8.5

// ZCL 2018.11.12
// #define BUFFER_SIZE 60 // Define the payload size here

// 把“位带地址＋位序号”转换别名地址宏
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
// 把该地址转换成一个指针
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
// IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) // 0x4001140C
/* #define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C     */

#define GPIOA_IDR_Addr (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) // 0x40011408
/* #define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08  */

// IO口操作,只对单一的IO口!
// 确保n的值小于16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // 输出
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)	 // 输入

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // 输出
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)	 // 输入

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // 输出
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)	 // 输入

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // 输出
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)	 // 输入

/* #define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入 */

#define RS485_CON PDout(3) // RS485控制端		//ZCL 2018.12.13  凑数临时的

// ZCL 2020.3.26 增加GPRS管脚的注释
// 1. 控制脚
#define PWR_KEY PBout(12)								// 模块开机或者关机脚，=1经过三极管出来低，模块开机，典型值500ms；=0，模块关机
#define RESET_4G PBout(13)								// 复位脚，=1经过三极管出来低，模块复位，典型值200ms，然后再=0
#define RESET_4G_ON GPIO_SetBits(GPIOB, GPIO_Pin_13)	// 开始复位
#define RESET_4G_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_13) // 停止复位
// 2. 输入脚。部分可以不使用
// DTE 准备就绪(将DTR管脚拉低可以唤醒模块.DTR管脚拉低20ms后,串口被激活)
// #define		DTR						PBout(3)		//ZCL 2018.12.3 凑数临时的		//ZCL 2020.3.26 这个脚今后可以取消
// #define		RING					PBin(4)			//ZCL 2018.12.3 凑数临时的		//ZCL 2020.3.26 这个脚今后可以取消
#define STATUS_4G PBin(8) // 状态输入脚 =1,亮；=0,灭			//ZCL 2020.3.26 这个脚程序使用了！

// 3. 对外的3个指示灯；因为灯不够，去掉了NET灯！ZCL 2020.3.26
#define PWR_LIGHT PCout(15) // ZCL 2018.12.3
#define COM2_DATA PCout(12) //=1,灭；=0,亮	就是：Data 指示灯 ！！！//ZCL 2018.12.3  （COM2口的USART2来 把已经连接上的网络跟用户串口进行数据交换）
// NET  后面用一个不存在的管脚代替了 //ZCL 2020.3.26

// 4. 控制4G模块的电源
#define POWERON_4G GPIO_ResetBits(GPIOC, GPIO_Pin_0) // 4G上电
#define POWEROFF_4G GPIO_SetBits(GPIOC, GPIO_Pin_0)	 // 4G断电

#define PWR_STA PCin(15)	  // ZCL 2013.9.2 本身输出用，这里做输入检测使用！！！//ZCL 2018.12.3
#define COM2_DATASTA PCin(12) // ZCL 2013.9.2 本身输出用，这里做输入检测使用！！！//ZCL 2018.12.3

// ZCL 2018.12.10  指示灯的反转
#define PWR_TOGGLE GPIO_PinReverse(GPIOC, GPIO_Pin_15)		 // ZCL 2018.12.10
#define COM2_DATA_TOGGLE GPIO_PinReverse(GPIOC, GPIO_Pin_12) // ZCL 2018.12.10

// ZCL 2018.12.13 因为指示灯不够用，这里NET由PA11定义成PD7，凑数用。 H2
// 实际NET灯来指示LORA -->LED0_TOGGLE PA11
#define NET PAout(11)								   // PAout(11)		//=1,灭；=0,亮		//ZCL 2018.12.3
#define NET_STA PAin(11)							   // PAin(11)		//ZCL 2013.9.2 本身输出用，这里做输入检测使用！！！//ZCL 2018.12.3
#define NET_TOGGLE GPIO_PinReverse(GPIOA, GPIO_Pin_11) // GPIO_PinReverse(GPIOA, GPIO_Pin_11)			//ZCL 2018.12.10

// 4. 对内的3个调试用的指示灯  ZCL 2020.3.26
// 1. NETLIGHT指示灯，没有引到CPU管脚，M35的输出脚（M35的NETLIGHT管脚）
// 2. STATUS_4G指示灯，引到CPU管脚，M35的输出脚
// 3. COM3_DATA指示灯，引到CPU管脚，由CPU来驱动。这里因为没脚了，用了个不存在的管脚，凑数用。
#define COM3_DATA PDout(5)									//=1,灭；=0,亮		//ZCL 2018.12.3  凑数临时的 （COM3口的USART3来 把CPU串口跟M35串口通讯，进行软件控制M35）
#define COM3_DATA_TOGGLE GPIO_PinReverse(GPIOD, GPIO_Pin_5) // ZCL 2020.3.26

// #define		COM0_TX				PAout(9)		//经过测试，COM1_LED输出模式，输出状态不能读取
// #define		COM0_RX				PAin(10)		//状态输入脚
// #define		COM1_TX				PBout(10)		//经过测试，COM1_LED输出模式，输出状态不能读取
// #define		COM1_RX				PAin(3)			//状态输入脚		//ZCL 2018.12.3

#define Qb_LcdBacklight PBout(0) //=1，亮

/* //DO 继电器输出
#define		Q1				PAout(15)			//=1，闭合

//继电器的输入检测
#define		Q1_in				PAin(15)		// */

// DI 开关量输入

// GPS的 PPS秒脉冲 GP_1PPS
#define GP_1PPS PCin(0) // GP_1PPS

// ZCL 2018.7.19  8个按键，正常按键顺序
#define KEY1 PAin(4) // STOPKEY
#define KEY2 PAin(5) // RUNKEY
// ZCL 2019.4.16 备注：DCM200_SCR_E中KEY3、KEY4、KEY5 被占用，GetKey()使用时要注释掉！
#define KEY3 PAin(6)   // LOCKKEY
#define KEY4 PAin(7)   // CHECKKEY
#define KEY5 PCin(4)   // SETKEY
#define KEY6 PCin(5)   // OKKEY
#define KEY7 PBin(1)   // RIGHTKEY
#define KEY8 PCin(13)  // DOWNKEY
#define KEY9 PCin(3)   // PBin(8)	//PBin(10) 		//LEFTKEY			ZCL 2018.10.18
#define KEY10 PCin(14) // PCin(12) 		//ESCKEY	ZCL 2018.12.05
#define KEY11 PBin(7)  // PCin(3)	//PCin(11) 		//UPKEY			ZCL 2018.10.18

#define STOPKEY 1
#define RUNKEY 2
#define LOCKKEY 3
#define CHECKKEY 4
#define SETKEY 5
#define OKKEY 6
#define RIGHTKEY 7
#define DOWNKEY 8
#define LEFTKEY 9
#define ESCKEY 10
#define UPKEY 11

/*
//7个按键
#define		DOWNKEY			11	//8  ZCL 2018.3.31  DOWNKEY键 UPKEY键 使用的IO跟 变频电机的不一样，这里修改
#define		LEFTKEY			5		//9  ZCL 2018.3.31  SETKEY键当成LEFTKEY键用，因为没有LEFTKEY键。没有RIGHTKEY按键。
#define		UPKEY				8		//11 ZCL 2018.3.31  UPKEY键 DOWNKEY键 使用的IO跟 变频电机的不一样，这里修改
		//变频电机 上升PC13，下降PC11；变频器上升PC11，下降PC13

//8个按键，上下键反着
#define		DOWNKEY			11			//8		ZCL 2018.5.21  新版8个键上下键反着
#define		UPKEY				8				//11	ZCL 2018.5.21  新版8个键上下键反着
	*/

/*
老液晶定义区 2013.12.4
#define		LCD_SCL				PBout(3)		//时钟 D0（SCLK）
#define		LCD_SDA				PBout(0)		//D1（MOSI） 数据
#define		LCD_RST				PAout(7)		//复位
#define		LCD_DC				PBout(1)		//数据/命令控制

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xCF
#define X_WIDTH 128
#define Y_WIDTH 64 */

// ZCL 2018.11.12 LORA SX1278-----------
// USART_BUAD_CONFIG
#define BAUD 115200
// USART_PRINTF_CONFIG
#define USED_COM_NUMBER USART1

// GPIO CONFIG			//ZCL 2018.11.9
#define RCC_LED_GPIOx RCC_APB2Periph_GPIOA
#define LED0_PINx GPIO_Pin_11 // ZCL 2018.12.13
#define LED0_GPIOx GPIOA
#define LED0_ON GPIO_SetBits(LED0_GPIOx, LED0_PINx)	   // ZCL 2018.11.9
#define LED0_OFF GPIO_ResetBits(LED0_GPIOx, LED0_PINx) // ZCL 2018.11.9
#define LED0_TOGGLE GPIO_PinReverse(LED0_GPIOx, LED0_PINx)

// ZCL 2018.11.9  再加1路LED
#define RCC_LED1_GPIOx RCC_APB2Periph_GPIOD
#define LED1_PINx GPIO_Pin_6 // ZCL 2018.12.13 凑数用的
#define LED1_GPIOx GPIOD
#define LED1_ON GPIO_SetBits(LED1_GPIOx, LED1_PINx)	   // ZCL 2018.11.9
#define LED1_OFF GPIO_ResetBits(LED1_GPIOx, LED1_PINx) // ZCL 2018.11.9
#define LED1_TOGGLE GPIO_PinReverse(LED1_GPIOx, LED1_PINx)
// ZCL 2018.11.12 LORA SX1278-----------

//---------------------------------2015.8.29 STM32 DSP参数
#define READSTM32DATA 1
#define WRITESTM32DATA 101
#define WRITESTM32MULDATA 201

#define MODPAR1 1
#define MODPAR2 2
#define MODPAR3 3
#define MODPAR4 4

#define DNB_ADDRESS 0			 // 读写参数地址；w_DNBParLst，访问时偏移0000访问  ZCL 2023.2.9
#define STM32_RPAR_ADDRESS 10000 // 只读参数地址；w_ParLst，访问时偏移10000访问
#define DSP_WRPAR_ADDRESS 20000	 // 读写参数地址；Pw_dspParLst，访问时偏移20000访问
#define DSP_RPAR_ADDRESS 30000	 // 只读参数地址；w_dspParLst，访问时偏移30000访问
#define SCR_WRPAR_ADDRESS 40000	 // 读写参数地址；w_ScrParLst，访问时偏移40000访问
// ZCL 2023.2.9
#define STM32_WRPAR_ADDRESS_SQB 50000 // 读写参数地址；Pw_ParLst，访问时偏移60000访问
#define STM32_WRPAR_ADDRESS_BPDJ 0	  // 读写参数地址；Pw_ParLst，访问时偏移60000访问

// 备注：这是ARM DCM100中FM25L16的分配
//-------特殊保存内容地址区；96个字节（可以整除32）
#define FMADD_FLASH_REC_NO 640	// FLASH记录序号
#define FMADD_FLASH_REC_NUM 642 // FLASH记录数量
#define FMADD_FAULT_REC_NUM 644 // 故障记录数量	定义在前，为了跟SM510对应起来
#define FMADD_FAULT_REC_NO 646	// 故障记录序号
#define FMADD_PUMP_RUN_TIME 648 //(15个字,30个字节)	 水泵运行时间
//-------
#define FMADD_STM32 0 // STM32设定参数保存地址 		存入FMRAM  0000-0639=640
// #define FMADD_FLASH_REC_NO	640			// FLASH记录序号  见上面
#define FMADD_DSP 736	 // DSP设定参数保存地址 			存入FMRAM  0736-1407=672
#define FMADD_FAULT 1408 // 故障内容记录地址 存入FMRAM  1408-1919=512
#define FMADD_CRC 1920	 // CRC内容记录地址 	存入FMRAM  1920-2039=60(1个字保存32个字节的CRC)

// ZCL 2019.4.4 FM25L16来保存参数，空间分配  2K字节 0-1023
// DCM200_SCR_E的FM25L16分配
// 1. 0-99 字节，50个字，保存SCR参数
// 2. 256 字节开始，288个字节（DTU_PAR_SIZE） 来保存 GPRS参数

// 软件时钟
#define SClk1Ms SoftClock[0]	// 软件时钟 1ms
#define SClk10Ms SoftClock[1]	// 软件时钟 10ms
#define SClkSecond SoftClock[2] // 软件时钟  s
#define SClkMinute SoftClock[3] // 软件时钟  m
#define SClkHour SoftClock[4]	// 软件时钟  h
#define SClkDay SoftClock[5]	// 软件时钟  d
#define SClkMonth SoftClock[6]	// 软件时钟  m
#define SClkYear SoftClock[7]	// 软件时钟  y
#define SClk0r5Ms SoftClock[8]	// 软件时钟  0.5MS	2010.8.6 定时中断中采集电流值

/* #define STM32_WRPAR_ADDRESS			0 			// 读写参数地址；Pw_ParLst，访问时偏移1000访问
#define STM32_RPAR_ADDRESS			10000 	// 只读参数地址；w_ParLst，访问时偏移1000访问
#define DSP_WRPAR_ADDRESS				20000 	// 只读参数地址；Pw_dspParLst，访问时偏移1000访问
#define DSP_RPAR_ADDRESS				30000 	// 只读参数地址；w_dspParLst，访问时偏移1000访问 */
// void	Define2_Stm32RWPar(void) {}
//------1.设定参数的定义---------------------------------------------------------------------
//  定义设定参数
#define Pw_OutPDecMax Pw_ParLst[0]		  //.3 出水表量程。初始化值1660，对应1.66MP		(小数位：.3)
#define Pw_InPDecMax Pw_ParLst[1]		  //.3 进水表量程。初始化值1000，对应1.00MP
#define Pw_SetP Pw_ParLst[2]			  //.3 设定的压力
#define Pw_HaveWaterRunP Pw_ParLst[3]	  //.3 有水开机压力(100，为1公斤)
#define Pw_NoWaterStopP Pw_ParLst[4]	  //.3 无水停机压力：初始化值3，对应0.003MP
#define Pw_HaveWaterRunDelay Pw_ParLst[5] //.0 有水开机延时：初始化值30秒
#define Pw_NoWaterStopDelay Pw_ParLst[6]  //.0 无水停机延时
#define Pw_VfToGpDelay Pw_ParLst[7]		  //.0 变频转工频延时时间
#define Pw_GpExitDelay Pw_ParLst[8]		  //.0 工频退出延时时间

#define Pw_MinRunFreqDec Pw_ParLst[10]	  //.1 频率运行下降限制值(约15HZ)
#define Pw_EquipmentNo Pw_ParLst[11]	  //.0 设备通讯地址
#define Pw_PumpNum Pw_ParLst[12]		  //.0 水泵数量
#define Pw_TimeChangeMin Pw_ParLst[13]	  //.0 定时交换分钟数
#define Pw_VvvfResetMax Pw_ParLst[14]	  //.0 变频器复位最大次数：初始化值5次
#define Pw_CheckCycleMinute Pw_ParLst[15] //.0 巡检周期时间（分钟数）
#define Pw_CheckRunFreq Pw_ParLst[16]	  //.1 巡检运行频率：初始化值200，对应约20.0Hz
#define Pw_CheckRunSec Pw_ParLst[17]	  //.0 巡检泵运行时间：初始化值30秒
#define Pw_EnableCheck Pw_ParLst[18]	  //.0 使能巡检功能
#define Pw_EnablePlayV Pw_ParLst[19]	  //.0 允许放音
#define Pw_BetweenPlaySec Pw_ParLst[20]	  //.0 放音间隔时间
#define Pw_VvvfComAddr Pw_ParLst[21]	  //.0 变频器通讯地址
#define Pw_LLJComAddr Pw_ParLst[22]		  //.0 流量计通讯地址

#define Pw_OverPThanSetPEk Pw_ParLst[24] //.3 超压设定偏差：初始化值200，对应0.200MP
//.0
#define Pw_VoltageMAX Pw_ParLst[25]		 //.1 系统电压最大值
#define Pw_1PumpCurrentMAX Pw_ParLst[26] //.1 1#泵电流最大值
#define Pw_2PumpCurrentMAX Pw_ParLst[27] //.1 2#泵电流最大值
#define Pw_3PumpCurrentMAX Pw_ParLst[28] //.1 3#泵电流最大值
#define Pw_4PumpCurrentMAX Pw_ParLst[29] //.1 4#泵电流最大值
#define Pw_5PumpCurrentMAX Pw_ParLst[30] //.1 5#泵电流最大值
#define Pw_YeWeiSensorMAX Pw_ParLst[31]	 //.2 液位传感器量程
#define Pw_YeWeiSensorZero Pw_ParLst[32] //.0 液位传感器初始值（4MA对应的值）
#define Pw_FluxMAX Pw_ParLst[33]		 //.1 瞬时流量最大值
//
#define Pw_EnableSmall Pw_ParLst[34]		 //.0 小流量保压功能使能
#define Pw_SmallHoldP Pw_ParLst[35]			 //.3 小流量保压值
#define Pw_InSmallFreqMinValue Pw_ParLst[36] //.1 小流量进入的最小频率值 (50)
#define Pw_SmallRiseSec Pw_ParLst[37]		 //.0 小流量上升时间(30)
#define Pw_BetweenSmallSec Pw_ParLst[38]	 //.0 小流量间隔时间 秒 (60秒)
#define Pw_ExitSmallEk Pw_ParLst[39]		 //.3 小流量退出偏差：初始化值3，对应0.03MP
#define Pw_SmallStablePEk Pw_ParLst[40]		 //.3 小流量恒压偏差  Invariable:恒定的
#define Pw_SmallJudgeDelay Pw_ParLst[41]	 //.0 小流量判断延时
// 42.43
// 44 音量大小控制
// 45--51 秒分时日月年 星期		//ZCL 2018.4.21备注：移到 w_SetSecond		w_ParLst[245]--[251]
#define Pw_VoiceValue Pw_ParLst[44] //.0

//
#define Pw_PID_Kc Pw_ParLst[52] //.3 设置PID比例系数
#define Pw_PID_Ts Pw_ParLst[53] //.3 设置PID采样时间
#define Pw_PID_Ti Pw_ParLst[54] //.3 设置PID积分时间
#define Pw_PID_Td Pw_ParLst[55] //.3 设置PID微分时间

#define Pw_Module1Add Pw_ParLst[57] //.0 SM510模块1地址       //子模块数量

#define Pw_Module2Add Pw_ParLst[59]		//.0 SM510模块2地址		//.0控制MD304L画面
#define Pw_InPBigSetDelay Pw_ParLst[60] //.0 自来水压力高延时
#define Pw_InPNormalDelay Pw_ParLst[61] //.0 自来水压力正常延时
#define Pw_InPNormalEk Pw_ParLst[62]	//.3 自来水压力正常偏差
#define Pw_InPType Pw_ParLst[63]		//.0 进水口压力传感器类型
#define Pw_OutPType Pw_ParLst[64]		//.0 出水口压力传感器类型
#define Pw_SoftOverPEn Pw_ParLst[65]	//.0 软件超压保护使能
//
#define Pw_EnableTime Pw_ParLst[66]			   //.0 使能定时启动停止功能
#define Pw_Time1Run Pw_ParLst[67]			   //.0 定时1启动
#define Pw_Time1Stop Pw_ParLst[68]			   //.0 定时1停止
#define Pw_Time1SetP Pw_ParLst[69]			   //.3 定时1设定压力
#define Pw_Time2Run Pw_ParLst[70]			   //.0 定时2启动
#define Pw_Time2Stop Pw_ParLst[71]			   //.0 定时2停止
#define Pw_Time2SetP Pw_ParLst[72]			   //.3 定时2设定压力
#define Pw_Time3Run Pw_ParLst[73]			   //.0 定时3启动
#define Pw_Time3Stop Pw_ParLst[74]			   //.0 定时3停止
#define Pw_Time3SetP Pw_ParLst[75]			   //.3 定时3设定压力
#define Pw_Time4Run Pw_ParLst[76]			   //.0 定时4启动
#define Pw_Time4Stop Pw_ParLst[77]			   //.0 定时4停止
#define Pw_Time4SetP Pw_ParLst[78]			   //.3 定时4设定压力
#define Pw_Time5Run Pw_ParLst[79]			   //.0 定时5启动
#define Pw_Time5Stop Pw_ParLst[80]			   //.0 定时5停止
#define Pw_Time5SetP Pw_ParLst[81]			   //.3 定时5设定压力
#define Pw_Time6Run Pw_ParLst[82]			   //.0 定时6启动
#define Pw_Time6Stop Pw_ParLst[83]			   //.0 定时6停止
#define Pw_Time6SetP Pw_ParLst[84]			   //.3 定时6设定压力
#define Pw_TimePwdStopPwd Pw_ParLst[85]		   //.0 定时密码停机口令
#define Pw_TimePwdStopMD Pw_ParLst[86]		   //.0 定时密码停机月日
#define Pw_InPSensorZero Pw_ParLst[87]		   //.0 进水口压力传感器初值
#define Pw_OutPSensorZero Pw_ParLst[88]		   //.0 出水口压力传感器初值
#define Pw_SmallNoStableSubOneEn Pw_ParLst[89] //.0 小流量不恒压减1使能

#define Pw_InPPidEkXiShu Pw_ParLst[90] //.0 ZCL 2017.8.17 改成进水压PID偏差系数，增大偏差，加快PID

#define Pw_NoWaterHz Pw_ParLst[91]			 //.1 无水停机运行频率 秦2017.8.4
#define Pw_NoWaterHzDownDelay1 Pw_ParLst[92] //.0无水降频延时  秦
#define Pw_NoWaterHzDownDelay2 Pw_ParLst[93] //.0 真正无水停机的延时

#define Pw_InPBigOutDelay Pw_ParLst[94] //.0 进水口大于出水口压力延时
#define Pw_MaxSupplyDelay Pw_ParLst[95] //.0 最大供水能力延时
#define Pw_InPBigSetEn Pw_ParLst[96]	//.0 进水口大于设定压力使能
#define Pw_InPBigOutEn Pw_ParLst[97]	//.0 进水口大于出水口压力使能
#define Pw_MaxSupplyEn Pw_ParLst[98]	//.0 最大供水能力使能
#define Pw_TimeChangeEn Pw_ParLst[99]	//.0 定时交换使能
//
#define Pw_MaxFreqHex Pw_ParLst[100]	 //.0 最大频率显示Hex值，校对频率
#define Pw_VADelay Pw_ParLst[101]		 //.0 电压电流显示延时MS，每隔此时间换算一次 2007.11.1
#define Pw_MaxPIDHex Pw_ParLst[102]		 //.0 最大PID运算Hex值，控制电压最大值，暂用
#define Pw_MaxAIHex Pw_ParLst[103]		 //.0 最大模拟输入Hex值，校对模拟输入，一般不用调整
#define Pw_AIFilterSetNum Pw_ParLst[104] //.0 模拟量过滤采样数目
#define Pw_DIStableSetNum Pw_ParLst[105] //.0 开关量稳定数目

// 106-110 Modem 删除

#define Pw_AlarmMask1 Pw_ParLst[111]	  //.0 报警屏蔽1
#define Pw_AlarmMask2 Pw_ParLst[112]	  //.0 报警屏蔽2
#define Pw_AlarmMask3 Pw_ParLst[113]	  //.0 报警屏蔽3
#define Pw_PowErrSwitchSel Pw_ParLst[114] //.0 电源故障开关选择  0=开点，1=闭点
#define Pw_EquipType Pw_ParLst[115]		  //.0 设备类型选择

#define Pw_VvvfResetSec Pw_ParLst[116]	  //.0 变频器报警复位时间
#define Pw_ClrVvvfResetSec Pw_ParLst[117] //.0 清除变频器已复位次数时间
#define Pw_MaxSupplyGpSum Pw_ParLst[118]  //.0 最大供水条件工频数量
#define Pw_MaxSupplyVfFreq Pw_ParLst[119] //.1 最大供水条件变频频率

#define Pw_TimeWriteRecSec Pw_ParLst[120]	  //.0 定时写记录时间
#define Pw_AheadDaysVoiceCount Pw_ParLst[121] //.0 提前天语音播放次数
#define Pw_MasterSlaveSel Pw_ParLst[122]	  //.0 主机从机选择
#define Pw_FanMotorOffDelay Pw_ParLst[123]	  //.0 机箱风扇断开延时
#define Pw_DownPID_Kc Pw_ParLst[124]		  //.3 设置下降PID比例系数
#define Pw_DownPID_Ts Pw_ParLst[125]		  //.3 设置下降PID采样时间
#define Pw_DownPID_Ti Pw_ParLst[126]		  //.3 设置下降PID积分时间
#define Pw_Time1ChangeHM Pw_ParLst[127]		  //.0 定时1交换 小时分钟
#define Pw_Time2ChangeHM Pw_ParLst[128]		  //.0 定时2交换 小时分钟
#define Pw_Time3ChangeHM Pw_ParLst[129]		  //.0 定时3交换 小时分钟
//
#define Pw_VfToGpEk Pw_ParLst[130]			  //.3 变频转工频 偏差
#define Pw_GpExitEk Pw_ParLst[131]			  //.3 工频退出  偏差
#define Pw_FluxBase Pw_ParLst[132]			  //.2 流量基值
#define Pw_MinFlux Pw_ParLst[133]			  //.2 最小流量
#define Pw_MaxFlux Pw_ParLst[134]			  //.2 最大流量
#define Pw_MinP Pw_ParLst[135]				  //.3 最小压力
#define Pw_MaxP Pw_ParLst[136]				  //.3 最大压力
#define Pw_BLBYEn Pw_ParLst[137]			  //.0 变量变压使能
#define Pw_VfToGpEn Pw_ParLst[138]			  //.0 变频转工频使能
#define Pw_CpuHotOffect Pw_ParLst[139]		  //.1 Cpu自热效应温度
#define Pw_PSensorProtectDelay Pw_ParLst[140] //.0 压力传感器保护延时 秒
#define Pw_BaudRate2 Pw_ParLst[141]			  //.0 软串口波特率
#define Pw_BaudRate0 Pw_ParLst[142]			  //.0 口0波特率
#define Pw_BaudRate1 Pw_ParLst[143]			  //.0 口1波特率
#define Pw_SmallBadClrSec Pw_ParLst[144]	  //.0 小流量破坏清除秒
//
#define Pw_SendToGprsEn Pw_ParLst[145]		  //.0 主动发送给GPRS返回数据使能
#define Pw_SendToGprsDataLen Pw_ParLst[146]	  //.0 主动发送给GPRS返回数据长度 字
#define Pw_SendToGprsSec Pw_ParLst[147]		  //.0 主动发送给GPRS返回数据间隔时间 秒
#define Pw_Com0BufType Pw_ParLst[148]		  //.0 通讯缓存类型(通讯协议类型) 普通=1,宁波=2
#define Pw_ASensorType Pw_ParLst[149]		  //.0 电流传感器类型
#define Pw_ASensorZero Pw_ParLst[150]		  //.0 电流传感器初值
#define Pw_VASumFilter Pw_ParLst[151]		  //.0 和滤波
#define Pw_DelayCheckVvvfAlarm Pw_ParLst[152] //.0 延时检测变频器报警 秒
#define Pw_BCQErrStopEN Pw_ParLst[153]		  //.0 补偿器预警停机使能  秦汉东 2017.6.3

#define Pw_ScrFastKeyBetweenMS Pw_ParLst[155] //.0快键键间隔MS  控制SCR里面的 ZCL 2015.9.6
#define Pw_ZDRunMode Pw_ParLst[156]			  //.0 定频运行模式
#define Pw_ZDRunHz Pw_ParLst[157]			  //.1 定频频率
#define Pw_SystemRunMode Pw_ParLst[158]		  //.0 系统运行模式: =1变频器; =0控制器(默认)
#define Pw_VfAlarmSubOneSecs Pw_ParLst[159]	  //.0 变频器报警减1次 秒 //ZCL 2018.6.13

// ZCL 2019.8.31 增加：供水变频模式
#define Pw_ISwitch1 Pw_ParLst[162] //.0 I_Switch1=0 控制器模式；I_Switch1=1 变频器模式

#define Pw_SoftOverPDelay Pw_ParLst[165] //.0软件超压延时

#define Pw_InP_CTLEn Pw_ParLst[166]		   //.0进水限制频率使能 2010.8.3 qhd
#define Pw_VfFreqDownSircle Pw_ParLst[167] //.0每秒递减频率值
#define Pw_VfFreqDownMin Pw_ParLst[168]	   //.1最小运行频率
#define Pw_SetInP Pw_ParLst[169]		   //.3进水设定压力

#define Pw_PumpSoftStopEn Pw_ParLst[170]   //.0设备软停使能
#define Pw_SoftStopDownDec Pw_ParLst[172]  //.1软停每秒递减频率值
#define Pw_FastKeyBetweenMS Pw_ParLst[173] //.0快键键间隔MS  控制SCR里面的 ZCL 2015.9.6
// 新加电能表参数  秦汉东2016.9.20
#define Pw_UnitSlect2 Pw_ParLst[174]  // 单位选择使能（1： 多少kwh代表1脉冲；2：多少脉冲代表1kWH）
#define Pw_PluseToDNB2 Pw_ParLst[175] // 多少KWH代表1脉冲
#define Pw_Clear2 Pw_ParLst[176]	  // 调整累积电量基值
#define Pw_PluseToDNB Pw_ParLst[177]  // 多少脉冲代表1kwh
#define Pw_SumDNBjz Pw_ParLst[178]	  // 预置脉冲低字
#define Pw_SumDNBjzH Pw_ParLst[179]	  // 预置脉冲高字

#define Pw_NoWaterReAlarmEn Pw_ParLst[182]		//.0无水停机预警功能使能
#define Pw_NoWaterReAlarmSetP Pw_ParLst[183]	//.3无水停机预警设定压力
#define Pw_NoWaterReAlarmDelay Pw_ParLst[184]	//.0无水停机预警延时
#define Pw_NoWaterNoReAlarmSetP Pw_ParLst[185]	//.3无水停机预警解除设定压力
#define Pw_NoWaterNoReAlarmDelay Pw_ParLst[186] //.0无水停机预警解除延时
#define Pw_PBoDongContlVfEn Pw_ParLst[187]		//.0压力波动限制设定压力使能
#define Pw_OutPDownEK Pw_ParLst[188]			//.3出水压降设定压差
#define Pw_SetPDownEK Pw_ParLst[189]			//.3 设定压力下降压差
#define Pw_PBoDongDelay Pw_ParLst[190]			//.0限制设定压力延时
#define Pw_PBoDongOKDelay Pw_ParLst[191]		//.0设定压力恢复延时

#define Pw_PresssureSteadyVfDownEn Pw_ParLst[192]	 //.0恒压降频使能	0
#define Pw_VfDownStartTime Pw_ParLst[193]			 //.0起始时间		00:30
#define Pw_VfDownStopTime Pw_ParLst[194]			 //.0结束时间		04:30
#define Pw_PresssureSteadyVfDownPEk Pw_ParLst[195]	 //.3恒压降频压差  0.020MPa
#define Pw_PresssureSteadyJudgeDelay Pw_ParLst[196]	 //.0恒压延时		120s
#define Pw_PresssureUnstablePEk Pw_ParLst[197]		 //.3不恒压压差
#define Pw_PresssureUnstableDelay Pw_ParLst[198]	 //.0不恒压延时
#define Pw_PressureSteadyVfDownMin Pw_ParLst[199]	 //.1恒压最小频率
#define Pw_PressureSteadyVfDownSircle Pw_ParLst[200] //.1恒压每秒降频率值
#define Pw_LowRunFreqDec Pw_ParLst[201]				 //.1最低运行频率值
#define Pw_SmallVfUpValue Pw_ParLst[202]			 //.1小流量处理频率上升值  秦2013.4.22

// 以下为新加参数  秦汉东2016.1.7
#define Pw_SlaveStartDelay Pw_ParLst[203] //.0从机启动延时
// 以下为新加参数  秦汉东2016.1.15
#define Pw_InOutPEqualEn Pw_ParLst[204] //.0进出水压力同步使能  秦 16.1.15
// 以下为流量计参数 秦2016.1.29

#define Pw_InstanFDecMax Pw_ParLst[205]		//.0流量计量程
#define Pw_InstanFSensorZero Pw_ParLst[206] //.0流量计初值
#define Pw_UnitSlect Pw_ParLst[207]			//.0单位选择使能（1： 多少立方米代表1脉冲；2：多少脉冲代表1立方）
#define Pw_PluseToFlux2 Pw_ParLst[208]		//.0多少立方米代表1脉冲
#define Pw_Clear Pw_ParLst[209]				//.0调整累积流量基值
#define Pw_PluseToFlux Pw_ParLst[210]		//.0多少脉冲代表1立方
#define Pw_SumFluxjz Pw_ParLst[211]			//.0预置脉冲低字
#define Pw_SumFluxjzH Pw_ParLst[212]		//.0预置脉冲高字
#define Pw_InstanFSensorType Pw_ParLst[213] //.0流量传感器类型(4-20mA;0-5V)
// 以下参数为进水变压变频参数 新加的
#define Pw_SlaveHaveWaterRunP Pw_ParLst[214]	 //.3 从机有水开机压力(100，为1公斤)
#define Pw_SlaveNoWaterStopP Pw_ParLst[215]		 //.3 从机无水停机压力：初始化值50，对应0.050MP
#define Pw_SlaveHaveWaterRunDelay Pw_ParLst[216] //.0 从机有水开机延时：初始化值5秒
#define Pw_SlaveNoWaterStopDelay Pw_ParLst[217]	 //.0 从机无水停机延时：初始化值5秒

#define Pw_HengYaLongDelay Pw_ParLst[218]	 //.0 恒压长延时
#define Pw_JiaYaShortDelay Pw_ParLst[219]	 //.0 加压短延时
#define Pw_InPChangeDelay Pw_ParLst[220]	 //.0 进水压变压 延时	//ZCL 2017.8.16 这个参数已经取消
#define Pw_InPChangeEk Pw_ParLst[221]		 //.3 PID 偏差  进水压变压偏差		进水压PID偏差	ZCL 2017.8.14
#define Pw_InPChangeOutPXiShu Pw_ParLst[222] //.0 变压系数 进水压改变出水压系数(倍数)
											 // #define	Pw_InPPidEkXiShu			Pw_ParLst[90]		//.0 进水PID偏差倍数 ZCL 2017.8.17 改成进水压PID偏差系数，增大偏差，加快PID
// 以下为新加参数 秦 16.4.18		2018.8.21
#define Pw_ChangeWaterSourceHour Pw_ParLst[223]	 // 自来水切换至水箱切换水源延时小时
#define Pw_DDFOpenDelay Pw_ParLst[224]			 // 电动阀打开最大延时
#define Pw_DDFCloseDelay Pw_ParLst[225]			 // 电动阀关闭最大延时
#define Pw_HaveWaterSel Pw_ParLst[226]			 // 水箱无水停机信号选择
#define Pw_ChangeWaterSourceHour2 Pw_ParLst[227] // 水箱切换至自来水切换水源延时小时
#define Pw_SelWaterToWater Pw_ParLst[228]		 // 定时定点交换选择

#define Pw_Time1HM Pw_ParLst[229] // 定点自来水切换至水箱1
#define Pw_Time2HM Pw_ParLst[230] // 定点自来水切换至水箱2
#define Pw_Time3HM Pw_ParLst[231] // 定点水箱切换至自来水1
#define Pw_Time4HM Pw_ParLst[232] // 定点水箱切换至自来水2
#define Pw_Test Pw_ParLst[233]	  // 车间调试选择  =1车间调试
#define Pw_Time5HM Pw_ParLst[234] // 定点自来水切换至水箱3
#define Pw_Time6HM Pw_ParLst[235] // 定点水箱切换至自来水3

// zcs2018.01.14
#define Pw_DDFManuAutoCtlEn Pw_ParLst[236] // 电动阀手动控制使能

// 注意：目前供水保存256个字参数，随参数增多，需要增加这个数值，一次增加一个区（32个字节，16个字）
//			DoWith.c 中ParLst_Init()		SavePar_Prompt()

/* // 定义模拟量端子配置参数：从地址240开始－地址254。共15个
 */
/* #define STM32_WRPAR_ADDRESS			0 			// 读写参数地址；Pw_ParLst，访问时偏移1000访问
#define STM32_RPAR_ADDRESS			10000 	// 只读参数地址；w_ParLst，访问时偏移1000访问
#define DSP_WRPAR_ADDRESS				20000 	// 只读参数地址；Pw_dspParLst，访问时偏移1000访问
#define DSP_RPAR_ADDRESS				30000 	// 只读参数地址；w_dspParLst，访问时偏移1000访问 */
// void	Define3_Stm32RPar(void) {}
//------2.只读参数的定义------------------------------------------------------------------------
//  定义只读参数：从地址0开始，但是访问时内部加1000来处理！ 周成磊 2015.6.28

#define w_Uk1 w_ParLst[0]			  //.0 频率输出Hex值
#define w_InPSensorValue w_ParLst[1]  //.0 进水口压力传感器读值
#define w_OutPSensorValue w_ParLst[2] //.0 出水口压力传感器读值
#define w_FaultRecNum w_ParLst[3]	  //.0 故障记录数量 ZCL 定义在前，为了跟SM510对应起来
#define w_FaultRecNo w_ParLst[4]	  //.0 故障记录号  ZCL

#define w_DelVvvfAlarmed w_ParLst[5] //.0 取消 检查变频报警 状态  ZCL		//ZCL 2022.10.3 =1 删除

#define w_ModPar w_ParLst[9]	  //.0 修改参数  ZCL 2015.9.9
#define w_ParInitial w_ParLst[10] //.0 参数初始化   ZCL 2015.9.9

#define w_FlashWrRdLock w_ParLst[11] //.0 Flash写读锁定

//.0#define	w_FlashRecNoNumPointerNo	w_ParLst[12]	//.0 FLASH记录序号数量的指针序号
#define w_SelFaultNo w_ParLst[13] //.0 选择故障号
#define w_SelRecNo w_ParLst[14]	  //.0 选择记录号
#define w_PreFaultNo w_ParLst[15] //.0 上次故障号
//
#define w_TestItemSel w_ParLst[16]		  //.0 测试项选择设定参数 (可以修改，不可以保存)
#define w_VvvfAlmNum w_ParLst[17]		  //.0 变频报警次数
#define w_BetweenSmall w_ParLst[18]		  //.0 小流量间隔状态
#define w_SmallStableRunSec w_ParLst[19]  //.0 小流量恒压走时
#define w_SoftVer w_ParLst[20]			  //.2 软件版本
#define w_TimePwdStopST w_ParLst[21]	  //.0 定时密码停机状态
#define w_EquipOperateStatus w_ParLst[22] //.0 设备操作状态
#define w_EquipOperateNum w_ParLst[23]	  //.0 设备操作状态数量
#define w_EquipAlarmStatus w_ParLst[24]	  //.0 设备停机原因（报警）
#define w_EquipStopNum w_ParLst[25]		  //.0 设备停机数量（报警）
//
#define w_EquipAlarmLast6 w_ParLst[26] //.0 设备故障停机原因上6次
#define w_EquipAlarm6YM w_ParLst[27]   //.0 设备故障停机6时间－年月
#define w_EquipAlarm6DH w_ParLst[28]   //.0 设备故障停机6时间－日时
#define w_EquipAlarm6MS w_ParLst[29]   //.0 设备故障停机6时间－分秒
#define w_EquipAlarmLast5 w_ParLst[30] //.0 设备故障停机原因上5次
#define w_EquipAlarm5YM w_ParLst[31]   //.0 设备故障停机5时间－年月
#define w_EquipAlarm5DH w_ParLst[32]   //.0 设备故障停机5时间－日时
#define w_EquipAlarm5MS w_ParLst[33]   //.0 设备故障停机5时间－分秒
#define w_EquipAlarmLast4 w_ParLst[34] //.0 设备故障停机原因上4次
#define w_EquipAlarm4YM w_ParLst[35]   //.0 设备故障停机4时间－年月
#define w_EquipAlarm4DH w_ParLst[36]   //.0 设备故障停机4时间－日时
#define w_EquipAlarm4MS w_ParLst[37]   //.0 设备故障停机4时间－分秒
#define w_EquipAlarmLast3 w_ParLst[38] //.0 设备故障停机原因上3次
#define w_EquipAlarm3YM w_ParLst[39]   //.0 设备故障停机3时间－年月
#define w_EquipAlarm3DH w_ParLst[40]   //.0 设备故障停机3时间－日时
#define w_EquipAlarm3MS w_ParLst[41]   //.0 设备故障停机3时间－分秒
#define w_EquipAlarmLast2 w_ParLst[42] //.0 设备故障停机原因上2次
#define w_EquipAlarm2YM w_ParLst[43]   //.0 设备故障停机2时间－年月
#define w_EquipAlarm2DH w_ParLst[44]   //.0 设备故障停机2时间－日时
#define w_EquipAlarm2MS w_ParLst[45]   //.0 设备故障停机2时间－分秒
#define w_EquipAlarmLast1 w_ParLst[46] //.0 设备故障停机原因上1次
#define w_EquipAlarm1YM w_ParLst[47]   //.0 设备故障停机1时间－年月
#define w_EquipAlarm1DH w_ParLst[48]   //.0 设备故障停机1时间－日时
#define w_EquipAlarm1MS w_ParLst[49]   //.0 设备故障停机1时间－分秒
//
#define w_SelEquipAlarm w_ParLst[50]	//.0 选择的设备故障停机原因
#define w_SelEquipAlarm1YM w_ParLst[51] //.0 选择的设备故障停机时间－年月
#define w_SelEquipAlarm1DH w_ParLst[52] //.0 选择的设备故障停机时间－日时
#define w_SelEquipAlarm1MS w_ParLst[53] //.0 选择的设备故障停机时间－分秒

#define w_FlashRecNo w_ParLst[54]	   //.0 FLASH记录号
#define w_FlashRecNum w_ParLst[55]	   //.0 FLASH记录数量
#define w_TimePwdStopDays w_ParLst[56] //.0 定时密码停机天数
#define w_RemoteStop w_ParLst[57]	   //.0 远程遥控变频停止 =1停机

#define w_RemoteVfRstRelayOn w_ParLst[59] //.0 遥控变频器复位继电器ON
#define w_TouchAutoManu w_ParLst[60]	  //.0 触摸 自动/手动
#define w_TouchRunStop w_ParLst[61]		  //.0 触摸 启动/停止
#define w_TouchDDFOnOFF w_ParLst[62]	  // 触摸 开关电动阀 zcs 2018.01.14

//.0#define	w_RunTimeSumNo				w_ParLst[67]	//.0 水泵累计运行指针序号
#define w_SaveRecNo w_ParLst[68]		 //.0 保存Flash记录号
#define w_SaveRecNum w_ParLst[69]		 //.0 保存Flash记录数量
#define w_CpuTemperatureHex w_ParLst[70] //.0 Cpu温度Hex
#define w_CpuWendu w_ParLst[71]			 //.1 Cpu温度换算值
#define w_SelPar w_ParLst[72]			 //.0 选择参数
#define w_SelParValue w_ParLst[73]		 //.0 选择参数的值

// 秦汉东新加参数 2016.3.5
#define w_FluxPluseOnL w_ParLst[74] //.0累计脉冲低字
#define w_FluxPluseOnH w_ParLst[75] //.0累计脉冲高字

// 秦汉东新加参数 2016.12.20
#define w_DNBPluseOnL w_ParLst[76] // 累脉冲低字
#define w_DNBPluseOnH w_ParLst[77] // 累计脉冲高字

// 远程监控集中查询常用参数 32个
#define w_ProcessNo w_ParLst[80] //.0 过程序号
// 远程监控集中查询常用参数 31个字
#define w_Pump12Status w_ParLst[81] //.0 泵12状态
#define w_Pump34Status w_ParLst[82] //.0 泵34状态			w_dspSysErrorCode1
#define w_Pump56Status w_ParLst[83] //.0 泵56状态			w_dspOutVoltage
#define w_Flag1Unit w_ParLst[84]	//.0 标志1单元
#define w_Flag2Unit w_ParLst[85]	//.0 标志2单元
#define w_Flag3Unit w_ParLst[86]	//.0 标志3单元
#define w_ResidualCL w_ParLst[87]	//.0 余氯 /标志4单元
#define w_PIDCalcP w_ParLst[88]		//.3 PID运算压力
#define w_VvvfFreq w_ParLst[89]		//.1 变频器频率
#define w_InPDec w_ParLst[90]		//.2 进水口压力
#define w_OutPDec w_ParLst[91]		//.2 出水口压力
#define w_InstanFlux w_ParLst[92]	//.2 瞬时流量		//ZCL 2007.6.15
#define w_Pump1Current w_ParLst[93] //.1 1号泵电流
#define w_Pump2Current w_ParLst[94] //.1 2号泵电流
#define w_Pump3Current w_ParLst[95] //.1 3号泵电流
#define w_Pump4Current w_ParLst[96] //.1 4号泵电流
#define w_Pump5Current w_ParLst[97] //.1 5号泵电流	w_dspNowTemperature
#define w_YeWeiDeep w_ParLst[98]	//.2 液位深度
#define w_SysVoltage w_ParLst[99]	//.1 系统电压		//ZCL 2007.6.15  w_dspBusBarVoltage

#define w_SumFluxL w_ParLst[100]  //.0 累计流量低字(小端模式：这就是低字2011.7.25)
#define w_SumFluxH w_ParLst[101]  //.0 累计流量高字(小端模式：这就是高字2011.7.25)
#define w_DDFStatus w_ParLst[102] // 1#电动阀状态 秦16.4.18
// #define	w_DDF2OpenValue				w_ParLst[103]		// 2#电动阀开度
// #define	w_DDF3OpenValue				w_ParLst[104]		// 3#电动阀开度
#define w_SumDNBL w_ParLst[103]		  // 累计电量低字(小端模式：这就是低字2011.7.25)
#define w_SumDNBH w_ParLst[104]		  // 累计电量高字(小端模式：这就是高字2011.7.25)
#define w_DDF4OpenValue w_ParLst[105] //.0 4#电动阀开度
#define w_DDF5OpenValue w_ParLst[106] //.0 5#电动阀开度		w_dspOutVoltage
#define w_PHValue w_ParLst[107]		  //.0 PH值
#define w_ZhuoDuValue w_ParLst[108]	  //.0 浊度
// 3 年月日时分秒 368 369 370
#define w_NowYM w_ParLst[109] //.0 年月
#define w_NowDH w_ParLst[110] //.0 日时
#define w_NowMS w_ParLst[111] //.0 分秒

// 记录显示参数  20060812 周成磊
#define w_RecProcessNo w_ParLst[120]	//.0 过程序号  0
#define w_RecPump12Status w_ParLst[121] //.0 泵12状态
#define w_RecPump34Status w_ParLst[122] //.0 泵34状态
#define w_RecPump56Status w_ParLst[123] //.0 泵56状态
#define w_RecFlag1Unit w_ParLst[124]	//.0 标志1单元
#define w_RecFlag2Unit w_ParLst[125]	//.0 标志2单元
#define w_RecFlag3Unit w_ParLst[126]	//.0 标志3单元
#define w_RecResidualCL w_ParLst[127]	//.0 余氯 /标志4单元
#define w_RecPIDCalcP w_ParLst[128]		//.3 PID运算压力 8
#define w_RecVvvfFreq w_ParLst[129]		//.1 变频器频率
#define w_RecInPDec w_ParLst[130]		//.2 进水口压力
#define w_RecOutPDec w_ParLst[131]		//.2 出水口压力
#define w_RecSysVoltage w_ParLst[132]	//.1 系统电压    //ZCL 2007.6.15
#define w_RecInstanFlux w_ParLst[133]	//.2 瞬时流量
#define w_RecPump1Current w_ParLst[134] //.1 1号泵电流
#define w_RecPump2Current w_ParLst[135] //.1 2号泵电流
#define w_RecPump3Current w_ParLst[136] //.1 3号泵电流
#define w_RecPump4Current w_ParLst[137] //.1 4号泵电流
#define w_RecPump5Current w_ParLst[138] //.1 5号泵电流
#define w_RecYeWeiDeep w_ParLst[139]	//.2 液位深度    18　　

#define w_RecSumFluxL w_ParLst[141] //.0 累计流量低字
#define w_RecSumFluxH w_ParLst[142] //.0 累计流量高字 21
// 7  143 144 145 146 147 148 149  22-28  1#电动阀开度-5#电动阀开度  PH值 浊度
// 3 年月日时分秒 150 151 152      29-31
//
#define w_RecPump1ManuAuto w_ParLst[153] //.0 泵1 手动/自动
#define w_RecPump1GpVf w_ParLst[154]	 //.0 泵1工频，变频，故障，停止
#define w_RecPump2ManuAuto w_ParLst[155] //.0 泵2 手动/自动
#define w_RecPump2GpVf w_ParLst[156]	 //.0 泵2工频，变频，故障，停止
#define w_RecPump3ManuAuto w_ParLst[157] //.0 泵3 手动/自动
#define w_RecPump3GpVf w_ParLst[158]	 //.0 泵3工频，变频，故障，停止
#define w_RecPump4ManuAuto w_ParLst[159] //.0 泵4 手动/自动
#define w_RecPump4GpVf w_ParLst[160]	 //.0 泵4工频，变频，故障，停止
#define w_RecPump5ManuAuto w_ParLst[161] //.0 泵5 手动/自动
#define w_RecPump5GpVf w_ParLst[162]	 //.0 泵5工频，变频，故障，停止
//
#define w_Pump1ManuAuto w_ParLst[163] //.0 泵1 手动/自动
#define w_Pump1GpVf w_ParLst[164]	  //.0 泵1工频，变频，故障，停止
#define w_Pump2ManuAuto w_ParLst[165] //.0 泵2 手动/自动
#define w_Pump2GpVf w_ParLst[166]	  //.0 泵2工频，变频，故障，停止
#define w_Pump3ManuAuto w_ParLst[167] //.0 泵3 手动/自动
#define w_Pump3GpVf w_ParLst[168]	  //.0 泵3工频，变频，故障，停止
#define w_Pump4ManuAuto w_ParLst[169] //.0 泵4 手动/自动
#define w_Pump4GpVf w_ParLst[170]	  //.0 泵4工频，变频，故障，停止
#define w_Pump5ManuAuto w_ParLst[171] //.0 泵5 手动/自动
#define w_Pump5GpVf w_ParLst[172]	  //.0 泵5工频，变频，故障，停止
// 173-187 原先泵1-泵5 累计运行时间
#define w_Pump1RunSecond w_ParLst[173] //.0 泵1运行秒
#define w_Pump1RunHour w_ParLst[174]   //.0 泵1运行小时
// 187

#define w_TSensorValue w_ParLst[188] //.0 温度传感器检测值

#define w_KglInNum w_ParLst[189]	 //.0
#define w_KglOutNum w_ParLst[190]	 //.0
#define w_KglInStatus w_ParLst[191]	 //.0
#define w_KglOutStatus w_ParLst[192] //.0
#define w_WriteDate w_ParLst[193]	 //.0程序编写日期
#define w_Writetime w_ParLst[194]	 //.0程序编写时间

#define w_AQ1 w_ParLst[202] //.0 模拟量输出值1
#define w_AQ2 w_ParLst[203] //.0 模拟量输出值2
#define w_AI1 w_ParLst[204] //.0 模拟量输入值1
#define w_AI2 w_ParLst[205] //.0
#define w_AI3 w_ParLst[206] //.0
#define w_AI4 w_ParLst[207] //.0
#define w_AI5 w_ParLst[208] //.0
#define w_AI6 w_ParLst[209] //.0
#define w_AI7 w_ParLst[210] //.0
#define w_AI8 w_ParLst[211] //.0 模拟量输入值8

#define w_ReadDspOKCount w_ParLst[212]		//.0 Com1 03指令发出，正确返回次数
#define w_WriteDspOKCount w_ParLst[213]		//.0 Com1 06指令发出，正确返回次数
#define w_ReadDspErrCount w_ParLst[214]		//.0 读DSP错误次数
#define w_WriteDspErrCount w_ParLst[215]	//.0 写DSP错误次数
#define w_RdDspNo w_ParLst[216]				//.0 读Dsp 序号
#define w_WrDspNo w_ParLst[217]				//.0 写Dsp 序号
#define w_ResetedDspCount w_ParLst[218]		//.0 已经复位DSP的次数
#define w_ResetDspDelayCount w_ParLst[219]	//.0 复位DSP延时计数器（复位前的延时）
#define w_ResetDspDelayCount2 w_ParLst[220] //.0 复位DSP延时计数器2（复位前的延时）

#define w_ManMadedspFault w_ParLst[221] //.0 人为制造故障 测试用 ZCL 2018.6.5
#define w_DspStopNum w_ParLst[222]		//.0 DSP停机数量 2018.8.27
#define w_DspStopStatus w_ParLst[223]	//.0 DSP停机原因 2018.8.27

#define w_SaveBaudRate0 w_ParLst[230] //.0 波特率保存值0
#define w_SaveBaudRate1 w_ParLst[231] //.0 波特率保存值1
#define w_SaveBaudRate2 w_ParLst[232] //.0 波特率保存值2
#define w_SaveBaudRate3 w_ParLst[233] //.0 波特率保存值3

//.0#define	w_SystemRunMode  					w_ParLst[234]	  // 变频器模式	-- 高电平

#define w_ModDspParAdd w_ParLst[236]   //.0 修改DSP参数的地址
#define w_ModDspParValue w_ParLst[237] //.0 修改DSP参数的值

#define w_DspOperateStatus w_ParLst[238] //.0 DSP操作状态
#define w_DspOperateNum w_ParLst[239]	 //.0 DSP操作状态数量
#define w_DspAlarmStatus w_ParLst[240]	 //.0 DSP停机原因（报警）
#define w_DspAlarmNum w_ParLst[241]		 //.0 DSP停机数量（报警）

#define w_FmramErrL w_ParLst[242] //.0 Fmram 读写错误次数 L
#define w_FmramErrH w_ParLst[243] //.0 Fmram 读写错误次数 H

#define w_SetSecond w_ParLst[245] //.0 设置秒
#define w_SetMinute w_ParLst[246] //.0 设置分
#define w_SetHour w_ParLst[247]	  //.0 设置时
#define w_SetDay w_ParLst[248]	  //.0 设置日
#define w_SetMonth w_ParLst[249]  //.0 设置月
#define w_SetYear w_ParLst[250]	  //.0 设置年
#define w_SetWeek w_ParLst[251]	  //.0 设置星期

#define w_AutoAssignTime w_ParLst[252] //.0自动分配时间
#define w_SmallVfUpValue w_ParLst[253] //.0小流量处理时频率每秒上升值  秦 2013.4.22
// 新加 秦16.4.18
#define w_ChangeWaterRunningHour w_ParLst[254]	// 切换水源自来水已运行的时间
#define w_ChangeWaterRunningHour2 w_ParLst[255] // 切换水源水箱已运行的时间

// #define	w_TextWriteYear				w_ParLst[253]		//程序编写日期		ZCL 2016.10.06
// #define	w_TextWriteDate				w_ParLst[254]		//程序编写时间

// ZCL 2019.10.19  YW310参数
//(在YW310中的定义，改成在DCM220_SCR_E中定义，地址调整)
//  #define	Pw_FWenDecMax						w_ParLst[1]		// 负温度量程。初始化值50，对应50度			ZCL 2017.11.17
//  #define	Pw_ZWenDecMax						w_ParLst[2]		// 正温度量程。初始化值100，对应100度	ZCL 2017.11.17
//  #define	Pw_PressureWenDuSel			w_ParLst[3]		// 压力温度选择	ZCL 2017.11.17 =0压力；=1温度
//  #define	Pw_PressureWenDuXiShu		w_ParLst[4]		// 压力温度系数	ZCL 2017.12.16

// #define	Pw_ModPar								w_ParLst[9]		// 修改参数
// #define	Pw_ParInitial						w_ParLst[23]	// 参数初始化
// #define	Pw_EquipmentNo					w_ParLst[11]	// 设备通讯地址
// #define	w_SetEquipmentNoST			w_ParLst[156]	// 设定地址状态 ZCL 2019.10.19  在此状态了，改了设备地址，也还能通讯上。
// #define	w_Counter							  w_ParLst[157]	// ZCL 2019.10.19 计数器，通讯时观察用。YW310 有H1通讯指示灯
// #define		w_SoftVer							w_ParLst[142]		// 软件版本
// #define	Pw_MaxAIHex			 				w_ParLst[54]	// 最大模拟输入Hex值，校对模拟输入，一般不用调整
// #define	Pw_AI1SensorZero				w_ParLst[60]		// AI1传感器初值

#define Pw_YW310FWenDecMax w_ParLst[256]		 // 负温度量程。初始化值50，对应50度			ZCL 2017.11.17
#define Pw_YW310ZWenDecMax w_ParLst[257]		 // 正温度量程。初始化值100，对应100度	ZCL 2017.11.17
#define Pw_YW310PressureWenDuSel w_ParLst[258]	 // 压力温度选择	ZCL 2017.11.17 =0压力；=1温度
#define Pw_YW310PressureWenDuXiShu w_ParLst[259] // 压力温度系数	ZCL 2017.12.16

#define Pw_YW310ModPar w_ParLst[260]		  // 修改参数
#define Pw_YW310ParInitial w_ParLst[261]	  // 参数初始化
#define Pw_YW310EquipmentNo w_ParLst[262]	  // 设备通讯地址
#define w_YW310SetEquipmentNoST w_ParLst[263] // 设定地址状态 ZCL 2019.10.19  在此状态了，改了设备地址，也还能通讯上。
#define w_YW310Counter w_ParLst[264]		  // ZCL 2019.10.19 计数器，通讯时观察用。YW310 有H1通讯指示灯
#define w_YW310SoftVer w_ParLst[265]		  // 软件版本

#define Pw_YW310MaxAIHex w_ParLst[266]		// 最大模拟输入Hex值，校对模拟输入，一般不用调整
#define Pw_YW310AI1SensorZero w_ParLst[267] // AI1传感器初值

// 跟上面用同样的地址
#define Pw_ZM220Current1 w_ParLst[266]	// 高压电机电流 ZCL 2022.3.15
#define Pw_ZM220HotMinute w_ParLst[267] // 加热分钟数 ZCL 2022.3.15

// ZCL 2019.10.19  SZM220 LORA参数
//  #define	Pw_ModPar										w_ParLst[9]		// 修改参数
//  #define	Pw_ParInitial								w_ParLst[23]	// 参数初始化
//  #define	Pw_LoRaEquipmentNo					w_ParLst[212]		// LORA设备通讯地址 ZCL 2018.12.13
//  #define	Pw_LoRaMasterSlaveSel				w_ParLst[213]		// 主机从机选择
//  #define	Pw_ZM220LoRaSet							w_ParLst[218]		//.0 LoRa 设定  LORA SX1278
// LORA 发射的设定参数 LoRaSettings  sx1276-LoRa.c
// #define	Pw_LoRaSetFreq							w_ParLst[226]	// 设置 4350000
// #define	Pw_LoRaSetPower							w_ParLst[227]	// 设置 20
// #define	Pw_LoRaSetSignalBW					w_ParLst[228]	// 设置 0-9
// #define	Pw_LoRaSetSpreadingFactor		w_ParLst[229]	// 设置 6-12
// #define	Pw_LoRaSetErrorCoding				w_ParLst[230]	// 设置 1-4
// #define	Pw_LoRaSetCrcOn							w_ParLst[231]	// 0-1
// #define	Pw_LoRaSetImplicitHeaderOn	w_ParLst[232]	// 0-1
// #define	Pw_LoRaSetRxSingleOn				w_ParLst[233]	// 0-1
// #define	Pw_LoRaSetFreqHopOn					w_ParLst[234]	// 0
// #define	Pw_LoRaSetHopPeriod					w_ParLst[235]	//
// #define	Pw_LoRaSetPayLoadLength			w_ParLst[236]	//
// #define	Pw_LoRaSetTxPacketTimeOut		w_ParLst[237]	//.0 //ZCL 2019.9.24 再重新加上
// #define	Pw_LoRaSetRxPacketTimeOut		w_ParLst[238]	//.0 //ZCL 2019.9.24 再重新加上
// #define	w_LoRaDateRate							w_ParLst[501]	//.2 空中速率	 kbps
// #define	w_Counter										w_ParLst[641]		//.0 计数器
// #define	w_SoftVer										w_ParLst[640]		//.2 软件版本
#define Pw_ZM220ModPar w_ParLst[260]	 // 修改参数
#define Pw_ZM220ParInitial w_ParLst[261] // 参数初始化

#define Pw_ZM220LoRaEquipmentNo w_ParLst[268]		  // LORA设备通讯地址 ZCL 2018.12.13
#define Pw_ZM220LoRaMasterSlaveSel w_ParLst[269]	  // 主机从机选择
#define Pw_ZM220LoRaSet w_ParLst[270]				  //.0 LoRa 设定  LORA SX1278
													  // LORA 发射的设定参数 LoRaSettings  sx1276-LoRa.c
#define Pw_ZM220LoRaSetFreq w_ParLst[271]			  // 设置 4350000
#define Pw_ZM220LoRaSetPower w_ParLst[272]			  // 设置 20
#define Pw_ZM220LoRaSetSignalBW w_ParLst[273]		  // 设置 0-9
#define Pw_ZM220LoRaSetSpreadingFactor w_ParLst[274]  // 设置 6-12
#define Pw_ZM220LoRaSetErrorCoding w_ParLst[275]	  // 设置 1-4
#define Pw_ZM220LoRaSetCrcOn w_ParLst[276]			  // 0-1
#define Pw_ZM220LoRaSetImplicitHeaderOn w_ParLst[277] // 0-1
#define Pw_ZM220LoRaSetRxSingleOn w_ParLst[278]		  // 0-1
#define Pw_ZM220LoRaSetFreqHopOn w_ParLst[279]		  // 0
#define Pw_ZM220LoRaSetHopPeriod w_ParLst[280]		  //
#define Pw_ZM220LoRaSetPayLoadLength w_ParLst[281]	  //
#define Pw_ZM220LoRaSetTxPacketTimeOut w_ParLst[282]  //.0 //ZCL 2019.9.24 再重新加上
#define Pw_ZM220LoRaSetRxPacketTimeOut w_ParLst[283]  //.0 //ZCL 2019.9.24 再重新加上
#define w_ZM220LoRaDateRate w_ParLst[286]			  //.2 空中速率	 kbps
#define w_ZM220Counter w_ParLst[287]				  //.0 计数器
#define w_ZM220SoftVer w_ParLst[288]				  //.2 软件版本

//
// ZCL 2019.3.20说明
// w_ScrGpsLatAllDu1 和 w_ScrGpsLatAllDu2 合起来是纬度
// w_ScrGpsLonAllDu1 和 w_ScrGpsLonAllDu2 合起来是经度
#define w_ScrGpsLatAllDu1 w_ParLst[300 + 0 * 24 + 0 - 4] //.2 纬度 度1； 如：35.44			2位小数
#define w_ScrGpsLatAllDu2 w_ParLst[300 + 0 * 24 + 0 - 3] //.0 纬度 度2；	00.00****
														 //****部分，添加到w_ScrGpsLatAllDu1后面。 如：****是 2459， 则总纬度是： 35.442459
#define w_ScrGpsLonAllDu1 w_ParLst[300 + 0 * 24 + 0 - 2] //.2 经度 度1；	如：127.39		2位小数
#define w_ScrGpsLonAllDu2 w_ParLst[300 + 0 * 24 + 0 - 1] //.0 经度 度2； 00.00****
														 //****部分，添加到w_ScrGpsLonAllDu1后面。如：****是 4628， 则总经度是： 127.394628

// ZCL 2019.10.15 高压电机采集
// ZCL 2019.9.28
// 电机1
#define w_HighV1SoftVersion w_ParLst[300 + 0 * 24 + 0]	  // 版本
#define w_HighV1Counter w_ParLst[300 + 0 * 24 + 1]		  // 计数器
#define w_HighV1WenDu1DecValue w_ParLst[300 + 0 * 24 + 2] //.1 温度值1
#define w_HighV1WenDu2DecValue w_ParLst[300 + 0 * 24 + 3] //.1 温度值2
#define w_HighV1WenDu3DecValue w_ParLst[300 + 0 * 24 + 4] //.1 温度值3
#define w_HighV1WenDu4DecValue w_ParLst[300 + 0 * 24 + 5] //.1 温度值4
#define w_HighV1WenDu5DecValue w_ParLst[300 + 0 * 24 + 6] //.1 温度值5
#define w_HighV1WenDu6DecValue w_ParLst[300 + 0 * 24 + 7] //.1 温度值6

#define w_HighV1DianLi1 w_ParLst[300 + 0 * 24 + 8]	 //.x 电力参数1
#define w_HighV1DianLi2 w_ParLst[300 + 0 * 24 + 9]	 //.x 电力参数1
#define w_HighV1DianLi3 w_ParLst[300 + 0 * 24 + 10]	 //.x 电力参数1
#define w_HighV1DianLi4 w_ParLst[300 + 0 * 24 + 11]	 //.x 电力参数1
#define w_HighV1DianLi5 w_ParLst[300 + 0 * 24 + 12]	 //.x 电力参数1
#define w_HighV1DianLi6 w_ParLst[300 + 0 * 24 + 13]	 //.x 电力参数1
#define w_HighV1DianLi7 w_ParLst[300 + 0 * 24 + 14]	 //.x 电力参数1
#define w_HighV1DianLi8 w_ParLst[300 + 0 * 24 + 15]	 //.x 电力参数1
#define w_HighV1DianLi9 w_ParLst[300 + 0 * 24 + 16]	 //.x 电力参数1
#define w_HighV1DianLi10 w_ParLst[300 + 0 * 24 + 17] //.x 电力参数1
#define w_HighV1DianLi11 w_ParLst[300 + 0 * 24 + 18] //.x 电力参数1
#define w_HighV1DianLi12 w_ParLst[300 + 0 * 24 + 19] //.x 电力参数1
#define w_HighV1DianLi13 w_ParLst[300 + 0 * 24 + 20] //.x 电力参数1
#define w_HighV1DianLi14 w_ParLst[300 + 0 * 24 + 21] //.x 电力参数1
#define w_HighV1DianLi15 w_ParLst[300 + 0 * 24 + 22] //.x 电力参数1
#define w_HighV1DianLi16 w_ParLst[300 + 0 * 24 + 23] //.x 电力参数1

// w_HighV1DispDianLi1=(u16)(FtoU32(w_HighV1DianLi1,w_HighV1DianLi2)/10);		//线电压1  去掉1位 变成2位小数  10110 变成 10.11kV
// w_HighV1DispDianLi2=(u16)(FtoU32(w_HighV1DianLi3,w_HighV1DianLi4)/10);		//线电压2  去掉1位 变成2位小数  10110 变成 10.11kV
// w_HighV1DispDianLi3=(u16)(FtoU32(w_HighV1DianLi5,w_HighV1DianLi6)/10);		//线电压3  去掉1位 变成2位小数  10110 变成 10.11kV
// w_HighV1DispDianLi4=(u16)(FtoU32(w_HighV1DianLi7,w_HighV1DianLi8));			//相电流1
// w_HighV1DispDianLi5=(u16)(FtoU32(w_HighV1DianLi9,w_HighV1DianLi10));			//相电流2
// w_HighV1DispDianLi6=(u16)(FtoU32(w_HighV1DianLi11,w_HighV1DianLi12));		//相电流3
// w_HighV1DispDianLi7=(u16)(FtoU32(w_HighV1DianLi13,w_HighV1DianLi14));		//总有功功率
// w_HighV1DispDianLi8=(u16)(FtoU32(w_HighV1DianLi15,w_HighV1DianLi16));		//频率

// 电机2
#define w_HighV2SoftVersion w_ParLst[300 + 1 * 24 + 0]	  // 计数器
#define w_HighV2Counter w_ParLst[300 + 1 * 24 + 1]		  // 计数器
#define w_HighV2WenDu1DecValue w_ParLst[300 + 1 * 24 + 2] //.1 温度值1
#define w_HighV2WenDu2DecValue w_ParLst[300 + 1 * 24 + 3] //.1 温度值2
#define w_HighV2WenDu3DecValue w_ParLst[300 + 1 * 24 + 4] //.1 温度值3
#define w_HighV2WenDu4DecValue w_ParLst[300 + 1 * 24 + 5] //.1 温度值4
#define w_HighV2WenDu5DecValue w_ParLst[300 + 1 * 24 + 6] //.1 温度值5
#define w_HighV2WenDu6DecValue w_ParLst[300 + 1 * 24 + 7] //.1 温度值6

#define w_HighV2DianLi1 w_ParLst[300 + 1 * 24 + 8]	 //.x 电力参数1
#define w_HighV2DianLi2 w_ParLst[300 + 1 * 24 + 9]	 //.x 电力参数1
#define w_HighV2DianLi3 w_ParLst[300 + 1 * 24 + 10]	 //.x 电力参数1
#define w_HighV2DianLi4 w_ParLst[300 + 1 * 24 + 11]	 //.x 电力参数1
#define w_HighV2DianLi5 w_ParLst[300 + 1 * 24 + 12]	 //.x 电力参数1
#define w_HighV2DianLi6 w_ParLst[300 + 1 * 24 + 13]	 //.x 电力参数1
#define w_HighV2DianLi7 w_ParLst[300 + 1 * 24 + 14]	 //.x 电力参数1
#define w_HighV2DianLi8 w_ParLst[300 + 1 * 24 + 15]	 //.x 电力参数1
#define w_HighV2DianLi9 w_ParLst[300 + 1 * 24 + 16]	 //.x 电力参数1
#define w_HighV2DianLi10 w_ParLst[300 + 1 * 24 + 17] //.x 电力参数1
#define w_HighV2DianLi11 w_ParLst[300 + 1 * 24 + 18] //.x 电力参数1
#define w_HighV2DianLi12 w_ParLst[300 + 1 * 24 + 19] //.x 电力参数1
#define w_HighV2DianLi13 w_ParLst[300 + 1 * 24 + 20] //.x 电力参数1
#define w_HighV2DianLi14 w_ParLst[300 + 1 * 24 + 21] //.x 电力参数1
#define w_HighV2DianLi15 w_ParLst[300 + 1 * 24 + 22] //.x 电力参数1
#define w_HighV2DianLi16 w_ParLst[300 + 1 * 24 + 23] //.x 电力参数1

// 电机3
#define w_HighV3SoftVersion w_ParLst[300 + 2 * 24 + 0]	  // 计数器
#define w_HighV3Counter w_ParLst[300 + 2 * 24 + 1]		  // 计数器
#define w_HighV3WenDu1DecValue w_ParLst[300 + 2 * 24 + 2] //.1 温度值1
#define w_HighV3WenDu2DecValue w_ParLst[300 + 2 * 24 + 3] //.1 温度值2
#define w_HighV3WenDu3DecValue w_ParLst[300 + 2 * 24 + 4] //.1 温度值3
#define w_HighV3WenDu4DecValue w_ParLst[300 + 2 * 24 + 5] //.1 温度值4
#define w_HighV3WenDu5DecValue w_ParLst[300 + 2 * 24 + 6] //.1 温度值5
#define w_HighV3WenDu6DecValue w_ParLst[300 + 2 * 24 + 7] //.1 温度值6

#define w_HighV3DianLi1 w_ParLst[300 + 2 * 24 + 8]	 //.x 电力参数1
#define w_HighV3DianLi2 w_ParLst[300 + 2 * 24 + 9]	 //.x 电力参数1
#define w_HighV3DianLi3 w_ParLst[300 + 2 * 24 + 10]	 //.x 电力参数1
#define w_HighV3DianLi4 w_ParLst[300 + 2 * 24 + 11]	 //.x 电力参数1
#define w_HighV3DianLi5 w_ParLst[300 + 2 * 24 + 12]	 //.x 电力参数1
#define w_HighV3DianLi6 w_ParLst[300 + 2 * 24 + 13]	 //.x 电力参数1
#define w_HighV3DianLi7 w_ParLst[300 + 2 * 24 + 14]	 //.x 电力参数1
#define w_HighV3DianLi8 w_ParLst[300 + 2 * 24 + 15]	 //.x 电力参数1
#define w_HighV3DianLi9 w_ParLst[300 + 2 * 24 + 16]	 //.x 电力参数1
#define w_HighV3DianLi10 w_ParLst[300 + 2 * 24 + 17] //.x 电力参数1
#define w_HighV3DianLi11 w_ParLst[300 + 2 * 24 + 18] //.x 电力参数1
#define w_HighV3DianLi12 w_ParLst[300 + 2 * 24 + 19] //.x 电力参数1
#define w_HighV3DianLi13 w_ParLst[300 + 2 * 24 + 20] //.x 电力参数1
#define w_HighV3DianLi14 w_ParLst[300 + 2 * 24 + 21] //.x 电力参数1
#define w_HighV3DianLi15 w_ParLst[300 + 2 * 24 + 22] //.x 电力参数1
#define w_HighV3DianLi16 w_ParLst[300 + 2 * 24 + 23] //.x 电力参数1

// 电机4
#define w_HighV4SoftVersion w_ParLst[300 + 3 * 24 + 0]	  // 计数器
#define w_HighV4Counter w_ParLst[300 + 3 * 24 + 1]		  // 计数器
#define w_HighV4WenDu1DecValue w_ParLst[300 + 3 * 24 + 2] //.1 温度值1
#define w_HighV4WenDu2DecValue w_ParLst[300 + 3 * 24 + 3] //.1 温度值2
#define w_HighV4WenDu3DecValue w_ParLst[300 + 3 * 24 + 4] //.1 温度值3
#define w_HighV4WenDu4DecValue w_ParLst[300 + 3 * 24 + 5] //.1 温度值4
#define w_HighV4WenDu5DecValue w_ParLst[300 + 3 * 24 + 6] //.1 温度值5
#define w_HighV4WenDu6DecValue w_ParLst[300 + 3 * 24 + 7] //.1 温度值6

#define w_HighV4DianLi1 w_ParLst[300 + 3 * 24 + 8]	 //.x 电力参数1
#define w_HighV4DianLi2 w_ParLst[300 + 3 * 24 + 9]	 //.x 电力参数1
#define w_HighV4DianLi3 w_ParLst[300 + 3 * 24 + 10]	 //.x 电力参数1
#define w_HighV4DianLi4 w_ParLst[300 + 3 * 24 + 11]	 //.x 电力参数1
#define w_HighV4DianLi5 w_ParLst[300 + 3 * 24 + 12]	 //.x 电力参数1
#define w_HighV4DianLi6 w_ParLst[300 + 3 * 24 + 13]	 //.x 电力参数1
#define w_HighV4DianLi7 w_ParLst[300 + 3 * 24 + 14]	 //.x 电力参数1
#define w_HighV4DianLi8 w_ParLst[300 + 3 * 24 + 15]	 //.x 电力参数1
#define w_HighV4DianLi9 w_ParLst[300 + 3 * 24 + 16]	 //.x 电力参数1
#define w_HighV4DianLi10 w_ParLst[300 + 3 * 24 + 17] //.x 电力参数1
#define w_HighV4DianLi11 w_ParLst[300 + 3 * 24 + 18] //.x 电力参数1
#define w_HighV4DianLi12 w_ParLst[300 + 3 * 24 + 19] //.x 电力参数1
#define w_HighV4DianLi13 w_ParLst[300 + 3 * 24 + 20] //.x 电力参数1
#define w_HighV4DianLi14 w_ParLst[300 + 3 * 24 + 21] //.x 电力参数1
#define w_HighV4DianLi15 w_ParLst[300 + 3 * 24 + 22] //.x 电力参数1
#define w_HighV4DianLi16 w_ParLst[300 + 3 * 24 + 23] //.x 电力参数1

// 电机5
#define w_HighV5SoftVersion w_ParLst[300 + 4 * 24 + 0]	  // 计数器
#define w_HighV5Counter w_ParLst[300 + 4 * 24 + 1]		  // 计数器
#define w_HighV5WenDu1DecValue w_ParLst[300 + 4 * 24 + 2] //.1 温度值1
#define w_HighV5WenDu2DecValue w_ParLst[300 + 4 * 24 + 3] //.1 温度值2
#define w_HighV5WenDu3DecValue w_ParLst[300 + 4 * 24 + 4] //.1 温度值3
#define w_HighV5WenDu4DecValue w_ParLst[300 + 4 * 24 + 5] //.1 温度值4
#define w_HighV5WenDu5DecValue w_ParLst[300 + 4 * 24 + 6] //.1 温度值5
#define w_HighV5WenDu6DecValue w_ParLst[300 + 4 * 24 + 7] //.1 温度值6

#define w_HighV5DianLi1 w_ParLst[300 + 4 * 24 + 8]	 //.x 电力参数1
#define w_HighV5DianLi2 w_ParLst[300 + 4 * 24 + 9]	 //.x 电力参数1
#define w_HighV5DianLi3 w_ParLst[300 + 4 * 24 + 10]	 //.x 电力参数1
#define w_HighV5DianLi4 w_ParLst[300 + 4 * 24 + 11]	 //.x 电力参数1
#define w_HighV5DianLi5 w_ParLst[300 + 4 * 24 + 12]	 //.x 电力参数1
#define w_HighV5DianLi6 w_ParLst[300 + 4 * 24 + 13]	 //.x 电力参数1
#define w_HighV5DianLi7 w_ParLst[300 + 4 * 24 + 14]	 //.x 电力参数1
#define w_HighV5DianLi8 w_ParLst[300 + 4 * 24 + 15]	 //.x 电力参数1
#define w_HighV5DianLi9 w_ParLst[300 + 4 * 24 + 16]	 //.x 电力参数1
#define w_HighV5DianLi10 w_ParLst[300 + 4 * 24 + 17] //.x 电力参数1
#define w_HighV5DianLi11 w_ParLst[300 + 4 * 24 + 18] //.x 电力参数1
#define w_HighV5DianLi12 w_ParLst[300 + 4 * 24 + 19] //.x 电力参数1
#define w_HighV5DianLi13 w_ParLst[300 + 4 * 24 + 20] //.x 电力参数1
#define w_HighV5DianLi14 w_ParLst[300 + 4 * 24 + 21] //.x 电力参数1
#define w_HighV5DianLi15 w_ParLst[300 + 4 * 24 + 22] //.x 电力参数1
#define w_HighV5DianLi16 w_ParLst[300 + 4 * 24 + 23] //.x 电力参数1

// 电机6
#define w_HighV6SoftVersion w_ParLst[300 + 5 * 24 + 0]	  // 计数器
#define w_HighV6Counter w_ParLst[300 + 5 * 24 + 1]		  // 计数器
#define w_HighV6WenDu1DecValue w_ParLst[300 + 5 * 24 + 2] //.1 温度值1
#define w_HighV6WenDu2DecValue w_ParLst[300 + 5 * 24 + 3] //.1 温度值2
#define w_HighV6WenDu3DecValue w_ParLst[300 + 5 * 24 + 4] //.1 温度值3
#define w_HighV6WenDu4DecValue w_ParLst[300 + 5 * 24 + 5] //.1 温度值4
#define w_HighV6WenDu5DecValue w_ParLst[300 + 5 * 24 + 6] //.1 温度值5
#define w_HighV6WenDu6DecValue w_ParLst[300 + 5 * 24 + 7] //.1 温度值6

#define w_HighV6DianLi1 w_ParLst[300 + 5 * 24 + 8]	 //.x 电力参数1
#define w_HighV6DianLi2 w_ParLst[300 + 5 * 24 + 9]	 //.x 电力参数1
#define w_HighV6DianLi3 w_ParLst[300 + 5 * 24 + 10]	 //.x 电力参数1
#define w_HighV6DianLi4 w_ParLst[300 + 5 * 24 + 11]	 //.x 电力参数1
#define w_HighV6DianLi5 w_ParLst[300 + 5 * 24 + 12]	 //.x 电力参数1
#define w_HighV6DianLi6 w_ParLst[300 + 5 * 24 + 13]	 //.x 电力参数1
#define w_HighV6DianLi7 w_ParLst[300 + 5 * 24 + 14]	 //.x 电力参数1
#define w_HighV6DianLi8 w_ParLst[300 + 5 * 24 + 15]	 //.x 电力参数1
#define w_HighV6DianLi9 w_ParLst[300 + 5 * 24 + 16]	 //.x 电力参数1
#define w_HighV6DianLi10 w_ParLst[300 + 5 * 24 + 17] //.x 电力参数1
#define w_HighV6DianLi11 w_ParLst[300 + 5 * 24 + 18] //.x 电力参数1
#define w_HighV6DianLi12 w_ParLst[300 + 5 * 24 + 19] //.x 电力参数1
#define w_HighV6DianLi13 w_ParLst[300 + 5 * 24 + 20] //.x 电力参数1
#define w_HighV6DianLi14 w_ParLst[300 + 5 * 24 + 21] //.x 电力参数1
#define w_HighV6DianLi15 w_ParLst[300 + 5 * 24 + 22] //.x 电力参数1
#define w_HighV6DianLi16 w_ParLst[300 + 5 * 24 + 23] //.x 电力参数1

// 电机7
#define w_HighV7SoftVersion w_ParLst[300 + 6 * 24 + 0]	  // 计数器
#define w_HighV7Counter w_ParLst[300 + 6 * 24 + 1]		  // 计数器
#define w_HighV7WenDu1DecValue w_ParLst[300 + 6 * 24 + 2] //.1 温度值1
#define w_HighV7WenDu2DecValue w_ParLst[300 + 6 * 24 + 3] //.1 温度值2
#define w_HighV7WenDu3DecValue w_ParLst[300 + 6 * 24 + 4] //.1 温度值3
#define w_HighV7WenDu4DecValue w_ParLst[300 + 6 * 24 + 5] //.1 温度值4
#define w_HighV7WenDu5DecValue w_ParLst[300 + 6 * 24 + 6] //.1 温度值5
#define w_HighV7WenDu6DecValue w_ParLst[300 + 6 * 24 + 7] //.1 温度值6

#define w_HighV7DianLi1 w_ParLst[300 + 6 * 24 + 8]	 //.x 电力参数1
#define w_HighV7DianLi2 w_ParLst[300 + 6 * 24 + 9]	 //.x 电力参数1
#define w_HighV7DianLi3 w_ParLst[300 + 6 * 24 + 10]	 //.x 电力参数1
#define w_HighV7DianLi4 w_ParLst[300 + 6 * 24 + 11]	 //.x 电力参数1
#define w_HighV7DianLi5 w_ParLst[300 + 6 * 24 + 12]	 //.x 电力参数1
#define w_HighV7DianLi6 w_ParLst[300 + 6 * 24 + 13]	 //.x 电力参数1
#define w_HighV7DianLi7 w_ParLst[300 + 6 * 24 + 14]	 //.x 电力参数1
#define w_HighV7DianLi8 w_ParLst[300 + 6 * 24 + 15]	 //.x 电力参数1
#define w_HighV7DianLi9 w_ParLst[300 + 6 * 24 + 16]	 //.x 电力参数1
#define w_HighV7DianLi10 w_ParLst[300 + 6 * 24 + 17] //.x 电力参数1
#define w_HighV7DianLi11 w_ParLst[300 + 6 * 24 + 18] //.x 电力参数1
#define w_HighV7DianLi12 w_ParLst[300 + 6 * 24 + 19] //.x 电力参数1
#define w_HighV7DianLi13 w_ParLst[300 + 6 * 24 + 20] //.x 电力参数1
#define w_HighV7DianLi14 w_ParLst[300 + 6 * 24 + 21] //.x 电力参数1
#define w_HighV7DianLi15 w_ParLst[300 + 6 * 24 + 22] //.x 电力参数1
#define w_HighV7DianLi16 w_ParLst[300 + 6 * 24 + 23] //.x 电力参数1

// 电机8
#define w_HighV8SoftVersion w_ParLst[300 + 7 * 24 + 0]	  // 计数器
#define w_HighV8Counter w_ParLst[300 + 7 * 24 + 1]		  // 计数器
#define w_HighV8WenDu1DecValue w_ParLst[300 + 7 * 24 + 2] //.1 温度值1
#define w_HighV8WenDu2DecValue w_ParLst[300 + 7 * 24 + 3] //.1 温度值2
#define w_HighV8WenDu3DecValue w_ParLst[300 + 7 * 24 + 4] //.1 温度值3
#define w_HighV8WenDu4DecValue w_ParLst[300 + 7 * 24 + 5] //.1 温度值4
#define w_HighV8WenDu5DecValue w_ParLst[300 + 7 * 24 + 6] //.1 温度值5
#define w_HighV8WenDu6DecValue w_ParLst[300 + 7 * 24 + 7] //.1 温度值6

#define w_HighV8DianLi1 w_ParLst[300 + 7 * 24 + 8]	 //.x 电力参数1
#define w_HighV8DianLi2 w_ParLst[300 + 7 * 24 + 9]	 //.x 电力参数1
#define w_HighV8DianLi3 w_ParLst[300 + 7 * 24 + 10]	 //.x 电力参数1
#define w_HighV8DianLi4 w_ParLst[300 + 7 * 24 + 11]	 //.x 电力参数1
#define w_HighV8DianLi5 w_ParLst[300 + 7 * 24 + 12]	 //.x 电力参数1
#define w_HighV8DianLi6 w_ParLst[300 + 7 * 24 + 13]	 //.x 电力参数1
#define w_HighV8DianLi7 w_ParLst[300 + 7 * 24 + 14]	 //.x 电力参数1
#define w_HighV8DianLi8 w_ParLst[300 + 7 * 24 + 15]	 //.x 电力参数1
#define w_HighV8DianLi9 w_ParLst[300 + 7 * 24 + 16]	 //.x 电力参数1
#define w_HighV8DianLi10 w_ParLst[300 + 7 * 24 + 17] //.x 电力参数1
#define w_HighV8DianLi11 w_ParLst[300 + 7 * 24 + 18] //.x 电力参数1
#define w_HighV8DianLi12 w_ParLst[300 + 7 * 24 + 19] //.x 电力参数1
#define w_HighV8DianLi13 w_ParLst[300 + 7 * 24 + 20] //.x 电力参数1
#define w_HighV8DianLi14 w_ParLst[300 + 7 * 24 + 21] //.x 电力参数1
#define w_HighV8DianLi15 w_ParLst[300 + 7 * 24 + 22] //.x 电力参数1
#define w_HighV8DianLi16 w_ParLst[300 + 7 * 24 + 23] //.x 电力参数1

// ZCL 2019.10.15 高压电机采集
// ZCL 2019.9.28
// 电机1
// ZCL 2019.10.18  液晶屏显示值
#define w_HighV1DispSoftVersion w_ParLst[300 + 8 * 24 + 0]	  // 版本
#define w_HighV1DispCounter w_ParLst[300 + 8 * 24 + 1]		  // 计数器
#define w_HighV1DispWenDu1DecValue w_ParLst[300 + 8 * 24 + 2] //.1 温度值1
#define w_HighV1DispWenDu2DecValue w_ParLst[300 + 8 * 24 + 3] //.1 温度值2
#define w_HighV1DispWenDu3DecValue w_ParLst[300 + 8 * 24 + 4] //.1 温度值3
#define w_HighV1DispWenDu4DecValue w_ParLst[300 + 8 * 24 + 5] //.1 温度值4
#define w_HighV1DispWenDu5DecValue w_ParLst[300 + 8 * 24 + 6] //.1 温度值5
#define w_HighV1DispWenDu6DecValue w_ParLst[300 + 8 * 24 + 7] //.1 温度值6

#define w_HighV1DispDianLi1 w_ParLst[300 + 8 * 24 + 8]	 //.x 电力参数1
#define w_HighV1DispDianLi2 w_ParLst[300 + 8 * 24 + 9]	 //.x 电力参数1
#define w_HighV1DispDianLi3 w_ParLst[300 + 8 * 24 + 10]	 //.x 电力参数1
#define w_HighV1DispDianLi4 w_ParLst[300 + 8 * 24 + 11]	 //.x 电力参数1
#define w_HighV1DispDianLi5 w_ParLst[300 + 8 * 24 + 12]	 //.x 电力参数1
#define w_HighV1DispDianLi6 w_ParLst[300 + 8 * 24 + 13]	 //.x 电力参数1
#define w_HighV1DispDianLi7 w_ParLst[300 + 8 * 24 + 14]	 //.x 电力参数1
#define w_HighV1DispDianLi8 w_ParLst[300 + 8 * 24 + 15]	 //.x 电力参数1
#define w_HighV1DispDianLi9 w_ParLst[300 + 8 * 24 + 16]	 //.x 电力参数1
#define w_HighV1DispDianLi10 w_ParLst[300 + 8 * 24 + 17] //.x 电力参数1
														 // w_HighV1DispDianLi1=(u16)(FtoU32(w_HighV1DianLi1,w_HighV1DianLi2)/10);		//线电压1  去掉1位 变成2位小数  10110 变成 10.11kV
														 // w_HighV1DispDianLi2=(u16)(FtoU32(w_HighV1DianLi3,w_HighV1DianLi4)/10);		//线电压2  去掉1位 变成2位小数  10110 变成 10.11kV
														 // w_HighV1DispDianLi3=(u16)(FtoU32(w_HighV1DianLi5,w_HighV1DianLi6)/10);		//线电压3  去掉1位 变成2位小数  10110 变成 10.11kV
														 // w_HighV1DispDianLi4=(u16)(FtoU32(w_HighV1DianLi7,w_HighV1DianLi8));		//相电流1
														 // w_HighV1DispDianLi5=(u16)(FtoU32(w_HighV1DianLi9,w_HighV1DianLi10));		//相电流2
														 // w_HighV1DispDianLi6=(u16)(FtoU32(w_HighV1DianLi11,w_HighV1DianLi12));		//相电流3
														 // w_HighV1DispDianLi7=(u16)(FtoU32(w_HighV1DianLi13,w_HighV1DianLi14));		//总有功功率
														 // w_HighV1DispDianLi8=(u16)(FtoU32(w_HighV1DianLi15,w_HighV1DianLi16));		//频率

// 电机2
#define w_HighV2DispSoftVersion w_ParLst[300 + 9 * 24 + 0]	  // 计数器
#define w_HighV2DispCounter w_ParLst[300 + 9 * 24 + 1]		  // 计数器
#define w_HighV2DispWenDu1DecValue w_ParLst[300 + 9 * 24 + 2] //.1 温度值1
#define w_HighV2DispWenDu2DecValue w_ParLst[300 + 9 * 24 + 3] //.1 温度值2
#define w_HighV2DispWenDu3DecValue w_ParLst[300 + 9 * 24 + 4] //.1 温度值3
#define w_HighV2DispWenDu4DecValue w_ParLst[300 + 9 * 24 + 5] //.1 温度值4
#define w_HighV2DispWenDu5DecValue w_ParLst[300 + 9 * 24 + 6] //.1 温度值5
#define w_HighV2DispWenDu6DecValue w_ParLst[300 + 9 * 24 + 7] //.1 温度值6

#define w_HighV2DispDianLi1 w_ParLst[300 + 9 * 24 + 8]	 //.x 电力参数1
#define w_HighV2DispDianLi2 w_ParLst[300 + 9 * 24 + 9]	 //.x 电力参数1
#define w_HighV2DispDianLi3 w_ParLst[300 + 9 * 24 + 10]	 //.x 电力参数1
#define w_HighV2DispDianLi4 w_ParLst[300 + 9 * 24 + 11]	 //.x 电力参数1
#define w_HighV2DispDianLi5 w_ParLst[300 + 9 * 24 + 12]	 //.x 电力参数1
#define w_HighV2DispDianLi6 w_ParLst[300 + 9 * 24 + 13]	 //.x 电力参数1
#define w_HighV2DispDianLi7 w_ParLst[300 + 9 * 24 + 14]	 //.x 电力参数1
#define w_HighV2DispDianLi8 w_ParLst[300 + 9 * 24 + 15]	 //.x 电力参数1
#define w_HighV2DispDianLi9 w_ParLst[300 + 9 * 24 + 16]	 //.x 电力参数1
#define w_HighV2DispDianLi10 w_ParLst[300 + 9 * 24 + 17] //.x 电力参数1

// 电机3
#define w_HighV3DispSoftVersion w_ParLst[300 + 10 * 24 + 0]	   // 计数器
#define w_HighV3DispCounter w_ParLst[300 + 10 * 24 + 1]		   // 计数器
#define w_HighV3DispWenDu1DecValue w_ParLst[300 + 10 * 24 + 2] //.1 温度值1
#define w_HighV3DispWenDu2DecValue w_ParLst[300 + 10 * 24 + 3] //.1 温度值2
#define w_HighV3DispWenDu3DecValue w_ParLst[300 + 10 * 24 + 4] //.1 温度值3
#define w_HighV3DispWenDu4DecValue w_ParLst[300 + 10 * 24 + 5] //.1 温度值4
#define w_HighV3DispWenDu5DecValue w_ParLst[300 + 10 * 24 + 6] //.1 温度值5
#define w_HighV3DispWenDu6DecValue w_ParLst[300 + 10 * 24 + 7] //.1 温度值6

#define w_HighV3DispDianLi1 w_ParLst[300 + 10 * 24 + 8]	  //.x 电力参数1
#define w_HighV3DispDianLi2 w_ParLst[300 + 10 * 24 + 9]	  //.x 电力参数1
#define w_HighV3DispDianLi3 w_ParLst[300 + 10 * 24 + 10]  //.x 电力参数1
#define w_HighV3DispDianLi4 w_ParLst[300 + 10 * 24 + 11]  //.x 电力参数1
#define w_HighV3DispDianLi5 w_ParLst[300 + 10 * 24 + 12]  //.x 电力参数1
#define w_HighV3DispDianLi6 w_ParLst[300 + 10 * 24 + 13]  //.x 电力参数1
#define w_HighV3DispDianLi7 w_ParLst[300 + 10 * 24 + 14]  //.x 电力参数1
#define w_HighV3DispDianLi8 w_ParLst[300 + 10 * 24 + 15]  //.x 电力参数1
#define w_HighV3DispDianLi9 w_ParLst[300 + 10 * 24 + 16]  //.x 电力参数1
#define w_HighV3DispDianLi10 w_ParLst[300 + 10 * 24 + 17] //.x 电力参数1

// 电机4
#define w_HighV4DispSoftVersion w_ParLst[300 + 11 * 24 + 0]	   // 计数器
#define w_HighV4DispCounter w_ParLst[300 + 11 * 24 + 1]		   // 计数器
#define w_HighV4DispWenDu1DecValue w_ParLst[300 + 11 * 24 + 2] //.1 温度值1
#define w_HighV4DispWenDu2DecValue w_ParLst[300 + 11 * 24 + 3] //.1 温度值2
#define w_HighV4DispWenDu3DecValue w_ParLst[300 + 11 * 24 + 4] //.1 温度值3
#define w_HighV4DispWenDu4DecValue w_ParLst[300 + 11 * 24 + 5] //.1 温度值4
#define w_HighV4DispWenDu5DecValue w_ParLst[300 + 11 * 24 + 6] //.1 温度值5
#define w_HighV4DispWenDu6DecValue w_ParLst[300 + 11 * 24 + 7] //.1 温度值6

#define w_HighV4DispDianLi1 w_ParLst[300 + 11 * 24 + 8]	  //.x 电力参数1
#define w_HighV4DispDianLi2 w_ParLst[300 + 11 * 24 + 9]	  //.x 电力参数1
#define w_HighV4DispDianLi3 w_ParLst[300 + 11 * 24 + 10]  //.x 电力参数1
#define w_HighV4DispDianLi4 w_ParLst[300 + 11 * 24 + 11]  //.x 电力参数1
#define w_HighV4DispDianLi5 w_ParLst[300 + 11 * 24 + 12]  //.x 电力参数1
#define w_HighV4DispDianLi6 w_ParLst[300 + 11 * 24 + 13]  //.x 电力参数1
#define w_HighV4DispDianLi7 w_ParLst[300 + 11 * 24 + 14]  //.x 电力参数1
#define w_HighV4DispDianLi8 w_ParLst[300 + 11 * 24 + 15]  //.x 电力参数1
#define w_HighV4DispDianLi9 w_ParLst[300 + 11 * 24 + 16]  //.x 电力参数1
#define w_HighV4DispDianLi10 w_ParLst[300 + 11 * 24 + 17] //.x 电力参数1

// 电机5
#define w_HighV5DispSoftVersion w_ParLst[300 + 12 * 24 + 0]	   // 计数器
#define w_HighV5DispCounter w_ParLst[300 + 12 * 24 + 1]		   // 计数器
#define w_HighV5DispWenDu1DecValue w_ParLst[300 + 12 * 24 + 2] //.1 温度值1
#define w_HighV5DispWenDu2DecValue w_ParLst[300 + 12 * 24 + 3] //.1 温度值2
#define w_HighV5DispWenDu3DecValue w_ParLst[300 + 12 * 24 + 4] //.1 温度值3
#define w_HighV5DispWenDu4DecValue w_ParLst[300 + 12 * 24 + 5] //.1 温度值4
#define w_HighV5DispWenDu5DecValue w_ParLst[300 + 12 * 24 + 6] //.1 温度值5
#define w_HighV5DispWenDu6DecValue w_ParLst[300 + 12 * 24 + 7] //.1 温度值6

#define w_HighV5DispDianLi1 w_ParLst[300 + 12 * 24 + 8]	  //.x 电力参数1
#define w_HighV5DispDianLi2 w_ParLst[300 + 12 * 24 + 9]	  //.x 电力参数1
#define w_HighV5DispDianLi3 w_ParLst[300 + 12 * 24 + 10]  //.x 电力参数1
#define w_HighV5DispDianLi4 w_ParLst[300 + 12 * 24 + 11]  //.x 电力参数1
#define w_HighV5DispDianLi5 w_ParLst[300 + 12 * 24 + 12]  //.x 电力参数1
#define w_HighV5DispDianLi6 w_ParLst[300 + 12 * 24 + 13]  //.x 电力参数1
#define w_HighV5DispDianLi7 w_ParLst[300 + 12 * 24 + 14]  //.x 电力参数1
#define w_HighV5DispDianLi8 w_ParLst[300 + 12 * 24 + 15]  //.x 电力参数1
#define w_HighV5DispDianLi9 w_ParLst[300 + 12 * 24 + 16]  //.x 电力参数1
#define w_HighV5DispDianLi10 w_ParLst[300 + 12 * 24 + 17] //.x 电力参数1

// 电机6
#define w_HighV6DispSoftVersion w_ParLst[300 + 13 * 24 + 0]	   // 计数器
#define w_HighV6DispCounter w_ParLst[300 + 13 * 24 + 1]		   // 计数器
#define w_HighV6DispWenDu1DecValue w_ParLst[300 + 13 * 24 + 2] //.1 温度值1
#define w_HighV6DispWenDu2DecValue w_ParLst[300 + 13 * 24 + 3] //.1 温度值2
#define w_HighV6DispWenDu3DecValue w_ParLst[300 + 13 * 24 + 4] //.1 温度值3
#define w_HighV6DispWenDu4DecValue w_ParLst[300 + 13 * 24 + 5] //.1 温度值4
#define w_HighV6DispWenDu5DecValue w_ParLst[300 + 13 * 24 + 6] //.1 温度值5
#define w_HighV6DispWenDu6DecValue w_ParLst[300 + 13 * 24 + 7] //.1 温度值6

#define w_HighV6DispDianLi1 w_ParLst[300 + 13 * 24 + 8]	  //.x 电力参数1
#define w_HighV6DispDianLi2 w_ParLst[300 + 13 * 24 + 9]	  //.x 电力参数1
#define w_HighV6DispDianLi3 w_ParLst[300 + 13 * 24 + 10]  //.x 电力参数1
#define w_HighV6DispDianLi4 w_ParLst[300 + 13 * 24 + 11]  //.x 电力参数1
#define w_HighV6DispDianLi5 w_ParLst[300 + 13 * 24 + 12]  //.x 电力参数1
#define w_HighV6DispDianLi6 w_ParLst[300 + 13 * 24 + 13]  //.x 电力参数1
#define w_HighV6DispDianLi7 w_ParLst[300 + 13 * 24 + 14]  //.x 电力参数1
#define w_HighV6DispDianLi8 w_ParLst[300 + 13 * 24 + 15]  //.x 电力参数1
#define w_HighV6DispDianLi9 w_ParLst[300 + 13 * 24 + 16]  //.x 电力参数1
#define w_HighV6DispDianLi10 w_ParLst[300 + 13 * 24 + 17] //.x 电力参数1

// 电机7
#define w_HighV7DispSoftVersion w_ParLst[300 + 14 * 24 + 0]	   // 计数器
#define w_HighV7DispCounter w_ParLst[300 + 14 * 24 + 1]		   // 计数器
#define w_HighV7DispWenDu1DecValue w_ParLst[300 + 14 * 24 + 2] //.1 温度值1
#define w_HighV7DispWenDu2DecValue w_ParLst[300 + 14 * 24 + 3] //.1 温度值2
#define w_HighV7DispWenDu3DecValue w_ParLst[300 + 14 * 24 + 4] //.1 温度值3
#define w_HighV7DispWenDu4DecValue w_ParLst[300 + 14 * 24 + 5] //.1 温度值4
#define w_HighV7DispWenDu5DecValue w_ParLst[300 + 14 * 24 + 6] //.1 温度值5
#define w_HighV7DispWenDu6DecValue w_ParLst[300 + 14 * 24 + 7] //.1 温度值6

#define w_HighV7DispDianLi1 w_ParLst[300 + 14 * 24 + 8]	  //.x 电力参数1
#define w_HighV7DispDianLi2 w_ParLst[300 + 14 * 24 + 9]	  //.x 电力参数1
#define w_HighV7DispDianLi3 w_ParLst[300 + 14 * 24 + 10]  //.x 电力参数1
#define w_HighV7DispDianLi4 w_ParLst[300 + 14 * 24 + 11]  //.x 电力参数1
#define w_HighV7DispDianLi5 w_ParLst[300 + 14 * 24 + 12]  //.x 电力参数1
#define w_HighV7DispDianLi6 w_ParLst[300 + 14 * 24 + 13]  //.x 电力参数1
#define w_HighV7DispDianLi7 w_ParLst[300 + 14 * 24 + 14]  //.x 电力参数1
#define w_HighV7DispDianLi8 w_ParLst[300 + 14 * 24 + 15]  //.x 电力参数1
#define w_HighV7DispDianLi9 w_ParLst[300 + 14 * 24 + 16]  //.x 电力参数1
#define w_HighV7DispDianLi10 w_ParLst[300 + 14 * 24 + 17] //.x 电力参数1

// 电机8
#define w_HighV8DispSoftVersion w_ParLst[300 + 15 * 24 + 0]	   // 计数器
#define w_HighV8DispCounter w_ParLst[300 + 15 * 24 + 1]		   // 计数器
#define w_HighV8DispWenDu1DecValue w_ParLst[300 + 15 * 24 + 2] //.1 温度值1
#define w_HighV8DispWenDu2DecValue w_ParLst[300 + 15 * 24 + 3] //.1 温度值2
#define w_HighV8DispWenDu3DecValue w_ParLst[300 + 15 * 24 + 4] //.1 温度值3
#define w_HighV8DispWenDu4DecValue w_ParLst[300 + 15 * 24 + 5] //.1 温度值4
#define w_HighV8DispWenDu5DecValue w_ParLst[300 + 15 * 24 + 6] //.1 温度值5
#define w_HighV8DispWenDu6DecValue w_ParLst[300 + 15 * 24 + 7] //.1 温度值6

#define w_HighV8DispDianLi1 w_ParLst[300 + 15 * 24 + 8]	  //.x 电力参数1
#define w_HighV8DispDianLi2 w_ParLst[300 + 15 * 24 + 9]	  //.x 电力参数1
#define w_HighV8DispDianLi3 w_ParLst[300 + 15 * 24 + 10]  //.x 电力参数1
#define w_HighV8DispDianLi4 w_ParLst[300 + 15 * 24 + 11]  //.x 电力参数1
#define w_HighV8DispDianLi5 w_ParLst[300 + 15 * 24 + 12]  //.x 电力参数1
#define w_HighV8DispDianLi6 w_ParLst[300 + 15 * 24 + 13]  //.x 电力参数1
#define w_HighV8DispDianLi7 w_ParLst[300 + 15 * 24 + 14]  //.x 电力参数1
#define w_HighV8DispDianLi8 w_ParLst[300 + 15 * 24 + 15]  //.x 电力参数1
#define w_HighV8DispDianLi9 w_ParLst[300 + 15 * 24 + 16]  //.x 电力参数1
#define w_HighV8DispDianLi10 w_ParLst[300 + 15 * 24 + 17] //.x 电力参数1

// 备注： w_ParLst[300+8*24+0] ----- w_ParLst[300+15*24+23]  区间被 液晶屏使用，只能用下面的区间。//ZCL 2022.3.21

// ZCL 2022.3.21 增加
#define w_HighV1RunSecond w_ParLst[684 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV1RunMinute w_ParLst[684 + 1]	   //.0 分钟
#define w_HighV1RunHour w_ParLst[684 + 2]	   //.0 小时
#define w_HighV1SpeedValue w_ParLst[684 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV1PulseCounter w_ParLst[684 + 4] //.0 脉冲计数器
#define w_HighV1NoUse_1 w_ParLst[684 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV1NoUse_2 w_ParLst[684 + 6]	   //.0 预留
#define w_HighV1NoUse_3 w_ParLst[684 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV1NoUse_4 w_ParLst[684 + 8]	   //.0 预留
#define w_HighV1NoUse_5 w_ParLst[684 + 9]	   //.0 预留	//ZCL 2022.3.21

#define w_HighV2RunSecond w_ParLst[694 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV2RunMinute w_ParLst[694 + 1]	   //.0 分钟
#define w_HighV2RunHour w_ParLst[694 + 2]	   //.0 小时
#define w_HighV2SpeedValue w_ParLst[694 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV2PulseCounter w_ParLst[694 + 4] //.0 脉冲计数器
#define w_HighV2NoUse_1 w_ParLst[694 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV2NoUse_2 w_ParLst[694 + 6]	   //.0 预留
#define w_HighV2NoUse_3 w_ParLst[694 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV2NoUse_4 w_ParLst[694 + 8]	   //.0 预留
#define w_HighV2NoUse_5 w_ParLst[694 + 9]	   //.0 预留	//ZCL 2022.3.21

#define w_HighV3RunSecond w_ParLst[704 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV3RunMinute w_ParLst[704 + 1]	   //.0 分钟
#define w_HighV3RunHour w_ParLst[704 + 2]	   //.0 小时
#define w_HighV3SpeedValue w_ParLst[704 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV3PulseCounter w_ParLst[704 + 4] //.0 脉冲计数器
#define w_HighV3NoUse_1 w_ParLst[704 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV3NoUse_2 w_ParLst[704 + 6]	   //.0 预留
#define w_HighV3NoUse_3 w_ParLst[704 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV3NoUse_4 w_ParLst[704 + 8]	   //.0 预留
#define w_HighV3NoUse_5 w_ParLst[704 + 9]	   //.0 预留	//ZCL 2022.3.21

#define w_HighV4RunSecond w_ParLst[714 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV4RunMinute w_ParLst[714 + 1]	   //.0 分钟
#define w_HighV4RunHour w_ParLst[714 + 2]	   //.0 小时
#define w_HighV4SpeedValue w_ParLst[714 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV4PulseCounter w_ParLst[714 + 4] //.0 脉冲计数器
#define w_HighV4NoUse_1 w_ParLst[714 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV4NoUse_2 w_ParLst[714 + 6]	   //.0 预留
#define w_HighV4NoUse_3 w_ParLst[714 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV4NoUse_4 w_ParLst[714 + 8]	   //.0 预留
#define w_HighV4NoUse_5 w_ParLst[714 + 9]	   //.0 预留	//ZCL 2022.3.21

#define w_HighV5RunSecond w_ParLst[724 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV5RunMinute w_ParLst[724 + 1]	   //.0 分钟
#define w_HighV5RunHour w_ParLst[724 + 2]	   //.0 小时
#define w_HighV5SpeedValue w_ParLst[724 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV5PulseCounter w_ParLst[724 + 4] //.0 脉冲计数器
#define w_HighV5NoUse_1 w_ParLst[724 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV5NoUse_2 w_ParLst[724 + 6]	   //.0 预留
#define w_HighV5NoUse_3 w_ParLst[724 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV5NoUse_4 w_ParLst[724 + 8]	   //.0 预留
#define w_HighV5NoUse_5 w_ParLst[724 + 9]	   //.0 预留	//ZCL 2022.3.21

#define w_HighV6RunSecond w_ParLst[734 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV6RunMinute w_ParLst[734 + 1]	   //.0 分钟
#define w_HighV6RunHour w_ParLst[734 + 2]	   //.0 小时
#define w_HighV6SpeedValue w_ParLst[734 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV6PulseCounter w_ParLst[734 + 4] //.0 脉冲计数器
#define w_HighV6NoUse_1 w_ParLst[734 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV6NoUse_2 w_ParLst[734 + 6]	   //.0 预留
#define w_HighV6NoUse_3 w_ParLst[734 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV6NoUse_4 w_ParLst[734 + 8]	   //.0 预留
#define w_HighV6NoUse_5 w_ParLst[734 + 9]	   //.0 预留	//ZCL 2022.3.21

#define w_HighV7RunSecond w_ParLst[744 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV7RunMinute w_ParLst[744 + 1]	   //.0 分钟
#define w_HighV7RunHour w_ParLst[744 + 2]	   //.0 小时
#define w_HighV7SpeedValue w_ParLst[744 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV7PulseCounter w_ParLst[744 + 4] //.0 脉冲计数器
#define w_HighV7NoUse_1 w_ParLst[744 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV7NoUse_2 w_ParLst[744 + 6]	   //.0 预留
#define w_HighV7NoUse_3 w_ParLst[744 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV7NoUse_4 w_ParLst[744 + 8]	   //.0 预留
#define w_HighV7NoUse_5 w_ParLst[744 + 9]	   //.0 预留	//ZCL 2022.3.21

#define w_HighV8RunSecond w_ParLst[754 + 0]	   //.0 秒		//ZCL 2022.3.10
#define w_HighV8RunMinute w_ParLst[754 + 1]	   //.0 分钟
#define w_HighV8RunHour w_ParLst[754 + 2]	   //.0 小时
#define w_HighV8SpeedValue w_ParLst[754 + 3]   //.0 转速	//ZCL 2022.3.21
#define w_HighV8PulseCounter w_ParLst[754 + 4] //.0 脉冲计数器
#define w_HighV8NoUse_1 w_ParLst[754 + 5]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV8NoUse_2 w_ParLst[754 + 6]	   //.0 预留
#define w_HighV8NoUse_3 w_ParLst[754 + 7]	   //.0 预留	//ZCL 2022.3.21
#define w_HighV8NoUse_4 w_ParLst[754 + 8]	   //.0 预留
#define w_HighV8NoUse_5 w_ParLst[754 + 9]	   //.0 预留	//ZCL 2022.3.21
/* #define STM32_WRPAR_ADDRESS			0 			// 读写参数地址；Pw_ParLst，访问时偏移1000访问
#define STM32_RPAR_ADDRESS			10000 	// 只读参数地址；w_ParLst，访问时偏移1000访问
#define DSP_WRPAR_ADDRESS				20000 	// 只读参数地址；Pw_dspParLst，访问时偏移1000访问
#define DSP_RPAR_ADDRESS				30000 	// 只读参数地址；w_dspParLst，访问时偏移1000访问 */
// void	Define4_DspRWPar(void) {}
//  3. DSP 设定参数--------------------------------------------------------------------------
#define Pw_dspAddSpeedTime Pw_dspParLst[0]	 //.0 加速时间	实时生效
#define Pw_dspMotorDirection Pw_dspParLst[1] //.0 电机转向	0/1 重启生效
#define Pw_dspTorquePromote Pw_dspParLst[2]	 //.0 转矩提升	0~30 重启生效
#define Pw_dspVFCurve Pw_dspParLst[3]		 //.0 VF曲线	0~10	重启生效
#define Pw_dspDeadSupplement Pw_dspParLst[4] //.0 死区补偿	0~100	重启生效
#define Pw_dspSubSpeedTime Pw_dspParLst[5]	 //.0 减速时间	0~6500S	实时生效

#define Pw_dspStartHz Pw_dspParLst[6]			 //.1 启动频率	0~500	实时生效
#define Pw_dspCarrierHz Pw_dspParLst[7]			 //.0 载波频率	2~10	重启生效 	DCM220是.1	DCM100是.0
#define Pw_dspUpperLimitHz Pw_dspParLst[8]		 //.1 上限频率	0~500	实时生效
#define Pw_dspLowerLimitHz Pw_dspParLst[9]		 //.1 下限频率	0~500	实时生效
#define Pw_dspMAXFREQ Pw_dspParLst[11]			 //.0 最大频率 50-100Hz ZCL 2017.5.5 设定会影响电压 //BASE_FREQ
#define Pw_dspStartHzStopMode Pw_dspParLst[12]	 //.0 启动频率停机模式 =0，快速停机；=1慢速停机（软停）默认0
#define Pw_dspFanRunTemperature Pw_dspParLst[13] //.1 风扇运转设定温度	ZCL 2018.4.20

#define Pw_dspWenDuSel Pw_dspParLst[14]		  //.0 温度选择  ZCL 2018.7.16 =0,15-65DU; =1 105DU;
#define Pw_dspOCPCurrent Pw_dspParLst[15]	  //.0 过流保护电流	实时生效	OverCurrentProtect
#define Pw_dspOVPVoltage Pw_dspParLst[16]	  //.0 过压保护电压	实时生效  OverVoltage
#define Pw_dspUVPVoltage Pw_dspParLst[17]	  //.0 欠压保护电压	实时生效  UnderVoltage
#define Pw_dspOHPTemperature Pw_dspParLst[18] //.0 过热保护温度	立即生效  Overheat
#define Pw_dspFanRunMode Pw_dspParLst[19]	  //.0 风扇运行方式	0/1	0-常开;1-温控
#define Pw_dspHuoErType Pw_dspParLst[20]	  //.0 霍尔类型 万位：厂家类型 1：硕亚  2：田村  3：甲神  0：特殊类型（预留）
											  //   后面3位  百位---个位  代表电流；个位还可以代表特殊含义 ZCL 2017.4.29

#define Pw_dspKeyStopMode Pw_dspParLst[21]		 //.0 按键停机模式 =0，快速停机；=1慢速停机（软停）
#define Pw_dspDIStopMode Pw_dspParLst[22]		 //.0 DI停机模式 =0，快速停机；=1慢速停机（软停）
#define Pw_dspFaultAutoResetEn Pw_dspParLst[23]	 //.0 故障自动复位使能，=1允许，默认允许
#define Pw_dspFaultAutoResetSec Pw_dspParLst[24] //.0 故障自动复位时间，单位秒，默认300秒 20秒

// 1-当前频率；2-输出电压；3-电流；4-温度；5-母线电压；6-三相电流和;7-MODBUS设置量；8-目标频率
#define Pw_dspAO1OutSource Pw_dspParLst[25] //.0 模拟量输出1数据源	0~10
#define Pw_dspAO2OutSource Pw_dspParLst[26] //.0 模拟量输出2数据源	0~10

// 0-不输出；1-ST配置；2-故障报警;3-系统PWM输出状态.
#define Pw_dspDO1OutSource Pw_dspParLst[27] //.0 继电器1输出数据源	0~10
#define Pw_dspDO2OutSource Pw_dspParLst[28] //.0 继电器2输出数据源	0~10

// ZCL 2018.3.29 这些在DCM220中使用
#define Pw_dspAddress Pw_dspParLst[29]	 //.0 变频器地址  2016.6.16
#define Pw_dspZDRunMode Pw_dspParLst[30] //.0 指定频率运行模式
#define Pw_dspRunHz Pw_dspParLst[31]	 //.1 指定运行频率

#define Pw_dspAngleCompRef Pw_dspParLst[32] //.0 角度补偿
#define Pw_dspAngleCompSet Pw_dspParLst[33] //.0 角度补偿2

#define Pw_dspICurQsRef Pw_dspParLst[34]  //.0 空载电流参考
#define Pw_dspVoltCompUp Pw_dspParLst[35] //.0 比例
#define Pw_dspVoltCompUd Pw_dspParLst[36] //.0 微分

#define Pw_dspSelABCShunXu Pw_dspParLst[37] //.0 选择ABC电流顺序

#define Pw_dspnCnt Pw_dspParLst[38]		   //.0 ZCL 2018.3.29 暂时留用
#define Pw_dspFrqCompUp Pw_dspParLst[39]   //.0 ZCL 2018.3.29 暂时留用
#define Pw_dspFrqCompLimt Pw_dspParLst[40] //.0 ZCL 2018.3.29 暂时留用
#define Pw_dspFrqCompUd Pw_dspParLst[41]   //.0 ZCL 2018.3.29 暂时留用

#define Pw_dspCompDirection Pw_dspParLst[42] //.0 补偿方向
#define Pw_dspHzAISource Pw_dspParLst[43]	 //.0 DSP 频率AI数据源  ZCL 2018.3.26 DCM220使用
#define Pw_dspAutoResetMax Pw_dspParLst[44]	 //.0 DSP 故障自动复位最大值  ZCL 2018.3.24
// #define	Pw_dspFanRunTemperature		Pw_dspParLst[45]	//.1 风扇运转设定温度	0~1000	*0.1 ZCL 2018.3.30 移到这里 DCM220

#define Pw_dspBusBarVoltageKi Pw_dspParLst[46]	//.2 ZCL 2019.12.24 电压修正比例
#define Pw_dspFastOverVoltage Pw_dspParLst[47]	//.0 ZCL 2019.12.24 快速过压保护值值
#define Pw_dspFastUnderVoltage Pw_dspParLst[48] //.0 ZCL 2019.12.24 快速欠压电压值

/* #define STM32_WRPAR_ADDRESS			0 			// 读写参数地址；Pw_ParLst，访问时偏移0访问
#define STM32_RPAR_ADDRESS			10000 	// 只读参数地址；w_ParLst，访问时偏移10000访问
#define DSP_WRPAR_ADDRESS				20000 	// 只读参数地址；Pw_dspParLst，访问时偏移20000访问
#define DSP_RPAR_ADDRESS				30000 	// 只读参数地址；w_dspParLst，访问时偏移30000访问 */
// void	Define5_DspRPar(void) {}

// 4.DSP只读参数的定义--------------------------------------------------------------------------
#define w_dspSoftVersion w_dspParLst[0] //.2 DSP软件系统版本号	0~65535
#define w_NoUse1 w_dspParLst[1]			//.0
// 2组. 通讯
#define w_dspTemp1 w_dspParLst[2] //.0 ZCL 2018.4.19 删除 A相ADC校正零点	0~65535
#define w_dspTemp2 w_dspParLst[3] //.0 ZCL 2018.4.19 删除 B相ADC校正零点	0~65535
#define w_dspTemp3 w_dspParLst[4] //.0 ZCL 2018.4.19 删除 C相ADC校正零点	0~65535
#define w_dspTemp4 w_dspParLst[5] //.0 ZCL 2018.4.19 删除 A相ADC校正增益	0~65535
#define w_dspTemp5 w_dspParLst[6] //.0 ZCL 2018.4.19 删除 B相ADC校正增益	0~65535
#define w_dspTemp6 w_dspParLst[7] //.0 ZCL 2018.4.19 删除 C相ADC校正增益	0~65535
// #define	w_dspAPhaseCalibrationZero  	w_dspParLst[2]	//.0 A相ADC校正零点	0~65535
// #define	w_dspBPhaseCalibrationZero  	w_dspParLst[3]	//.0 B相ADC校正零点	0~65535
// #define	w_dspCPhaseCalibrationZero  	w_dspParLst[4]	//.0 C相ADC校正零点	0~65535
// #define	w_dspAPhaseCalibrationGain  	w_dspParLst[5]	//.0 A相ADC校正增益	0~65535
// #define	w_dspBPhaseCalibrationGain  	w_dspParLst[6]	//.0 B相ADC校正增益	0~65535
// #define	w_dspCPhaseCalibrationGain  	w_dspParLst[7]	//.0 C相ADC校正增益	0~65535

#define w_dspAPhaseDLHZero w_dspParLst[8]			//.0 A相电流互感零点	0~65535
#define w_dspBPhaseDLHZero w_dspParLst[9]			//.0 B相电流互感零点	0~65535
#define w_dspCPhaseDLHZero w_dspParLst[10]			//.0 C相电流互感零点	0~65535
#define w_dspAPhaseCurrentADCValue w_dspParLst[11]	//.0 A相电流ADC值
#define w_dspBPhaseCurrentADCValue w_dspParLst[12]	//.0 B相电流ADC值
#define w_dspCPhaseCurrentADCValue w_dspParLst[13]	//.0 C相电流ADC值
#define w_dspBusBarVoltageADCValue w_dspParLst[14]	//.0 母线电压ADC值
#define w_dspNowTemperatureADCValue w_dspParLst[15] //.0 当前温度ADC值		0~1000	x10

// 1组. 通讯
#define w_dspAPhaseCurrent w_dspParLst[16]	  //.2 A相电流 0~65535	x100
#define w_dspBPhaseCurrent w_dspParLst[17]	  //.2 B相电流 0~65535	x100
#define w_dspCPhaseCurrent w_dspParLst[18]	  //.2 C相电流0~65535	x100
#define w_dsp3PhaseCurrentSum w_dspParLst[19] //.0 三相电流之和
// 2018.3.23 临时目标频率，计算中间量
#define w_dspTmpTargetHz w_dspParLst[20]	//.1 临时目标频率，计算中间量	0~500	x10
#define w_dspNowHz w_dspParLst[21]			//.1 当前频率	0~500	x10
#define w_dspNowRotateSpeed w_dspParLst[22] //.0 当前转速	0~60000	x10
#define w_dspNowTemperature w_dspParLst[23] //.1 当前温度	0~1000	x10
#define w_dspOutVoltage w_dspParLst[24]		//.0 输出电压	0~500	x1
#define w_dspBusBarVoltage w_dspParLst[25]	//.0 母线电压	0~800	x1

#define w_dspCounter w_dspParLst[26]		//.0 DCM220不需要；DCM100主循环计数器
#define w_dspCommErrCounter w_dspParLst[27] //.0 DCM220不需要；DCM100通讯错误计数器，便于观察通讯运行
											// w_dspCommErrCounter ZCL 2018.9.13  用来随意保存Pw_ScrKeyMode
#define w_dspCounter2 w_dspParLst[28]		//.0 DCM220不需要；DCM100上电读Pw参数计数器
											// w_dspCounter2 ZCL 2018.9.13  用来随意保存Pw_ScrKeyMode
#define w_dspCounter3 w_dspParLst[29]		//.0 DCM220 DCM100: dsp计数器，秒

#define w_dspSysErrorCode1 w_dspParLst[30] //.0 系统错误码E0	0~65535	 使用位
#define w_dspSysErrorCode2 w_dspParLst[31] //.0 系统错误码E2	0~65535	 No Use ZCL 2018.3.21
#define w_dspSysRunStatus1 w_dspParLst[32] //.0 系统运行状态码1		使用位
#define w_dspSysRunStatus2 w_dspParLst[33] //.0 系统运行状态码2		No Use ZCL 2018.4.19

#define w_dspCounter4 w_dspParLst[35] //.0 DCM220不需要；DCM100：ModbusDataValidCount
#define w_dspCounter5 w_dspParLst[36] //.0 DCM220 DCM100: my_PDPINTA_ISR中断  F_dspIpmFault 计数器

// ZCL 2015.9.16 加入保护延时计数器观看参数
#define w_dspOverVPDelayCount w_dspParLst[41]	  //.0 过压保护延时计数器
#define w_dspOverCPDelayCount w_dspParLst[42]	  //.0 过流保护延时计数器
#define w_dspLosePhasePDelayCount w_dspParLst[43] //.0 缺相保护延时计数器
#define w_dspUnderVPDelayCount w_dspParLst[44]	  //.0 欠压保护延时计数器
#define w_dspOverHotPDelayCount w_dspParLst[45]	  //.0 过热保护延时计数器
#define w_dspOverHotCancelCount w_dspParLst[46]	  //.0 过热取消保护延时计数器

#define w_dspModPar w_dspParLst[48]		//.0 修改参数   ZCL 2015.9.9 DCM220用
#define w_dspParInitial w_dspParLst[49] //.0 参数初始化 ZCL 2015.9.9 DCM220用

/*---DSP接收到的STM32的设置控制字！！！-----------------------*/
// 3. 定时写目标频率、模拟量输出1、模拟量输出2 到DSP	（MAXTimeWriteNum==3 控制读写数量）
#define w_dspTargetHz w_dspParLst[50] //.1 目标频率	0~500	x10
#define w_dspAO1 w_dspParLst[51]	  //.0 DSP模拟量当前输出量1	0~4095	0~5V输出
#define w_dspAO2 w_dspParLst[52]	  //.0 DSP模拟量当前输出量2	0~4095	0~5V输出

// ZCL 2018.3.24 DCM220使用
#define w_dspAI1ADCValue w_dspParLst[53] //.0 AI1 ADC采集值 ZCL 2016.6.21
#define w_dspAI2ADCValue w_dspParLst[54] //.0 AI2 ADC采集值

#define w_dspWriteYear w_dspParLst[57] //.0程序编写年		ZCL 2013.3.22
#define w_dspWriteDate w_dspParLst[58] //.0程序编写日期

#define w_dspFaultRecNum w_dspParLst[59] //.0 故障记录数量 ZCL 定义在前，为了跟SM510对应起来
#define w_dspFaultRecNo w_dspParLst[60]	 //.0 故障记录号  ZCL
#define w_dspFaultNum w_dspParLst[61]	 //.0 DSP故障数量（报警）  2016.8.23
#define w_dspFaultStatus w_dspParLst[62] //.0 DSP故障原因（报警） 2016.8.23

#define w_dspStopNum w_dspParLst[63]	//.0 DSP停机数量 2018.4.24
#define w_dspStopStatus w_dspParLst[64] //.0 DSP停机原因 2018.4.24

#define w_dspSelFaultNo w_dspParLst[69]	  //.0 选择故障号
#define w_dspSelRecNo w_dspParLst[70]	  //.0 选择记录号
#define w_dspPreFaultNo w_dspParLst[71]	  //.0 上次故障号
#define w_dspTouchRunStop w_dspParLst[72] //.0 触摸 启动/停止 2016.12.5
#define w_dspDIResetCount w_dspParLst[73] //.0 DI复位计数器 DCM220用 2018.3.24
#define w_dspAutoResetNum w_dspParLst[74] //.0 DSP 故障自动复位数量 保存 观看用 2018.3.24
#define w_dspManMadeFault w_dspParLst[75] //.0 人为制造故障 测试用 2018.3.24

#define w_dspEquipAlarmLast6 w_dspParLst[82]   //.0 设备停机原因上6次
#define w_dspEquipAlarm6YM w_dspParLst[83]	   //.0 设备停机6时间－年月
#define w_dspEquipAlarm6DH w_dspParLst[84]	   //.0 设备停机6时间－日时
#define w_dspEquipAlarm6MS w_dspParLst[85]	   //.0 设备停机6时间－分秒
#define w_dspEquipAlarmLast5 w_dspParLst[86]   //.0 设备停机原因上5次
#define w_dspEquipAlarm5YM w_dspParLst[87]	   //.0 设备停机5时间－年月
#define w_dspEquipAlarm5DH w_dspParLst[88]	   //.0 设备停机5时间－日时
#define w_dspEquipAlarm5MS w_dspParLst[89]	   //.0 设备停机5时间－分秒
#define w_dspEquipAlarmLast4 w_dspParLst[90]   //.0 设备停机原因上4次
#define w_dspEquipAlarm4YM w_dspParLst[91]	   //.0 设备停机4时间－年月
#define w_dspEquipAlarm4DH w_dspParLst[92]	   //.0 设备停机4时间－日时
#define w_dspEquipAlarm4MS w_dspParLst[93]	   //.0 设备停机4时间－分秒
#define w_dspEquipAlarmLast3 w_dspParLst[94]   //.0 设备停机原因上3次
#define w_dspEquipAlarm3YM w_dspParLst[95]	   //.0 设备停机3时间－年月
#define w_dspEquipAlarm3DH w_dspParLst[96]	   //.0 设备停机3时间－日时
#define w_dspEquipAlarm3MS w_dspParLst[97]	   //.0 设备停机3时间－分秒
#define w_dspEquipAlarmLast2 w_dspParLst[98]   //.0 设备停机原因上2次
#define w_dspEquipAlarm2YM w_dspParLst[99]	   //.0 设备停机2时间－年月
#define w_dspEquipAlarm2DH w_dspParLst[100]	   //.0 设备停机2时间－日时
#define w_dspEquipAlarm2MS w_dspParLst[101]	   //.0 设备停机2时间－分秒
#define w_dspEquipAlarmLast1 w_dspParLst[102]  //.0 设备停机原因上1次
#define w_dspEquipAlarm1YM w_dspParLst[103]	   //.0 设备停机1时间－年月
#define w_dspEquipAlarm1DH w_dspParLst[104]	   //.0 设备停机1时间－日时
#define w_dspEquipAlarm1MS w_dspParLst[105]	   //.0 设备停机1时间－分秒
#define w_dspSelEquipAlarm w_dspParLst[106]	   //.0 选择的设备停机原因
#define w_dspSelEquipAlarm1YM w_dspParLst[107] //.0 选择的设备停机时间－年月
#define w_dspSelEquipAlarm1DH w_dspParLst[108] //.0 选择的设备停机时间－日时
#define w_dspSelEquipAlarm1MS w_dspParLst[109] //.0 选择的设备停机时间－分秒
#define w_dspFlashRecNo w_dspParLst[110]	   //.0 FLASH记录号
#define w_dspFlashRecNum w_dspParLst[111]	   //.0 FLASH记录数量

#define w_dspdeadComp1Ds w_dspParLst[120]	   //.0 w_dspAPhaseCalibrationZero   2017.2.4
#define w_dspdeadComp1Qs w_dspParLst[121]	   //.0 w_dspBPhaseCalibrationZero
#define w_dspFrqComp w_dspParLst[122]		   //.0 w_dspAPhaseCalibrationZero   2017.2.4
#define w_dspFrqCompAfterLimt w_dspParLst[123] //.0 w_dspBPhaseCalibrationZero

// 随时注意数组的容量不能溢出，根据增加的变量增大，并修改相应的 ARM，DSP中容量！！！！！ ZCL 2018.9.21

/* #define	w_dspSysErrorCode1  		w_dspParLst[30]	  // 系统错误码E0	0~65535	 */
#define F_dspOverCurrent MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 0))			 // 过流保护	0/1
#define F_dspOverVoltage MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 1))			 // 过压保护	0/1
#define F_dspUnderVoltage MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 2))			 // 欠压保护	0/1
#define F_dspLosePhase MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 3))			 // 缺相保护	0/1
#define F_dspIpmFault MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 4))				 // IPM故障	0/1
#define F_dspDLHFault MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 5))				 // 电流？ 互感器故障	0/1
#define F_dspIpmHot MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 6))				 // IPM过热	0/1
#define F_dspMoterHot MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 7))				 // 电机过热	0/1
#define F_dspFanFault MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 8))				 // 风扇故障	0/1
#define F_dspEepromFault MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 9))			 // EEPROM故障	0/1
#define F_dspDspFault MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 10))			 // DSP处理器故障	0/1
#define F_dspStm32Fault MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 11))			 // STM处理器故障	0/1
#define F_dspGroundProtect MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 12))		 // 接地保护	0/1
#define F_dspAddSpeedUnderVoltage MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 13)) // 加速中欠电压	0/1
#define F_dspSubSpeedOverVoltage MEM_ADDR(BITBAND((u32) & w_dspParLst[30], 14))	 // 减速中过电压	0/1

/* #define	w_dspSysRunStatus1  		w_dspParLst[32]	  // 系统运行状态码1 */
#define F_dspFan1Run MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 0))		// 风扇1开启标志位	0/1
#define F_dspFan2Run MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 1))		// 风扇2开启标志位	0/1
#define F_dspQ1Status MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 2))		// 继电器1当前输出状态	0/1
#define F_dspQ2Status MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 3))		// 继电器2当前输出状态	0/1
#define F_dspDspRun MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 4))		// DSP运行标志	0/1
#define F_dspDspPwmOut MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 5))	// DSP输出PWM标志	0/1
#define F_dspDspStartLock MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 6)) // DSP启动解锁	0/1
#define F_dspMainJDQ MEM_ADDR(BITBAND((u32) & w_dspParLst[32], 7))		// DSP主回路继电器状态	0/1

// 5. SCR 液晶屏参数--------------------------------------------------------------------------

// 随时注意数组的容量不能溢出，根据增加的变量增大，并修改相应的 ARM，DSP中容量！！！！！ ZCL 2018.9.21

//------------- GPRS ZCL 2018.12.10
#define SCR_PAR_SIZE 50	 // NV 参数数量	字
#define DTU_PAR_SIZE 288 // DTU参数数量	字节

// ZCL 2018.12.10
//  定义GPRS设定参数在RAM中的地址 2012.7.23
#define MobileServerBase 0			  // 8+1 服务代码:01
#define AccessPointBase 9			  // 24+1 Access Point Name 接入点名称:05
#define DtuNoBase 34				  // 11+1 DTU身份识别码:06
#define MaxSendLengthBase 46		  // 2 最大传输包长:07
#define LastPacketIdleIntervalBase 48 // 2 最后包空闲时间间隔:08 600毫秒 QITCFG=3,2,512,1 200
#define DataReturnBase 50			  // 1 支持数据回执:09
#define AutoAnswerBase 51			  // 1 自动应答:0A
#define RedialIntervalBase 52		  // 2 重拨号时间间隔:0B
#define ConsoleInfoTypeBase 54		  // 1 控制台信息类型:0C
#define BaudRateBase 55				  // 8 串口工作方式:0D
#define PacketCompartIDBase 63		  // 1 数据包分隔标识:0E
#define EnTDPBase 64				  // 1 启用TDP协议:0F
#define TerminalTypeBase 65			  // 1 终端类型:0x10
#define CallTypeBase 66				  // 1 呼叫方式:0x11
#define CallIntervalBase 67			  // 2 呼叫时间间隔:0x12
#define OfflineIntervalBase 69		  // 2 空闲下线时间间隔:0x13
#define OfflineModeBase 71			  // 1 空闲下线模式:0x14

#define Ip0Base 72					  // 4 IP地址:0x20------
#define Ip1Base 76					  // 4 IP地址:0x30
#define Ip2Base 80					  // 4 IP地址:0x40
#define Ip3Base 84					  // 4 IP地址:0x50
#define Port0Base 88				  // 2	端口号:0x22------
#define Port1Base 90				  // 2	端口号:0x32
#define Port2Base 92				  // 2	端口号:0x42
#define Port3Base 94				  // 2	端口号:0x52
#define LoclaPort0Base 96			  // 2 本地端口号:0x23------
#define LoclaPort1Base 98			  // 2 本地端口号:0x33
#define LoclaPort2Base 100			  // 2 本地端口号:0x43
#define LoclaPort3Base 102			  // 2 本地端口号:0x53
#define LinkTCPUDP0Base 104			  // 1 TCP=1.UDP=0 类型:0x24------
#define LinkTCPUDP1Base 105			  // 1 TCP.UDP 类型:0x34
#define LinkTCPUDP2Base 106			  // 1 TCP.UDP 类型:0x44
#define LinkTCPUDP3Base 107			  // 1 TCP.UDP 类型:0x54
#define EnDDP0Base 108				  // 1 DDP协议:0x25  =1DDP协议；=0透明协议------
#define EnDDP1Base 109				  // 1 DDP协议:0x35
#define EnDDP2Base 110				  // 1 DDP协议:0x45
#define EnDDP3Base 111				  // 1 DDP协议:0x55
#define OnlineReportInterval0Base 112 // 2 在线报告时间间隔:0x26 心跳时间 40秒------
#define OnlineReportInterval1Base 114 // 2 在线报告时间间隔:0x36 心跳时间 40秒
#define OnlineReportInterval2Base 116 // 2 在线报告时间间隔:0x46 心跳时间 40秒
#define OnlineReportInterval3Base 118 // 2 在线报告时间间隔:0x56 心跳时间 40秒
#define NetCheckInterval0Base 120	  // 2 网络检测时间间隔:0x27------
#define NetCheckInterval1Base 122	  // 2 网络检测时间间隔:0x37
#define NetCheckInterval2Base 124	  // 2 网络检测时间间隔:0x47
#define NetCheckInterval3Base 126	  // 2 网络检测时间间隔:0x57
#define MaxLoginDSCNum0Base 128		  // 1 最大登陆DSC次数:0x28------
#define MaxLoginDSCNum1Base 129		  // 1 最大登陆DSC次数:0x38
#define MaxLoginDSCNum2Base 130		  // 1 最大登陆DSC次数:0x48
#define MaxLoginDSCNum3Base 131		  // 1 最大登陆DSC次数:0x58
#define MaxNetCheckNum0Base 132		  // 1 最大网络检测次数:0x29------
#define MaxNetCheckNum1Base 133		  // 1 最大网络检测次数:0x39
#define MaxNetCheckNum2Base 134		  // 1 最大网络检测次数:0x49
#define MaxNetCheckNum3Base 135		  // 1 最大网络检测次数:0x59
#define ControlLevel0Base 136		  // 1 控制级别:0x2B------
#define ControlLevel1Base 137		  // 1 控制级别:0x3B
#define ControlLevel2Base 138		  // 1 控制级别:0x4B
#define ControlLevel3Base 139		  // 1 控制级别:0x5B
#define ReConnect0Base 140			  // 2 重连接时间间隔:0x2C------
#define ReConnect1Base 142			  // 2 重连接时间间隔:0x3C
#define ReConnect2Base 144			  // 2 重连接时间间隔:0x4C
#define ReConnect3Base 146			  // 2 重连接时间间隔:0x5C

#define LinkModeBase 148	 // 1 通道工作模式:0x60 "------"
#define TcpKeepaliveBase 149 // 2 TCP保活时间间隔:0x61
#define DNSAddressBase 151	 // 4 DNS地址:0x62
#define PPPKeepaliveBase 155 // 2 PPP保活时间间隔:0x63

#define DomainName0Base 157 // 30+1	域名:0x21------
#define DomainName1Base 188 // 30+1	域名:0x31
#define DomainName2Base 219 // 30+1	域名:0x41
#define DomainName3Base 250 // 30+1	域名:0x51

// volatile uint	xdata	w_GprsParLst[50];					// 参数字列表区
//  Gprs LoRa Scr 的设定参数，都用w_GprsParLst[]数组，保存到液晶屏本身ARM Flash中
//  #define	Pw_GprsOutPDecMax					w_GprsParLst[0]		//.2 出水表量程。初始化值1660，对应1.66MP
//  #define	Pw_GprsInPDecMax					w_GprsParLst[1]		//.2 进水表量程。初始化值1000，对应1.00MP
#define Pw_RebootInterval w_GprsParLst[0]	  //.0 重启间隔时间 分钟
#define Pw_ConsoleInfo w_GprsParLst[1]		  //.0 =1，可以显示控制台信息
#define Pw_LoRaEquipmentNo w_GprsParLst[2]	  //.0 LORA设备通讯地址 ZCL 2018.12.13
#define Pw_LoRaMasterSlaveSel w_GprsParLst[3] //.0 主机从机选择
#define Pw_LoRaModule1Add w_GprsParLst[4]	  //.0 LORA模块1地址
#define Pw_LoRaModule2Add w_GprsParLst[5]	  //.0 LORA模块2地址
#define Pw_LoRaModule3Add w_GprsParLst[6]	  //.0 LORA模块3地址
#define Pw_LoRaModule4Add w_GprsParLst[7]	  //.0 LORA模块4地址

// #define Pw_GprsFaultRestart w_GprsParLst[8] //.0 故障重启次数 2012.10.18
#define Pw_EquipmentType w_GprsParLst[8] //.0 设备类型，=0，双驱泵；=1，变频电机

#define Pw_ScrEquipPower w_GprsParLst[9] //.1 设备功率 2019.9.11

// ZCL 2019.9.11 保存纬度和经度，防止搜星的时候，是零，掉到海里。
#define Pw_ScrGpsLatAllDu1 w_GprsParLst[10] //.2 纬度 度1； 如：35.44			2位小数
#define Pw_ScrGpsLatAllDu2 w_GprsParLst[11] //.0 纬度 度2；	00.00****			****部分，添加到w_ScrGpsLatAllDu1后面。 如：****是 2459， 则总纬度是： 35.442459
#define Pw_ScrGpsLonAllDu1 w_GprsParLst[12] //.2 经度 度1；	如：127.39		2位小数
#define Pw_ScrGpsLonAllDu2 w_GprsParLst[13] //.0 经度 度2； 00.00****			****部分，添加到w_ScrGpsLonAllDu1后面。如：****是 4628， 则总经度是： 127.394628

#define Pw_LoRaModule5Add w_GprsParLst[14] //.0 LORA模块5地址  ZCL 2019.10.18
#define Pw_LoRaModule6Add w_GprsParLst[15] //.0 LORA模块6地址  ZCL 2019.10.18
// 11-15

#define Pw_LoRaSet w_GprsParLst[16]		   //.0 LoRa 设定  LORA SX1278
										   // （LoRa Gprs 设定参数分开，否则在一起用文本不好设定）
#define Pw_GprsSet w_GprsParLst[17]		   //.0 Gprs 设定
#define Pw_ScrLoRaTest1 w_GprsParLst[18]   //.0 发射测试2  LORA SX1278
#define Pw_ScrLoRaSend2 w_GprsParLst[19]   //.0 发射测试3  LORA SX1278
#define Pw_ScrKeyMode w_GprsParLst[20]	   //.0 液晶屏按键模式
										   //=0，8个按键，正常模式; =1，7个按键;	=2，8个按键，上下键反着
#define Pw_ScrMenuMode w_GprsParLst[21]	   //.0 菜单模式 	=0，控制器模式; =1，变频器模式；
#define Pw_ScrCurrentBits w_GprsParLst[22] //.0 电流小数 	=2，正常模式; =1，大功率变频器模式；

// 23 24
#define Pw_LoRaModule7Add w_GprsParLst[23] //.0 LORA模块7地址  ZCL 2019.10.18
#define Pw_LoRaModule8Add w_GprsParLst[24] //.0 LORA模块8地址  ZCL 2019.10.18

#define Pw_GprsSetSecond w_GprsParLst[25]	  //.0 设置秒	// 注意:地址不能随便改动,被数组使用  ZCL
#define Pw_GprsSetMinute w_GprsParLst[26]	  //.0 设置分
#define Pw_GprsSetHour w_GprsParLst[27]		  //.0 设置时
#define Pw_GprsSetDay w_GprsParLst[28]		  //.0 设置日
#define Pw_GprsSetTest w_GprsParLst[29]		  //.0 测试			ZCL 2019.3.22
#define Pw_GprsBaudRate3 w_GprsParLst[30]	  //.0 串口3波特率（固定是57600，这个参数没用）
#define Pw_GprsBaudRate2 w_GprsParLst[31]	  //.0 串口2波特率
#define Pw_GprsDataBitLen2 w_GprsParLst[32]	  //.0 串口2 数据位长度
#define Pw_GprsStopBitLen2 w_GprsParLst[33]	  //.0 串口2 停止位长度
#define Pw_GprsParityBitLen2 w_GprsParLst[34] //.0 串口2 校验位长度
#define Pw_GprsFlowControl2 w_GprsParLst[35]  //.0 串口2 流量控制

// LORA 发射的设定参数 LoRaSettings  sx1276-LoRa.c
#define Pw_LoRaSetFreq w_GprsParLst[36]				//.0 设置 4350000
#define Pw_LoRaSetPower w_GprsParLst[37]			//.0 设置 20
#define Pw_LoRaSetSignalBW w_GprsParLst[38]			//.0 设置 0-9
#define Pw_LoRaSetSpreadingFactor w_GprsParLst[39]	//.0 设置 6-12
#define Pw_LoRaSetErrorCoding w_GprsParLst[40]		//.0 设置 1-4
#define Pw_LoRaSetCrcOn w_GprsParLst[41]			//.0 0-1
#define Pw_LoRaSetImplicitHeaderOn w_GprsParLst[42] //.0 0-1
#define Pw_LoRaSetRxSingleOn w_GprsParLst[43]		//.0 0-1
#define Pw_LoRaSetFreqHopOn w_GprsParLst[44]		//.0 0
#define Pw_LoRaSetHopPeriod w_GprsParLst[45]		//.0
													// ZCL 2019.3.22 空间不够了
#define Pw_LoRaSetPayLoadLength w_GprsParLst[46]	//
#define Pw_LoRaSetTxPacketTimeOut w_GprsParLst[47]	//.0 //ZCL 2019.9.24 再重新加上
#define Pw_LoRaSetRxPacketTimeOut w_GprsParLst[48]	//.0 //ZCL 2019.9.24 再重新加上

// 特殊协议 YLS 2023.11.28
#define w_ZhouShanProtocol w_GprsParLst[49]										 // 特殊协议
#define w_ZhouShanProtocol_bit0 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 0)) // 定义第1路通道，=0，通用协议；=1舟山协议
#define w_ZhouShanProtocol_bit1 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 1)) // 定义第2路通道，=0，通用协议；=1舟山协议
#define w_ZhouShanProtocol_bit2 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 2)) // 定义第3路通道，=0，通用协议；=1舟山协议
#define w_ZhouShanProtocol_bit3 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 3)) // 定义第4路通道，=0，通用协议；=1舟山协议

#define w_ZhouShanProtocol_bit4 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 4)) // 4-6位定义主动发送间隔时间
#define w_ZhouShanProtocol_bit5 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 5)) // 000:20分钟；001；10分钟；010:5分钟；011；2分钟
#define w_ZhouShanProtocol_bit6 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 6)) // 100：1分钟；101:30秒；110:20秒；111：10秒
#define w_ZhouShanProtocol_bit7 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 7)) // =0，发送32个字；=1，发送40个字

#define w_ZhouShanProtocol_bit8 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 8))   // 串口2发送协议，=0，通用协议；=1，舟山协议
#define w_ZhouShanProtocol_bit9 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 9))   // 串口1主动发送，=1，主动发送，=0，不主动发送
#define w_ZhouShanProtocol_bit10 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 10)) // 串口1进行LoRa监控数据收发，=1，监控，=0，不监控
#define w_ZhouShanProtocol_bit11 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 11)) // 保留

#define w_ZhouShanProtocol_bit12 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 12)) // 保留
#define w_ZhouShanProtocol_bit13 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 13)) // 保留
#define w_ZhouShanProtocol_bit14 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 14)) // 保留
#define w_ZhouShanProtocol_bit15 MEM_ADDR(BITBAND((u32) & w_ZhouShanProtocol, 15)) // 保留

// 定义观看参数
#define w_GprsSaveBaudRate3 w_GprsParLst[50]	 //.0 保存串口3波特率
#define w_GprsSaveBaudRate2 w_GprsParLst[51]	 //.0 保存串口2波特率
#define w_GprsSaveDataBitLen2 w_GprsParLst[52]	 //.0 保存串口2 数据位长度
#define w_GprsSaveStopBitLen2 w_GprsParLst[53]	 //.0 保存串口2 停止位长度
#define w_GprsSaveParityBitLen2 w_GprsParLst[54] //.0 保存串口2 校验位长度
#define w_GprsSaveFlowControl2 w_GprsParLst[55]	 //.0 保存串口2 流量控制
#define w_GprsModPar w_GprsParLst[56]			 //.0 修改参数		//ZCL 2019.9.11 用w_ModPar=2000也可以来修改SCR参数
#define w_GprsParInitial w_GprsParLst[57]		 //.0 参数初始化 //ZCL 2019.9.11 只能用w_GprsParInitial来初始化SCR参数

#define w_GprsXmodemFTItem w_GprsParLst[61] //.0 Xmodem文件传输 项  ZCL 2019.4.5
#define w_GprsXmodemRcvLen w_GprsParLst[62] //.0 Xmodem接收长度
#define w_GprsTemp2 w_GprsParLst[63]		//.0
#define w_GprsCSQ w_GprsParLst[64]			//.0 信号强度  2017.5.25
#define w_GprsSoftVer w_GprsParLst[65]		//.2 软件版本
// ZCL 2019.9.24
#define w_LoRaDateRate w_GprsParLst[66] //.2 空中速率	 kbps

#define w_LoRaLeiDaModuleAdd w_GprsParLst[67] //.0 ZCL 2020.9.30 专门用于雷达监测的 SZM220地址
#define w_LoRaLeiDaDiValue w_GprsParLst[68]	  //.0 ZCL 2020.9.30 雷达监测的 SZM220 DI值
// 11 往下过程参数
#define w_ScrSaveKeyMode w_GprsParLst[71]	  //.0 保存液晶屏按键模式
#define w_ScrVERSION w_GprsParLst[72]		  //.0 液晶屏软件版本
#define w_ScrWriteYear w_GprsParLst[73]		  //.0 ZCL 2018.4.21  2018.8.4
#define w_ScrWriteDate w_GprsParLst[74]		  //.0 ZCL 2018.4.21  2018.8.4
#define w_ScrSaveMenuMode w_GprsParLst[75]	  //.0 保存菜单模式 	=0，BPDJ模式; =1，变频器模式；
#define w_ScrSaveCurrentBits w_GprsParLst[76] //.0 保存电流小数 	=2，正常模式; =1，大功率变频器模式；
#define w_ScrTemp1 w_GprsParLst[77]			  //.0 2018.10.19

#define w_ScrGpsLatDu w_GprsParLst[78]	 //.0 纬度 度；	ddmm.mmmm之dd
#define w_ScrGpsLatFen1 w_GprsParLst[79] //.0 纬度 分；		ddmm.mmmm之mm
#define w_ScrGpsLatFen2 w_GprsParLst[80] //.0 纬度 分；		ddmm.mmmm之mmmm
#define w_ScrGpsLonDu w_GprsParLst[81]	 //.0 经度 度；	dddmm.mmmm之ddd
#define w_ScrGpsLonFen1 w_GprsParLst[82] //.0 经度 分；		dddmm.mmmm之mm
#define w_ScrGpsLonFen2 w_GprsParLst[83] //.0 经度 分；		dddmm.mmmm之mmmm

// ZCL 2019.3.20说明
//  w_ScrGpsLatAllDu1 和 w_ScrGpsLatAllDu2 合起来是纬度
//  w_ScrGpsLonAllDu1 和 w_ScrGpsLonAllDu2 合起来是经度

// #define	w_ScrGpsLatAllDu1				w_GprsParLst[84]		//.2 纬度 度1； 如：35.44			2位小数
// #define	w_ScrGpsLatAllDu2				w_GprsParLst[85]		//.0 纬度 度2；	00.00****			****部分，添加到w_ScrGpsLatAllDu1后面。 如：****是 2459， 则总纬度是： 35.442459
// #define	w_ScrGpsLonAllDu1				w_GprsParLst[86]		//.2 经度 度1；	如：127.39		2位小数
// #define	w_ScrGpsLonAllDu2				w_GprsParLst[87]		//.0 经度 度2； 00.00****			****部分，添加到w_ScrGpsLonAllDu1后面。如：****是 4628， 则总经度是： 127.394628

// ZCL 2019.3.11
// 变频电机1

// ZCL 2019.3.11
// 变频电机1
#define w_dsp1SoftVersion w_GprsParLst[88]	  //.2 DSP软件系统版本号	0~65535
#define w_dsp1NowHz w_GprsParLst[89]		  //.1 当前频率	0~500	x10
#define w_dsp1APhaseCurrent w_GprsParLst[90]  //.2 A相电流 0~65535	x100
#define w_dsp1OutVoltage w_GprsParLst[91]	  //.0 输出电压	0~500	x1
#define w_dsp1NowTemperature w_GprsParLst[92] //.1 当前温度	0~1000	x10
#define w_dsp1SysErrorCode1 w_GprsParLst[93]  //.0 系统错误码E0	0~65535	 使用位
#define w_dsp1SysRunStatus1 w_GprsParLst[94]  //.0 系统运行状态码1		使用位
#define w_dsp1AI1ADCValue w_GprsParLst[95]	  //.0 AI1 ADC采集值 ZCL 2016.6.21
#define w_dsp1AI2ADCValue w_GprsParLst[96]	  //.0 AI2 ADC采集值
#define w_dsp1Counter3 w_GprsParLst[97]		  //.0 DCM220 DCM100: dsp计数器，秒

// #define	w_dsp1BusBarVoltage			w_GprsParLst[93]		//.0 母线电压	0~800	x1
// #define	w_dsp1TargetHz					w_GprsParLst[99]	//.1 目标频率	0~500	x10		暂留

// #define	w_dsp1Tmp1							w_GprsParLst[13]
// #define	w_dsp1Tmp2							w_GprsParLst[14]
// #define	w_dsp1Tmp3							w_GprsParLst[15]

// 变频电机2
#define w_dsp2SoftVersion w_GprsParLst[98]	   //.2 DSP软件系统版本号	0~65535
#define w_dsp2NowHz w_GprsParLst[99]		   //.1 当前频率	0~500	x10
#define w_dsp2APhaseCurrent w_GprsParLst[100]  //.2 A相电流 0~65535	x100
#define w_dsp2OutVoltage w_GprsParLst[101]	   //.0 输出电压	0~500	x1
#define w_dsp2NowTemperature w_GprsParLst[102] //.1 当前温度	0~1000	x10
#define w_dsp2SysErrorCode1 w_GprsParLst[103]  //.0 系统错误码E0	0~65535	 使用位
#define w_dsp2SysRunStatus1 w_GprsParLst[104]  //.0 系统运行状态码1		使用位
#define w_dsp2AI1ADCValue w_GprsParLst[105]	   //.0 AI1 ADC采集值 ZCL 2016.6.21
#define w_dsp2AI2ADCValue w_GprsParLst[106]	   //.0 AI2 ADC采集值
#define w_dsp2Counter3 w_GprsParLst[107]	   //.0 DCM220 DCM100: dsp计数器，秒

// 变频电机3
#define w_dsp3SoftVersion w_GprsParLst[108]	   //.2 DSP软件系统版本号	0~65535
#define w_dsp3NowHz w_GprsParLst[109]		   //.1 当前频率	0~500	x10
#define w_dsp3APhaseCurrent w_GprsParLst[110]  //.2 A相电流 0~65535	x100
#define w_dsp3OutVoltage w_GprsParLst[111]	   //.0 输出电压	0~500	x1
#define w_dsp3NowTemperature w_GprsParLst[112] //.1 当前温度	0~1000	x10
#define w_dsp3SysErrorCode1 w_GprsParLst[113]  //.0 系统错误码E0	0~65535	 使用位
#define w_dsp3SysRunStatus1 w_GprsParLst[114]  //.0 系统运行状态码1		使用位
#define w_dsp3AI1ADCValue w_GprsParLst[115]	   //.0 AI1 ADC采集值 ZCL 2016.6.21
#define w_dsp3AI2ADCValue w_GprsParLst[116]	   //.0 AI2 ADC采集值
#define w_dsp3Counter3 w_GprsParLst[117]	   //.0 DCM220 DCM100: dsp计数器，秒

// 变频电机4
#define w_dsp4SoftVersion w_GprsParLst[118]	   //.2 DSP软件系统版本号	0~65535
#define w_dsp4NowHz w_GprsParLst[119]		   //.1 当前频率	0~500	x10
#define w_dsp4APhaseCurrent w_GprsParLst[120]  //.2 A相电流 0~65535	x100
#define w_dsp4OutVoltage w_GprsParLst[121]	   //.0 输出电压	0~500	x1
#define w_dsp4NowTemperature w_GprsParLst[122] //.1 当前温度	0~1000	x10
#define w_dsp4SysErrorCode1 w_GprsParLst[123]  //.0 系统错误码E0	0~65535	 使用位
#define w_dsp4SysRunStatus1 w_GprsParLst[124]  //.0 系统运行状态码1		使用位
#define w_dsp4AI1ADCValue w_GprsParLst[125]	   //.0 AI1 ADC采集值 ZCL 2016.6.21
#define w_dsp4AI2ADCValue w_GprsParLst[126]	   //.0 AI2 ADC采集值
#define w_dsp4Counter3 w_GprsParLst[127]	   //.0 DCM220 DCM100: dsp计数器，秒

// ZCL 2019.9.11  变频电机1
#define w_1InPDec w_GprsParLst[128]				//.3 进水压力
#define w_1OutPDec w_GprsParLst[129]			//.3 出水压力
#define w_1PIDCalcP w_GprsParLst[130]			//.3 设定压力
#define w_1EquipOperateStatus w_GprsParLst[131] //.0 供水状态
#define w_1EquipAlarmStatus w_GprsParLst[132]	//.0 停机原因
#define w_1ScrEquipPower w_GprsParLst[133]		//.0 设备功率
#define w_1NoUse1 w_GprsParLst[134]				//.0 预留1
#define w_1NoUse2 w_GprsParLst[135]				//.0 预留2
#define w_1NoUse3 w_GprsParLst[136]				//.0 预留3
#define w_1NoUse4 w_GprsParLst[137]				//.0 预留4

// ZCL 2019.9.11  变频电机2
#define w_2InPDec w_GprsParLst[138]				//.3 进水压力
#define w_2OutPDec w_GprsParLst[139]			//.3 出水压力
#define w_2PIDCalcP w_GprsParLst[140]			//.3 设定压力
#define w_2EquipOperateStatus w_GprsParLst[141] //.0 供水状态
#define w_2EquipAlarmStatus w_GprsParLst[142]	//.0 停机原因
#define w_2ScrEquipPower w_GprsParLst[143]		//.0 设备功率
#define w_2NoUse1 w_GprsParLst[144]				//.0 预留1
#define w_2NoUse2 w_GprsParLst[145]				//.0 预留2
#define w_2NoUse3 w_GprsParLst[146]				//.0 预留3
#define w_2NoUse4 w_GprsParLst[147]				//.0 预留4

// ZCL 2019.9.11  变频电机3
#define w_3InPDec w_GprsParLst[148]				//.3 进水压力
#define w_3OutPDec w_GprsParLst[149]			//.3 出水压力
#define w_3PIDCalcP w_GprsParLst[150]			//.3 设定压力
#define w_3EquipOperateStatus w_GprsParLst[151] //.0 供水状态
#define w_3EquipAlarmStatus w_GprsParLst[152]	//.0 停机原因
#define w_3ScrEquipPower w_GprsParLst[153]		//.0 设备功率
#define w_3NoUse1 w_GprsParLst[154]				//.0 预留1
#define w_3NoUse2 w_GprsParLst[155]				//.0 预留2
#define w_3NoUse3 w_GprsParLst[156]				//.0 预留3
#define w_3NoUse4 w_GprsParLst[157]				//.0 预留4

// ZCL 2019.9.11  变频电机4
#define w_4InPDec w_GprsParLst[158]				//.3 进水压力
#define w_4OutPDec w_GprsParLst[159]			//.3 出水压力
#define w_4PIDCalcP w_GprsParLst[160]			//.3 设定压力
#define w_4EquipOperateStatus w_GprsParLst[161] //.0 供水状态
#define w_4EquipAlarmStatus w_GprsParLst[162]	//.0 停机原因
#define w_4ScrEquipPower w_GprsParLst[163]		//.0 设备功率
#define w_4NoUse1 w_GprsParLst[164]				//.0 预留1
#define w_4NoUse2 w_GprsParLst[165]				//.0 预留2
#define w_4NoUse3 w_GprsParLst[166]				//.0 预留3
#define w_4NoUse4 w_GprsParLst[167]				//.0 预留4

#define w_Com1TXCounter w_GprsParLst[168] // ZCL 2019.10.19 计数器，通讯时观察用
#define w_Com1RXCounter w_GprsParLst[169] // ZCL 2019.10.19 计数器，通讯时观察用

#define w_LoraCounter1 w_GprsParLst[170] // LORA	计数器
#define w_LoraCounter2 w_GprsParLst[171] // LORA	计数器
#define w_LoraCounter3 w_GprsParLst[172] // LORA	计数器
#define w_LoraCounter4 w_GprsParLst[173] // LORA	计数器
#define w_LoraCounter5 w_GprsParLst[174] // LORA	计数器
#define w_LoraCounter6 w_GprsParLst[174] // LORA	计数器
#define w_LoraCounter7 w_GprsParLst[174] // LORA	计数器
#define w_LoraCounter8 w_GprsParLst[174] // LORA	计数器

// u16  	w_GprsParLst[180];  //ZCL 2019.10.19 注意长度

// ZCL 2019.2.18
/* 	(0). Pw_LoRaSet .0位 =1   LORA运行	F_LoRaEn
		(1). Pw_LoRaSet .1位 =1   LORA输出到COM2 (透传使用)	F_LoRaToCom

		(0). Pw_GprsSet .0位 =1   GPRS运行	F_GprsEn
		(1). Pw_GprsSet .1位 =1   GPRS透传模式，输出到COM2（此模式下=GP311功能）	F_GprsTransToCom

		(2). Pw_GprsSet .2位 =1   主机GPRS模式		F_GprsMasterNotToCom					//ZCL 新加  2019.3.11
		(3). Pw_GprsSet .3位 =1   主机GPRS模式下的接收数据输出到COM2，方便调试用	F_GprsMasterToCom //ZCL 新加  2019.3.11

		(4). Pw_GprsSet .4位 =1  即：0x0010 	//GPS！ 透传数据输出到COM2  .4位 值是16		F_GpsToCom

	 */

#define F_LoRaEn MEM_ADDR(BITBAND((u32) & Pw_LoRaSet, 0))	 // LORA运行	0/1
#define F_LoRaToCom MEM_ADDR(BITBAND((u32) & Pw_LoRaSet, 1)) // LORA输出到COM2 (透传使用)		0/1

#define F_GprsEn MEM_ADDR(BITBAND((u32) & Pw_GprsSet, 0))		  // GPRS运行	0/1
#define F_GprsTransToCom MEM_ADDR(BITBAND((u32) & Pw_GprsSet, 1)) // GPRS透传模式，输出到COM2		0/1

// ZCL 2019.3.11 主机GPRS模式下，不输出信息到串口（默认）	0/1
#define F_GprsMasterNotToCom MEM_ADDR(BITBAND((u32) & Pw_GprsSet, 2)) // MasterNotToCom

// ZCL 2019.3.11 主机GPRS模式，设定输出信息到串口	 2019.3.11
#define F_GprsMasterToCom MEM_ADDR(BITBAND((u32) & Pw_GprsSet, 3)) // MasterToCom

#define F_GpsToCom MEM_ADDR(BITBAND((u32) & Pw_GprsSet, 4)) // GPS！ 透传数据输出到COM2	0/1

// ZCL 2019.3.11
// ZCL 2019.9.28  21个  25个  水表
// 下面这些参数从监控变频电机的程序中移植过来，原来是在w_dsp1ParLst[88]-[114]中的
#define w_WaterMeterID1 w_GprsParLst[188]		   //.1
#define w_WaterMeterID2 w_GprsParLst[189]		   //.1
#define w_WaterMeterSum1 w_GprsParLst[190]		   //.1
#define w_WaterMeterSum2 w_GprsParLst[191]		   //.1
#define w_WaterMeterInstantFlow1 w_GprsParLst[192] //.1
#define w_WaterMeterInstantFlow2 w_GprsParLst[193] //.1
#define w_WaterMeterInSum1 w_GprsParLst[194]	   //.x
#define w_WaterMeterInSum2 w_GprsParLst[195]	   //.x

#define w_WaterMeterOutSum1 w_GprsParLst[196]	   //.x
#define w_WaterMeterOutSum2 w_GprsParLst[197]	   //.x
#define w_WaterMeterRemainMoney1 w_GprsParLst[198] //.x
#define w_WaterMeterRemainMoney2 w_GprsParLst[199] //.x
#define w_WaterMeterOverdraft1 w_GprsParLst[200]   //.x
#define w_WaterMeterOverdraft2 w_GprsParLst[201]   //.x
#define w_WaterMeterStatus1 w_GprsParLst[202]	   //.x
#define w_WaterMeterStatus2 w_GprsParLst[203]	   //.x

#define w_WaterMeterNoUse1 w_GprsParLst[204]  //.x
#define w_WaterMeterNoUse2 w_GprsParLst[205]  //.x
#define w_WaterMeterTime1 w_GprsParLst[206]	  //.x
#define w_WaterMeterTime2 w_GprsParLst[207]	  //.x
#define w_WaterMeterTime3 w_GprsParLst[208]	  //.x
#define w_WaterMeterSoftVer w_GprsParLst[209] //.2 软件版本
#define w_WaterMeterNoUse3 w_GprsParLst[210]  //.x
#define w_WaterMeterNoUse4 w_GprsParLst[211]  //.x
#define w_WaterMeterNoUse5 w_GprsParLst[212]  //.x
#define w_WaterMeterNoUse6 w_GprsParLst[213]  //.x
#define w_WaterMeterNoUse7 w_GprsParLst[214]  //.x

// ZCL 2019.11.21 Pw_LoRaSet、Pw_GprsSet设定和测试说明：
// （1）通常LORA主机设定：
//  Pw_LoRaSet=1（F_LoRaEn=1，F_LoRaToCom=0）；
//  Pw_GprsSet=9（F_GprsEn=1，F_GprsTransToCom=0，F_GprsMasterNotToCom=0，F_GprsMasterToCom=1）

// （2）当使用串口2访问LORA主机时，设定说明：
//  Pw_LoRaSet=1（F_LoRaEn=1，F_LoRaToCom=0）；
//  Pw_GprsSet=0（F_GprsEn=0，F_GprsTransToCom=0，F_GprsMasterNotToCom=0，F_GprsMasterToCom=0） GPRS关闭
//  Pw_GprsSet=1 GPRS打开，指令不送到COM。能连接上宏电 gprsdemo.exe，用demo发指令，没有返回；
//  Pw_GprsSet=5 GPRS打开，指令不送到COM。因为F_GprsMasterNotToCom=1，能连接上宏电 gprsdemo.exe，用demo发指令，可以返回；

//----------LoRa 主机（或者 单个从机） 参数保存区
// u16  	w_dsp1ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7
// u16  	w_dsp2ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7
// u16  	w_dsp3ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7
// u16  	w_dsp4ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7
// ZCL 2019.3.11 删除，放到w_GprsParLst[]

// ZCL 2022.6.1 220的LORA远传给主机的。 备注：ZCL 2022.9.13  通过485采集的子机的电能数据，送到这个区就可以
//  电能表1直接是本机的填充到这里。 电能表2,3,4 通讯后，填充到这里。
// 电能表1
#define w_Lora1PtL w_DNBParLst[0] // 合相有功功率
#define w_Lora1PtH w_DNBParLst[1] // 合相有功功率

#define w_Lora1SetVfFreq w_DNBParLst[2]	 // 设定的变频频率 ZCL 2023.3.20
#define w_Lora1OutVoltage w_DNBParLst[3] // 输出电压  ZCL 2023.3.20

#define w_Lora1EptL w_DNBParLst[4] // 合相有功电能（可配置读后清零）
#define w_Lora1EptH w_DNBParLst[5] // 合相有功电能（可配置读后清零）

#define w_Lora1VfErrCode w_DNBParLst[6]	  // 变频器故障代码  ZCL 2023.2.10
#define w_Lora1SoftVersion w_DNBParLst[7] // 软件版本
#define w_Lora1XiaoLv w_DNBParLst[8]	  // 效率
#define w_Lora1StopNum w_DNBParLst[9]	  // 停机数量 ZCL 2023.2.10		//压力2

#define w_Lora1ShunShiFlowL w_DNBParLst[10] // 瞬时流量  ZCL 2022.5.17
#define w_Lora1ShunShiFlowH w_DNBParLst[11] // 瞬时流量

#define w_Lora1SumFlowL w_DNBParLst[12] // 累计流量   ZCL注：文本显示器里用 低字在前 高字在后
#define w_Lora1SumFlowH w_DNBParLst[13] // 累计流量

#define w_Lora1UabRms w_DNBParLst[14] // Ab线电压有效值

#define w_Lora1TouchRunStop w_DNBParLst[15] // 触摸 启动/停止  =1 运行；=0 停机

#define w_Lora1Speed w_DNBParLst[16] // 电机转速

#define w_Lora1Pft w_DNBParLst[17] // 合相功率因数

#define w_Lora1Counter1_BPQ w_DNBParLst[18] // 电能表/变频器计数器
#define w_Lora1Counter2 w_DNBParLst[19]		// SZM220计数器

#define w_Lora1IaRms w_DNBParLst[20] // A相电流有效值

#define w_Lora1YeWei1 w_DNBParLst[21] // 液位1  ZCL 2023.8.8
#define w_Lora1YeWei2 w_DNBParLst[22] // 液位2  ZCL 2023.8.8

#define w_Lora1PumpRunSecond w_DNBParLst[23] //	电机累计运行时间秒
#define w_Lora1PumpRunMinute w_DNBParLst[24] //	电机累计运行时间分钟
#define w_Lora1PumpRunHourL w_DNBParLst[25]	 //	电机累计运行时间小时低字
#define w_Lora1PumpRunHourH w_DNBParLst[26]	 //	电机累计运行时间小时高字

#define w_Lora1VvvfFreq w_DNBParLst[27] // 变频器频率  ZCL 2022.8.19

#define w_Lora1InP w_DNBParLst[28]			 // 进水压力
#define w_Lora1OutP w_DNBParLst[29]			 // 出水压力
#define w_Lora1WenDu1 w_DNBParLst[30]		 // 温度1
#define w_Lora1WenDu2 w_DNBParLst[31]		 // 温度2
#define w_Lora1SetP w_DNBParLst[32]			 // 设定压力
#define w_Lora1Flag1Unit w_DNBParLst[33]	 // 标志1单元  设备状态
#define w_Lora1Flag2Unit w_DNBParLst[34]	 // 标志2单元
#define w_Lora1Flag3Unit w_DNBParLst[35]	 // 标志3单元
#define w_Lora1Pump12Status w_DNBParLst[36]	 // 泵状态
#define w_Lora1RemoteZDRunHz w_DNBParLst[37] // 遥控定频  ZCL 2024.1.5  大于10.0HZ才起作用

#define F_LoRa1_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora1Flag3Unit, 0))	  // 1#泵LORA通讯异常	0/1
#define F_LoRa1_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora1Flag3Unit, 1)) // 1#泵变频器通讯异常	0/1

// 电能表2
#define w_Lora2PtL w_DNBParLst[0 + 40] // 合相有功功率
#define w_Lora2PtH w_DNBParLst[1 + 40] // 合相有功功率

#define w_Lora2StL w_DNBParLst[2 + 40] // 合相视在功率
#define w_Lora2StH w_DNBParLst[3 + 40] // 合相视在功率

#define w_Lora2EptL w_DNBParLst[4 + 40] // 合相有功电能（可配置读后清零）
#define w_Lora2EptH w_DNBParLst[5 + 40] // 合相有功电能（可配置读后清零）

#define w_Lora2YaLi w_DNBParLst[6 + 40]		   // 压力  未使用，预留
#define w_Lora2SoftVersion w_DNBParLst[7 + 40] // 软件版本
#define w_Lora2XiaoLv w_DNBParLst[8 + 40]	   // 效率
#define w_Lora2YaLi2 w_DNBParLst[9 + 40]	   // 压力2  未使用，预留

#define w_Lora2ShunShiFlowL w_DNBParLst[10 + 40] // 瞬时流量  ZCL 2022.5.17
#define w_Lora2ShunShiFlowH w_DNBParLst[11 + 40] // 瞬时流量

#define w_Lora2SumFlowL w_DNBParLst[12 + 40] // 累计流量   ZCL注：文本显示器里用 低字在前 高字在后
#define w_Lora2SumFlowH w_DNBParLst[13 + 40] // 累计流量

#define w_Lora2UabRms w_DNBParLst[14 + 40] // Ab线电压有效值

#define w_Lora2TouchRunStop w_DNBParLst[15 + 40] // 触摸 启动/停止  =1 运行；=0 停机

#define w_Lora2Speed w_DNBParLst[16 + 40] // 电机转速

#define w_Lora2Pft w_DNBParLst[17 + 40] // 合相功率因数

#define w_Lora2Counter1_BPQ w_DNBParLst[18 + 40] // 电能表计数器
#define w_Lora2Counter2 w_DNBParLst[19 + 40]	 // SZM220计数器

#define w_Lora2IaRms w_DNBParLst[20 + 40] // A相电流有效值

#define w_Lora2QtL w_DNBParLst[21 + 40] // 合相无功功率
#define w_Lora2QtH w_DNBParLst[22 + 40] // 合相无功功率

#define w_Lora2PumpRunSecond w_DNBParLst[23 + 40] //	电机累计运行时间秒
#define w_Lora2PumpRunMinute w_DNBParLst[24 + 40] //	电机累计运行时间分钟
#define w_Lora2PumpRunHourL w_DNBParLst[25 + 40]  //	电机累计运行时间小时低字
#define w_Lora2PumpRunHourH w_DNBParLst[26 + 40]  //	电机累计运行时间小时高字

#define w_Lora2VvvfFreq w_DNBParLst[27 + 40] // 变频器频率  ZCL 2022.8.19

#define w_Lora2InP w_DNBParLst[28 + 40]			 // 进水压力
#define w_Lora2OutP w_DNBParLst[29 + 40]		 // 出水压力
#define w_Lora2WenDu1 w_DNBParLst[30 + 40]		 // 温度1
#define w_Lora2WenDu2 w_DNBParLst[31 + 40]		 // 温度2
#define w_Lora2SetP w_DNBParLst[32 + 40]		 // 设定压力
#define w_Lora2Flag1Unit w_DNBParLst[33 + 40]	 // 标志1单元  设备状态
#define w_Lora2Flag2Unit w_DNBParLst[34 + 40]	 // 标志2单元
#define w_Lora2Flag3Unit w_DNBParLst[35 + 40]	 // 标志3单元
#define w_Lora2Pump12Status w_DNBParLst[36 + 40] // 泵状态

#define F_LoRa2_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora2Flag3Unit, 0))	  // 2#泵LORA通讯异常	0/1
#define F_LoRa2_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora2Flag3Unit, 1)) // 2#泵变频器通讯异常	0/1

// 电能表3  ZCL  2022.9.13
#define w_Lora3PtL w_DNBParLst[0 + 80] // 合相有功功率
#define w_Lora3PtH w_DNBParLst[1 + 80] // 合相有功功率

#define w_Lora3StL w_DNBParLst[2 + 80] // 合相视在功率
#define w_Lora3StH w_DNBParLst[3 + 80] // 合相视在功率

#define w_Lora3EptL w_DNBParLst[4 + 80] // 合相有功电能（可配置读后清零）
#define w_Lora3EptH w_DNBParLst[5 + 80] // 合相有功电能（可配置读后清零）

#define w_Lora3YaLi w_DNBParLst[6 + 80]		   // 压力  未使用，预留
#define w_Lora3SoftVersion w_DNBParLst[7 + 80] // 软件版本
#define w_Lora3XiaoLv w_DNBParLst[8 + 80]	   // 效率
#define w_Lora3YaLi2 w_DNBParLst[9 + 80]	   // 压力2  未使用，预留

#define w_Lora3ShunShiFlowL w_DNBParLst[10 + 80] // 瞬时流量  ZCL 2022.5.17
#define w_Lora3ShunShiFlowH w_DNBParLst[11 + 80] // 瞬时流量

#define w_Lora3SumFlowL w_DNBParLst[12 + 80] // 累计流量   ZCL注：文本显示器里用 低字在前 高字在后
#define w_Lora3SumFlowH w_DNBParLst[13 + 80] // 累计流量

#define w_Lora3UabRms w_DNBParLst[14 + 80] // Ab线电压有效值

#define w_Lora3TouchRunStop w_DNBParLst[15 + 80] // 触摸 启动/停止  =1 运行；=0 停机

#define w_Lora3Speed w_DNBParLst[16 + 80] // 电机转速

#define w_Lora3Pft w_DNBParLst[17 + 80] // 合相功率因数

#define w_Lora3Counter1_BPQ w_DNBParLst[18 + 80] // 电能表计数器
#define w_Lora3Counter2 w_DNBParLst[19 + 80]	 // SZM220计数器

#define w_Lora3IaRms w_DNBParLst[20 + 80] // A相电流有效值

#define w_Lora3QtL w_DNBParLst[21 + 80] // 合相无功功率
#define w_Lora3QtH w_DNBParLst[22 + 80] // 合相无功功率

#define w_Lora3PumpRunSecond w_DNBParLst[23 + 80] //	电机累计运行时间秒
#define w_Lora3PumpRunMinute w_DNBParLst[24 + 80] //	电机累计运行时间分钟
#define w_Lora3PumpRunHourL w_DNBParLst[25 + 80]  //	电机累计运行时间小时低字
#define w_Lora3PumpRunHourH w_DNBParLst[26 + 80]  //	电机累计运行时间小时高字

#define w_Lora3VvvfFreq w_DNBParLst[27 + 80] // 变频器频率  ZCL 2022.8.19

#define w_Lora3InP w_DNBParLst[28 + 80]			 // 进水压力
#define w_Lora3OutP w_DNBParLst[29 + 80]		 // 出水压力
#define w_Lora3WenDu1 w_DNBParLst[30 + 80]		 // 温度1
#define w_Lora3WenDu2 w_DNBParLst[31 + 80]		 // 温度2
#define w_Lora3SetP w_DNBParLst[32 + 80]		 // 设定压力
#define w_Lora3Flag1Unit w_DNBParLst[33 + 80]	 // 标志1单元  设备状态
#define w_Lora3Flag2Unit w_DNBParLst[34 + 80]	 // 标志2单元
#define w_Lora3Flag3Unit w_DNBParLst[35 + 80]	 // 标志3单元
#define w_Lora3Pump12Status w_DNBParLst[36 + 80] // 泵状态

#define F_LoRa3_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora3Flag3Unit, 0))	  // 3#泵LORA通讯异常	0/1
#define F_LoRa3_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora3Flag3Unit, 1)) // 3#泵变频器通讯异常	0/1

// 电能表4  ZCL  2022.9.13
#define w_Lora4PtL w_DNBParLst[0 + 120] // 合相有功功率
#define w_Lora4PtH w_DNBParLst[1 + 120] // 合相有功功率

#define w_Lora4StL w_DNBParLst[2 + 120] // 合相视在功率
#define w_Lora4StH w_DNBParLst[3 + 120] // 合相视在功率

#define w_Lora4EptL w_DNBParLst[4 + 120] // 合相有功电能（可配置读后清零）
#define w_Lora4EptH w_DNBParLst[5 + 120] // 合相有功电能（可配置读后清零）

#define w_Lora4YaLi w_DNBParLst[6 + 120]		// 压力  未使用，预留
#define w_Lora4SoftVersion w_DNBParLst[7 + 120] // 软件版本
#define w_Lora4XiaoLv w_DNBParLst[8 + 120]		// 效率
#define w_Lora4YaLi2 w_DNBParLst[9 + 120]		// 压力2  未使用，预留

#define w_Lora4ShunShiFlowL w_DNBParLst[10 + 120] // 瞬时流量  ZCL 2022.5.17
#define w_Lora4ShunShiFlowH w_DNBParLst[11 + 120] // 瞬时流量

#define w_Lora4SumFlowL w_DNBParLst[12 + 120] // 累计流量   ZCL注：文本显示器里用 低字在前 高字在后
#define w_Lora4SumFlowH w_DNBParLst[13 + 120] // 累计流量

#define w_Lora4UabRms w_DNBParLst[14 + 120] // Ab线电压有效值

#define w_Lora4TouchRunStop w_DNBParLst[15 + 120] // 触摸 启动/停止  =1 运行；=0 停机

#define w_Lora4Speed w_DNBParLst[16 + 120] // 电机转速

#define w_Lora4Pft w_DNBParLst[17 + 120] // 合相功率因数

#define w_Lora4Counter1_BPQ w_DNBParLst[18 + 120] // 电能表计数器
#define w_Lora4Counter2 w_DNBParLst[19 + 120]	  // SZM220计数器

#define w_Lora4IaRms w_DNBParLst[20 + 120] // A相电流有效值

#define w_Lora4QtL w_DNBParLst[21 + 120] // 合相无功功率
#define w_Lora4QtH w_DNBParLst[22 + 120] // 合相无功功率

#define w_Lora4PumpRunSecond w_DNBParLst[23 + 120] //	电机累计运行时间秒
#define w_Lora4PumpRunMinute w_DNBParLst[24 + 120] //	电机累计运行时间分钟
#define w_Lora4PumpRunHourL w_DNBParLst[25 + 120]  //	电机累计运行时间小时低字
#define w_Lora4PumpRunHourH w_DNBParLst[26 + 120]  //	电机累计运行时间小时高字

#define w_Lora4VvvfFreq w_DNBParLst[27 + 120] // 变频器频率  ZCL 2022.8.19

#define w_Lora4InP w_DNBParLst[28 + 120]		  // 进水压力
#define w_Lora4OutP w_DNBParLst[29 + 120]		  // 出水压力
#define w_Lora4WenDu1 w_DNBParLst[30 + 120]		  // 温度1
#define w_Lora4WenDu2 w_DNBParLst[31 + 120]		  // 温度2
#define w_Lora4SetP w_DNBParLst[32 + 120]		  // 设定压力
#define w_Lora4Flag1Unit w_DNBParLst[33 + 120]	  // 标志1单元  设备状态
#define w_Lora4Flag2Unit w_DNBParLst[34 + 120]	  // 标志2单元
#define w_Lora4Flag3Unit w_DNBParLst[35 + 120]	  // 标志3单元
#define w_Lora4Pump12Status w_DNBParLst[36 + 120] // 泵状态

#define F_LoRa4_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora4Flag3Unit, 0))	  // 4#泵LORA通讯异常	0/1
#define F_LoRa4_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora4Flag3Unit, 1)) // 4#泵变频器通讯异常	0/1

// 电能表5
#define w_Lora5PtL w_DNBParLst[0 + 160] // 合相有功功率
#define w_Lora5PtH w_DNBParLst[1 + 160] // 合相有功功率
#define w_Lora5StL w_DNBParLst[2 + 160] // 合相视在功率
#define w_Lora5StH w_DNBParLst[3 + 160] // 合相视在功率

#define w_Lora5EptL w_DNBParLst[4 + 160] // 合相有功电能（可配置读后清零）
#define w_Lora5EptH w_DNBParLst[5 + 160] // 合相有功电能（可配置读后清零）

#define w_Lora5YaLi w_DNBParLst[6 + 160]		// 压力  未使用，预留
#define w_Lora5SoftVersion w_DNBParLst[7 + 160] // 软件版本
#define w_Lora5XiaoLv w_DNBParLst[8 + 160]		// 效率
#define w_Lora5YaLi2 w_DNBParLst[9 + 160]		// 压力2  未使用，预留

#define w_Lora5ShunShiFlowL w_DNBParLst[10 + 160] // 瞬时流量  ZCL 2022.5.17
#define w_Lora5ShunShiFlowH w_DNBParLst[11 + 160] // 瞬时流量

#define w_Lora5SumFlowL w_DNBParLst[12 + 160] // 累计流量   ZCL注：文本显示器里用 低字在前 高字在后
#define w_Lora5SumFlowH w_DNBParLst[13 + 160] // 累计流量

#define w_Lora5UabRms w_DNBParLst[14 + 160] // Ab线电压有效值

#define w_Lora5TouchRunStop w_DNBParLst[15 + 160] // 触摸 启动/停止  =1 运行；=0 停机

#define w_Lora5Speed w_DNBParLst[16 + 160] // 电机转速

#define w_Lora5Pft w_DNBParLst[17 + 160] // 合相功率因数

#define w_Lora5Counter1_BPQ w_DNBParLst[18 + 160] // 电能表计数器
#define w_Lora5Counter2 w_DNBParLst[19 + 160]	  // SZM220计数器

#define w_Lora5IaRms w_DNBParLst[20 + 160] // A相电流有效值

#define w_Lora5QtL w_DNBParLst[21 + 160] // 合相无功功率
#define w_Lora5QtH w_DNBParLst[22 + 160] // 合相无功功率

#define w_Lora5PumpRunSecond w_DNBParLst[23 + 160] //	电机累计运行时间秒
#define w_Lora5PumpRunMinute w_DNBParLst[24 + 160] //	电机累计运行时间分钟
#define w_Lora5PumpRunHourL w_DNBParLst[25 + 160]  //	电机累计运行时间小时低字
#define w_Lora5PumpRunHourH w_DNBParLst[26 + 160]  //	电机累计运行时间小时高字

#define w_Lora5VvvfFreq w_DNBParLst[27 + 160] // 变频器频率  ZCL 2022.8.19

#define w_Lora5InP w_DNBParLst[28 + 160]		  // 进水压力
#define w_Lora5OutP w_DNBParLst[29 + 160]		  // 出水压力
#define w_Lora5WenDu1 w_DNBParLst[30 + 160]		  // 温度1
#define w_Lora5WenDu2 w_DNBParLst[31 + 160]		  // 温度2
#define w_Lora5SetP w_DNBParLst[32 + 160]		  // 设定压力
#define w_Lora5Flag1Unit w_DNBParLst[33 + 160]	  // 标志1单元  设备状态
#define w_Lora5Flag2Unit w_DNBParLst[34 + 160]	  // 标志2单元
#define w_Lora5Flag3Unit w_DNBParLst[35 + 160]	  // 标志3单元
#define w_Lora5Pump12Status w_DNBParLst[36 + 160] // 泵状态

#define F_LoRa5_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora5Flag3Unit, 0))	  // 5#泵LORA通讯异常	0/1
#define F_LoRa5_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora5Flag3Unit, 1)) // 5#泵变频器通讯异常	0/1

// 电能表6
#define w_Lora6Flag3Unit w_DNBParLst[35 + 200]								  // 标志3单元
#define F_LoRa6_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora6Flag3Unit, 0))	  // 6#泵LORA通讯异常	0/1
#define F_LoRa6_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora6Flag3Unit, 1)) // 6#泵变频器通讯异常	0/1

// 电能表7
#define w_Lora7Flag3Unit w_DNBParLst[35 + 240]								  // 标志3单元
#define F_LoRa7_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora7Flag3Unit, 0))	  // 7#泵LORA通讯异常	0/1
#define F_LoRa7_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora7Flag3Unit, 1)) // 7#泵变频器通讯异常	0/1

// 电能表8
#define w_Lora8Flag3Unit w_DNBParLst[35 + 280]								  // 标志3单元
#define F_LoRa8_Comm_Fault MEM_ADDR(BITBAND((u32) & w_Lora8Flag3Unit, 0))	  // 8#泵LORA通讯异常	0/1
#define F_LoRa8_VVVFComm_Fault MEM_ADDR(BITBAND((u32) & w_Lora8Flag3Unit, 1)) // 8#泵变频器通讯异常	0/1

// 启停地址定义，用于通过液晶屏修改直接控制启停
#define Addr_Lora1TouchRunStop 15  // 触摸 启动/停止  =1 运行；=0 停机
#define Addr_Lora2TouchRunStop 55  // 触摸 启动/停止  =1 运行；=0 停机
#define Addr_Lora3TouchRunStop 95  // 触摸 启动/停止  =1 运行；=0 停机
#define Addr_Lora4TouchRunStop 135 // 触摸 启动/停止  =1 运行；=0 停机
#define Addr_Lora5TouchRunStop 175 // 触摸 启动/停止  =1 运行；=0 停机

// ZCL 2022.5.19   2019.3.20说明
//  w_ScrGpsLatAllDu1 和 w_ScrGpsLatAllDu2 合起来是纬度
//  w_ScrGpsLonAllDu1 和 w_ScrGpsLonAllDu2 合起来是经度
#define w_DNBScrGpsLatAllDu1 w_DNBParLst[160 + 500] //.2 纬度 度1； 如：35.44			2位小数
#define w_DNBScrGpsLatAllDu2 w_DNBParLst[161 + 500] //.0 纬度 度2；	00.00****
													//****部分，添加到w_ScrGpsLatAllDu1后面。 如：****是 2459， 则总纬度是： 35.442459
#define w_DNBScrGpsLonAllDu1 w_DNBParLst[162 + 500] //.2 经度 度1；	如：127.39		2位小数
#define w_DNBScrGpsLonAllDu2 w_DNBParLst[163 + 500] //.0 经度 度2； 00.00****
													//****部分，添加到w_ScrGpsLonAllDu1后面。如：****是 4628， 则总经度是： 127.394628

#define w_Pump1DispWenDu1DecValue w_DNBParLst[170 + 500] //.1 温度值1
#define w_Pump1DispWenDu2DecValue w_DNBParLst[171 + 500] //.1 温度值2
#define w_Pump2DispWenDu1DecValue w_DNBParLst[172 + 500] //.1 温度值1
#define w_Pump2DispWenDu2DecValue w_DNBParLst[173 + 500] //.1 温度值2
#define w_Pump3DispWenDu1DecValue w_DNBParLst[174 + 500] //.1 温度值1
#define w_Pump3DispWenDu2DecValue w_DNBParLst[175 + 500] //.1 温度值2
#define w_Pump4DispWenDu1DecValue w_DNBParLst[176 + 500] //.1 温度值1
#define w_Pump4DispWenDu2DecValue w_DNBParLst[177 + 500] //.1 温度值2
#define w_Pump5DispWenDu1DecValue w_DNBParLst[178 + 500] //.1 温度值1
#define w_Pump5DispWenDu2DecValue w_DNBParLst[179 + 500] //.1 温度值2

// ZCL 2023.2.10
#define w_Pump1RemoteStop w_DNBParLst[181 + 500]	 //.0 泵1遥控启停			//ZCL 2023.2.10
#define w_Pump2RemoteStop w_DNBParLst[182 + 500]	 //.0 泵2遥控启停
#define w_Pump3RemoteStop w_DNBParLst[183 + 500]	 //.0 泵3遥控启停
#define w_Pump4RemoteStop w_DNBParLst[184 + 500]	 //.0 泵4遥控启停
#define w_Pump5RemoteStop w_DNBParLst[185 + 500]	 //.0 泵5遥控启停
#define w_SavePump1RemoteStop w_DNBParLst[186 + 500] //.0 泵1遥控启停			//ZCL 2023.2.10
#define w_SavePump2RemoteStop w_DNBParLst[187 + 500] //.0 泵2遥控启停
#define w_SavePump3RemoteStop w_DNBParLst[188 + 500] //.0 泵3遥控启停
#define w_SavePump4RemoteStop w_DNBParLst[189 + 500] //.0 泵4遥控启停
#define w_SavePump5RemoteStop w_DNBParLst[190 + 500] //.0 泵5遥控启停

// ZCL 2023.2.10
#define w_ZhuanFaAdd w_DNBParLst[191 + 500]	 //.0 预留
#define w_ZhuanFaData w_DNBParLst[192 + 500] //.0 预留	//ZCL 2022.3.21

// #define w_Com1RcVOkCounter w_DNBParLst[220 + 500] // 计数器 接收数据 +1  ZCL 2023.3.2
// #define w_Com2RcVOkCounter w_DNBParLst[221 + 500] // 计数器 接收数据 +1
// #define w_Com3RcVOkCounter w_DNBParLst[222 + 500] // 计数器 接收数据 +1
// SIM7600用的参数

// 第一路通道的IP地址
#define Pw_Link1IP1 Pw_ParLst_GPRS[0]
#define Pw_Link1IP2 Pw_ParLst_GPRS[1]
#define Pw_Link1IP3 Pw_ParLst_GPRS[2]
#define Pw_Link1IP4 Pw_ParLst_GPRS[3]

// 第二路通道的IP地址
#define Pw_Link2IP1 Pw_ParLst_GPRS[4]
#define Pw_Link2IP2 Pw_ParLst_GPRS[5]
#define Pw_Link2IP3 Pw_ParLst_GPRS[6]
#define Pw_Link2IP4 Pw_ParLst_GPRS[7]

// 第三路通道的IP地址
#define Pw_Link3IP1 Pw_ParLst_GPRS[8]
#define Pw_Link3IP2 Pw_ParLst_GPRS[9]
#define Pw_Link3IP3 Pw_ParLst_GPRS[10]
#define Pw_Link3IP4 Pw_ParLst_GPRS[11]

// 第四路通道的IP地址
#define Pw_Link4IP1 Pw_ParLst_GPRS[12]
#define Pw_Link4IP2 Pw_ParLst_GPRS[13]
#define Pw_Link4IP3 Pw_ParLst_GPRS[14]
#define Pw_Link4IP4 Pw_ParLst_GPRS[15]

#define Pw_Link1localPort Pw_ParLst_GPRS[16] // 本地1端口
#define Pw_Link2localPort Pw_ParLst_GPRS[17] // 本地2端口
#define Pw_Link3localPort Pw_ParLst_GPRS[18] // 本地3端口
#define Pw_Link4localPort Pw_ParLst_GPRS[19] // 本地4端口

#define Pw_Link1Port Pw_ParLst_GPRS[20] // 第一路链接远端端口
#define Pw_Link2Port Pw_ParLst_GPRS[21] // 第二路链接远端端口
#define Pw_Link3Port Pw_ParLst_GPRS[22] // 第三路链接远端端口
#define Pw_Link4Port Pw_ParLst_GPRS[23] // 第四路链接远端端口

#define Pw_Link1TCP_UDPType Pw_ParLst_GPRS[24] // 0:UDP,1:TCP
#define Pw_Link2TCP_UDPType Pw_ParLst_GPRS[25]
#define Pw_Link3TCP_UDPType Pw_ParLst_GPRS[26]
#define Pw_Link4TCP_UDPType Pw_ParLst_GPRS[27]

#define Pw_Link1NetWorkCheckTime Pw_ParLst_GPRS[28] // 网络检测间隔时间
#define Pw_Link2NetWorkCheckTime Pw_ParLst_GPRS[29]
#define Pw_Link3NetWorkCheckTime Pw_ParLst_GPRS[30]
#define Pw_Link4NetWorkCheckTime Pw_ParLst_GPRS[31]

#define Pw_RFModularAddress Pw_ParLst_GPRS[32] // 无线模块的地址
#define Pw_DTUIDNUM1 Pw_ParLst_GPRS[33]		   // DTU号码
#define Pw_DTUIDNUM2 Pw_ParLst_GPRS[34]
#define Pw_DTUIDNUM3 Pw_ParLst_GPRS[35]

#define Pw_link1OnlineReportTime Pw_ParLst_GPRS[36] // 连接1 在线报告时间间隔(心跳)(10毫秒)=5000
#define Pw_link2OnlineReportTime Pw_ParLst_GPRS[37]
#define Pw_link3OnlineReportTime Pw_ParLst_GPRS[38]
#define Pw_link4OnlineReportTime Pw_ParLst_GPRS[39]

#define Pw_link1ReconnectTimeBetwen Pw_ParLst_GPRS[40] // 数据发送错误应答10次,或者连续8次心跳没有正确发送成功,则200MS重新连接，默认取20(*10ms)
#define Pw_GPRSRTModeSelect Pw_ParLst_GPRS[41]
#define Pw_LastPacketIdleTime Pw_ParLst_GPRS[42] // 最后包空闲时间间隔
#define Pw_SupportDataReceipt Pw_ParLst_GPRS[43] //=1，支持数据显示；=0，不显示

#endif /* __GLOBALCONST_H */
