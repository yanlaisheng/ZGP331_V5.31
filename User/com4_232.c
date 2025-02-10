/**
  ******************************************************************************
  * @file    com4_485.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.10
  * @date    2015-04-13
  * @brief   Com1 USE UART4
	******************************************************************************
	* @note	2015.04.13
		COM4: GPS ATGM336H
	*/

/* Includes ------------------------------------------------------------------*/
#include "com4_232.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include <stdio.h>	//加上此句可以用printf
#include "string.h" //strstr函数：找出str2字符串在str1字符串中第一次出现的位置

#include "spi_flash.h" //ZCL 2020.3.26
#include "com1_232.h"  //ZCL 2020.3.26

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char Txd4Buffer[TXD4_MAX]; // 发送缓冲区
char Rcv4Buffer[RCV4_MAX]; // 接收缓冲区
u16 Cw_Rcv4;			   // 接收计数器//
u16 Cw_Txd4;			   // 发送计数器//
u16 Cw_BakRcv4;			   // 接收计数器//
u16 Cw_Txd4Max;			   // 有多少个字符需要发送//
u16 Lw_Txd4ChkSum;		   // 发送校验和，lo,hi 两位//
u16 Lw_Com4RegAddr;		   // 串口1寄存器地址
//
u8 B_Com4Cmd03;
u8 B_Com4Cmd16;
u8 B_Com4Cmd01;
u8 B_Com4Cmd06;
u8 B_Com4Cmd73;
u8 T_NoRcv4; // 没有接收计数器
u16 C_NoRcv4;

u8 B_SaveLatLon; // ZCL 2019.9.11  定时保存，经纬度

u8 *Gps_ptr, *Gps_Gps_ptr2, *Gps_Gps_ptr3; // 指针
u8 GPS_StringBuffer[12];				   // BCD码转成字符串缓存。

/* Private variables extern --------------------------------------------------*/
extern u16 C_ForceSavPar; // 强制保存参数计数器
extern u8 F_ModDspPar;	  // 修改DSP参数

extern u8 Txd2Buffer[TXD1_MAX]; // 发送缓冲区
extern u8 Rcv2Buffer[RCV1_MAX]; // 接收缓冲区
extern u16 Cw_Rcv2;				// 接收计数器//
extern u16 Cw_Txd2;				// 发送计数器//
extern u16 Cw_BakRcv2;			// 接收计数器//
extern u16 Cw_Txd2Max;			// 有多少个字符需要发送//

/* Private function prototypes -----------------------------------------------*/
void GPIO_Com4_Configuration(void); // GPIO配置
void Com4_config(void);
u16 CRC16(u8 *pCrcData, u8 CrcDataLen);
void Delay_MS(vu16 nCount);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : GPIO_Com4_Configuration
 * Description    : Configures the different GPIO ports.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Com4_Configuration(void) // GPIO配置
{
	GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体

	// 串口说明：2个串口分别是Usart1.Usart3(USART口下标从1开始,跟STM32F103 ARM寄存器兼容)
	// Com4 实际为第一个串口
	/* Configure 232  */ // RS232没有控制端
	/* Configure Com4(使用硬件:UART4) Rx (PC.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure Com4(使用硬件:UART4) Tx (PC.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// GPIO_ResetBits(GPIOC, GPIO_Pin_10);
}

/* UART4  configuration ------------------------------------------------------*/
/* USART  configured as follow:
		- BaudRate = 9600 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
				RTS/CTS协议(Request To Send/Clear To Send)即请求发送/允许发送协议
*/
void Com4_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_DeInit(UART4);					// 复位 Usart1
	USART_StructInit(&USART_InitStructure); // 把USART_InitStruct中的每一个参数按缺省值填入

	USART_InitStructure.USART_BaudRate = 9600; // 2.25M/BPS   RS232芯片:230.4kbps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure 串口1-UART4 */
	USART_Init(UART4, &USART_InitStructure);
	/* Enable UART4 Receive and Transmit interrupts */
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	/* Enable the UART4 */
	USART_Cmd(UART4, ENABLE);
}

