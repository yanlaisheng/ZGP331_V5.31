/**
  ******************************************************************************
  * @file    com_LoRa.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.20
  * @date    2018-12-14
  * @brief   LoRa USE SPI3
	******************************************************************************
	* @note	2015.04.13

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

// #include "sys_config.h"			//ZCL 2018.11.12
#include "spi.h" //ZCL 2018.11.12		sx12xxEiger文件夹中

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// ZCL 2018.11.12
#define BUFFER_SIZE 100 // Define the payload size here

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// ZCL 2018.11.12
tRadioDriver *Radio = NULL;
const uint8_t MY_TEST_Msg[] = "LoRa_SX1278_SANLI";
static uint16_t BufferSize = BUFFER_SIZE; // RF buffer size

uint8_t LoRaTxBuf[BUFFER_SIZE]; // ZCL 2018.11.12
uint8_t LoRaRxBuf[BUFFER_SIZE]; // RF buffer

u8 B_LoRaSendTest;
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

//------------------------2016.4.2
u8 S_LoRaMSend; // 步骤

u8 T_LoRaCountNormalNext;	  // 定时器
u16 C_LoRaCountNormalNext;	  // 计数器
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

u8 B_LoRaErrWithModule1; // 标志 与子模块1通讯失败
u8 B_LoRaErrWithModule2; // 标志 与子模块2通讯失败
u8 B_LoRaErrWithModule3; // 标志 与子模块1通讯失败
u8 B_LoRaErrWithModule4; // 标志 与子模块2通讯失败

u8 F_LoRaRCVCommandOk;
u8 F_LoRaSendNext;

u8 T_LoRaNoRcvMasterCMD;  // 定时器
u16 C_LoRaNoRcvMasterCMD; // 计数器
u8 F_LoRaNoRcvMasterCMD;

u8 T_LoRaSendOverTime; // ZCL 2019.1.22 Lora发送超时
u16 C_LoRaSendOverTime;

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

/* Private function prototypes -----------------------------------------------*/
void GPIO_LoRa_Configuration(void); // GPIO配置
void LoRa_config(void);
u16 CRC16(u8 *pCrcData, u8 CrcDataLen);
void Delay_MS(vu16 nCount);
u16 Address(u16 *p, u16 Area); // 绝对地址

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
	B_LoRaSendTest = 0;
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

	F_LoRaSendNext = 1;
}

// ZCL 2019.1.24 备注
/* 		F_LoRaSendNext=1;//发送
		F_LoRaSendNext=2;//等待发送完成
		F_LoRaSendNext=3;//进入接收状态 */

