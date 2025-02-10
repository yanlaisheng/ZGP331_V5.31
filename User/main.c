/**
 ******************************************************************************
 * @file    main.c
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.00
 * @date    2014-02-12
 * @brief   Main program body
 * 				 Hardware	: STM32F103CBT6,LSM303DLHC
 * 				 Platform	: IAR6.40,Keil-MDK5.01,LIB:V3.5.0(11/03/2011),JLINK V6-V7
 ******************************************************************************
 * History Platform:
 *				2013.12.31 GP311 IAR640 MDK501 LIB3.5模板 周成磊
 * Change Log:
 ******************************************************************************/

/* 定义后使用SWD调试，2015.9.3 ZCL 增加条件编译
	 注释后使用串口调试。  */
#define SWD // ZCL 2019.9.11 备注：SCR_E板 SWD口已经专用，程序也修改。SWD的定义已经没有用！

/*ZCL  2019.9.11 备注
1. DCM200_SCR_E 带LORA,GPRS,GPS,FM25L16

2. SCR本身参数，保存到FM25L16中，跟DCM100_SCR_B不一样（这个保存到ARM内部FLASH中）
	//ZCL 2019.4.4 FM25L16来保存参数，空间分配  2K字节 0-1023
		//DCM200_SCR_E的FM25L16分配
		//1. 0-99 字节，50个字，保存SCR参数
		//2. 256 字节开始，288个字节（DTU_PAR_SIZE） 来保存 GPRS参数

3. 调试时把看门狗注释掉，正常使用时再打开看门狗，否则影响仿真。 ZCL 2019.9.12

4. 串口通讯中：查询地址和数量要进行检验，防止溢出，程序死机！！！ ZCL 2019.11.21

5. //ZCL 2019.11.21 Pw_LoRaSet、Pw_GprsSet设定和测试说明：
	（1）通常LORA主机设定：
	Pw_LoRaSet=1（F_LoRaEn=1，F_LoRaToCom=0）；
	Pw_GprsSet=9（F_GprsEn=1，F_GprsTransToCom=0，F_GprsMasterNotToCom=0，F_GprsMasterToCom=1）

	（2）当使用串口2访问LORA主机时，设定说明：
		Pw_LoRaSet=1（F_LoRaEn=1，F_LoRaToCom=0）；
		Pw_GprsSet=0（F_GprsEn=0，F_GprsTransToCom=0，F_GprsMasterNotToCom=0，F_GprsMasterToCom=0） GPRS关闭
		Pw_GprsSet=1 GPRS打开，网络指令不送到COM2。能连接上宏电 gprsdemo.exe，用demo发指令，没有返回；
		Pw_GprsSet=5 GPRS打开，网络指令不送到COM2。因为F_GprsMasterNotToCom=1，能连接上宏电 gprsdemo.exe，用demo发指令，可以返回；
		Pw_GprsSet=9 GPRS打开，网络指令送到COM2。因为F_GprsMasterToCom=1，能连接上宏电 gprsdemo.exe，用demo发指令，可以返回；（注：设成9也能用，但最好设成5，这样网络指令不送到COM2）

6、用串口2可以访问 w_ParLst等5区  ZCL 2019.11.21  2019.11.22 （先改的高压电机的程序，移植到这里）
	#define STM32_WRPAR_ADDRESS			0 			// 读写参数地址；Pw_ParLst，访问时偏移0000访问
	#define STM32_RPAR_ADDRESS			10000 		// 只读参数地址；w_ParLst，访问时偏移10000访问
	#define DSP_WRPAR_ADDRESS				20000 		// 读写参数地址；Pw_dspParLst，访问时偏移20000访问
	#define DSP_RPAR_ADDRESS				30000 		// 只读参数地址；w_dspParLst，访问时偏移30000访问
	#define SCR_WRPAR_ADDRESS				40000 		// 读写参数地址；w_ScrParLst，访问时偏移40000访问

7.  ZCL 2019.11.29 加GPS配置
		需设置（1）：F_GpsToCom=1; 		//即设置Pw_GprsSet为16；
				  （2）Pw_LoRaSet=0;  或者  Pw_LoRaSet=1;

		20191129-V802-1 M J-LINK LORA 上位机访问：串口2 OK（串口2可以访问w_ParLst等5区） OK ，GSP配置 OK
		搜 2019.11.29 4处地方，另改版本号
	（1）//ZCL 2019.11.29
			extern u8 Txd4Buffer[];
	（2）		 && F_GpsToCom==0 )			//ZCL 2019.11.29 加 && F_GpsToCom==0

	（3）			//ZCL 2019.11.29 Pw_GprsSet .4位 =1  F_GpsToCom  增加: COM2接收到电脑指令，转发到GPS口(COM4)
			else if( F_GpsToCom )

	（4）		//ZCL 2019.11.29 GPS配置；下面直接赋值在测试时用


  8. ZCL 2020.9.30
			一个SZM220 DI用于雷达监测（固定地址99），通过LORA转发给LORA主机，LORA主机转发给其他SZM220 DO输出 OK



*/

