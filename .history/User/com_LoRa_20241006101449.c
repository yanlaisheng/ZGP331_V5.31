/**
  ******************************************************************************
  * @file    com_LoRa.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.20
  * @date    2018-12-14
  * @brief   LoRa USE SPI3
	******************************************************************************
	* @note	2015.04.13

	//ZCL 2019.1.24 备注
	SPI3

  8. ZCL 2020.9.30
			一个SZM220 DI用于雷达监测（固定地址99），通过LORA转发给LORA主机，LORA主机转发给其他SZM220 DO输出 OK
			//!=99, 正常的高压电机SZM220;   =99 雷达监测用SZM220

	9. ZCL 2022.3.10  LORA采集 高压电机220的数据量，由24个字，增加到30个字。
		//ZCL 2022.3.10 24个不够，增加3个字的运行时间。改成30个，预留3个
	*/

/* Includes ------------------------------------------------------------------*/
#include "com_LoRa.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include <stdio.h> //加上此句可以用printf

#include "stm32f10x.h"
#include "com1_232.h"
#include "com2_232.h"
#include "com3_232.h" //ZCL 2018.10.18
#include "com4_232.h" //ZCL 2018.10.18
#include "DoWith.h"
#include "LCD.h"
#include "Key_Menu.h"

#include "LcdGraphic.H"

// #include "sys_config.h" //ZCL 2018.11.12
#include "spi.h" //ZCL 2018.11.12		sx12xxEiger文件夹中

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
// ZCL 2018.11.12  2022.5.19
#define BUFFER_SIZE 100 // Define the payload size here  ZCL 2022.3.17    60  80

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
// ZCL 2018.11.12
tRadioDriver *Radio = NULL;
// const uint8_t MY_TEST_Msg[] = "LoRa_SX1278_SANLI";
static uint16_t BufferSize = BUFFER_SIZE; // RF buffer size

uint8_t LoRaTxBuf[BUFFER_SIZE]; // ZCL 2018.11.12
uint8_t LoRaRxBuf[BUFFER_SIZE]; // RF buffer

uint8_t LoRaTxBuf2[BUFFER_SIZE]; // ZCL 2021.11.18
uint8_t LoRaRxBuf2[BUFFER_SIZE]; // RF buffer

u8 B_LoRaSendMaster;
u8 T_LoRaSendTest;
u16 C_LoRaSendTest;

u8 T_LoRaRcv; // ZCL 2018.11.13  LoRaRcv
u16 C_LoRaRcv;
u8 B_LoRaRcv;

u16 Cw_LoRaRx;	   // 接收计数器//
u16 Cw_LoRaTx;	   // 发送计数器//
u16 Cw_BakLoRaRx;  // 接收计数器//
u16 Cw_LoRaTxMax;  // 有多少个字符需要发送//
u16 Lw_LoRaChkSum; // 发送校验和，lo,hi 两位//
// u16		Lw_LoRaRegAddr;						// 串口1寄存器地址
//
u8 B_LoRaCmd03;
u8 B_LoRaCmd16;
u8 B_LoRaCmd01;
u8 B_LoRaCmd06;
u8 B_LoRaCmd73;
u8 B_LoRaCmd03_Master;
u8 B_LoRaCmd06_Master;

//------------------------2016.4.2
u8 S_LoRaSendNoMaster; // 步骤

u8 T_LoRaCountNormalNext;  // 定时器
u16 C_LoRaCountNormalNext; // 计数器
u8 T_LoRaDelaySend;		   // 定时器
u16 C_LoRaDelaySend;	   // 计数器

u8 T_LoRaCount1OvertimeNext;  // 定时器
u16 C_LoRaCount1OvertimeNext; // 计数器
u8 T_LoRaCount1Err;			  // 定时器
u16 C_LoRaCount1Err;		  // 计数器
u8 T_LoRaCount2Err;			  // 定时器
u16 C_LoRaCount2Err;		  // 计数器
u8 T_LoRaCount3Err;			  // 定时器
u16 C_LoRaCount3Err;		  // 计数器
u8 T_LoRaCount4Err;			  // 定时器
u16 C_LoRaCount4Err;		  // 计数器

u8 T_LoRaCount5Err;	 // 定时器		2019.10.18
u16 C_LoRaCount5Err; // 计数器
u8 T_LoRaCount6Err;	 // 定时器
u16 C_LoRaCount6Err; // 计数器
u8 T_LoRaCount7Err;	 // 定时器
u16 C_LoRaCount7Err; // 计数器
u8 T_LoRaCount8Err;	 // 定时器
u16 C_LoRaCount8Err; // 计数器		2019.10.18

u8 T_LoRaCount9Err;		 // 定时器
u16 C_LoRaCount9Err;	 // 计数器		2019.10.18
u8 B_LoRaErrWithModule9; //

u8 B_LoRaErrWithModule1; // 标志 与子模块1通讯失败
u8 B_LoRaErrWithModule2; // 标志 与子模块2通讯失败
u8 B_LoRaErrWithModule3; // 标志 与子模块1通讯失败
u8 B_LoRaErrWithModule4; // 标志 与子模块2通讯失败

u8 B_LoRaErrWithModule5; // 标志 与子模块1通讯失败		2019.10.18
u8 B_LoRaErrWithModule6; // 标志 与子模块2通讯失败
u8 B_LoRaErrWithModule7; // 标志 与子模块1通讯失败
u8 B_LoRaErrWithModule8; // 标志 与子模块2通讯失败		2019.10.18

u8 B_LoRaRCVCommandOk;
u8 B_LoRaSendMaster;

u8 T_LoRaNoRcvMasterCMD;  // 定时器
u16 C_LoRaNoRcvMasterCMD; // 计数器
u8 B_LoRaNoRcvMasterCMD;

u8 T_LoRaSendOverTime; // ZCL 2019.1.22 Lora发送超时
u16 C_LoRaSendOverTime;

u8 B_LoraSendWrite; // ZCL 2021.11.16 LORA 写命令的发送
u8 B_LoraSendWriteLength;

u8 T_LoRaLeiDa; // ZCL 2022.3.23  //雷达继电器2S后断开
u16 C_LoRaLeiDa;

// u8	B_WaitTxDone;					//ZCL 2019.2.15 等待发送完成

/* Private variables extern --------------------------------------------------*/
extern u16 C_ForceSavPar; // 强制保存参数计数器

extern uchar Txd1Buffer[]; // UART发送缓冲区
extern uint Cw_Txd1;	   // 发送计数器//
extern uint Cw_Txd1Max;	   // 有多少个字符需要发送//

extern uchar Txd2Buffer[]; // UART发送缓冲区
extern uint Cw_Txd2;	   // 发送计数器//
extern uint Cw_Txd2Max;	   // 有多少个字符需要发送//
extern u16 Cw_BakRcv2;	   // 接收计数器备份

extern uchar Txd3Buffer[]; // UART发送缓冲区
extern uint Cw_Txd3;	   // 发送计数器//
extern uint Cw_Txd3Max;	   // 有多少个字符需要发送//