// 接收处理程序 校验程序
void LoRa_RcvProcess(void)
{
	u8 k, s, i = 0; // 临时变量
	u16 j;
	u16 *p_wRead;

	// 1. LORA接收数据存储  LORA主机 或者 LORA从机
	if (B_LoRaRcv == 0 && B_LoRaSendTest == 1) // F_LoRaSendNext==3
	{
		while (Radio->Process() == RF_RX_DONE)
		{
			Radio->GetRxPacket(LoRaRxBuf, (uint16_t *)&BufferSize);
			// ZCL 2018.11.9 RX用
			// if( strncmp( ( const char* )LoRaRxBuf, ( const char* )MY_TEST_Msg, 18 ) == 0 )
			//{			//ZCL 2018.11.9 RX用

			// ZCL 2018.12.14 Pw_LoRaSet .1位 =1   LORA输出到COM2 (透传使用)
			if (Pw_LoRaSet & 0x0002)
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
				USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
				USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
			}

			// else
			if (1)
			{
				// 把LoRa接收到的数据存到LoRaRxBuf，进行处理
				for (i = 0; i < BufferSize; i++)
				{
					LoRaRxBuf[i] = LoRaRxBuf[i];
				}
				Cw_LoRaRx = BufferSize;
				Cw_BakLoRaRx = Cw_LoRaRx; // 把 Cw_LoRaRx 保存

				// ZCL 2018.12.14 B_LoRaRcv=1,进行处理
				if (Cw_BakLoRaRx > 0)
					B_LoRaRcv = 1;
			}

			LED0_TOGGLE;

			/*
							//送到串口1
							for(i=0;i < BufferSize;i++)
							{
								USART_putchar(USART1,LoRaRxBuf[i]);
							}
							printf("\n");
							LED0_TOGGLE;
							//清空LoRaRxBuf
							for(i=0;i<BUFFER_SIZE;i++)
								LoRaRxBuf[i] = 0;

						}			//ZCL 2018.11.9 RX用

						*/

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
					F_LoRaNoRcvMasterCMD = 0;
				}
				else
					i = 1;

				// ZCL 2018.12.14   从机的返回（接收到主机命令的返回）
				LoRa_SlaveSend();
			}
			else
				i = 2;
		}

		// LORA主机 接收到数据 作为主模块(主机接收到的) 2018.12.14
		// LORA子模块1返回-->给主机的内容			=3.4.5.6 扩展从机（只有73指令）
		else if (LoRaTxBuf[0] == Pw_LoRaModule1Add) // 从地址检测
		{
			j = CRC16(LoRaTxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaTxBuf[Cw_BakLoRaRx - 2] && s == LoRaTxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaTxBuf[1] == 73) // 73 跟子模块通讯
				{
					/*ZCL 2017.6.18 切换页面时数据有跳动，仿真发现前面一页的数据返回来有延时，传送到这一页上，
							造成数据跳动。改成返回的数据，根据前面Txd1Buffer的发送地址来赋值，现在好了！ OK*/

					// 1. 转化地址 ZCL 2018.12.14
					j = LoRaTxBuf[2];
					Lw_Reg = (j << 8) + LoRaTxBuf[3];

					p_wRead = AddressChange();

					// 2. 接收到数据存储 ZCL 2018.12.14
					for (i = 0; i < LoRaRxBuf[2] / 2; i++)
					{
						j = LoRaRxBuf[3 + i * 2];
						*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2];
					}
					//
					F_LoRaRCVCommandOk = 1;
					T_LoRaCount1Err = 100;
					C_LoRaCount1Err = 0;
					B_LoRaErrWithModule1 = 0;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块2返回-->给主机的内容
		else if (LoRaTxBuf[0] == Pw_LoRaModule2Add) // 从地址检测
		{
			j = CRC16(LoRaTxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaTxBuf[Cw_BakLoRaRx - 2] && s == LoRaTxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaTxBuf[1] == 73) // 73 跟子模块通讯
				{
					// 1. 转化地址 ZCL 2018.12.14
					j = LoRaTxBuf[2];
					Lw_Reg = (j << 8) + LoRaTxBuf[3];

					p_wRead = AddressChange();

					// 2. 接收到数据存储 ZCL 2018.12.14
					for (i = 0; i < LoRaRxBuf[2] / 2; i++)
					{
						j = LoRaRxBuf[3 + i * 2];
						*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // ZCL123 这里的地址还需要改改！
					}

					F_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount2Err = 100;
					C_LoRaCount2Err = 0;
					B_LoRaErrWithModule2 = 0;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块3返回-->给主机的内容
		else if (LoRaTxBuf[0] == Pw_LoRaModule3Add) // 从地址检测
		{
			j = CRC16(LoRaTxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaTxBuf[Cw_BakLoRaRx - 2] && s == LoRaTxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaTxBuf[1] == 73) // 73 跟子模块通讯
				{
					// 1. 转化地址 ZCL 2018.12.14
					j = LoRaTxBuf[2];
					Lw_Reg = (j << 8) + LoRaTxBuf[3];

					p_wRead = AddressChange();

					// 2. 接收到数据存储 ZCL 2018.12.14
					for (i = 0; i < LoRaRxBuf[2] / 2; i++)
					{
						j = LoRaRxBuf[3 + i * 2];
						*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // ZCL123 这里的地址还需要改改！
					}

					F_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount3Err = 100;
					C_LoRaCount3Err = 0;
					B_LoRaErrWithModule3 = 0;
				}
			}
		}

		//  LORA主机 作为主模块(主机接收到的) 2018.12.14
		// 子模块4返回-->给主机的内容
		else if (LoRaTxBuf[0] == Pw_LoRaModule4Add) // 从地址检测
		{
			j = CRC16(LoRaTxBuf, Cw_BakLoRaRx - 2); // CRC 校验
			k = j >> 8;								// ZCL 注意
			s = j;
			if (k == LoRaTxBuf[Cw_BakLoRaRx - 2] && s == LoRaTxBuf[Cw_BakLoRaRx - 1])
			{							// CRC校验正确
				if (LoRaTxBuf[1] == 73) // 73 跟子模块通讯
				{
					// 1. 转化地址 ZCL 2018.12.14
					j = LoRaTxBuf[2];
					Lw_Reg = (j << 8) + LoRaTxBuf[3];

					p_wRead = AddressChange();

					// 2. 接收到数据存储 ZCL 2018.12.14
					for (i = 0; i < LoRaRxBuf[2] / 2; i++)
					{
						j = LoRaRxBuf[3 + i * 2];
						*(p_wRead + i) = (j << 8) + LoRaRxBuf[4 + i * 2]; // ZCL123 这里的地址还需要改改！
					}

					F_LoRaRCVCommandOk = 1;
					//

					T_LoRaCount4Err = 100;
					C_LoRaCount4Err = 0;
					B_LoRaErrWithModule4 = 0;
				}
			}
		}

		else
			i = 3;
	}

	// 3. 延时后再次接收
	if (B_LoRaRcv == 2)
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
}