/* @note	2014.1.3 ----------------------------------------------------------
  周成磊：
	  Version_DCM200_SCR_E.C
  1. COM1连接变频电机ARM
  2. GPRS用USART3 转发到 USART2(COM2)
  3. LORA用SPI3 转发到 USART2(COM2)
  4. 注意void DoWith(void)	中 有直接硬件IO的反转，更换接口时要注意！
		  //COM2_DATA 翻转  2014.11.27修改
		  GPIO_WriteBit(GPIOC, GPIO_Pin_12,
					 (BitAction)((1-GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_12))));		//ZCL 2018.12.4
  5. 现在重新下载程序，GPRS参数不会丢；初始化DTU ID不会改！

  6. ZCL 2019.4.16 备注：DCM200_SCR_E中KEY3、KEY4、KEY5 被占用，使用时要注释掉！

  7. ZCL 2019.6.25 GPS偶尔数据成0，原因猜测：GPRS重启，导致GPS没有复位好。
  修改：GPIO_Configuration()中 关闭GPS
		  延时后，在主循环前，打开GPS.
		  GPIO_SetBits(GPIOC, GPIO_Pin_1); 			//ZCL 2019.6.25 打开GPS

  8. 周成磊备注：增加画面后，调整跳转的方法：从522开始逐渐往前替换

		  旧版本：500---522  999
		  新版本：500---520  523---532  999

很重要：从旧版本522开始搜索 替换成新版本的532；
			  再旧版本521搜索，   替换成新版本的53*；
			  。。。
			  依次往前搜索和替换。

  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "GlobalV.h"
#include "GlobalConst.h"
#include "com1_232.h"
#include "com2_232.h"
#include "DoWith.h"
#include "LCD.h"
#include "Key_Menu.h"

#include "LcdGraphic.H"

#include "com3_232.h" //ZCL 2018.10.18
#include "com4_232.h" //ZCL 2018.10.18
#include "com_LoRa.h"

// #include "sys_config.h" //ZCL 2018.11.12
#include "spi.h" //ZCL 2018.11.12		sx12xxEiger文件夹中

#include "spi_flash.h"

/** @addtogroup STM32F10x_StdPeriph_Template
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
EXTI_InitTypeDef EXTI_InitStructure; // 外部中断初始化结构体
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

/* Private define ------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 S_Display; // 控制显示内容

/* Private variables extern --------------------------------------------------*/
extern u16 Lw_SavePageNo;
extern u16 Lw_PageNo;	// 显示的画面序号
extern u8 B_LockCodeOK; // 锁定密码正确标志

extern tRadioDriver *Radio; // ZCL 2018.11.13
extern u8 B_LoRaRcv;		// ZCL 2019.01.29

extern uchar S_M35;
extern uchar AT_ComOk_Flag;
extern uchar AT_3EOk_Flag; //>回显标志（准备输入发送内容）
extern uint C_M35Step;	   // M35模块步骤进入 2012.7.20
extern uchar B_PcSetGprs;  // PC设置GPRS标志 2013.6.6
extern uchar B_NextS35Cmd;
extern u8 B_LockCodeOK; // 锁定密码正确标志
extern u8 B_LoRaRCVCommandOk;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
// void SysTick_Config(void);
void TIM_Configuration(void);
void EXTI_Configuration(void);	  // ZCL 2009.5.18
void IWDG_Init(u8 prer, u16 rlr); // 独立看门狗初始化 2013.7.3
void IWDG_Feed(void);			  // 独立看门狗喂狗	 2013.7.3
void PowerDelay(u16 nCount);