extern uchar Txd4Buffer[]; // UART发送缓冲区
extern uint Cw_Txd4;	   // 发送计数器//
extern uint Cw_Txd4Max;	   // 有多少个字符需要发送//
extern uchar S_M35;		   // s步骤：M35流程

extern uint Cw_Txd3Max;	   // 有多少个字符需要发送//
extern uint Lw_Txd3ChkSum; // 发送校验和，lo,hi 两位//
extern uint Cw_Txd3TmpMax;
extern uchar B_GprsDataReturn; // GPRS 数据返回。有连接，收到串口2数据，就通过DTU发送出去
extern uchar Txd3TmpBuffer[];
/* Private function prototypes -----------------------------------------------*/
void GPIO_LoRa_Configuration(void); // GPIO配置
void LoRa_config(void);
u16 CRC16(u8 *pCrcData, u8 CrcDataLen);
void Delay_MS(vu16 nCount);
u16 Address(u16 *p, u16 Area); // 绝对地址
extern void u1_printf(char *fmt, ...);
extern void u2_printf(uint8_t txd_buffer[], uint16_t send_num);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
 * Function Name  : LoRa_Init
 * Description    : Initializes the peripherals used by the Usart driver.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LoRa_Init(void)
{
	B_LoRaSendMaster = 1;
	B_LoRaRcv = 0;

	// ZCL 2018.11.12 测试
	LoRaTxBuf[0] = 'Z';
	LoRaTxBuf[1] = 'C';
	LoRaTxBuf[2] = 'L';
	LoRaTxBuf[3] = 'T';
	LoRaTxBuf[4] = 'E';
	LoRaTxBuf[5] = 'S';
	LoRaTxBuf[6] = 'T';
	LoRaTxBuf[7] = '2';
	LoRaTxBuf[8] = '0';
	LoRaTxBuf[9] = '1';
	LoRaTxBuf[10] = '8';
	LoRaTxBuf[11] = '1';
	LoRaTxBuf[12] = '1';
	LoRaTxBuf[13] = '1';
	LoRaTxBuf[14] = '2';
	LoRaTxBuf[15] = 0;
	LoRaTxBuf[16] = 1;
	LoRaTxBuf[17] = 2;
	LoRaTxBuf[18] = 3;
	LoRaTxBuf[19] = 4;
	LoRaTxBuf[20] = 5;
	LoRaTxBuf[21] = 6;
	LoRaTxBuf[22] = 7;
	LoRaTxBuf[23] = 8;
	LoRaTxBuf[24] = 9;
	LoRaTxBuf[25] = 10;
	LoRaTxBuf[26] = 11;
	LoRaTxBuf[27] = 12;
	LoRaTxBuf[28] = 13;
	LoRaTxBuf[29] = 14;
}