void LoRa_SlaveSend(void) // 串口2从机发送
{
	u16 m, n;
	u8 j = 0, k;
	u16 *p_wRead;
	u8 *p_bMove;
	u8 *p_bGen;
	u16 *p_wTarget; // 指向目标字符串　xdata zcl

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
		Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax); // RFLR_STATE_TX_INIT
	}

	else if (B_LoRaCmd16 || B_LoRaCmd06) // 16预置多寄存器
	{
		j = 1;

		// ZCL123 2018.12.14   内容不全，继续加其他内容 --------------------------------

		// ZCL 2018.12.14 这里进行地址合成
		j = LoRaRxBuf[2];
		Lw_Reg = (j << 8) + LoRaRxBuf[3];
		// ZCL 2018.12.14 地址转换
		p_wRead = AddressChange();

		B_LoRaCmd16 = 0;
		B_LoRaCmd06 = 0;
		Cw_LoRaTx = 0;
		// ZCL 2018.12.14 发送
		Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax); // RFLR_STATE_TX_INIT
	}

	else if (B_LoRaCmd73) // 73
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
			// m=*(p_wRead+Lw_Reg+k);			//ZCL 2019.1.23 错误
			m = *(p_wRead + k);
			*(p_bMove + 3 + k * 2) = m >> 8;
			*(p_bMove + 3 + k * 2 + 1) = m;
		}
		Lw_LoRaChkSum = CRC16(LoRaTxBuf, LoRaTxBuf[2] + 3);
		LoRaTxBuf[LoRaTxBuf[2] + 3] = Lw_LoRaChkSum >> 8; // /256
		LoRaTxBuf[LoRaTxBuf[2] + 4] = Lw_LoRaChkSum;	  // 低位字节

		Cw_LoRaTxMax = LoRaTxBuf[2] + 5;
		B_LoRaCmd73 = 0;
		Cw_LoRaTx = 0;
		// ZCL 2018.12.14 发送
		Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax); // RFLR_STATE_TX_INIT
	}
}