/*******************************************************************************
 * Function Name  : Com4_Init
 * Description    : Initializes the peripherals used by the Usart driver.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Com4_Init(void)
{
	GPIO_Com4_Configuration();
	Com4_config();
	//
	Cw_Txd4 = 0;
	Cw_Rcv4 = 0;
}

// 接收处理程序 校验程序
void Com4_RcvProcess(void)
{
	u8 i = 0, k, m, n; // 临时变量 k,
	u8 u = 0;
	//	u8 v=0,w=0;
	u16 j;
	float nf_Du; // ZCL 2018.10.28

	// 作为主机,指定接收时间到了,就可以处理接收到的字符串了
	//  在没收到串行字符的时间超过设定时，可以对接收缓存进行处理了
	//  **********************************rcv_counter<>0,收到字符才能处理
	if (Cw_Rcv4 > 0 && T_NoRcv4 != SClk1Ms)
	{ // 接收处理过程
		// RS485_CON=0;
		T_NoRcv4 = SClk1Ms; //
		C_NoRcv4++;
		// 1.时间校验
		if (C_NoRcv4 > NORCVMAXMS) //
		{
			//
			USART_ITConfig(UART4, USART_IT_RXNE, DISABLE); // 禁止UART1串口中断,防止在处理期间更改
			Cw_BakRcv4 = Cw_Rcv4;						   // 把 Cw_Rcv4 保存
			C_NoRcv4 = 0;								   // 清没有接收计数器
			//
			// if(Cw_BakRcv4<=RCV4_MAX)		// 接收长度正确,继续处理.  ZCL123 2018.10.26
			// 2. 接收数量校验
			if (Cw_BakRcv4 > 10)
			{
				// 3. 起始符'$'  伪结束符'*' （后面还有异或校验码2字节和 0x0d  0x0a）
				if (Rcv4Buffer[0] == '$' && Rcv4Buffer[Cw_BakRcv4 - 5] == '*' && Rcv4Buffer[Cw_BakRcv4 - 2] == 0x0d && Rcv4Buffer[Cw_BakRcv4 - 1] == 0x0a)
				{
					j = CalcCheck((u8 *)Rcv4Buffer, Cw_BakRcv4 - 5); // CRC 校验
					m = (j % 256) / 16;
					if (m >= 10) // A-F(A:0x41)
						m = m - 10 + 0x41;
					else // 0-9
						m = m + 0x30;

					n = (j % 16);
					if (n >= 10) // A-F(A:0x41)
						n = n - 10 + 0x41;
					else // 0-9
						n = n + 0x30;
					// 4. 异或码校验
					if (m == Rcv4Buffer[Cw_BakRcv4 - 4] && n == Rcv4Buffer[Cw_BakRcv4 - 3])
					{
						// 在搜不到星的时候，返回,,,,,等
						if (strstr((char *)Rcv4Buffer, ",,,") == NULL) // 正常数据的时候，搜不到,,,，返回值为0
						{
							// 示例 $GPGGA,235316.000,2959.9925,S,12000.0090,E,1,06,1.21,62.77,M,0.00,M,,*7B
							// 5.1 纬度计算填充		ZCL123  注意验证 度：第一位为0是否也发送出来？
							// Lat ddmm.mmmm 纬度，前 2 字符表示度，后面的字符表示分
							// w_ScrGpsLat				//.0 纬度 度-分；	ddmm.mmmm之ddmm

							// ZCL 2020.11.24 经纬度后4位和5位小数的问题
							if (strstr(Rcv4Buffer, ",N,") != NULL)
							{
								Gps_ptr = (u8 *)strstr(Rcv4Buffer, ",N,"); // ZCL 2020.11.24 原先4位小数点
								if (*(Gps_ptr - 5) == '.')				   // 0x2E:'.'
								{
									w_ScrGpsLatDu = (*(Gps_ptr - 9) - 0x30) * 10 + (*(Gps_ptr - 8) - 0x30);
									w_ScrGpsLatFen1 = (*(Gps_ptr - 7) - 0x30) * 10 + (*(Gps_ptr - 6) - 0x30);
									w_ScrGpsLatFen2 = (*(Gps_ptr - 4) - 0x30) * 1000 + (*(Gps_ptr - 3) - 0x30) * 100 + (*(Gps_ptr - 2) - 0x30) * 10 + (*(Gps_ptr - 1) - 0x30);
								}
								else if (*(Gps_ptr - 6) == '.') // 0x2E:'.'				//ZCL 2020.11.24 现在5位小数点
								{
									w_ScrGpsLatDu = (*(Gps_ptr - 10) - 0x30) * 10 + (*(Gps_ptr - 9) - 0x30);
									w_ScrGpsLatFen1 = (*(Gps_ptr - 8) - 0x30) * 10 + (*(Gps_ptr - 7) - 0x30);
									w_ScrGpsLatFen2 = (*(Gps_ptr - 5) - 0x30) * 1000 + (*(Gps_ptr - 4) - 0x30) * 100 + (*(Gps_ptr - 3) - 0x30) * 10 + (*(Gps_ptr - 2) - 0x30);
								}
							}
							else
							{
								w_ScrGpsLatDu = 0;
								w_ScrGpsLatFen1 = 0;
								w_ScrGpsLatFen2 = 0;
							}

							// 5.2 经度计算填充
							// Lon dddmm.mmmm 经度，前 3 字符表示度，后面的字符表示分

							// ZCL 2020.11.24 经纬度后4位和5位小数的问题
							if (strstr(Rcv4Buffer, ",E,") != NULL)
							{
								Gps_ptr = (u8 *)strstr((char *)Rcv4Buffer, ",E,"); // ZCL 2020.11.24 原先4位小数点
								if (*(Gps_ptr - 5) == '.')						   // 0x2E:'.'
								{
									w_ScrGpsLonDu = (*(Gps_ptr - 10) - 0x30) * 100 + (*(Gps_ptr - 9) - 0x30) * 10 + (*(Gps_ptr - 8) - 0x30);
									w_ScrGpsLonFen1 = (*(Gps_ptr - 7) - 0x30) * 10 + (*(Gps_ptr - 6) - 0x30);
									w_ScrGpsLonFen2 = (*(Gps_ptr - 4) - 0x30) * 1000 + (*(Gps_ptr - 3) - 0x30) * 100 + (*(Gps_ptr - 2) - 0x30) * 10 + (*(Gps_ptr - 1) - 0x30);
								}
								else if (*(Gps_ptr - 6) == '.') // 0x2E:'.'					//ZCL 2020.11.24 现在5位小数点
								{
									w_ScrGpsLonDu = (*(Gps_ptr - 11) - 0x30) * 100 + (*(Gps_ptr - 10) - 0x30) * 10 + (*(Gps_ptr - 9) - 0x30);
									w_ScrGpsLonFen1 = (*(Gps_ptr - 8) - 0x30) * 10 + (*(Gps_ptr - 7) - 0x30);
									w_ScrGpsLonFen2 = (*(Gps_ptr - 5) - 0x30) * 1000 + (*(Gps_ptr - 4) - 0x30) * 100 + (*(Gps_ptr - 3) - 0x30) * 10 + (*(Gps_ptr - 2) - 0x30);
								}
							}
							else
							{
								w_ScrGpsLonDu = 0;
								w_ScrGpsLonFen1 = 0;
								w_ScrGpsLonFen2 = 0;
							}
							i = 1;

							// 6. 纬度 经度  格式转换
							// ZCL 2019.9.12 一旦拔掉GPS天线，返回值,,,，程序走不到这里！
							// 纬度
							u = 0;
							//							v=0;
							//							w=0;
							nf_Du = (float)w_ScrGpsLatDu + (float)w_ScrGpsLatFen1 / 60 + (float)w_ScrGpsLatFen2 / 600000;

							if (nf_Du > 0.1) // ZCL 2019.9.11
							{
								w_ScrGpsLatAllDu1 = (int)(nf_Du * 100);								  // 35.44
								w_ScrGpsLatAllDu2 = (int)((nf_Du * 100 - w_ScrGpsLatAllDu1) * 10000); // 00.00****
								// ZCL 2019.9.11 如果纬度基本不变，不用保存
								if (w_ScrGpsLatAllDu1 == Pw_ScrGpsLatAllDu1)
								{
									if ((w_ScrGpsLatAllDu2 / 100) == (Pw_ScrGpsLatAllDu2 / 100)) // ZCL 2019.9.11 这个10根据变化再调整
									{
										// if(B_SaveLatLon>0)
										// B_SaveLatLon--;
										//										v=1; //v>0不需要保存，观察参数，可以删掉！主要看u参数。ZCL 2019.9.12
									}
									else
										u = 1;
								}
								else
									u = 2;
							}
							else // ZCL 2019.9.11 使用保存的值
							{
								w_ScrGpsLatAllDu1 = Pw_ScrGpsLatAllDu1;
								w_ScrGpsLatAllDu2 = Pw_ScrGpsLatAllDu2;
								// if(B_SaveLatLon>0)
								// B_SaveLatLon--;
								//								v=2; //v>0不需要保存，观察参数，可以删掉！主要看u参数。ZCL 2019.9.12
							}

							// 经度
							nf_Du = (float)w_ScrGpsLonDu + (float)w_ScrGpsLonFen1 / 60 + (float)w_ScrGpsLonFen2 / 600000;
							if (nf_Du > 0.1) // ZCL 2019.9.11
							{
								w_ScrGpsLonAllDu1 = (int)(nf_Du * 100);
								w_ScrGpsLonAllDu2 = (int)((nf_Du * 100 - w_ScrGpsLonAllDu1) * 10000); // 00.00****
								// ZCL 2019.9.11 如果经度基本不变，不用保存
								if (w_ScrGpsLonAllDu1 == Pw_ScrGpsLonAllDu1)
								{
									if ((w_ScrGpsLonAllDu2 / 100) == (Pw_ScrGpsLonAllDu2 / 100)) // ZCL 2019.9.11 这个10根据变化再调整
									{
										// if(B_SaveLatLon>0)
										// B_SaveLatLon--;
										//										v=3; //v>0不需要保存，观察参数，可以删掉！主要看u参数。ZCL 2019.9.12
									}
									else
										u = 3;
								}
								else
									u = 4;
							}
							else // ZCL 2019.9.11 使用保存的值
							{
								w_ScrGpsLonAllDu1 = Pw_ScrGpsLonAllDu1;
								w_ScrGpsLonAllDu1 = Pw_ScrGpsLonAllDu1;
								// if(B_SaveLatLon>0)
								// B_SaveLatLon--;
								//								v=4; //v>0不需要保存，观察参数，可以删掉！主要看u参数。ZCL 2019.9.12
							}

							// ZCL 2019.9.11
							// 7. 经纬度变化大立即保存。原先计划6秒钟后保存
							// 7.1
							if (u > 0) // u>0 经纬度变化大，先更新保存值
							{
								Pw_ScrGpsLatAllDu1 = w_ScrGpsLatAllDu1;
								Pw_ScrGpsLatAllDu2 = w_ScrGpsLatAllDu2;

								Pw_ScrGpsLonAllDu1 = w_ScrGpsLonAllDu1;
								Pw_ScrGpsLonAllDu2 = w_ScrGpsLonAllDu2;

								//
								SPI_FLASH_CS_LOW();
								// ZCL 2019.9.11  保存GPS 纬度和经度参数 到 FM25L16

								// #define	Pw_ScrGpsLatAllDu1				w_GprsParLst[10]  ZCL 2019.9.11 10*2的由来
								// SPI_FMRAM_BufferWrite((u8 *)(&Pw_ScrGpsLatAllDu1), 10*2, 4*2);	//测试好用！

								// ZCL 2019.9.11 如果上面不好用，用这个  ZCL测试1	测试好用！
								SPI_FMRAM_BufferWrite((u8 *)(&Pw_ScrGpsLatAllDu1), Position(w_GprsParLst, &Pw_ScrGpsLatAllDu1) * 2, 4 * 2);

								// ZCL 2019.9.11 如果上面不好用，用这个  ZCL测试2
								// SPI_FMRAM_BufferWrite((u8 *)(&w_GprsParLst), 0, 14*2);
							}
							// ZCL 2022.5.22  经纬度 填充到 电能表1数据区
							w_DNBScrGpsLatAllDu1 = w_ScrGpsLatAllDu1;
							w_DNBScrGpsLatAllDu2 = w_ScrGpsLatAllDu2;

							w_DNBScrGpsLonAllDu1 = w_ScrGpsLonAllDu1;
							w_DNBScrGpsLonAllDu2 = w_ScrGpsLonAllDu2;
						}
						else
							i = 2;
					}
					else
						i = 3;
				}
				else
					i = 4;
			}
			else
				i = 5;

			// ZCL 2018.12.6
			if (i > 0)
			{
				if (F_GpsToCom) // GPS 透传数据输出到COM2  .4位 值是16
				{
					for (k = 0; k < Cw_BakRcv4; k++)
					{
						Txd2Buffer[k] = Rcv4Buffer[k]; // GPS数据发送到串口2
					}

					// ZCL 2018.12.6 i==2是还要清除所有缓存区
					if (i == 2)
					{
						for (k = 0; k < RCV4_MAX; k++)
						{
							Rcv4Buffer[k] = 0; // GPS数据发送到串口2
						}
					}

					// 准备发送
					Cw_Txd2Max = Cw_BakRcv4;
					Cw_Txd2 = 0;

					// 2010.8.5 周成磊 TXE改成TC，一句改为两句
					// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
					USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
					USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
				}
				else
				{
					// ZCL 2018.12.6 i==2是还要清除所有缓存区
					if (i == 2)
					{
						for (k = 0; k < RCV4_MAX; k++)
						{
							Rcv4Buffer[k] = 0; // GPS数据发送到串口2
						}
						i = 0; // ZCL 2019.6.25
					}
				}
			}

			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
			Cw_Rcv4 = 0; // 准备下次接收到缓存开始
		}
	}

	// ZCL 2018.10.27 清除接收缓存
	if (i > 0)
	{
		for (j = 0; j < 20; j++)
		{
			Rcv4Buffer[j] = 0;
		}
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

		Cw_Rcv4 = 0; // 准备下次接收到缓存开始  2019.6.25
	}
}

void Com4_SlaveSend(void) // 串口2从机发送
{
	u16 m, n;
	u8 j = 0, k;
	u16 *p_wRead;
	u8 *p_bMove;
	u8 *p_bGen;
	u16 *p_wTarget; // 指向目标字符串　xdata zcl

	if (Pw_EquipmentType == 0) // 双驱泵
	{
		//
		if (B_Com4Cmd03) // 读取保持寄存器
		{
			Txd4Buffer[0] = Pw_LoRaEquipmentNo; // 设备从地址Pw_EquipmentNo
			Txd4Buffer[1] = Rcv4Buffer[1];		// 功能码
			Txd4Buffer[2] = Rcv4Buffer[5] * 2;	// Rcv4Buffer[5]=字数 　

			// 转化地址 ZCL 2015.7.11

			p_wRead = AddressChange_2(Lw_Com4RegAddr);

			p_bMove = (u8 *)Txd4Buffer;
			for (k = 0; k < Rcv4Buffer[5]; k++) // 填充查询内容
			{
				// m = *(p_wRead + Lw_Com4RegAddr + k);	//2023.12.16 YLS
				m = *(p_wRead + k); // 2023.12.16 YLS 在AddressChange_2(Lw_Com4RegAddr)函数中已经加上了Lw_Com4RegAddr这个偏移量
				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
			Lw_Txd4ChkSum = CRC16((u8 *)Txd4Buffer, Txd4Buffer[2] + 3);
			Txd4Buffer[Txd4Buffer[2] + 3] = Lw_Txd4ChkSum >> 8; // /256
			Txd4Buffer[Txd4Buffer[2] + 4] = Lw_Txd4ChkSum;		// 低位字节
			Cw_Txd4Max = Txd4Buffer[2] + 5;
			//
			//
			B_Com4Cmd03 = 0;
			Cw_Txd4 = 0;
			// RS485_CON=1;
			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
			// USART_ITConfig(UART4, USART_IT_TXE, ENABLE);				// 开始发送.
			USART_SendData(UART4, Txd4Buffer[Cw_Txd4++]);
			USART_ITConfig(UART4, USART_IT_TC, ENABLE); // 开始发送.
		}
		//
		else if (B_Com4Cmd16 || B_Com4Cmd06) // 16预置多寄存器
		{

			j = 1;

			// 修改参数单元
			if (j)
			{
				// 转化地址 ZCL 2015.7.11
				if (Lw_Com4RegAddr < STM32_RPAR_ADDRESS)
				{
					p_wTarget = Pw_ParLst;						   // 设定STM32 PAR区
					Lw_Com4RegAddr = Lw_Com4RegAddr - DNB_ADDRESS; //  2023.12.16 YLS
				}
				else if (Lw_Com4RegAddr < DSP_WRPAR_ADDRESS)
				{
					p_wTarget = w_ParLst; // 读STM32 PAR区
					Lw_Com4RegAddr = Lw_Com4RegAddr - STM32_RPAR_ADDRESS;
				}
				else if (Lw_Com4RegAddr < DSP_RPAR_ADDRESS) // 原先有修改参数标志，不再保存
				{
					p_wTarget = Pw_dspParLst; // 设定DSP PAR区
					Lw_Com4RegAddr = Lw_Com4RegAddr - DSP_WRPAR_ADDRESS;
					// 表示要修改DSP参数，需要在COM1中转发这个指令  ZCL 2015.7.11
					if (!F_ModDspPar)
						F_ModDspPar = 1;
					else // 原先有修改参数标志，则清除此次的修改指令。
					{
						B_Com4Cmd16 = 0;
						B_Com4Cmd06 = 0;
					}
				}
				/* 			else
							{
								p_wTarget=w_dspParLst;			// 读DSP PAR区
								Lw_Com4RegAddr=Lw_Com4RegAddr-DSP_RPAR_ADDRESS;
							}				 */

				if (B_Com4Cmd06) // 预置单个
				{
					m = Rcv4Buffer[4];
					m = (m << 8) + Rcv4Buffer[5];
					*(p_wTarget + Lw_Com4RegAddr) = m;

					// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
					if (F_ModDspPar)
					{
						w_ModDspParAdd = Lw_Com4RegAddr;
						w_ModDspParValue = m;
					}
				}

				else if (B_Com4Cmd16) // 预置多个
				{
					if (Rcv4Buffer[5] < 90)
					{
						p_bGen = (u8 *)Rcv4Buffer;
						for (k = 0; k < Rcv4Buffer[5]; k++) // Rcv4Buffer[5]=字数
						{
							m = *(p_bGen + 7 + k * 2);
							n = *(p_bGen + 7 + k * 2 + 1);
							m = (m << 8) + n;
							*(p_wTarget + Lw_Com4RegAddr + k) = m;
						}

						// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
						// 这里暂时只适合预置一个DSP参数  ZCL  2015.7.11
						if (F_ModDspPar)
						{
							w_ModDspParAdd = Lw_Com4RegAddr;
							w_ModDspParValue = m;
						}
					}
				}
			}

			// -------------------------
			// 返回数据
			// 2015.7.11 加入判断，因为设定DSP参数时，如果原先已经有设定在运行，丢弃。
			if (B_Com4Cmd16 || B_Com4Cmd06)
			{
				Txd4Buffer[0] = 2;			   // 设备从地址
				Txd4Buffer[1] = Rcv4Buffer[1]; // 功能码
				Txd4Buffer[2] = Rcv4Buffer[2]; // 开始地址高位字节
				Txd4Buffer[3] = Rcv4Buffer[3]; // 开始地址低位字节
				Txd4Buffer[4] = Rcv4Buffer[4]; // 寄存器数量高位
				Txd4Buffer[5] = Rcv4Buffer[5]; // 寄存器数量低位
				if (j == 0)					   // 如果不能被正常预置，返回FFFF zcl
				{
					Txd4Buffer[4] = 0xff; // 寄存器数量高位、预置数据
					Txd4Buffer[5] = 0xff; // 寄存器数量低位、预置数据
				}
				Lw_Txd4ChkSum = CRC16((u8 *)Txd4Buffer, 6);
				Txd4Buffer[6] = Lw_Txd4ChkSum >> 8; // /256
				Txd4Buffer[7] = Lw_Txd4ChkSum;		// 低位字节
				Cw_Txd4Max = 8;
				B_Com4Cmd16 = 0;
				B_Com4Cmd06 = 0;
				Cw_Txd4 = 0;
				// RS485_CON=1;
				// 2010.8.5 周成磊 TXE改成TC，一句改为两句
				// USART_ITConfig(UART4, USART_IT_TXE, ENABLE);				// 开始发送.
				USART_SendData(UART4, Txd4Buffer[Cw_Txd4++]);
				USART_ITConfig(UART4, USART_IT_TC, ENABLE); // 开始发送.
			}
		} // 06、16预置寄存器 结束
	}
	else // 变频电机
	{
		//
		if (B_Com4Cmd03) // 读取保持寄存器
		{
			Txd4Buffer[0] = 2;				   // 设备从地址Pw_EquipmentNo
			Txd4Buffer[1] = Rcv4Buffer[1];	   // 功能码
			Txd4Buffer[2] = Rcv4Buffer[5] * 2; // Rcv4Buffer[5]=字数 　

			// 转化地址 ZCL 2015.7.11
			if (Lw_Com4RegAddr < STM32_RPAR_ADDRESS)
			{
				p_wRead = Pw_ParLst; // 设定STM32 PAR区
				Lw_Com4RegAddr = Lw_Com4RegAddr - STM32_WRPAR_ADDRESS_BPDJ;
			}
			else if (Lw_Com4RegAddr < DSP_WRPAR_ADDRESS)
			{
				p_wRead = w_ParLst; // 读STM32 PAR区
				Lw_Com4RegAddr = Lw_Com4RegAddr - STM32_RPAR_ADDRESS;
			}
			else if (Lw_Com4RegAddr < DSP_RPAR_ADDRESS)
			{
				p_wRead = Pw_dspParLst; // 设定DSP PAR区
				Lw_Com4RegAddr = Lw_Com4RegAddr - DSP_WRPAR_ADDRESS;
			}
			else
			{
				p_wRead = w_dspParLst; // 读DSP PAR区
				Lw_Com4RegAddr = Lw_Com4RegAddr - DSP_RPAR_ADDRESS;
			}

			p_bMove = (u8 *)Txd4Buffer;
			for (k = 0; k < Rcv4Buffer[5]; k++) // 填充查询内容
			{
				m = *(p_wRead + Lw_Com4RegAddr + k);
				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
			Lw_Txd4ChkSum = CRC16((u8 *)Txd4Buffer, Txd4Buffer[2] + 3);
			Txd4Buffer[Txd4Buffer[2] + 3] = Lw_Txd4ChkSum >> 8; // /256
			Txd4Buffer[Txd4Buffer[2] + 4] = Lw_Txd4ChkSum;		// 低位字节
			Cw_Txd4Max = Txd4Buffer[2] + 5;
			//
			//
			B_Com4Cmd03 = 0;
			Cw_Txd4 = 0;
			// RS485_CON=1;
			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
			// USART_ITConfig(UART4, USART_IT_TXE, ENABLE);				// 开始发送.
			USART_SendData(UART4, Txd4Buffer[Cw_Txd4++]);
			USART_ITConfig(UART4, USART_IT_TC, ENABLE); // 开始发送.
		}
		//
		else if (B_Com4Cmd16 || B_Com4Cmd06) // 16预置多寄存器
		{

			j = 1;

			// 修改参数单元
			if (j)
			{
				// 转化地址 ZCL 2015.7.11
				if (Lw_Com4RegAddr < STM32_RPAR_ADDRESS)
				{
					p_wTarget = Pw_ParLst; // 设定STM32 PAR区
					Lw_Com4RegAddr = Lw_Com4RegAddr - STM32_WRPAR_ADDRESS_BPDJ;
				}
				else if (Lw_Com4RegAddr < DSP_WRPAR_ADDRESS)
				{
					p_wTarget = w_ParLst; // 读STM32 PAR区
					Lw_Com4RegAddr = Lw_Com4RegAddr - STM32_RPAR_ADDRESS;
				}
				else if (Lw_Com4RegAddr < DSP_RPAR_ADDRESS) // 原先有修改参数标志，不再保存
				{
					p_wTarget = Pw_dspParLst; // 设定DSP PAR区
					Lw_Com4RegAddr = Lw_Com4RegAddr - DSP_WRPAR_ADDRESS;
					// 表示要修改DSP参数，需要在COM1中转发这个指令  ZCL 2015.7.11
					if (!F_ModDspPar)
						F_ModDspPar = 1;
					else // 原先有修改参数标志，则清除此次的修改指令。
					{
						B_Com4Cmd16 = 0;
						B_Com4Cmd06 = 0;
					}
				}
				/* 			else
							{
								p_wTarget=w_dspParLst;			// 读DSP PAR区
								Lw_Com4RegAddr=Lw_Com4RegAddr-DSP_RPAR_ADDRESS;
							}				 */

				if (B_Com4Cmd06) // 预置单个
				{
					m = Rcv4Buffer[4];
					m = (m << 8) + Rcv4Buffer[5];
					*(p_wTarget + Lw_Com4RegAddr) = m;

					// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
					if (F_ModDspPar)
					{
						w_ModDspParAdd = Lw_Com4RegAddr;
						w_ModDspParValue = m;
					}
				}

				else if (B_Com4Cmd16) // 预置多个
				{
					if (Rcv4Buffer[5] < 90)
					{
						p_bGen = (u8 *)Rcv4Buffer;
						for (k = 0; k < Rcv4Buffer[5]; k++) // Rcv4Buffer[5]=字数
						{
							m = *(p_bGen + 7 + k * 2);
							n = *(p_bGen + 7 + k * 2 + 1);
							m = (m << 8) + n;
							*(p_wTarget + Lw_Com4RegAddr + k) = m;
						}

						// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
						// 这里暂时只适合预置一个DSP参数  ZCL  2015.7.11
						if (F_ModDspPar)
						{
							w_ModDspParAdd = Lw_Com4RegAddr;
							w_ModDspParValue = m;
						}
					}
				}
			}

			// -------------------------
			// 返回数据
			// 2015.7.11 加入判断，因为设定DSP参数时，如果原先已经有设定在运行，丢弃。
			if (B_Com4Cmd16 || B_Com4Cmd06)
			{
				Txd4Buffer[0] = 2;			   // 设备从地址
				Txd4Buffer[1] = Rcv4Buffer[1]; // 功能码
				Txd4Buffer[2] = Rcv4Buffer[2]; // 开始地址高位字节
				Txd4Buffer[3] = Rcv4Buffer[3]; // 开始地址低位字节
				Txd4Buffer[4] = Rcv4Buffer[4]; // 寄存器数量高位
				Txd4Buffer[5] = Rcv4Buffer[5]; // 寄存器数量低位
				if (j == 0)					   // 如果不能被正常预置，返回FFFF zcl
				{
					Txd4Buffer[4] = 0xff; // 寄存器数量高位、预置数据
					Txd4Buffer[5] = 0xff; // 寄存器数量低位、预置数据
				}
				Lw_Txd4ChkSum = CRC16((u8 *)Txd4Buffer, 6);
				Txd4Buffer[6] = Lw_Txd4ChkSum >> 8; // /256
				Txd4Buffer[7] = Lw_Txd4ChkSum;		// 低位字节
				Cw_Txd4Max = 8;
				B_Com4Cmd16 = 0;
				B_Com4Cmd06 = 0;
				Cw_Txd4 = 0;
				// RS485_CON=1;
				// 2010.8.5 周成磊 TXE改成TC，一句改为两句
				// USART_ITConfig(UART4, USART_IT_TXE, ENABLE);				// 开始发送.
				USART_SendData(UART4, Txd4Buffer[Cw_Txd4++]);
				USART_ITConfig(UART4, USART_IT_TC, ENABLE); // 开始发送.
			}
		} // 06、16预置寄存器 结束
	}
}