// LoRa接收处理程序 校验程序
void LoRa_RcvProcess(void)
{
	u8 k, s, i = 0; // 临时变量
	u16 j;
	u16 *p_wRead;

	// 1. LORA接收数据存储  LORA主机 或者 LORA从机
	// 发送完，主机=4，延时后继续发送；从机=1，等待接收
	if (B_LoRaSendMaster == 4 || B_LoRaSendMaster == 1)
	{
		// if( Radio->Process( ) == RF_RX_DONE )
		while (Radio->Process() == RF_RX_DONE)
		{
			Radio->GetRxPacket(LoRaRxBuf, (uint16_t *)&BufferSize);
			// if( strncmp( ( const char* )LoRaRxBuf, ( const char* )MY_TEST_Msg, 18 ) == 0 )
			Cw_LoRaRx = BufferSize;
			Cw_BakLoRaRx = BufferSize; // 把 Cw_LoRaRx 保存

			if (w_ZhouShanProtocol_bit10) // 串口1进行LoRa监控数据收发，=1，监控，=0，不监控
			{
				if (S_M35 == 0x1B)
					u2_printf(LoRaRxBuf, Cw_BakLoRaRx); // 接收的数据转发到串口1
			}

			// ZCL 2018.12.14 Pw_LoRaSet .1位 =1   LORA输出到COM2 (透传使用)
			if (F_LoRaToCom)
			{
				// 把LoRa接收到的数据发送到串口2		ZCL 2018.11.13 OK
				for (i = 0; i < BufferSize; i++)
				{
					Txd2Buffer[i] = LoRaRxBuf[i];
				}
				Cw_Txd2Max = BufferSize;
				Cw_Txd2 = 0;
				// RS485_CON=1;
				// 2010.8.5 周成磊 TXE改成TC，一句改为两句
				USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]); // ZCL 2022.9.19
				USART_ITConfig(USART2, USART_IT_TC, ENABLE);   // 开始发送.
			}

			// LED0_TOGGLE;		//ZCL 2019.10.15 接收完成指示，改成发送完成指示！

			// ZCL 2019.2.16 LORA从机接收到，返回数据
			//  if(Pw_LoRaMasterSlaveSel==0)
			//  B_LoRaRcv=1;								//从机需要返回数据
			// else
			// B_LoRaRcv=0;

			// ZCL 2019.3.9
			B_LoRaRcv = 1; // 从机需要返回数据

			// Soft_delay_ms(35);		//ZCL 2018.11.10  2,5,7不行   8,20,15慢1次，25偶尔丢次，35，30,50,200可以
			// LCD_DLY_ms(35);				//ZCL 2018.11.12
		}
	}

	// 2. LORA接收存储数据的处理  LORA主机 或者 LORA从机
	if (B_LoRaRcv == 1) // 接收处理过程
	{
		B_LoRaRcv = 2;

		// LORA从机 接收到数据 （从地址检测－接收到的上位机查询指令）
		// ZCL注：这时候LORA是单纯从机，接收文本等上位机查询。 =2单纯从机（有3.16.1.6指令）。
		if (Pw_LoRaMasterSlaveSel == 0) // ZCL 2022.10.4 液晶屏用作主机，所以这段程序是无用的。
		{
			if (LoRaRxBuf[0] == Pw_LoRaEquipmentNo)
			{
				j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
				k = j >> 8;
				s = j;
				if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
				{						   // CRC校验正确
					if (LoRaRxBuf[1] == 3) // 03读取保持寄存器
					{
						B_LoRaCmd03 = 1;
					}
					else if (LoRaRxBuf[1] == 16) // 16预置多寄存器
					{
						C_ForceSavPar = 0; // 强制保存参数计数器=0
						B_LoRaCmd16 = 1;
					}
					else if (LoRaRxBuf[1] == 1) // 01读取线圈状态
					{
						B_LoRaCmd01 = 1;
					}
					else if (LoRaRxBuf[1] == 6) // 06预置单寄存器
					{
						C_ForceSavPar = 0; // 强制保存参数计数器=0
						B_LoRaCmd06 = 1;
					}
					// 2016.4.5
					//  （作为主模块的子机 (子模块)的程序：（主模块发送－73带参数读指令）－－－－－！！！
					else if (LoRaRxBuf[1] == 73) // 73带参数读指令
					{
						B_LoRaCmd73 = 1;
						//
						T_LoRaNoRcvMasterCMD = 100;
						C_LoRaNoRcvMasterCMD = 0;
						B_LoRaNoRcvMasterCMD = 0;
					}
					else
						i = 1;
				}
				else
					i = 2;
			}
		}

		// LORA主机 接收到数据 作为主模块(主机接收到的) 2018.12.14
		// LORA子模块1返回-->给主机的内容			=3.4.5.6 扩展从机（只有73指令）
		else if (LoRaRxBuf[0] == Pw_LoRaModule1Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1]) // CRC校验
			// if ( 0xD7==LoRaRxBuf[Cw_BakLoRaRx-2]
			// && 0x98==LoRaRxBuf[Cw_BakLoRaRx-1] )		//ZCL123 2019.10.17	测试
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{
					/*ZCL 2017.6.18 切换页面时数据有跳动，仿真发现前面一页的数据返回来有延时，传送到这一页上，
							造成数据跳动。改成返回的数据，根据前面Txd1Buffer的发送地址来赋值，现在好了！ OK*/

					p_wRead = w_DNBParLst; // ZCL 2022.5.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 40; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Reg
						}
					}

					//
					B_LoRaRCVCommandOk = 1;
					// B_LoRaSendMaster=8;				//ZCL 2019.10.17
					//
					// Radio->StartRx( );   //RFLR_STATE_RX_INIT			ZCL 2019.10.17

					T_LoRaCount1Err = 100;
					C_LoRaCount1Err = 0;
					B_LoRaErrWithModule1 = 0;

					// ZCL 2022.10.4
					w_LoraCounter1++; // 主机
				}
				else if (LoRaRxBuf[1] == 03) // 03 跟子模块通讯
				{
					B_LoRaCmd03_Master = 1;
				}
				else if (LoRaRxBuf[1] == 06) // 06 跟子模块通讯
				{
					B_LoRaCmd06_Master = 1;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块2返回-->给主机的内容
		else if (LoRaRxBuf[0] == Pw_LoRaModule2Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{
					p_wRead = &w_DNBParLst[40]; // ZCL 2022.5.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 40; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Regd
						}
					}

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount2Err = 100;
					C_LoRaCount2Err = 0;
					B_LoRaErrWithModule2 = 0;
					// ZCL 2022.10.4
					w_LoraCounter1++; // 主机
				}
			}
		}

		// ZCL 2022.9.10  修改加入：子模块3  子模块4

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块3返回-->给主机的内容
		else if (LoRaRxBuf[0] == Pw_LoRaModule3Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{
					p_wRead = &w_DNBParLst[80]; // ZCL 2022.9.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 40; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Reg
						}
					}

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount3Err = 100;
					C_LoRaCount3Err = 0;
					B_LoRaErrWithModule3 = 0;
					// ZCL 2022.10.4
					w_LoraCounter1++; // 主机
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块4返回-->给主机的内容
		else if (LoRaRxBuf[0] == Pw_LoRaModule4Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{
					p_wRead = &w_DNBParLst[120]; // ZCL 2022.9.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 40; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Reg
						}
					}

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount4Err = 100;
					C_LoRaCount4Err = 0;
					B_LoRaErrWithModule4 = 0;
					// ZCL 2022.10.4
					w_LoraCounter1++; // 主机
				}
			}
		}

		// ZCL 2022.6.1  下面的没有用到----------------------------------------------------------------
		//   LORA主机 作为主模块(主机接收到的) 2018.12.14
		//  子模块5返回-->给主机的内容
		else if (LoRaRxBuf[0] == Pw_LoRaModule5Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{
					p_wRead = &w_DNBParLst[160]; // ZCL 2022.5.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 40; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Reg
						}
					}

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount5Err = 100;
					C_LoRaCount5Err = 0;
					B_LoRaErrWithModule5 = 0;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.15
		// 子模块6返回-->给主机的内容
		else if (LoRaRxBuf[0] == Pw_LoRaModule6Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{

					p_wRead = &w_DNBParLst[500]; // ZCL 2022.5.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 45; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + 256 + 45 * 2 + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Reg
						}
					}

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount6Err = 100;
					C_LoRaCount6Err = 0;
					B_LoRaErrWithModule6 = 0;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块7返回-->给主机的内容
		else if (LoRaRxBuf[0] == Pw_LoRaModule7Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{

					p_wRead = &w_DNBParLst[500]; // ZCL 2022.5.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 45; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + 256 + 45 * 3 + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Reg
						}
					}

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount7Err = 100;
					C_LoRaCount7Err = 0;
					B_LoRaErrWithModule7 = 0;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块8返回-->给主机的内容
		else if (LoRaRxBuf[0] == Pw_LoRaModule8Add) // 从地址检测
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{

					p_wRead = &w_DNBParLst[500]; // ZCL 2022.5.19

					// 2. 接收到数据存储 ZCL 2018.12.14
					if (LoRaRxBuf[2] == 40 * 2) // ZCL 2019.9.11  2019.10.15
					{
						for (i = 0; i < 45; i++) // ZCL 2022.3.21  30
						{
							j = LoRaRxBuf[3 + i * 2];
							//*(p_wRead+i)=(j<<8)+LoRaRxBuf[4+i*2];
							*(p_wRead + 256 + 45 * 3 + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // 256 Lw_Reg
						}
					}

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount8Err = 100;
					C_LoRaCount8Err = 0;
					B_LoRaErrWithModule8 = 0;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) ZCL 2020.9.30
		// 子模块 99 返回-->给主机的内容 (雷达监测的SZM220 返回的数据)
		else if (0) // 从地址检测		LoRaRxBuf[0]==w_LoRaLeiDaModuleAdd
		{
			j = CRC16(LoRaRxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaRxBuf[Cw_BakLoRaRx - 2] && s == LoRaRxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaRxBuf[1] == 73) // 73 跟子模块通讯
				{
					// ZCL 2020.9.30  雷达专用SZM220送过来的DI值
					j = LoRaRxBuf[3];
					w_LoRaLeiDaDiValue = (j << 8) + LoRaRxBuf[4];

					B_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount9Err = 100; // ZCL 2022.3.11 这几个原先写成了8，这里修正！ T_LoRaCount8Err
					C_LoRaCount9Err = 0;
					B_LoRaErrWithModule9 = 0;
				}
			}
		}

		else
			i = 3;
	}

	// 3. 延时后再次接收
	if (B_LoRaRcv == 52) //=52 暂时没有此功能
	{
		if (T_LoRaRcv != SClk1Ms)
		{
			T_LoRaRcv = SClk1Ms; //
			C_LoRaRcv++;
			if (C_LoRaRcv > 35)
			{
				T_LoRaRcv = 100;
				C_LoRaRcv = 0;
				//
				B_LoRaRcv = 0;

				// ZCL 2018.12.14 接收处理完成，启动接收
				for (j = 0; j < Cw_BakLoRaRx; j++)
				{
					LoRaRxBuf[j] = 0;
				}
				Radio->StartRx(); // RFLR_STATE_RX_INIT

				Cw_LoRaRx = 0;
				Cw_BakLoRaRx = 0;
			}
		}
	}

	// ZCL 2022.3.23
	// 雷达继电器2S后断开
	if (w_LoRaLeiDaDiValue == 1)
	{
		if (T_LoRaLeiDa != SClk10Ms)
		{
			T_LoRaLeiDa = SClk10Ms; //
			C_LoRaLeiDa++;
			if (C_LoRaLeiDa > 200)
			{
				T_LoRaLeiDa = 100;
				C_LoRaLeiDa = 0;
				w_LoRaLeiDaDiValue = 0;
			}
		}
	}
}