// 作为主模块时程序 2016.4.1
void LoRa_MasterSend(void) // 串口2主发送程序
{
	u8 i = 0, j = 0, m = 0;

	if (Pw_LoRaModule1Add > 0)
		i++; // 统计发送数量
	if (Pw_LoRaModule2Add > 0)
		i++;
	if (Pw_LoRaModule3Add > 0)
		i++;
	if (Pw_LoRaModule4Add > 0)
		i++;
	//
	if (i > 10) // 必须有子模块或者变频器，流量计才能进入发送程序
	{
		if (F_LoRaSendNext == 1)
		{
			// 1. 控制发送步骤
			S_LoRaMSend++;
			if (S_LoRaMSend > 4)
				S_LoRaMSend = 1; // COM_UNIT_NUM

			// 2.进行顺序发送
			if (Pw_LoRaModule1Add > 0 && S_LoRaMSend == 1)
			{
				LoRaTxBuf[0] = Pw_LoRaModule1Add; // 设备从地址
				j = 1;
			}
			else if (Pw_LoRaModule2Add > 0 && S_LoRaMSend == 2)
			{
				LoRaTxBuf[0] = Pw_LoRaModule2Add; // 设备从地址
				j = 2;
			}
			else if (Pw_LoRaModule3Add > 0 && S_LoRaMSend == 3)
			{
				LoRaTxBuf[0] = Pw_LoRaModule3Add; // 设备从地址
				j = 3;
			}
			else if (Pw_LoRaModule4Add > 0 && S_LoRaMSend == 4)
			{
				LoRaTxBuf[0] = Pw_LoRaModule4Add; // 设备从地址
				j = 4;
			}

			if (j > 0) // LORA 发往子模块
			{
				// LoRaTxBuf[0]=Pw_EquipmentNo+S_LoRaMSend;
				LoRaTxBuf[1] = 73;	 // 功能码 73=0x49
				LoRaTxBuf[2] = 0x75; // 开始地址（高位）  30016: w_dspParLst[16]	//.2 A相电流
				LoRaTxBuf[3] = 0x40; // 开始地址（低位）
				LoRaTxBuf[4] = 0x00; // 读取个数（高位）
				LoRaTxBuf[5] = 0x0A; // 读取个数（低位） 10个，可以修改数量

				Lw_LoRaChkSum = CRC16(LoRaTxBuf, 6);
				LoRaTxBuf[6] = Lw_LoRaChkSum >> 8; // /256
				LoRaTxBuf[7] = Lw_LoRaChkSum;	   // 低位字节
				Cw_LoRaTxMax = 8;
				Cw_LoRaTx = 0;

				F_LoRaSendNext = 2; // 清除标志

				// ZCL 2018.12.15 发送
				Radio->SetTxPacket(LoRaTxBuf, Cw_LoRaTxMax); // RFLR_STATE_TX_INIT
			}
		}

		// 3.延时操作		// 发送结束
		// 判断接收正常后－延时发送下一条 6MS
		// ZCL 2018.12.15 无线速度慢，延时长 100MS
		if (F_LoRaRCVCommandOk) // 接收正常延时后，发送下一条
		{
			if (T_LoRaCountNormalNext != SClk10Ms)
			{
				T_LoRaCountNormalNext = SClk10Ms; //
				C_LoRaCountNormalNext++;
				if (C_LoRaCountNormalNext > 10) //
				{
					F_LoRaRCVCommandOk = 0;
					T_LoRaCountNormalNext = 100;
					C_LoRaCountNormalNext = 0;
					T_LoRaCount1OvertimeNext = 100;
					C_LoRaCount1OvertimeNext = 0;
					F_LoRaSendNext = 1; //
				}
			}
		}

		// 发送完但发送不正确，没有F_LoRaRCVCommandOk标志，延时发送下一条

		// ZCL 2019.1.24 备注
		/* 		F_LoRaSendNext=1;//发送
				F_LoRaSendNext=2;//等待发送完成
				F_LoRaSendNext=3;//进入接收状态 */
		// 2.2S没有接收到内容，进入发送状态。

		// 4.1 LORA正常发送完，进入接收状态
		// 4.2 LORA发送，但没有检测到RF_TX_DONE，延时退出让F_LoRaSendNext=3;
		if (F_LoRaSendNext == 2)
		{
			// 4.1 LORA发送完，进入接收状态
			while (Radio->Process() == RF_TX_DONE)
			{
				F_LoRaSendNext = 3;

				LCD_DLY_ms(1); // ZCL 2018.11.12
				// ZCL 2018.11.13 发送完成启动接收
				Radio->StartRx(); // RFLR_STATE_RX_INIT
				// LCD_DLY_ms(100);				//ZCL123 2018.11.12

				T_LoRaSendOverTime = 100;
				C_LoRaSendOverTime = 0;
			}

			// 4.2 LORA发送(F_LoRaSendNext==2)，但没有检测到RF_TX_DONE，延时退出让F_LoRaSendNext=3;
			// 没有while(Radio->Process( ) == RF_TX_DONE )	，进行延时
			if (T_LoRaSendOverTime != SClk10Ms)
			{
				T_LoRaSendOverTime = SClk10Ms; //
				C_LoRaSendOverTime++;
				if (C_LoRaSendOverTime > 400) //
				{
					T_LoRaSendOverTime = 100;
					C_LoRaSendOverTime = 0;

					F_LoRaSendNext = 3; // 这里延时让 F_LoRaSendNext=3，退出此状态。启动接收

					LCD_DLY_ms(1); // ZCL 2018.11.12
					// ZCL 2018.11.13 发送完成启动接收
					Radio->StartRx(); // RFLR_STATE_RX_INIT
				}
			}
		}

		// ZCL 2019.1.24 备注
		/* 		F_LoRaSendNext=1;//发送
				F_LoRaSendNext=2;//等待发送完成
				F_LoRaSendNext=3;//进入接收状态 */
		// 2.2S没有接收到内容，进入发送状态。
		// 4.3发送完成，延时进行再次发送。
		if (F_LoRaSendNext == 3)
		{
			if (T_LoRaCount1OvertimeNext != SClk10Ms)
			{
				T_LoRaCount1OvertimeNext = SClk10Ms; //
				C_LoRaCount1OvertimeNext++;
				if (C_LoRaCount1OvertimeNext > 220) //
				{
					T_LoRaCount1OvertimeNext = 100;
					C_LoRaCount1OvertimeNext = 0;
					F_LoRaSendNext = 1; // =1发送
				}
			}
		}

		// B_LoRaSendEnd=S_LoRaMSend;
		//}

		//----------------------------------------------------------------
		// 6. 判断超时
		// 6.1判断超时－延时通讯故障 4000MS   与子模块1通讯失败 // 6000
		if (Pw_LoRaModule1Add > 0 && !B_LoRaErrWithModule1) // 发送完进行计时　时间到故障
		{
			if (T_LoRaCount1Err != SClk10Ms)
			{
				T_LoRaCount1Err = SClk10Ms; //
				C_LoRaCount1Err++;
				if (C_LoRaCount1Err > 1000) //
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
				if (C_LoRaCount2Err > 1000) //
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
				if (C_LoRaCount3Err > 1000) //
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
				if (C_LoRaCount4Err > 1000) //
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
	}

	// ZCL123 2019.1.24 测试串口发送到LORA
	if (B_LoRaSendTest == 2)
	{
		B_LoRaSendTest = 3;
		// ZCL 2018.11.12
		// Radio->SetTxPacket( MY_TEST_Msg, strlen(MY_TEST_Msg) );   //RFLR_STATE_TX_INIT
		Radio->SetTxPacket(LoRaTxBuf, 10); // RFLR_STATE_TX_INIT
	}

	// printf("运行在RF_LoRa_TX_OK？ \n");
	if (B_LoRaSendTest == 3)
	{
		while (Radio->Process() == RF_TX_DONE)
		{
			printf("RF_LoRa_TX_OK! ZCL1! CAN DEL \n");
			LED0_TOGGLE;
			// Soft_delay_ms(1000);	//ZCL 2018.11.12
			// LCD_DLY_ms(1000);				//ZCL 2018.11.12

			// ZCL 2018.11.13 B_LoRaSendTest=1,延时后发送
			B_LoRaSendTest = 1;

			LCD_DLY_ms(1); // ZCL 2018.11.12 LCD_DLY_ms(10);
			// ZCL 2018.11.13 发送完成启动接收
			Radio->StartRx(); // RFLR_STATE_RX_INIT
			B_LoRaRcv = 1;
		}
	}
}

/* ZCL 说明： 2018.12.20
	Pw_ScrLoRaTest1==10 测试发送模式
	Pw_ScrLoRaTest1==11 测试接收模式
	Pw_ScrLoRaTest1==1	透传模式： LoRa 跟 COM2
	COM2中赋值，当接收到数据，让B_LoRaSend=2，然后这里可以LoRa发送
	注意发送完，需要点延时，才能再次发送？ 因为接收需要花时间，还是没有发送完？
*/
void LoRaSendTest(void) // LoRa 发送测试
{
	// u8	i;

	// ZCL 2018.11.12 LORA SX1278 调试
	// SX1278 TX	测试
	if (Pw_ScrLoRaTest1 == 10) //=10 测试发送模式
	{
		if (B_LoRaSendTest == 0)
		{
			Radio->SetTxPacket(MY_TEST_Msg, 18);

			B_LoRaSendTest = 2; //=2,立即发送
		}

		if (B_LoRaSendTest == 1 && T_LoRaSendTest != SClk1Ms)
		{
			T_LoRaSendTest = SClk1Ms; //
			C_LoRaSendTest++;
			if (C_LoRaSendTest > 1000)
			{
				T_LoRaSendTest = 100;
				C_LoRaSendTest = 0;
				//
				B_LoRaSendTest = 2;
			}
		}

		if (B_LoRaSendTest == 2)
		{
			B_LoRaSendTest = 3;
			// ZCL 2018.11.12
			// Radio->SetTxPacket( MY_TEST_Msg, strlen(MY_TEST_Msg) );   //RFLR_STATE_TX_INIT
			Radio->SetTxPacket(LoRaTxBuf, 30); // RFLR_STATE_TX_INIT
		}

		// printf("运行在RF_LoRa_TX_OK？ \n");
		while (Radio->Process() == RF_TX_DONE && B_LoRaSendTest == 3)
		{
			printf("RF_LoRa_TX_OK! ZCL1! CAN DEL \n");
			LED0_TOGGLE;
			// Soft_delay_ms(1000);	//ZCL 2018.11.12
			// LCD_DLY_ms(1000);				//ZCL 2018.11.12

			// ZCL 2018.11.13 B_LoRaSendTest=1,延时后发送
			B_LoRaSendTest = 1;
		}
	}

	//=11 接收测试
	else if (Pw_ScrLoRaTest1 == 11)
	{
		// 2. SX1278 RX接收
		LoRaRcvTest();
	}

	//=1 透传模式
	else if (Pw_ScrLoRaTest1 == 1)
	{
		// 1. SX1278 TX发送		COM2中赋值，当接收到数据，让B_LoRaSend=2，然后这里可以LoRa发送
		if (B_LoRaSendTest == 2)
		{
			B_LoRaSendTest = 3;
			// ZCL 2018.11.12
			// Radio->SetTxPacket( MY_TEST_Msg, strlen(MY_TEST_Msg) );   //RFLR_STATE_TX_INIT
			Radio->SetTxPacket(LoRaTxBuf, Cw_BakRcv2); // RFLR_STATE_TX_INIT
		}

		// printf("运行在RF_LoRa_TX_OK？ \n");
		if (B_LoRaSendTest == 3)
		{
			while (Radio->Process() == RF_TX_DONE)
			{
				printf("RF_LoRa_TX_OK! ZCL1! CAN DEL \n");
				LED0_TOGGLE;
				// Soft_delay_ms(1000);	//ZCL 2018.11.12
				// LCD_DLY_ms(1000);				//ZCL 2018.11.12

				// ZCL 2018.11.13 B_LoRaSendTest=1,延时后发送
				B_LoRaSendTest = 1;

				LCD_DLY_ms(1); // ZCL 2018.11.12 LCD_DLY_ms(10);
				// ZCL 2018.11.13 发送完成启动接收
				Radio->StartRx(); // RFLR_STATE_RX_INIT
				B_LoRaRcv = 1;
			}
		}

		else
			// 2. SX1278 RX接收
			LoRaRcvTest();
	}
}

void LoRaRcvTest(void) // LoRa 接收测试
{
	u8 i;
	// 2. SX1278 RX接收
	// 2.1 延时
	if (B_LoRaRcv == 1 && T_LoRaRcv != SClk1Ms)
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
			// if( strncmp( ( const char* )LoRaRxBuf, ( const char* )MY_TEST_Msg, 18 ) == 0 )		//ZCL 2018.11.9 RX用
			//{			//ZCL 2018.11.9 RX用

			// 把LoRa接收到的数据发送到串口2		ZCL 2018.11.13 OK
			for (i = 0; i < BufferSize; i++)
			{
				Txd2Buffer[i] = LoRaRxBuf[i];
			}
			// Txd2Buffer[BufferSize]=0x0D;    //ZCL 2019.1.24 注释掉
			// Txd2Buffer[BufferSize+1]=0x0A;
			Cw_Txd2Max = BufferSize + 2;
			Cw_Txd2 = 0;
			// RS485_CON=1;
			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
			USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
			USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);

			LED0_TOGGLE;
			// 清空缓存
			for (i = 0; i < BUFFER_SIZE; i++)
				LoRaRxBuf[i] = 0;

			/* 				for(i=0;i < BufferSize;i++)
							{
								USART_putchar(USART1,LoRaRxBuf[i]);
							}
							printf("\n");
							LED0_TOGGLE;
							for(i=0;i<BUFFER_SIZE;i++)
								LoRaRxBuf[i] = 0;
						}			//ZCL 2018.11.9 RX用

						Soft_delay_ms(35);		//ZCL 2018.11.10  2,5,7不行   8,20,15慢1次，25偶尔丢次，35，30,50,200可以
						LCD_DLY_ms(35);				//ZCL 2018.11.12

						*/

			// ZCL 2018.11.13 B_LoRaSendTest=1,延时后接收
			B_LoRaRcv = 1;
		}
	}
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