// 作为主模块时程序
void Com4_MasterSend(void) // 串口2主发送程序
{
}

// 异或计算
u16 CalcCheck(u8 *Bytes, u16 len)
{
	u16 i, result;
	for (result = Bytes[1], i = 2; i < len; i++)
	{
		result ^= Bytes[i];
	}
	return result;
}

/* BK:
							if( strstr(Rcv4Buffer,",S,")!=NULL )
							{
								Gps_ptr=strstr(Rcv4Buffer,",S,");
								w_ScrGpsLat= (*(Gps_ptr-9)-0x30)*1000 +  (*(Gps_ptr-8)-0x30)*100 + (*(Gps_ptr-7)-0x30)*10 + (*(Gps_ptr-6)-0x30);
								w_ScrGpsLatFen= (*(Gps_ptr-4)-0x30)*1000 +  (*(Gps_ptr-3)-0x30)*100 + (*(Gps_ptr-2)-0x30)*10 + (*(Gps_ptr-1)-0x30);
							}
							else
							{
								w_ScrGpsLat=0;
								w_ScrGpsLatFen=0;
							}
							//5.2 经度计算填充
							//Lon dddmm.mmmm 经度，前 3 字符表示度，后面的字符表示分
							if( strstr(Rcv4Buffer,",E,")!=NULL )
							{
								Gps_ptr=strstr(Rcv4Buffer,",E,");
								w_ScrGpsLon= (*(Gps_ptr-10)-0x30)*10000 +  (*(Gps_ptr-9)-0x30)*1000 + (*(Gps_ptr-8)-0x30)*100 + (*(Gps_ptr-7)-0x30)*10 + (*(Gps_ptr-6)-0x30);
								w_ScrGpsLonFen= (*(Gps_ptr-4)-0x30)*1000 +  (*(Gps_ptr-3)-0x30)*100 + (*(Gps_ptr-2)-0x30)*10 + (*(Gps_ptr-1)-0x30);
							}
							else
							{
								w_ScrGpsLon=0;
								w_ScrGpsLonFen=0;
							}
							i=1; */