void XmodemFileTransfer(void);
void XmodemFileTransfer_Com1(void);
void GPIO_PinReverse(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Delay_US(vu16 nCount);
void Delay_MS(vu16 nCount);
/* Private functions ---------------------------------------------------------*/

// MDK5用，同时在MDK Options里面加入SEMIHOSTING的宏定义，这样不用“use MicroLIB”
#ifdef SEMIHOSTING
#pragma import(__use_no_semihosting)
_sys_exit(int x)
{
	x = x;
}
struct __FILE
{
	int handle;
	/* Whatever you require here. If the only file you are using is */
	/* standard output using printf() for debugging, no file handling */
	/* is required. */
};
/* FILE is typedef’ d in stdio.h. */
FILE __stdout; // ZCL 2019.3.29
#endif

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */

int main(void)
{
	/*!< At this stage the microcontroller clock setting is already configured,
		 this is done through SystemInit() function which is called from startup
		 file (startup_stm32f10x_xx.s) before to branch to application main.
		 To reconfigure the default setting of SystemInit() function, refer to
		 system_stm32f10x.c file
	   */
	// u8 i;

#ifdef DEBUG
	debug();
#endif

	PowerDelay(700);

	/* System Clocks Configuration */
	RCC_Configuration();
	/* GPIO ports pins Con2figuration */
	GPIO_Configuration();

	// ZCL 2015.9.12 上电延时 ！！！ DCM100 ARM中需要
	PowerDelay(300);

	// ZCL  2015.9.12 对RCC_Configuration  GPIO_Configuration 上电稳定后，再执行一次
	/* System Clocks Configuration */
	RCC_Configuration();
	/* GPIO ports pins Configuration */
	GPIO_Configuration();

	SysTick_Config(72000); // 使用内核函数
	TIM_Configuration();
	//	tim2_init();                     // 定时器2初始化

	SPI_FLASH_Init(); // 在LORA中初始化
	SpiInit();		  //

	/* Initialize the SPI FLASH driver --------------*/
	Variable_Init(); // Initialize VARIABLE
	ParLst_Init();	 // RAM中设定参数表列初始化
	Boot_ParLst();	 // 初始化设定参数		ZCL 2018.12.22 增加

	EXTI_Configuration();
	// NVIC_Configuration();
	Com1_Init();
	Com2_Init();

	Com3_Init(); // ZCL 2018.10.18
	Com4_Init(); // ZCL 2018.10.18

	// Xmodem函数里传输时是57600，结束后设成正常波特率(一般9600)
	XmodemFileTransfer_Com1(); // XMODEM文件传输
	XmodemFileTransfer();	   // XMODEM文件传输

	/* NVIC Configuration */ // 中断在这里打开
	NVIC_Configuration();

	// ZCL 2018.11.12  LORA SX1278 初始化
	SpiInit();
	LED0_ON;
	// LED1_ON;
	// ZCL 2018.11.21 tRadioDriver *Radio = NULL;  自定义结构体指针
	Radio = RadioDriverInit(); // ZCL radio.c中  tRadioDriver* RadioDriverInit( void )
	Radio->Init();
	Radio->StartRx(); // RFLR_STATE_RX_INIT

	// printf("RF_LoRa_TX_OK？ \n");
	LoRa_Init(); // ZCL 2021.10.25 TEST

	GetLockCode();	  // 2013.10.26 得到锁定密码
	B_LockCodeOK = 1; // 2013.10.26 注释后，设置为具有密码保护功能，否则=1，相当于没有保护

	LCD_DLY_ms(800);
	LCD12864_Init(); // oled 初始化

	ADC_DMA_Init();

	// 2015.9.6 ZCL 		画面延时大时，加大看门狗时间，否则造成重启。IWDG_Init(4,938)
	// 预分频数为64,重载值为625,溢出时间为1s;  预分频数为64,重载值为938,溢出时间为1.5s	IWDG_Init(A,B): 4*(2^A)*B/40

	// ZCL 仿真的时候把看门狗注释掉，在产品运行的时候打开。
	// IWDG_Init(4,938);				//1.5S 	 //ZCL 2021.10.25 TEST
	// IWDG_Init(4,625);    	//1S
	//		IWDG_Init(5,1250); 		//4S

	Pw_ScrLoRaTest1 = 0; // ZCL 2018.11.13  =1 透传模式 =0非透传 (=10测试发送，=11测试接收删除)

	Lw_SavePageNo = 1000;

	if (Pw_LoRaMasterSlaveSel == 1)
		Lw_PageNo = 500; // ZCL 2019.3.8 显示LORA从机参数（规划4个从机）
	else if (Pw_LoRaMasterSlaveSel == 0)
	{
		if (Pw_ScrMenuMode == 1)
			Lw_PageNo = 200; // ZCL 从变频参数画面开始显示，无供水画面。注释掉：正常显示，有供水画面
		else if (Pw_ScrMenuMode == 0)
			Lw_PageNo = 0; //
	}

	GPIO_SetBits(GPIOC, GPIO_Pin_1); // ZCL 2019.6.25 打开GPS
									 //	F_RunAgain = 1;
									 //	F_RunCheckModular = 1;

	B_LoRaRCVCommandOk = 1;
	/* Infinite loop */
	while (1)
	{
		// Delay_MS(4000);					//ZCL 2019.4.26 测试看门狗复位！
		Boot_ParLst();	  // 初始化设定参数
		ParLimit();		  // 参数限制
		DoWith();		  // 一些数据的处理
		SavePar_Prompt(); // 保存设定参数到FLASH"+"状态提示
		ForceSavePar();	  // 强制保存参数  ZCL 2018.8.3 改写，来保存液晶屏参数
		Time_Output();
		//
		if (w_ZhouShanProtocol_bit9) // 串口1主动查询控制标志位
			Com1_MasterSend();		 // 串口1主发送程序　20060923	DB9串口1

		Com1_RcvProcess(); // 串口1接收处理
		Com1_SlaveSend();  // 串口1从机发送

		Com2_MasterSend(); // 串口2主发送程序　20060923	DB9串口2
		Com2_RcvProcess(); // 串口2接收处理
		Com2_SlaveSend();  // 串口2从机发送

		Com3_MasterSend(); // 串口3主发送程序　20181211 GPRS
		Com3_RcvProcess(); // 串口3接收处理
		Com3_SlaveSend();  // 串口3从机发送

		Com4_MasterSend(); // 串口4主发送程序　20181211 GPS
		Com4_RcvProcess(); // 串口4接收处理
		Com4_SlaveSend();  // 串口4从机发送
		Judge_Reset_GPS(); // 判断是否需要重启GPS，串口4超过60秒没收到GPS发来的数据
		PowerOnOff_GPS();  // 重启GPS
		// ZCL 2019.1.19 加参数来设定功能  Pw_LoRaSet
		if (Pw_ScrLoRaTest1 == 0) // Pw_ScrLoRaTest1=0 非透传
		{
			if (F_LoRaEn) //.0 =1运行LORA		//Pw_LoRaSet .0 =1运行LORA
			{
				if (Pw_LoRaMasterSlaveSel == 1)//LoRa主机
				{
					LoRa_MasterSend();		   // LoRa主发送程序
					if (Pw_EquipmentType == 0) // 双驱泵才会判断变频器通讯是否正常
						Jude_VVVF_Comm();	   // 判断变频器通讯
				}

				LoRa_RcvProcess(); // LoRa接收处理
				LoRa_SlaveSend();  // LoRa从机发送
			}
		}

		// ZCL 2019.1.24 测试LORA
		if (Pw_ScrLoRaTest1 == 1) // Pw_ScrLoRaTest1=1 透传
			LoRaRcvTest();

		LoRaSendToDone(); // LoRaSendTest

		IWDG_Feed(); // 喂狗	2013.7.3

		// AnologIn(); 						//ZCL 2018.11.13 暂存，里面的程序已删除
		/* 		TIM_SetCompare2(TIM2, w_AI1Value); 		//PWM-DAC */
		// TIM2->CCR2=w_AI1Value; 		//也可以直接操作寄存器

		if (w_TestItemSel >= 1) // 测试选择 2014.5.6
		{
			Test(); // 测试程序
		}
		else if (B_LockCodeOK == 1) // 2013.10.26 需要锁定代码校验正确才执行
		{
			;
		}

		// 调试画面用 片头画面用（只显示一次）
		if (S_Display == 0)
		{
			Menu_Welcome(); // 画面 欢迎

			S_Display = 1; // ZCL 2019.4.26  放在这里，在中断中也使用，控制喂狗
		}

		if (S_Display == 1) // ZCL 2019.1.29
		{

			GetKey();
			Menu_Change();

			if (F_GprsEn) // Pw_GprsSet.0=1	GPRS打开，运行
			{
				// 执行条件：1. <3  2.有OK 和 > 标志 3.延时时间到(C_M35Step 10MS计数器)
				if (S_M35 < 3 || S_M35 >= 0x1B)
					M35_LOOP(); // M35 模块循环执行程序

				else if (AT_ComOk_Flag || AT_3EOk_Flag || B_NextS35Cmd || C_M35Step >= 100)
					M35_LOOP(); // M35 模块循环执行程序

				if (B_PcSetGprs == 2)	 // PC串口软件来设置GPRS参数 2013.6.6
					SmsAndPC_SetupInq(); // 短信和PC设置和查询GPRS参数
			}
		}

	} // while end
}

/*******************************************************************************
 * Function Name  : RCC_Configuration
 * Description    : Configures the different system clocks.
 * Input          : None
 * Output         : None
 * Return     xia    : None
 *******************************************************************************/
void RCC_Configuration(void)
{
	// ErrorStatus HSEStartUpStatus;

	// /* RCC system reset(for debug purpose) */
	// RCC_DeInit();

	// /* Enable HSE */
	// RCC_HSEConfig(RCC_HSE_ON);

	// /* Wait till HSE is ready */
	// HSEStartUpStatus = RCC_WaitForHSEStartUp();

	// if(HSEStartUpStatus == SUCCESS)
	// {
	// /* HCLK = SYSCLK */
	// RCC_HCLKConfig(RCC_SYSCLK_Div1);

	// /* PCLK2 = HCLK */
	// RCC_PCLK2Config(RCC_HCLK_Div1);

	// /* PCLK1 = HCLK/2 */
	// RCC_PCLK1Config(RCC_HCLK_Div2);

	// /* ADCCLK = PCLK2/6 */
	// RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	// /* Flash 2 wait state */
	// FLASH_SetLatency(FLASH_Latency_2);

	// /* Enable Prefetch Buffer */
	// FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	// /* PLLCLK = 8MHz * 4 = 32 MHz */
	// RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4);   // 2013.1.17

	// /* Enable PLL */
	// RCC_PLLCmd(ENABLE);

	// /* Wait till PLL is ready */
	// while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	// {
	// }

	// /* Select PLL as system clock source */
	// RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	// /* Wait till PLL is used as system clock source */
	// while(RCC_GetSYSCLKSource() != 0x08)
	// {
	// }
	// }

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // USART1的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // USART2的时钟

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // USART3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);  // UART4的时钟

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // DAC用 2014.4.23
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 延时US,MS用

	// 2013.12.8 ADC采集
	/* Enable DMA clock */							   // !!! 新库文件中RCC_AHBPeriph_DMA1   RCC_AHBPeriph_DMA2 2008.11.26
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 使能DMA时钟  ADC用
	/* Enable ADC1                 /and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1时钟

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE); // ZCL 2018.11.9 注意SPI3使用APB1时钟
}

/*******************************************************************************
 * Function Name  : GPIO_Configuration
 * Description    : Configures the different GPIO ports.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef SWD
	// 2015.9.3 周成磊 重新使用JTAG的SWD功能，方便调试。
	// 要用JTDI 加上这一句 2013.8.30
	// 解释：GPIO_Remap_SWJ_JTAGDisable  （JTAG-DP 失能 + SW-DP使能 ）
	// PB4、PB3、PA15三个引脚不是普通IO，而是JTAG的复用功能，分别为JNTRST、JTDI、JTDO。
	//
#else
	// 关掉JTAG功能,释放PA13和PA14
	// GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //SWJ 完全失能（JTAG+SW-DP）
#endif

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	//-------------------------1. LCD -- 12864液晶接口
	// LCD_RS	 --	  PB11-->PB9	ZCL 2018.10.18
	// LCD_RW	 --	  PB12-->PC4  ZCL 2018.12.4
	// LCD_En	 --	  PB13-->PA7	ZCL 2018.11.29

	// LCD_DATA0	 --	  PB14
	// LCD_DATA1	 --	  PB15

	// LCD_DATA2	 --	  PC6
	// LCD_DATA3	 --	  PC7
	// LCD_DATA4	 --	  PC8
	// LCD_DATA5	 --	  PC9

	// LCD_DATA6	 --	  PA8
	// LCD_DATA7	 --	  PA11-->PA15 ZCL 2018.11.29

	// LCD_PSB	 --	  PA12

	// LCD_RST	 --	  PA13-->PC2			ZCL 2018.10.18
	// 2015.3.28 LCD_RST改成PA15；PA13用成SWDIO 周成磊
	// 2015.3.28 CPU_BUZZER蜂鸣器取消，PA14用SWCLK 周成磊

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15 | GPIO_Pin_12 | GPIO_Pin_7; // ZCL 2018.11.29
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// LCD_RST
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//-------------------------1.2 液晶背光
	// LCD_BACKLIGHT  --  PB0

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		 //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	Qb_LcdBacklight = 1;

	// 2015.3.28 LCD_RST改成PA15；PA13用成SWDIO 周成磊
	/* 	#ifdef SWD
		;
		#else
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 ;
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		#endif */

	/* #ifdef SWD
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		//2015.3.28 LCD_RST改成PA15；PA13用成SWDIO 周成磊
		GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif */

	//-------------------------2. 按键端口
	// KEY1		 --	  	PA4
	// KEY2		 --	  	PA5
	// KEY3		 --	  	PA6			ZCL 2018.11.30  删除
	// KEY4		 --	  	PA7			ZCL 2018.11.30  删除

	// KEY5		 --	  	PC4			ZCL 2018.11.30  删除
	// KEY6		 --	  	PC5

	// KEY7		 --	  	PB1
	// KEY8		 --	  	PC13
	// KEY9		 --	  	PB10-->PB8-->PC3				//ZCL 2018.10.18  //ZCL 2018.11.30
	// KEY10		 --	  	PC12-->PC14  						//ZCL 2018.12.5
	// KEY11		 --	  	PC11-->PC3-->PB7				//ZCL 2018.10.18  //ZCL 2018.11.30

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //| GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_7; // ZCL 2018.11.30  GPIO_Pin_8
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // ZCL 2018.12.3	 GPIO_Pin_4 |
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_14 | GPIO_Pin_13; // ZCL 2018.12.5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//-------------------------3. 输出端口
	// PA0			ZCL 2018.10.18 	改成：LoRa_NSS
	// PA1			ZCL 2018.10.18 	改成：LoRa_RESET
	// 3.1 LoRa
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 输入	DIO1:PD2  DIO2:PA6
	/* Configure 	as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 3.2 GPRS
	// 输出  PWR_KEY:PBout(12)，RESET_4G:PBout(13)
	/* Configure as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 上面语句初始化后值为0，下面根据要求赋值
	// GPIO_ResetBits(GPIOB, GPIO_Pin_12);		//=0经过三极管出来高，模块不开机
	GPIO_SetBits(GPIOB, GPIO_Pin_12); //=1经过三极管出来低，模块开机
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);

	// GPRS指示灯初始化
	// PC12: COM1_DATA——GPRS指示灯（H1）
	// PC15: PWR——WORK指示灯（H0）
	// PA11: NET——LoRa指示灯（H2）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 上面语句初始化后值为0，下面根据要求赋值
	GPIO_SetBits(GPIOC, GPIO_Pin_15); // H0
	GPIO_SetBits(GPIOC, GPIO_Pin_12); // H1
	GPIO_SetBits(GPIOA, GPIO_Pin_11); // H2

	// 输入GPRS	STATUS_4G:PB8
	/* Configure 	as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 3.3 GPS  GP_1PPS:PC0 秒脉冲输入		GP_ON/OFF(RESET):PC1  低电平关断
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_0); // YLS 2023.12.25 打开4G模块电源

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_1); // ZCL 2019.6.25 关闭GPS

	//	 GPIO_SetBits(GPIOC, GPIO_Pin_1); 			//ZCL 2018.10.19 打开GPS

	//-------------------------4. SPI 铁电存储器
	// SPI3_SCK	  --   PB3
	// SPI3_MISO	  --   PB4
	// SPI3_MOSI	  --   PB5
	// SPI3_NSS	  --   PB6		FM25L16 Chip select

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  //
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  //

	// Configure FM25L16 Chip select:  PB6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  //

	GPIO_SetBits(GPIOB, GPIO_Pin_6);
}

/*******************************************************************************
 * Function Name  : EXTI_Configuration
 * Description    : Configures the used EXTI lines.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
// 3   EXTI_Mode_Interrupt = 0x00,  		//中断触发
// 4   EXTI_Mode_Event = 0x04           //事件触发

// 9   EXTI_Trigger_Rising = 0x08,       //上升沿触发
// 10   EXTI_Trigger_Falling = 0x0C,        //下降沿触发
// 11   EXTI_Trigger_Rising_Falling = 0x10  //高低电平触发
void EXTI_Configuration(void)
{
	// /* Connect EXTI Line2 to PD2 */
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);

	// /* Configure EXTI Line0 to generate an interrupt on falling edge */
	// EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	//EXTI_Trigger_Falling;
	// EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	// EXTI_Init(&EXTI_InitStructure);

	// /* Connect EXTI Line6 to PA.6 */
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);

	// /* Configure EXTI Line1 to generate an interrupt on falling edge */
	// EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	//EXTI_Trigger_Falling;
	// EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	// EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
 * Function Name  : NVIC_Configuration
 * Description    : Configures the NVIC and Vector Table base address.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void NVIC_Configuration(void)
{

#ifdef VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	/* Configure the NVIC Preemption Priority Bits 配置优先级组 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the USART1 Interrupt */				  // 旧V2.0.3库 USART3_IRQChannel
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; // 2014 ZCL FOR LIB-V3.5.0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 Interrupt */				  // 旧V2.0.3库 USART3_IRQChannel
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; // 2014 ZCL FOR LIB-V3.5.0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 Interrupt */				  // 旧V2.0.3库 USART3_IRQChannel
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // 2014 ZCL FOR LIB-V3.5.0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the UART4 Interrupt */ // ZCL 2018.10.23
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; // 2014 ZCL FOR LIB-V3.5.0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //
	NVIC_Init(&NVIC_InitStructure);

	// ZCL 2019.2.14
	//  /* Enable the EXTI2_IRQChannel Interrupt */
	// NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);

	// /* Enable the EXTI6_IRQChannel Interrupt */
	// NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
 * Function Name  : SysTick_Config
 * Description    : Configure a SysTick Base time to 10 ms.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
// void SysTick_Config(void)
// {
// /* Configure HCLK clock as SysTick clock source */
// SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

// /* SysTick interrupt each 1000 Hz with HCLK equal to 32MHz */		//equal:相等
// SysTick_SetReload(32000);		// 32000=1000HZ  //320=100 000HZ速度明显加快

// /* Enable the SysTick Interrupt */
// SysTick_ITConfig(ENABLE);

// /* Enable the SysTick Counter */
// SysTick_CounterCmd(SysTick_Counter_Enable);
// }

/*在键寄存器(IWDG_KR)中写入0xCCCC，开始启用独立看门狗；
此时计数器开始从其复位值0xFFF递减计数。
当计数器计数到末尾0x000时，会产生一个复位信号(IWDG_RESET)。
无论何时，只要键寄存器IWDG_KR中被写入0xAAAA， IWDG_RLR中的值就会被重新加载到计数器中从而避免产生看门狗复位 。*/
// 看门狗驱动代码 //初始化独立看门狗
// prer：分频数：0~7（只有低3位有效！）
// 预分频因子=4*2^prer，最大值为256
// rlr: 重装载寄存器值: 低11 位有效.
// 时间计算(大概):Tout=((4×2^prer) ×rlr)/40 (ms).
void IWDG_Init(u8 prer, u16 rlr)
{
	IWDG->KR = 0X5555; // 使能对IWDG->PR和IWDG->RLR的写
	IWDG->PR = prer;   // 设置分频系数
	IWDG->RLR = rlr;   // 重装载寄存器 IWDG->RLR
	IWDG->KR = 0XAAAA; // reload
	IWDG->KR = 0XCCCC; // 使能看门狗
}

// 喂独立看门狗
void IWDG_Feed(void)
{
	IWDG->KR = 0XAAAA; // reload
}

void TIM_Configuration(void)
{
	/* TIM2 Configuration ---------------------------------------------------
	 Generates PWM signals
	 TIM2CLK = 72 MHz, Prescaler = 0x0, TIM2 counter clock = 72 MHz
	 TIM2 frequency = TIM2 counter clock/(TIM2_Period + 1) = 17.57 KHz	72000000/4096
	   - TIM2 Channel1 & Channel1N duty cycle = TIM2->CCR1 / (TIM2_Period + 1) = 50%
	   - TIM2 Channel2 & Channel2N duty cycle = TIM2->CCR2 / (TIM2_Period + 1) = 37.5%
	   - TIM2 Channel3 & Channel3N duty cycle = TIM2->CCR3 / (TIM2_Period + 1) = 25%
	   - TIM2 Channel4 duty cycle = TIM2->CCR4 / (TIM2_Period + 1) = 12.5%
	   //周成磊 现在晶振频率为36MHZ，所以PWM频率减半
	   //相应管脚复用推挽输出

	----------------------------------------------------------------------- */

	/* TIM2 Peripheral Configuration */
	// Resets the TIM peripheral registers to their default reset values
	// TIMx: where x can be 1 to 8 to select the TIM peripheral.
	TIM_DeInit(TIM2); // 默认初始化

	// ZCL 2021.5.4 GD32		代码如果不配置 TIM 的周期 GD 的周期值会是 0，而 ST 不配置周期值，周期值是 0xFFFF;
	// ZCL 这里原先程序就有周期值

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFF;  // 1.控制周期
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 2.控制时钟频率=clock/(时钟预分频+1)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // TIM_OCMode_PWM2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	// TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;    N输出根据实际设定 周成磊
	TIM_OCInitStructure.TIM_Pulse = 0x0;					 // 控制占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // 极性 当计数值小于TIM_Pulse时的电平
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	/* TIM2 counter enable */
	TIM_Cmd(TIM2, ENABLE);

	/* TIM1 Main Output Enable */ // 高级定时器才有此功能
								  // Enables or disables the TIM peripheral’s main outputs.
	// TIM_CtrlPWMOutputs(TIM1,ENABLE);

	// TIM_OC1PreloadConfig(),TIM_ARRPreloadConfig();这两个函数控制的是ccr1和arr的预装在使能，
	// 使能和失能的区别就是：使能的时候这两个寄存器的读写需要等待有更新事件发生时才能被改变
	// （比如计数溢出就是更新事件）。失能的时候可以直接进行读写而没有延迟。

	// 另外在运行当中想要改变pwm的频率和占空比调用：
	// TIM_SetAutoreload()
	// TIM_SetCompare1()这两个函数就可以了。

	// 2014.7.10 TIM3 配置(延时)	 ZCL 2018.12.10
	TIM_TimeBaseStructure.TIM_Period = 1;		  // 自动装载
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // 周成磊：根据晶振72M分频率到1000KHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

#ifdef GD32
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0; // ZCL 2021.5.4 GD32 新加
#else
	;
#endif

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down; // 向下计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
}

void SPI2Init(void)
{
	/* 	GPIO_InitTypeDef           GPIO_InitStructure;						  //
		SPI_InitTypeDef            SPI_InitStructure;						  //

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                 //使能端口B时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 				  //
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);                  //使能SPI2时钟时钟

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//复用
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                       //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;					  //
		GPIO_Init(GPIOB, &GPIO_InitStructure);								  //

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                            //SPI2_NSS PB12
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                      //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;					  //
		GPIO_Init(GPIOB, &GPIO_InitStructure);								  //

		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                         //设置SPI工作模式:设置为主SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                     //设置SPI的数据大小:SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                            //选择了串行时钟的稳态:时钟悬空低
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                          //数据捕获于第二个时钟沿
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                             //SPI_NSS_Soft;  //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //定义波特率预分频的值:波特率预分频值为256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                    //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;                              //CRC值计算的多项式
		SPI_Init(SPI2, &SPI_InitStructure);                                   //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);                     //SPI2中断控制 ,关闭
		SPI_Cmd(SPI2, ENABLE);                                                //使能SPI外设
	 */
}

void SPI3Init(void)
{
	/* 	GPIO_InitTypeDef           GPIO_InitStructure;						  //
		SPI_InitTypeDef            SPI_InitStructure;						  //

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                 //使能端口B时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 				  //
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);                  //使能SPI3时钟时钟

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;		//复用
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                       //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;					  //
		GPIO_Init(GPIOB, &GPIO_InitStructure);								  //

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;                             //SPI3_NSS PB6
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                      //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;					  //
		GPIO_Init(GPIOB, &GPIO_InitStructure);								  //

		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                         //设置SPI工作模式:设置为主SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                     //设置SPI的数据大小:SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                            //选择了串行时钟的稳态:时钟悬空低
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                          //数据捕获于第二个时钟沿
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                             //SPI_NSS_Soft;  //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //定义波特率预分频的值:波特率预分频值为256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                    //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;                              //CRC值计算的多项式
		SPI_Init(SPI3, &SPI_InitStructure);                                   //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);                     //SPI2中断控制 ,关闭
		SPI_Cmd(SPI3, ENABLE);                                                //使能SPI外设
	 */
}

/********************************************************
 * us延时程序                                         *
 ********************************************************/
void Delay_US(vu16 nCount)
{
	u16 TIMCounter = nCount;
	TIM_Cmd(TIM3, ENABLE);
	TIM_SetCounter(TIM3, TIMCounter);
	while (TIMCounter > 1)
	{
		TIMCounter = TIM_GetCounter(TIM3);
	}
	TIM_Cmd(TIM3, DISABLE);
}

/********************************************************
 * ms延时程序                                         *
 ********************************************************/
void Delay_MS(vu16 nCount)
{
	u16 i;
	for (i = 0; i < nCount; i++)
	{
		Delay_US(1000); // 1MS
	}
}

void delay_ms(vu16 nCount)
{
	u16 i;
	for (i = 0; i < nCount; i++)
	{
		Delay_US(1000); // 1MS
	}
}

/********************************************************
 * 上电延时程序  约MS  ZCL                               *
 ********************************************************/
void PowerDelay(u16 nCount) // 2015.9.12
{
	// ZCL 2015.9.12 测试 unsigned int = u32 可以大于65535，对于ARM STM32来说
	unsigned int i = 36000;
	while (nCount--)
	{
		while (i > 0)
			i--;
		i = 36000;
	}
}

// ZCL 2018.11.13
void GPIO_PinReverse(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	/* Check the parameters */
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	GPIOx->ODR ^= GPIO_Pin;
}

#ifdef DEBUG
/*******************************************************************************
 * Function Name  : assert_failed
 * Description    : Reports the name of the source file and the source line number
 *                  where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name
 *                  - line: assert_param error line source number
 * Output         : None
 * Return         : None
 *******************************************************************************/
void assert_failed(u8 *file, u32 line)
{
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