void LoRa_SlaveSend(void) // LoRa从机发送
{
	u16 m; //,n;
	u8 j = 0, k;
	u16 *p_wRead;
	u8 *p_bMove;
	// u8	 *p_bGen;
	// u16	 *p_wTarget;			// 指向目标字符串　xdata zcl

	// ZCL 2018.12.15
	if (B_LoRaCmd03) // 读取保持寄存器
	{
		LoRaTxBuf[0] = LoRaRxBuf[0];	 // 设备从地址Pw_EquipmentNo
		LoRaTxBuf[1] = LoRaRxBuf[1];	 // 功能码
		LoRaTxBuf[2] = LoRaRxBuf[5] * 2; // LoRaRxBuf[5]=字数

		// ZCL 2018.12.14 这里进行地址合成
		j = LoRaRxBuf[2];
		Lw_Reg = (j << 8) + LoRaRxBuf[3];
		// ZCL 2018.12.14 地址转换
		p_wRead = AddressChange();

		p_bMove = LoRaTxBuf;
		for (k = 0; k < LoRaRxBuf[5]; k++) // 填充查询内容
		{
			m = *(p_wRead + Lw_Reg + k);
			*(p_bMove + 3 + k * 2) = m >> 8;
			*(p_bMove + 3 + k * 2 + 1) = m;
		}
		Lw_LoRaChkSum = CRC16(LoRaTxBuf, LoRaTxBuf[2] + 3);
		LoRaTxBuf[LoRaTxBuf[2] + 3] = Lw_LoRaChkSum >> 8; // /256
		LoRaTxBuf[LoRaTxBuf[2] + 4] = Lw_LoRaChkSum;	  // 低位字节

		Cw_LoRaTxMax = LoRaTxBuf[2] + 5;
		B_LoRaCmd03 = 0;
		Cw_LoRaTx = 0;
		// ZCL 2018.12.14 发送
		// Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax );   //RFLR_STATE_TX_INIT
		B_LoRaSendMaster = 22;
	}

	else if (B_LoRaCmd16 || B_LoRaCmd06) // 16预置多寄存器
	{
		j = 1;

		// ZCL123 2018.12.14   内容不全，继续加其他内容 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// ZCL 2018.12.14 这里进行地址合成
		j = LoRaRxBuf[2];
		Lw_Reg = (j << 8) + LoRaRxBuf[3];
		// ZCL 2018.12.14 地址转换
		p_wRead = AddressChange();

		B_LoRaCmd16 = 0;
		B_LoRaCmd06 = 0;
		Cw_LoRaTx = 0;
		// ZCL 2018.12.14 发送
		// Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax );   //RFLR_STATE_TX_INIT
		B_LoRaSendMaster = 22;
	}

	// ZCL 2019.9.24 查询16个字，加帧头帧尾共计37个字节
	else if (B_LoRaCmd73) // 73
	{
		Delay_MS(5); // 500? ZCL123

		LoRaTxBuf[0] = LoRaRxBuf[0];	 // 设备从地址Pw_EquipmentNo
		LoRaTxBuf[1] = LoRaRxBuf[1];	 // 功能码
		LoRaTxBuf[2] = LoRaRxBuf[5] * 2; // LoRaRxBuf[5]=字数 　

		// ZCL 2018.12.14 这里进行地址合成
		j = LoRaRxBuf[2];
		Lw_Reg = (j << 8) + LoRaRxBuf[3];
		// ZCL 2018.12.14 地址转换
		// p_wRead=AddressChange();
		p_wRead = &w_dsp1SoftVersion; // ZCL 2019.3.8

		p_bMove = LoRaTxBuf;

		if (LoRaRxBuf[5] == 10) // ZCL 2019.9.11
		{
			for (k = 0; k < LoRaRxBuf[5]; k++) // 填充查询内容
			{
				// m=*(p_wRead+Lw_Reg+k);			//ZCL 2019.1.23 错误
				// m=*(p_wRead+k);
				m = *(p_wRead + Lw_Reg + k); // ZCL 2019.3.7 因为用了新数组，所以需要Lw_Reg

				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
		}

		// ZCL 2019.9.11 加大到16个字
		else if (LoRaRxBuf[5] == 16)
		{
			p_wRead = &w_dsp1SoftVersion; // ZCL 2019.3.8
			for (k = 0; k < 10; k++)	  // 填充查询内容
			{
				// m=*(p_wRead+Lw_Reg+k);			//ZCL 2019.1.23 错误
				// m=*(p_wRead+k);
				m = *(p_wRead + Lw_Reg + k); // ZCL 2019.3.7 因为用了新数组，所以需要Lw_Reg

				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}

			p_wRead = &w_1InPDec;	  // ZCL 2019.9.11
			for (k = 10; k < 16; k++) // 填充查询内容，2019.9.11  填充后6个
			{
				// m=*(p_wRead+Lw_Reg+k);			//ZCL 2019.1.23 错误
				// m=*(p_wRead+k);
				m = *(p_wRead + Lw_Reg + k - 10); // ZCL 2019.3.7 因为用了新数组，所以需要Lw_Reg		  ZCL 2019.9.11  -10

				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
			//
		}

		Lw_LoRaChkSum = CRC16(LoRaTxBuf, LoRaTxBuf[2] + 3);
		LoRaTxBuf[LoRaTxBuf[2] + 3] = Lw_LoRaChkSum >> 8; // /256
		LoRaTxBuf[LoRaTxBuf[2] + 4] = Lw_LoRaChkSum;	  // 低位字节

		Cw_LoRaTxMax = LoRaTxBuf[2] + 5; // ZCL 2019.9.24 查询16个字，加帧头帧尾共计37个字节
		B_LoRaCmd73 = 0;
		Cw_LoRaTx = 0;
		// ZCL 2018.12.14 发送
		// Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax );   //RFLR_STATE_TX_INIT
		B_LoRaSendMaster = 22;
	}

	else if (B_LoRaCmd03_Master) // 读取从机参数
	{
		Txd3TmpBuffer[0] = Pw_LoRaEquipmentNo; // 本机LoRa地址

		// YLS 2024.10.06 为了能让上位机区分是那台从机返回的数据，对03功能码进行改进，便于上位机区分
		if (LoRaRxBuf[0] == Pw_LoRaModule1Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x10; // 功能码
		}
		else if (LoRaRxBuf[0] == Pw_LoRaModule2Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x20; // 功能码
		}
		else if (LoRaRxBuf[0] == Pw_LoRaModule3Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x30; // 功能码
		}
		else if (LoRaRxBuf[0] == Pw_LoRaModule4Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x40; // 功能码
		}
		else if (LoRaRxBuf[0] == Pw_LoRaModule5Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x50; // 功能码
		}
		else if (LoRaRxBuf[0] == Pw_LoRaModule6Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x60; // 功能码
		}
		else if (LoRaRxBuf[0] == Pw_LoRaModule7Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x70; // 功能码
		}
		else if (LoRaRxBuf[0] == Pw_LoRaModule8Add)
		{
			Txd3TmpBuffer[1] = LoRaRxBuf[1] + 0x80; // 功能码
		}

		Txd3TmpBuffer[2] = LoRaRxBuf[2]; // Rcv2Buffer[5]=字数 　

		for (k = 1; k <= LoRaRxBuf[2]; k++) // 填充查询内容
		{
			Txd3TmpBuffer[2 + k] = LoRaRxBuf[2 + k]; // Rcv2Buffer[5]=字数 　
		}
		Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, Txd3TmpBuffer[2] + 3);
		Txd3TmpBuffer[Txd3TmpBuffer[2] + 3] = Lw_Txd3ChkSum >> 8; // /256
		Txd3TmpBuffer[Txd3TmpBuffer[2] + 4] = Lw_Txd3ChkSum;	  // 低位字节
		Cw_Txd3TmpMax = Txd3TmpBuffer[2] + 5;
		//
		B_LoRaCmd03_Master = 0;
		Cw_Txd3 = 0;

		// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
		B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络
	}
}

// 作为主模块时程序 2016.4.1
void LoRa_MasterSend(void) // LoRa主发送程序
{
	u8 i = 0, j = 0;

	w_LoRaLeiDaModuleAdd = 0;

	if (Pw_LoRaModule1Add > 0)
		i++; // 统计发送数量
	if (Pw_LoRaModule2Add > 0)
		i++;
	if (Pw_LoRaModule3Add > 0)
		i++;
	if (Pw_LoRaModule4Add > 0)
		i++;
	if (Pw_LoRaModule5Add > 0)
		i++;
	if (Pw_LoRaModule6Add > 0)
		i++;
	if (Pw_LoRaModule7Add > 0)
		i++;
	if (Pw_LoRaModule8Add > 0)
		i++;

	if (i > 0) // 必须有子模块或者变频器，流量计才能进入发送程序
	{
		if (B_LoRaSendMaster == 1) //&& B_LoRaRCVCommandOk
		{
			if (T_LoRaDelaySend != SClk10Ms)
			{
				T_LoRaDelaySend = SClk10Ms; //
				C_LoRaDelaySend++;
				if (C_LoRaDelaySend > 20) // 210可以 ZCL 2019.10.17	；使用连续接收模式后，2都可以了！ 解决！
				{
					T_LoRaDelaySend = 100;
					C_LoRaDelaySend = 0;

					if (F_ModeParLora == 0) // 如没有修改命令，就发送查询指令
					{
						// 1. 控制发送步骤
						S_LoRaSendNoMaster++;

						// S_LoRaSendNoMaster=1;  //ZCL123		测试时用，只发送1个地址的，后面删除，恢复++

						if (S_LoRaSendNoMaster > i) // ZCL 2020.9.30  8  ZCL 2023.2.9  4
							S_LoRaSendNoMaster = 1; // COM_UNIT_NUM

						// 2.进行顺序发送
						if (Pw_LoRaModule1Add > 0 && S_LoRaSendNoMaster == 1)
						{
							LoRaTxBuf[0] = Pw_LoRaModule1Add; // 设备从地址
							j = 1;
						}
						else if (Pw_LoRaModule2Add > 0 && S_LoRaSendNoMaster == 2)
						{
							LoRaTxBuf[0] = Pw_LoRaModule2Add; // 设备从地址
							j = 2;
						}
						else if (Pw_LoRaModule3Add > 0 && S_LoRaSendNoMaster == 3)
						{
							LoRaTxBuf[0] = Pw_LoRaModule3Add; // 设备从地址
							j = 3;
						}
						else if (Pw_LoRaModule4Add > 0 && S_LoRaSendNoMaster == 4)
						{
							LoRaTxBuf[0] = Pw_LoRaModule4Add; // 设备从地址
							j = 4;
						}

						// ZCL 2019.10.18
						else if (Pw_LoRaModule5Add > 0 && S_LoRaSendNoMaster == 5)
						{
							LoRaTxBuf[0] = Pw_LoRaModule5Add; // 设备从地址
							j = 5;
						}
						else if (Pw_LoRaModule6Add > 0 && S_LoRaSendNoMaster == 6)
						{
							LoRaTxBuf[0] = Pw_LoRaModule6Add; // 设备从地址
							j = 6;
						}
						else if (Pw_LoRaModule7Add > 0 && S_LoRaSendNoMaster == 7)
						{
							LoRaTxBuf[0] = Pw_LoRaModule7Add; // 设备从地址
							j = 7;
						}
						else if (Pw_LoRaModule8Add > 0 && S_LoRaSendNoMaster == 8)
						{
							LoRaTxBuf[0] = Pw_LoRaModule8Add; // 设备从地址
							j = 8;
						}

						else
							j = 0;

						if (j > 0) // LORA 发往子模块
								   //|| B_LoRaSendMaster==2)			//ZCL 2019.1.24 =2立即发送，这是2只是补上结构，实际没有使用
						{
							// LoRaTxBuf[0]=Pw_EquipmentNo+S_LoRaSendNoMaster;
							LoRaTxBuf[1] = 73;	 // 功能码 73=0x49
							LoRaTxBuf[2] = 0x00; // 开始地址（高位） 0x7540:30016: w_dspParLst[16]	//.2 A相电流
							LoRaTxBuf[3] = 0x00; // 开始地址（低位）
							// LoRaTxBuf[4]=0x00;						// 读取个数（高位）
							LoRaTxBuf[4] = 0;  // 读取个数（高位）  ZCL 2020.9.30  借用了这个字节  发送给各高压电机从机
							LoRaTxBuf[5] = 40; // 读取个数（低位）  ZCL 2019.9.11 10改成16个，根据情况调整数值  24个
											   // ZCL 2022.3.10 24个不够，增加3个字的运行时间。改成30个，预留3个
							Lw_LoRaChkSum = CRC16(LoRaTxBuf, 6);
							LoRaTxBuf[6] = Lw_LoRaChkSum >> 8; // /256
							LoRaTxBuf[7] = Lw_LoRaChkSum;	   // 低位字节
							Cw_LoRaTxMax = 8;
							Cw_LoRaTx = 0;

							B_LoRaSendMaster = 12; //=12 主机发送
							B_LoRaRCVCommandOk = 0;

							// ZCL 2018.12.15 发送
							// Radio->SetTxPacket( LoRaTxBuf, Cw_LoRaTxMax );   //RFLR_STATE_TX_INIT
						}
					}

					// ZCL 2023.2.9  发送 16  06指令 修改参数
					else if (F_ModeParLora == 1)
					{
						if (w_ZhuanFaAdd < 40)
						{
							LoRaTxBuf[0] = Pw_LoRaModule1Add; // 泵1
							LoRaTxBuf[2] = w_ZhuanFaAdd >> 8; // 开始地址（高位） 		// /256;
							LoRaTxBuf[3] = w_ZhuanFaAdd;	  // 开始地址（低位）
						}

						if (w_ZhuanFaAdd >= 40 && w_ZhuanFaAdd < 80)
						{
							LoRaTxBuf[0] = Pw_LoRaModule2Add;		 // 泵2
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 40) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = w_ZhuanFaAdd - 40;		 // 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 80 && w_ZhuanFaAdd < 120)
						{
							LoRaTxBuf[0] = Pw_LoRaModule3Add;		 // ??3
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 80) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = w_ZhuanFaAdd - 80;		 // 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 120 && w_ZhuanFaAdd < 160)
						{
							LoRaTxBuf[0] = Pw_LoRaModule4Add;		  // ??4
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 120) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = w_ZhuanFaAdd - 120;		  // 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 160 && w_ZhuanFaAdd < 200)
						{
							LoRaTxBuf[0] = Pw_LoRaModule5Add;		  // ??5
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 160) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = w_ZhuanFaAdd - 160;		  // 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 200 && w_ZhuanFaAdd < 240)
						{
							LoRaTxBuf[0] = Pw_LoRaModule6Add;		  // ??6
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 200) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = w_ZhuanFaAdd - 200;		  // 开始地址（低位）
						}

						if (w_ZhuanFaAdd >= 240 && w_ZhuanFaAdd < 280)
						{
							LoRaTxBuf[0] = Pw_LoRaModule7Add;		  // 泵7			YLS 2023.10.24修改地址错误
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 240) >> 8; // 开始地址（高位） 		// /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 240);	  // 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 280 && w_ZhuanFaAdd < 320)
						{
							LoRaTxBuf[0] = Pw_LoRaModule8Add;		  // 泵8			YLS 2023.10.24修改地址错误
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 280) >> 8; // 开始地址（高位） 		// /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 280);	  // 开始地址（低位）
						}

						LoRaTxBuf[1] = 6; // 功能码 73=0x49

						LoRaTxBuf[4] = w_ZhuanFaData >> 8; // 数据（高位） 		// /256;
						LoRaTxBuf[5] = w_ZhuanFaData;	   // 数据（低位）

						Lw_LoRaChkSum = CRC16(LoRaTxBuf, 6);
						LoRaTxBuf[6] = Lw_LoRaChkSum >> 8; // /256
						LoRaTxBuf[7] = Lw_LoRaChkSum;	   // 低位字节
						Cw_LoRaTxMax = 8;
						Cw_LoRaTx = 0;

						B_LoRaSendMaster = 12; //=12 主机发送
						B_LoRaRCVCommandOk = 0;

						F_ModeParLora = 0; // ZCL 2023.2.10  清空预置标志

						// ZCL 2018.12.15 发送
						// Radio->SetTxPacket( LoRaTxBuf, Cw_LoRaTxMax );   //RFLR_STATE_TX_INIT
					}
					// 读某一台从机的参数，03指令，
					// 地址[10000,19999)→1#，[20000,29999)→2#，[30000,39999)→3#，[40000,49999)→4#，[50000,59999)→5#
					else if (F_ModeParLora == 2)
					{
						if (w_ZhuanFaAdd >= 10000 && w_ZhuanFaAdd < 19999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule1Add;			// 泵1
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 10000) >> 8; // 开始地址（高位） 		// /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 10000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 20000 && w_ZhuanFaAdd < 29999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule2Add;			// 泵2
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 20000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 20000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 30000 && w_ZhuanFaAdd < 39999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule3Add;			// 泵3
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 30000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 30000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 40000 && w_ZhuanFaAdd < 49999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule4Add;			// 泵4
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 40000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 40000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 50000 && w_ZhuanFaAdd < 59999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule5Add;			// 泵5
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 50000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 50000);		// 开始地址（低位）
						}

						LoRaTxBuf[1] = 3; // 功能码 03

						LoRaTxBuf[4] = w_ZhuanFaData >> 8; // 数据（高位） 		// /256;
						LoRaTxBuf[5] = w_ZhuanFaData;	   // 数据（低位）

						Lw_LoRaChkSum = CRC16(LoRaTxBuf, 6);
						LoRaTxBuf[6] = Lw_LoRaChkSum >> 8; // /256
						LoRaTxBuf[7] = Lw_LoRaChkSum;	   // 低位字节
						Cw_LoRaTxMax = 8;
						Cw_LoRaTx = 0;

						B_LoRaSendMaster = 12; //=12 主机发送
						B_LoRaRCVCommandOk = 0;

						// F_ModeParLora = 0; // ZCL 2023.2.10  清空预置标志
					}
					// 写某一台从机的参数，06指令，
					// 地址[10000,19999)→1#，[20000,29999)→2#，[30000,39999)→3#，[40000,49999)→4#，[50000,59999)→5#
					else if (F_ModeParLora == 3)
					{
						if (w_ZhuanFaAdd >= 10000 && w_ZhuanFaAdd < 19999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule1Add;			// 泵1
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 10000) >> 8; // 开始地址（高位） 		// /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 10000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 20000 && w_ZhuanFaAdd < 29999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule2Add;			// 泵2
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 20000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 20000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 30000 && w_ZhuanFaAdd < 39999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule3Add;			// 泵3
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 30000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 30000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 40000 && w_ZhuanFaAdd < 49999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule4Add;			// 泵4
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 40000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 40000);		// 开始地址（低位）
						}
						if (w_ZhuanFaAdd >= 50000 && w_ZhuanFaAdd < 59999)
						{
							LoRaTxBuf[0] = Pw_LoRaModule5Add;			// 泵5
							LoRaTxBuf[2] = (w_ZhuanFaAdd - 50000) >> 8; // 开始地址（高位）         // /256;
							LoRaTxBuf[3] = (w_ZhuanFaAdd - 50000);		// 开始地址（低位）
						}

						LoRaTxBuf[1] = 6; // 功能码 06

						LoRaTxBuf[4] = w_ZhuanFaData >> 8; // 数据（高位） 		// /256;
						LoRaTxBuf[5] = w_ZhuanFaData;	   // 数据（低位）

						Lw_LoRaChkSum = CRC16(LoRaTxBuf, 6);
						LoRaTxBuf[6] = Lw_LoRaChkSum >> 8; // /256
						LoRaTxBuf[7] = Lw_LoRaChkSum;	   // 低位字节
						Cw_LoRaTxMax = 8;
						Cw_LoRaTx = 0;

						B_LoRaSendMaster = 12; //=12 主机发送
						B_LoRaRCVCommandOk = 0;

						F_ModeParLora = 0; // ZCL 2023.2.10  清空预置标志
					}
				}
			}
		}

		//----------------------------------------------------------------
		// 6. 判断超时
		// 6.1判断超时－延时通讯故障 4000MS   与子模块1通讯失败 // 6000
		if (Pw_LoRaModule1Add > 0 && !B_LoRaErrWithModule1) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount1Err != SClk10Ms)
			{
				T_LoRaCount1Err = SClk10Ms; //
				C_LoRaCount1Err++;
				if (C_LoRaCount1Err > 600) //
				{
					T_LoRaCount1Err = 100;
					C_LoRaCount1Err = 0;
					B_LoRaErrWithModule1 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule1Add == 0)
		{
			B_LoRaErrWithModule1 = 0; //
			C_LoRaCount1Err = 0;
		}

		//----------------------------------------------------------------
		// 6.2判断超时－延时通讯故障 4000MS	与子模块2通讯失败
		if (Pw_LoRaModule2Add > 0 && !B_LoRaErrWithModule2) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount2Err != SClk10Ms)
			{
				T_LoRaCount2Err = SClk10Ms; //
				C_LoRaCount2Err++;
				if (C_LoRaCount2Err > 600) //
				{
					T_LoRaCount2Err = 100;
					C_LoRaCount2Err = 0;
					B_LoRaErrWithModule2 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule2Add == 0)
		{
			B_LoRaErrWithModule2 = 0; //
			C_LoRaCount2Err = 0;
		}

		//----------------------------------------------------------------
		// 6.3判断超时－延时通讯故障 4000MS	与子模块3通讯失败  ZCL 2018.12.15
		if (Pw_LoRaModule3Add > 0 && !B_LoRaErrWithModule3) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount3Err != SClk10Ms)
			{
				T_LoRaCount3Err = SClk10Ms; //
				C_LoRaCount3Err++;
				if (C_LoRaCount3Err > 600) //
				{
					T_LoRaCount3Err = 100;
					C_LoRaCount3Err = 0;
					B_LoRaErrWithModule3 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule3Add == 0)
		{
			B_LoRaErrWithModule3 = 0; //
			C_LoRaCount3Err = 0;
		}

		//----------------------------------------------------------------
		// 6.4判断超时－延时通讯故障 4000MS	与子模块4通讯失败   ZCL 2018.12.15
		if (Pw_LoRaModule4Add > 0 && !B_LoRaErrWithModule4) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount4Err != SClk10Ms)
			{
				T_LoRaCount4Err = SClk10Ms; //
				C_LoRaCount4Err++;
				if (C_LoRaCount4Err > 600) //
				{
					T_LoRaCount4Err = 100;
					C_LoRaCount4Err = 0;
					B_LoRaErrWithModule4 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule4Add == 0)
		{
			B_LoRaErrWithModule4 = 0; //
			C_LoRaCount4Err = 0;
		}

		//----------------------------------------------------------------
		// 6.5判断超时－延时通讯故障 4000MS	与子模块5通讯失败   ZCL 2018.12.15  2019.10.18
		if (Pw_LoRaModule5Add > 0 && !B_LoRaErrWithModule5) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount5Err != SClk10Ms)
			{
				T_LoRaCount5Err = SClk10Ms; //
				C_LoRaCount5Err++;
				if (C_LoRaCount5Err > 600) //
				{
					T_LoRaCount5Err = 100;
					C_LoRaCount5Err = 0;
					B_LoRaErrWithModule5 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule5Add == 0)
		{
			B_LoRaErrWithModule5 = 0; //
			C_LoRaCount5Err = 0;
		}

		//----------------------------------------------------------------
		// 6.6判断超时－延时通讯故障 4000MS	与子模块6通讯失败   ZCL 2018.12.15  2019.10.18
		if (Pw_LoRaModule6Add > 0 && !B_LoRaErrWithModule6) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount6Err != SClk10Ms)
			{
				T_LoRaCount6Err = SClk10Ms; //
				C_LoRaCount6Err++;
				if (C_LoRaCount6Err > 600) //
				{
					T_LoRaCount6Err = 100;
					C_LoRaCount6Err = 0;
					B_LoRaErrWithModule6 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule6Add == 0)
		{
			B_LoRaErrWithModule6 = 0; //
			C_LoRaCount6Err = 0;
		}

		//----------------------------------------------------------------
		// 6.7判断超时－延时通讯故障 4000MS	与子模块7通讯失败   ZCL 2018.12.15  2019.10.18
		if (Pw_LoRaModule7Add > 0 && !B_LoRaErrWithModule7) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount7Err != SClk10Ms)
			{
				T_LoRaCount7Err = SClk10Ms; //
				C_LoRaCount7Err++;
				if (C_LoRaCount7Err > 600) //
				{
					T_LoRaCount7Err = 100;
					C_LoRaCount7Err = 0;
					B_LoRaErrWithModule7 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule7Add == 0)
		{
			B_LoRaErrWithModule7 = 0; //
			C_LoRaCount7Err = 0;
		}

		//----------------------------------------------------------------
		// 6.8判断超时－延时通讯故障 4000MS	与子模块8通讯失败   ZCL 2018.12.15  2019.10.18
		if (Pw_LoRaModule8Add > 0 && !B_LoRaErrWithModule8) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount8Err != SClk10Ms)
			{
				T_LoRaCount8Err = SClk10Ms; //
				C_LoRaCount8Err++;
				if (C_LoRaCount8Err > 600) //
				{
					T_LoRaCount8Err = 100;
					C_LoRaCount8Err = 0;
					B_LoRaErrWithModule8 = 1; //
				}
			}
		}
		else if (Pw_LoRaModule8Add == 0)
		{
			B_LoRaErrWithModule8 = 0; //
			C_LoRaCount8Err = 0;
		}

		//----------------------------------------------------------------
		// 6.9判断超时－延时通讯故障 4000MS	与子模块9通讯失败   ZCL 2022.3.11
		if (w_LoRaLeiDaModuleAdd > 0 && !B_LoRaErrWithModule9) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount9Err != SClk10Ms)
			{
				T_LoRaCount9Err = SClk10Ms; //
				C_LoRaCount9Err++;
				if (C_LoRaCount9Err > 600) //
				{
					T_LoRaCount9Err = 100;
					C_LoRaCount9Err = 0;
					B_LoRaErrWithModule9 = 1; //
				}
			}
		}
		else if (w_LoRaLeiDaModuleAdd == 0)
		{
			B_LoRaErrWithModule9 = 0; //
			C_LoRaCount9Err = 0;
		}
	}
}