void Judge_Reset_GPS(void)
{
	// 强制定时重启
	if (F_Reset_GPS == 0 && T_Reset_GPS != SClkSecond)
	{
		T_Reset_GPS = SClkSecond;
		C_Reset_GPS++;
		if (C_Reset_GPS >= 30) // 串口4超过60秒没收到GPS发来的数据，则置重启标志位
		{
			C_Reset_GPS = 0;
			F_Reset_GPS = 1; // 置重启标志位
			C_PowerOnOfft_GPS = 0;
		}
	}
}

// 重启GPS模块
void PowerOnOff_GPS(void)
{
	if (F_Reset_GPS)
	{
		if (T_PowerOnOff_GPS != SClkSecond)
		{
			T_PowerOnOff_GPS = SClkSecond;
			C_PowerOnOfft_GPS++;
			if (C_PowerOnOfft_GPS < 3) // 串口4超过60秒没收到GPS发来的数据，则置重启标志位
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_1); // ZCL 2019.6.25 关闭GPS
			}
			else
			{
				C_PowerOnOfft_GPS = 0;
				F_Reset_GPS = 0;				 // 清重启标志位
				GPIO_SetBits(GPIOC, GPIO_Pin_1); // ZCL 2018.10.19 打开GPS
			}
		}
	}
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