void LoRaSendToDone(void) // LoRa 发送，并检测是否完成
{
	// 4.0. SX1278 TX发送		COM2中赋值，当接收到数据，让B_LoRaSendMaster=2，然后这里可以LoRa发送

	// ZCL 2019.2.16 测试串口收到后，发送到LORA
	if (B_LoRaSendMaster == 2 || B_LoRaSendMaster == 12 || B_LoRaSendMaster == 22) //	|| B_LoRaSendMaster==3
	{
		LCD_DLY_ms(10); // ZCL123 2018.11.12

		//=12 主机发送指令
		if (B_LoRaSendMaster == 12)
		{
			Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax); // RFLR_STATE_TX_INIT
		}

		//=22	从机收到，发送返回数据
		else if (B_LoRaSendMaster == 22)
		{
			Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax); // RFLR_STATE_TX_INIT
		}

		//=2 串口转发
		else if (B_LoRaSendMaster == 2)
		{
			// ZCL 2018.11.12
			Cw_LoRaTxMax = Cw_BakRcv2;
			Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax); // RFLR_STATE_TX_INIT
		}

		// 发送到串口1，进行监控 YLS 2023.03.23
		if (w_ZhouShanProtocol_bit10) // 串口1进行LoRa监控数据收发，=1，监控，=0，不监控
		{
			if (S_M35 == 0x1B)
				u2_printf(LoRaTxBuf, Cw_LoRaTxMax);
		}

		LCD_DLY_ms(10); // ZCL123 2018.11.12
		B_LoRaSendMaster = 3;
		// ZCL 2019.2.16 不能这么写，必须用 Radio->Process( ) 来判断是否发送完
		// Radio->StartRx( );   //RFLR_STATE_RX_INIT
	}

	// ZCL 2019.2.16
	//=3 进入发送后，等待LORA发送完成 RF_TX_DONE。 发送完，主机=4，延时后继续发送；从机=1，等待接收
	if (B_LoRaSendMaster == 3)
	{
		while (Radio->Process() == RF_TX_DONE)
		{
			Radio->StartRx(); // RFLR_STATE_RX_INIT

			// ZCL  2019.2.16 主机发送完，等待接收 B_LoRaSendMaster=4
			if (Pw_LoRaMasterSlaveSel == 1)
				B_LoRaSendMaster = 4;
			else
				B_LoRaSendMaster = 1;

			LED0_TOGGLE; // ZCL 2019.10.15 接收完成指示，改成发送完成指示！
		}
	}

	// 4.1 LORA正常发送完，进入接收状态
	// 4.2 LORA发送，但没有检测到RF_TX_DONE，延时退出让B_LoRaSendMaster=3;
	if (B_LoRaSendMaster == 53) //=53暂时不执行 ZCL 2019.2.18
	{
		// 4.1 LORA 检查是否发送完，发送完进入接收状态
		while (Radio->Process() == RF_TX_DONE)
		{
			printf("RF_LoRa_TX_OK! \n"); // ZCL123 2019.1.24 正常应用时删除！

			LED0_TOGGLE;

			B_LoRaSendMaster = 4; // B_LoRaSendMaster=4，发送完成。延时后启动接收

			// ZCL 2018.11.13 发送完成启动接收
			Radio->StartRx(); // RFLR_STATE_RX_INIT
			LCD_DLY_ms(50);	  // ZCL 2018.11.12

			B_LoRaRcv = 0;

			T_LoRaSendOverTime = 100;
			C_LoRaSendOverTime = 0;
			C_LoRaCount1OvertimeNext = 0;
		}

		// 4.2 LORA发送(B_LoRaSendMaster==2)，但没有检测到RF_TX_DONE，延时退出让B_LoRaSendMaster=3;
		// 没有 while(Radio->Process( ) == RF_TX_DONE )	，进行延时
		if (T_LoRaSendOverTime != SClk10Ms) //
		{
			T_LoRaSendOverTime = SClk10Ms; //
			C_LoRaSendOverTime++;
			if (C_LoRaSendOverTime > 400) //
			{
				T_LoRaSendOverTime = 100;
				C_LoRaSendOverTime = 0;
				C_LoRaCount1OvertimeNext = 0;

				// printf("RF_LoRa_TX_OverTime \n");
				printf(" TX OverTime \n"); // ZCL123 2019.1.24 正常应用时删除！

				B_LoRaSendMaster = 4; // B_LoRaSendMaster=4，发送完成。延时后启动接收

				LCD_DLY_ms(10); // ZCL 2018.11.12
				// ZCL 2018.11.13 发送完成启动接收
				Radio->StartRx(); // RFLR_STATE_RX_INIT
			}
		}
	}

	// 4.3发送完成，延时进行再次发送。
	if (B_LoRaSendMaster == 54) //=54暂时不执行 ZCL 2019.2.18
	{
		if (Pw_ScrLoRaTest1 == 1)
		{
			T_LoRaCount1OvertimeNext = 100;
			C_LoRaCount1OvertimeNext = 0;
			B_LoRaSendMaster = 1; // =1发送
		}

		else if (T_LoRaCount1OvertimeNext != SClk10Ms)
		{
			T_LoRaCount1OvertimeNext = SClk10Ms; //
			C_LoRaCount1OvertimeNext++;
			if (C_LoRaCount1OvertimeNext > 300) //
			{
				T_LoRaCount1OvertimeNext = 100;
				C_LoRaCount1OvertimeNext = 0;
				B_LoRaSendMaster = 1; // =1发送
			}
		}
	}

	// 4.4发送完成，延时进行再次发送。  ZCL 2019.2.18 正常运行
	if (B_LoRaSendMaster == 4 || B_LoRaSendMaster == 3)
	{
		if (T_LoRaCount1OvertimeNext != SClk10Ms)
		{
			T_LoRaCount1OvertimeNext = SClk10Ms; //
			C_LoRaCount1OvertimeNext++;
			if (C_LoRaCount1OvertimeNext > 60) // (1)2S正好， 1.5S都不太正常！单次接收模式 ZCL 2019.10.17;  (2) 改成连续接收模式后，600MS都很好！
			{
				T_LoRaCount1OvertimeNext = 100;
				C_LoRaCount1OvertimeNext = 0;
				if (B_LoRaSendMaster == 3)
					Radio->StartRx(); // RFLR_STATE_RX_INIT
				B_LoRaSendMaster = 1; // =1发送

				C_LoRaCountNormalNext = 0;
			}
		}
	}
}

void LoRaRcvTest(void) // LoRa 接收测试
{
	u8 i;
	// 2. SX1278 RX接收
	// 2.1 延时
	if (B_LoRaRcv == 1)
	{
		if (T_LoRaRcv != SClk1Ms)
		{
			T_LoRaRcv = SClk1Ms; //
			C_LoRaRcv++;
			if (C_LoRaRcv > 1) // ZCL123 35
			{
				T_LoRaRcv = 100;
				C_LoRaRcv = 0;
				//
				B_LoRaRcv = 2;
			}
		}
	}

	// 2.2 进入接收状态
	if (B_LoRaRcv == 2)
	{
		B_LoRaRcv = 0;
		// Radio->StartRx( );
	}

	// 2.3 判断接收是否完成
	if (B_LoRaRcv == 0)
	{
		while (Radio->Process() == RF_RX_DONE)
		{
			Radio->GetRxPacket(LoRaRxBuf, (uint16_t *)&BufferSize);

			// 把LoRa接收到的数据发送到串口2		ZCL 2018.11.13 OK
			for (i = 0; i < BufferSize; i++)
			{
				Txd2Buffer[i] = LoRaRxBuf[i];
			}
			Cw_Txd2Max = BufferSize + 2;
			Cw_Txd2 = 0;
			// RS485_CON=1;
			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
			USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]); // ZCL 2022.9.19
			USART_ITConfig(USART2, USART_IT_TC, ENABLE);   // 开始发送.

			LED0_TOGGLE;
			// 清空缓存
			for (i = 0; i < BUFFER_SIZE; i++)
				LoRaRxBuf[i] = 0;

			// Soft_delay_ms(35);		//ZCL 2018.11.10  2,5,7不行   8,20,15慢1次，25偶尔丢次，35，30,50,200可以
			// LCD_DLY_ms(35);				//ZCL 2018.11.12

			// ZCL 2018.11.13 B_LoRaSendMaster=1,延时后接收
			B_LoRaRcv = 1;
		}
	}
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
