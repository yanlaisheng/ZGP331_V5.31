/**
  ******************************************************************************
  * @file    com1_232.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.10
  * @date    2015-04-13
  * @brief   Com1 USE USART1
	******************************************************************************
	* @note	2015.04.13
		COM1: 液晶跟变频通讯
	*/

/* Includes ------------------------------------------------------------------*/
#include "com1_232.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include <stdio.h> //加上此句可以用printf
#include "CRCdata.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 Txd1Buffer[TXD1_MAX];	  // 发送缓冲区
u8 Rcv1Buffer[RCV1_MAX];	  // 接收缓冲区
u8 Txd1Buffer_TEMP[TXD1_MAX]; // 发送缓冲区
u16 Cw_Rcv1;				  // 接收计数器//
u16 Cw_Txd1;				  // 发送计数器//
u16 Cw_BakRcv1;				  // 接收计数器//
u16 Cw_Txd1Max;				  // 有多少个字符需要发送//
u16 Lw_Txd1ChkSum;			  // 发送校验和，lo,hi 两位//
u16 Lw_Com1RegAddr;			  // 串口1寄存器地址
u16 w_Txd1ChkSum;			  // 发送校验和，lo,hi 两位//
u16 w_Com1RegAddr;			  // 串口1寄存器地址
u16 Txd1Max;				  // 有多少个字符需要发送//
u16 Txd1Counter;			  // 发送计数器//
u16 BakRcv1Count;			  // 接收计数器//
//
u8 B_Com1Cmd03;
u8 B_Com1Cmd16;
u8 B_Com1Cmd01;
u8 B_Com1Cmd06;
u8 B_Com1Cmd73;
u8 T_NoRcv1; // 没有接收计数器
u16 C_NoRcv1;

u8 S_Com1SendNoNum; // Com1发送次数
u8 S_Com1Send;		// Com1发送步骤
u8 F_Com1Send;		// Com1发送标志
u8 T_Com1Send;
u16 C_Com1Send;

u8 T_Com1SendDelay;
u16 C_Com1SendDelay;
u8 S_RdWrNo; // 读写序号

u8 B_ModYW310SZM220; // ZCL 2019.10.19
u8 B_Com1Con;		 // ZCL 2019.9.22
u8 T_Com1Err1;
u16 C_Com1Err1;
u8 T_Com1Err2;
u16 C_Com1Err2;
u8 T_Com1NormalNext;
u16 C_Com1NormalNext;
u8 T_Com1OvertimeNext;
u16 C_Com1OvertimeNext;
u8 B_Com1Send1;
u8 B_Com1Send2;
u8 B_Com1Sd1End;
u8 B_Com1Sd2End;
u8 S_Com1MSend;
u8 F_Com1SendNext;
u8 F_Com1RCVCommandOk;

u16 Lw_Com1EquipmentNo1; // ZCL 2022.9.13
u16 Lw_Com1EquipmentNo2; // ZCL 2022.9.13
/* Private variables extern --------------------------------------------------*/
extern u16 C_ForceSavPar; // 强制保存参数计数器
extern u8 F_ModStm32Par;  // 修改STM32参数

// ZCL 2018.5.14
extern u8 S_ModParStatus;	   // 修改参数确认后，指向下一个参数
extern u8 ModParNo;			   // 修改参数序号++
extern u8 ModParBitPos;		   // 修改参数位位置=0
extern u8 S_OldModParStatus;   // 旧的修改参数状态（保存对比用）ZCL 2018.5.16
extern u8 B_PressedStopRunKey; // 按下STOP或者RUN键 2016.12.6
extern u8 B_ForceSavPar;	   // 强制保存参数标志
/* Private function prototypes -----------------------------------------------*/
void GPIO_Com1_Configuration(void); // GPIO配置
void Com1_config(void);
u16 CRC16(u8 *pCrcData, u8 CrcDataLen);
void Delay_MS(vu16 nCount);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : GPIO_Com1_Configuration
 * Description    : Configures the different GPIO ports.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Com1_Configuration(void) // GPIO配置
{
	GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体

	// 串口说明：USART1(USART口下标从1开始,跟STM32F103 ARM寄存器兼容)
	// Com1 实际为第一个串口
	/* Configure 485  */ // RS485没有控制端
	/* Configure Com1(使用硬件:USART1) Tx (PA.9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure Com1(使用硬件:USART1) Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* USART1  configuration ------------------------------------------------------*/
/* USART  configured as follow:
		- BaudRate = 9600 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
				RTS/CTS协议(Request To Send/Clear To Send)即请求发送/允许发送协议
*/
void Com1_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_DeInit(USART1);					// 复位 Usart
	USART_StructInit(&USART_InitStructure); // 把USART_InitStruct中的每一个参数按缺省值填入

	USART_InitStructure.USART_BaudRate = Pw_GprsBaudRate2; // 57600或9600;

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure 串口1-USART1 */
	USART_Init(USART1, &USART_InitStructure);
	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
 * Function Name  : Com1_Init
 * Description    : Initializes the peripherals used by the Usart driver.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Com1_Init(void)
{
	GPIO_Com1_Configuration();
	Com1_config();
	//
	Cw_Txd1 = 0;
	Cw_Rcv1 = 0;
}

// 接收处理程序 校验程序
void Com1_RcvProcess(void)
{
	u8 k, s, i = 0; // 临时变量
					//	u8 err_num = 0;
	u16 j;
	u16 *p_wRead;

	// 作为主机,指定接收时间到了,就可以处理接收到的字符串了
	//  在没收到串行字符的时间超过设定时，可以对接收缓存进行处理了
	//  **********************************rcv_counter<>0,收到字符才能处理
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Cw_Rcv1 > 0 && T_NoRcv1 != SClk1Ms)
		{ // 接收处理过程
			// RS485_CON=0;
			T_NoRcv1 = SClk1Ms; //
			C_NoRcv1++;
			if (C_NoRcv1 > NORCVMAXMS) //
			{
				//
				USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // 禁止UART1串口中断,防止在处理期间更改
				Cw_BakRcv1 = Cw_Rcv1;							// 把 Cw_Rcv1 保存
				C_NoRcv1 = 0;									// 清没有接收计数器
				//
				if (Cw_BakRcv1 <= RCV1_MAX) // 接收长度正确,继续处理.
				{
					if (Rcv1Buffer[0] == Pw_LoRaEquipmentNo // ZCL 2019.10.20  接收从机的返回数据！
						|| B_ModYW310SZM220 == 1			// ZCL 2019.10.20  读写 YW310、SZM220中的参数，可以不用校验地址
					)
					{
						j = CRC16(Rcv1Buffer, Cw_BakRcv1 - 2); // CRC 校验
						k = j >> 8;
						s = j;
						if (k == Rcv1Buffer[Cw_BakRcv1 - 2] && s == Rcv1Buffer[Cw_BakRcv1 - 1])
						{							// CRC校验正确
							if (Rcv1Buffer[1] == 3) // 03读取保持寄存器
							{

								// Lw_Reg 在Read_Stm32Data( Address(&Pw_SoftOverPEn,0), 1 ) 中赋值！
								// 转化地址 ZCL 2015.9.1 这里做主机，从机的返回数据

								/*ZCL 2017.6.18 切换页面时数据有跳动，仿真发现前面一页的数据返回来有延时，传送到这一页上，
										造成数据跳动。改成返回的数据，根据前面Txd1Buffer的发送地址来赋值，现在好了！ OK*/
								j = Txd1Buffer[2];
								Lw_Reg = (j << 8) + Txd1Buffer[3];

								// ZCL 2019.10.19 Address()进行地址的变换。 注意：Com1_RcvProcess接收时，进行地址的反变换
								if (B_ModYW310SZM220 == 1) // ZCL 2019.10.19 读写 YW310、SZM220中的参数，地址要变换的
								{
									p_wRead = w_ParLst; // 读STM32 PAR区

									// ZCL 2019.10.19  地址变回
									// YW310
									if (Lw_Reg == 9)
										Lw_Reg = 260;
									else if (Lw_Reg == 23)
										Lw_Reg = 261;
									else if (Lw_Reg == 11)
										Lw_Reg = 262;
									else if (Lw_Reg == 156)
										Lw_Reg = 263;
									else if (Lw_Reg == 157)
										Lw_Reg = 264;
									else if (Lw_Reg == 142)
										Lw_Reg = 265; // ZCL 2019.10.20

									else if (Lw_Reg == 1)
										Lw_Reg = 256;
									else if (Lw_Reg == 2)
										Lw_Reg = 257;
									else if (Lw_Reg == 3)
										Lw_Reg = 258;
									else if (Lw_Reg == 4)
										Lw_Reg = 259; // ZCL 2019.10.21
									else if (Lw_Reg == 54)
										Lw_Reg = 266;
									else if (Lw_Reg == 60)
										Lw_Reg = 267; // ZCL 2019.10.21

									// SZM220
									else if (Lw_Reg == 212)
										Lw_Reg = 268;
									else if (Lw_Reg == 213)
										Lw_Reg = 269;
									else if (Lw_Reg == 218)
										Lw_Reg = 270;
									else if (Lw_Reg == 226)
										Lw_Reg = 271;
									else if (Lw_Reg == 227)
										Lw_Reg = 272;
									else if (Lw_Reg == 228)
										Lw_Reg = 273;
									else if (Lw_Reg == 229)
										Lw_Reg = 274;
									else if (Lw_Reg == 230)
										Lw_Reg = 275;
									else if (Lw_Reg == 231)
										Lw_Reg = 276;
									else if (Lw_Reg == 232)
										Lw_Reg = 277;
									else if (Lw_Reg == 233)
										Lw_Reg = 278;
									else if (Lw_Reg == 234)
										Lw_Reg = 279;
									else if (Lw_Reg == 235)
										Lw_Reg = 280;
									else if (Lw_Reg == 236)
										Lw_Reg = 281;
									else if (Lw_Reg == 237)
										Lw_Reg = 282;
									else if (Lw_Reg == 238)
										Lw_Reg = 283;
									else if (Lw_Reg == 501)
										Lw_Reg = 286;
									else if (Lw_Reg == 641)
										Lw_Reg = 287;
									else if (Lw_Reg == 640)
										Lw_Reg = 288; // ZCL 2019.10.20

									p_wRead += Lw_Reg;
									B_ModYW310SZM220 = 0;
								}
								else if (w_ZhouShanProtocol_bit9) // 主动查询
								{
									F_Com1RCVCommandOk = 1;
									T_Com1NormalNext = 100;
									C_Com1NormalNext = 0;
									T_Com1OvertimeNext = 100;
									C_Com1OvertimeNext = 0;
									T_Com1Err1 = 100;
									C_Com1Err1 = 0;
									B_Com1ErrWithModule1 = 0; //
									B_Com1Send1 = 0;
									B_Com1Sd1End = 0;
									// w_Com1RcVOkCounter++; // 主机  2023.3.2
								}
								else
								{
									B_Com1Cmd03 = 1;
									j = Rcv1Buffer[2];
									Lw_Com1RegAddr = (j << 8) + Rcv1Buffer[3];
								}
							}
							else if (Rcv1Buffer[1] == 6) // 06预置单寄存器
							{
								// F_Com1Send = 11;
								C_ForceSavPar = 0; // 强制保存参数计数器=0
								B_Com1Cmd06 = 1;
								j = Rcv1Buffer[2];
								Lw_Com1RegAddr = (j << 8) + Rcv1Buffer[3];
							}
							else if (Rcv1Buffer[1] == 16) // 16预置多寄存器
							{
								// F_Com1Send = 12;
								C_ForceSavPar = 0; // 强制保存参数计数器=0
								B_Com1Cmd16 = 1;
								j = Rcv1Buffer[2];
								Lw_Com1RegAddr = (j << 8) + Rcv1Buffer[3];
							}
							else
								i = 1;

							// ZCL 2019.10.20 接收到正确指令，接收计数器+1
							//						w_Com1RXCounter++;
						}
						else
							i = 2;
					}
					else
						i = 3;
				}
				else
					i = 4;
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
				Cw_Rcv1 = 0; // 准备下次接收到缓存开始
			}
		}
	}
	else // 变频电机
	{
		if (Cw_Rcv1 > 0 && T_NoRcv1 != SClk1Ms)
		{ // 接收处理过程
			// RS485_CON=0;
			T_NoRcv1 = SClk1Ms; //
			C_NoRcv1++;
			if (C_NoRcv1 > NORCVMAXMS) //
			{
				//
				USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // 禁止UART1串口中断,防止在处理期间更改
				Cw_BakRcv1 = Cw_Rcv1;							// 把 Cw_Rcv1 保存
				C_NoRcv1 = 0;									// 清没有接收计数器
				//
				if (Cw_BakRcv1 <= RCV1_MAX) // 接收长度正确,继续处理.
				{
					if (Rcv1Buffer[0] == 2) // 从地址检测－接收到的上位机查询指令
					{
						j = CRC16(Rcv1Buffer, Cw_BakRcv1 - 2); // CRC 校验
						k = j >> 8;
						s = j;
						if (k == Rcv1Buffer[Cw_BakRcv1 - 2] && s == Rcv1Buffer[Cw_BakRcv1 - 1])
						{							// CRC校验正确
							if (Rcv1Buffer[1] == 3) // 03读取保持寄存器
							{

								// Lw_Reg 在Read_Stm32Data( Address(&Pw_SoftOverPEn,0), 1 ) 中赋值！
								// 转化地址 ZCL 2015.9.1 这里做主机，从机的返回数据

								/*ZCL 2017.6.18 切换页面时数据有跳动，仿真发现前面一页的数据返回来有延时，传送到这一页上，
										造成数据跳动。改成返回的数据，根据前面Txd1Buffer的发送地址来赋值，现在好了！ OK*/
								j = Txd1Buffer[2];
								Lw_Reg = (j << 8) + Txd1Buffer[3];

								if (Lw_Reg < STM32_RPAR_ADDRESS)
								{
									p_wRead = Pw_ParLst; // 设定STM32 PAR区
									p_wRead += Lw_Reg - STM32_WRPAR_ADDRESS_BPDJ;
								}
								else if (Lw_Reg >= STM32_RPAR_ADDRESS && Lw_Reg < DSP_WRPAR_ADDRESS)
								{
									p_wRead = w_ParLst; // 读STM32 PAR区
									p_wRead += Lw_Reg - STM32_RPAR_ADDRESS;
								}
								else if (Lw_Reg >= DSP_WRPAR_ADDRESS && Lw_Reg < DSP_RPAR_ADDRESS)
								{
									p_wRead = Pw_dspParLst; // 设定DSP PAR区
									p_wRead += Lw_Reg - DSP_WRPAR_ADDRESS;
								}

								// else if(Lw_Reg>=DSP_RPAR_ADDRESS )  //ZCL 2019.11.22 改成下面
								else if (Lw_Reg >= DSP_RPAR_ADDRESS && Lw_Reg < SCR_WRPAR_ADDRESS)
								{
									p_wRead = w_dspParLst; // 读DSP PAR区
									p_wRead += Lw_Reg - DSP_RPAR_ADDRESS;
								}

								// ZCL 2019.11.22 加入 w_GprsParLst PAR区
								else if (Lw_Reg >= SCR_WRPAR_ADDRESS)
								{
									p_wRead = w_GprsParLst; // w_GprsParLst PAR区
									p_wRead += Lw_Reg - SCR_WRPAR_ADDRESS;
								}

								//
								for (i = 0; i < Rcv1Buffer[2] / 2; i++)
								{
									j = Rcv1Buffer[3 + i * 2];
									*(p_wRead + i) = (j << 8) + Rcv1Buffer[4 + i * 2];
								}

								F_Com1Send = 10;
							}
							else if (Rcv1Buffer[1] == 6) // 06预置单寄存器
							{
								F_Com1Send = 11;
							}
							else if (Rcv1Buffer[1] == 16) // 16预置多寄存器
							{
								F_Com1Send = 12;
							}
							else if (Rcv1Buffer[1] == 1) // 01读取线圈状态
							{
								B_Com1Cmd01 = 1;
							}

							else
								i = 1;

							// ZCL 2019.10.20 接收到正确指令，接收计数器+1
							//						w_Com1RXCounter++;
						}
						else
							i = 2;
					}
					else
						i = 3;
				}
				else
					i = 4;
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
				Cw_Rcv1 = 0; // 准备下次接收到缓存开始
			}
		}
	}

	if (i > 0)
	{
		for (j = 0; j < 20; j++)
		{
			Rcv1Buffer[j] = 0;
		}
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	}
}

void Com1_SlaveSend(void) // 串口1从机发送
{
	u16 m, n, j;
	u8 k;
	u16 *p_wRead;
	u8 *p_bMove;
	u8 *p_bGen;
	u16 *p_wTarget;
	u16 Lw_Com1Addr; // 串口3寄存器地址

	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (B_Com1Cmd03) // 读取保持寄存器
		{
			Txd1Buffer[0] = Pw_LoRaEquipmentNo; // 设备从地址Pw_EquipmentNo
			Txd1Buffer[1] = Rcv1Buffer[1];		// 功能码
			Txd1Buffer[2] = Rcv1Buffer[5] * 2;	// Rcv1Buffer[5]=字数 　
			j = Rcv1Buffer[2];
			Lw_Com1Addr = (j << 8) + Rcv1Buffer[3];

			// 转化地址 ZCL 2015.7.11
			p_wRead = AddressConvert(Lw_Com1Addr); // 2023.12.16 YLS
			p_bMove = Txd1Buffer;
			for (k = 0; k < Rcv1Buffer[5]; k++) // 填充查询内容
			{
				// m = *(p_wRead + Lw_Com1RegAddr + k); //2023.12.16 YLS
				m = *(p_wRead + k); // 2023.12.16 YLS  函数AddressConvert中已经加上了Lw_Com1RegAddr这个偏移量
				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
			Lw_Txd1ChkSum = CRC16((u8 *)Txd1Buffer, Txd1Buffer[2] + 3);
			Txd1Buffer[Txd1Buffer[2] + 3] = Lw_Txd1ChkSum >> 8; // /256
			Txd1Buffer[Txd1Buffer[2] + 4] = Lw_Txd1ChkSum;		// 低位字节
			Cw_Txd1Max = Txd1Buffer[2] + 5;
			//
			B_Com1Cmd03 = 0;
			Cw_Txd1 = 0;
			// RS185_CON=1;
			USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
			USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
		}
		//
		else if (B_Com1Cmd16 || B_Com1Cmd06) // 16预置多寄存器
		{
			if (B_Com1Cmd06) // 预置单个
			{
				// m = Rcv1Buffer[2];
				// m = (m << 8) + Rcv1Buffer[3];
				j = Rcv1Buffer[2];
				Lw_Com1Addr = (j << 8) + Rcv1Buffer[3];
				// 添加修改ZGP331本身参数功能
				if (Lw_Com1Addr >= STM32_RPAR_ADDRESS) // 如果地址在10000以上，就修改ZGP331本身的参数
				{
					// 这是预置本机的 设定参数；
					p_wTarget = AddressConvert(Lw_Com1Addr);
					m = Rcv1Buffer[4];
					// w_ParLst[Lw_Com3RegAddr] = (m << 8) + Txd2Buffer[5];
					*p_wTarget = (m << 8) + Rcv1Buffer[5];
					B_ForceSavPar = 1; // 保存参数
				}
				else
				{
					w_ZhuanFaAdd = Lw_Com1Addr;
					m = Rcv1Buffer[4];
					m = (m << 8) + Rcv1Buffer[5];
					//				*(p_wTarget + Lw_Com2RegAddr) = m;
					w_ZhuanFaData = m;
					F_ModeParLora = 1;
				}
			}

			else if (B_Com1Cmd16) // 预置多个
			{
				if (Rcv1Buffer[6] == 2)
				{
					m = Rcv1Buffer[2];
					m = (m << 8) + Rcv1Buffer[3];
					w_ZhuanFaAdd = m;

					m = Rcv1Buffer[7];
					m = (m << 8) + Rcv1Buffer[8];
					w_ZhuanFaData = m;

					F_ModeParLora = 1;
				}
				else
					F_ModeParLora = 0;
			}

			// -------------------------
			// 返回数据
			// 2015.7.11 加入判断，因为设定DSP参数时，如果原先已经有设定在运行，丢弃。
			if (B_Com1Cmd16 || B_Com1Cmd06)
			{
				Txd1Buffer[0] = Pw_LoRaEquipmentNo; // 设备从地址
				Txd1Buffer[1] = Rcv1Buffer[1];		// 功能码
				Txd1Buffer[2] = Rcv1Buffer[2];		// 开始地址高位字节
				Txd1Buffer[3] = Rcv1Buffer[3];		// 开始地址低位字节
				Txd1Buffer[4] = Rcv1Buffer[4];		// 寄存器数量高位
				Txd1Buffer[5] = Rcv1Buffer[5];		// 寄存器数量低位
				// if (j == 0)					   // 如果不能被正常预置，返回FFFF zcl
				// {
				// 	Txd1Buffer[4] = 0xff; // 寄存器数量高位、预置数据
				// 	Txd1Buffer[5] = 0xff; // 寄存器数量低位、预置数据
				// }
				Lw_Txd1ChkSum = CRC16((u8 *)Txd1Buffer, 6);
				Txd1Buffer[6] = Lw_Txd1ChkSum >> 8; // /256
				Txd1Buffer[7] = Lw_Txd1ChkSum;		// 低位字节
				Cw_Txd1Max = 8;
				B_Com1Cmd16 = 0;
				B_Com1Cmd06 = 0;
				Cw_Txd1 = 0;
				// RS185_CON=1;
				USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
				USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
			}
		} // 06、16预置寄存器 结束
	}
	else // 变频电机
	{
		//
		if (B_Com1Cmd03) // 读取保持寄存器
		{
			Txd1Buffer[0] = 2;				   // 设备从地址Pw_EquipmentNo
			Txd1Buffer[1] = Rcv1Buffer[1];	   // 功能码
			Txd1Buffer[2] = Rcv1Buffer[5] * 2; // Rcv1Buffer[5]=字数 　

			// 转化地址 ZCL 2015.7.11
			if (Lw_Com1RegAddr < STM32_RPAR_ADDRESS)
			{
				p_wRead = Pw_ParLst; // 设定STM32 PAR区
				Lw_Com1RegAddr = Lw_Com1RegAddr - STM32_WRPAR_ADDRESS_BPDJ;
			}
			else if (Lw_Com1RegAddr < DSP_WRPAR_ADDRESS)
			{
				p_wRead = w_ParLst; // 读STM32 PAR区
				Lw_Com1RegAddr = Lw_Com1RegAddr - STM32_RPAR_ADDRESS;
			}
			else if (Lw_Com1RegAddr < DSP_RPAR_ADDRESS)
			{
				p_wRead = Pw_dspParLst; // 设定DSP PAR区
				Lw_Com1RegAddr = Lw_Com1RegAddr - DSP_WRPAR_ADDRESS;
			}
			else if (Lw_Com1RegAddr < SCR_WRPAR_ADDRESS)
			{
				p_wRead = w_dspParLst; // 读DSP PAR区
				Lw_Com1RegAddr = Lw_Com1RegAddr - DSP_RPAR_ADDRESS;
			}
			// ZCL 2019.11.22 加入 w_GprsParLst PAR区
			else
			{
				p_wRead = w_GprsParLst; // w_GprsParLst PAR区
				Lw_Com1RegAddr = Lw_Com1RegAddr - SCR_WRPAR_ADDRESS;
			}

			p_bMove = Txd1Buffer;
			for (k = 0; k < Rcv1Buffer[5]; k++) // 填充查询内容
			{
				m = *(p_wRead + Lw_Com1RegAddr + k);
				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
			Lw_Txd1ChkSum = CRC16((u8 *)Txd1Buffer, Txd1Buffer[2] + 3);
			Txd1Buffer[Txd1Buffer[2] + 3] = Lw_Txd1ChkSum >> 8; // /256
			Txd1Buffer[Txd1Buffer[2] + 4] = Lw_Txd1ChkSum;		// 低位字节
			Cw_Txd1Max = Txd1Buffer[2] + 5;
			//
			//
			B_Com1Cmd03 = 0;
			Cw_Txd1 = 0;
			// RS185_CON=1;
			USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
			USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
		}
		//
		else if (B_Com1Cmd16 || B_Com1Cmd06) // 16预置多寄存器
		{

			j = 1;

			// 修改参数单元
			if (j)
			{
				// 转化地址 ZCL 2015.7.11
				if (Lw_Com1RegAddr < STM32_RPAR_ADDRESS)
				{
					p_wTarget = Pw_ParLst; // 设定STM32 PAR区
					Lw_Com1RegAddr = Lw_Com1RegAddr - STM32_WRPAR_ADDRESS_BPDJ;
				}
				else if (Lw_Com1RegAddr < DSP_WRPAR_ADDRESS)
				{
					p_wTarget = w_ParLst; // 读STM32 PAR区
					Lw_Com1RegAddr = Lw_Com1RegAddr - STM32_RPAR_ADDRESS;
				}
				else if (Lw_Com1RegAddr < DSP_RPAR_ADDRESS) // 原先有修改参数标志，不再保存
				{
					p_wTarget = Pw_dspParLst; // 设定DSP PAR区
					Lw_Com1RegAddr = Lw_Com1RegAddr - DSP_WRPAR_ADDRESS;
				}
				else if (Lw_Com1RegAddr < SCR_WRPAR_ADDRESS)
				{
					// p_wTarget=w_dspParLst;			// 读DSP PAR区
					// Lw_Com1RegAddr=Lw_Com1RegAddr-DSP_RPAR_ADDRESS;
					;
				}

				// ZCL 2019.11.22 加入 w_GprsParLst PAR区
				else
				{
					p_wTarget = w_GprsParLst; // w_GprsParLst PAR区
					Lw_Com1RegAddr = Lw_Com1RegAddr - SCR_WRPAR_ADDRESS;
				}

				if (B_Com1Cmd06) // 预置单个
				{
					m = Rcv1Buffer[4];
					m = (m << 8) + Rcv1Buffer[5];
					*(p_wTarget + Lw_Com1RegAddr) = m;

					// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
					//				if(F_ModDspPar)
					//				{
					//					w_ModDspParAdd=Lw_Com1RegAddr;
					//					w_ModDspParValue=m;
					//				}
				}

				else if (B_Com1Cmd16) // 预置多个
				{
					if (Rcv1Buffer[5] < 90)
					{
						p_bGen = Rcv1Buffer;
						for (k = 0; k < Rcv1Buffer[5]; k++) // Rcv1Buffer[5]=字数
						{
							m = *(p_bGen + 7 + k * 2);
							n = *(p_bGen + 7 + k * 2 + 1);
							m = (m << 8) + n;
							*(p_wTarget + Lw_Com1RegAddr + k) = m;
						}

						// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
						// 这里暂时只适合预置一个DSP参数  ZCL  2015.7.11
						//					if(F_ModDspPar)
						//					{
						//						w_ModDspParAdd=Lw_Com1RegAddr;
						//						w_ModDspParValue=m;
						//					}
					}
				}
			}

			// -------------------------
			// 返回数据
			// 2015.7.11 加入判断，因为设定DSP参数时，如果原先已经有设定在运行，丢弃。
			if (B_Com1Cmd16 || B_Com1Cmd06)
			{
				Txd1Buffer[0] = 2;			   // 设备从地址
				Txd1Buffer[1] = Rcv1Buffer[1]; // 功能码
				Txd1Buffer[2] = Rcv1Buffer[2]; // 开始地址高位字节
				Txd1Buffer[3] = Rcv1Buffer[3]; // 开始地址低位字节
				Txd1Buffer[1] = Rcv1Buffer[1]; // 寄存器数量高位
				Txd1Buffer[5] = Rcv1Buffer[5]; // 寄存器数量低位
				if (j == 0)					   // 如果不能被正常预置，返回FFFF zcl
				{
					Txd1Buffer[1] = 0xff; // 寄存器数量高位、预置数据
					Txd1Buffer[5] = 0xff; // 寄存器数量低位、预置数据
				}
				Lw_Txd1ChkSum = CRC16((u8 *)Txd1Buffer, 6);
				Txd1Buffer[6] = Lw_Txd1ChkSum >> 8; // /256
				Txd1Buffer[7] = Lw_Txd1ChkSum;		// 低位字节
				Cw_Txd1Max = 8;
				B_Com1Cmd16 = 0;
				B_Com1Cmd06 = 0;
				Cw_Txd1 = 0;
				// RS185_CON=1;
				// 2010.8.5 周成磊 TXE改成TC，一句改为两句
				USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
				USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
			}
		} // 06、16预置寄存器 结束
	}
}

// 作为主模块时程序
void Com1_MasterSend(void) // 串口1主发送程序
{
	u8 i = 0, j = 0;

	Lw_Com1EquipmentNo1 = 2;
	Lw_Com1EquipmentNo2 = 3;

	if (Lw_Com1EquipmentNo1 > 0)
		i++; // 统计发送数量
	if (Lw_Com1EquipmentNo2 > 0)
		i++;

	// if ( Pw_VvvfComAddr>0 )	i++;
	// if ( Pw_LLJComAddr>0 )	i++;
	//
	if (i > 0) // 必须有子模块或者变频器，流量计才能进入发送程序
	{
		if (F_Com1SendNext) // 1. 控制发送步骤
		{
			S_Com1MSend++;
			if (S_Com1MSend > 2)
				S_Com1MSend = 1; // COM_UNIT_NUM
			//
			// 2.进行顺序发送
			if (Lw_Com1EquipmentNo1 > 0 && S_Com1MSend == 1)
			{
				F_Com1SendNext = 0; // 清除标志
				B_Com1Send1 = 1;
				Txd1Buffer[0] = Lw_Com1EquipmentNo1; // 设备从地址
				j = 1;
			}
			else if (Lw_Com1EquipmentNo2 > 0 && S_Com1MSend == 2)
			{
				F_Com1SendNext = 0; // 清除标志
				B_Com1Send2 = 1;
				Txd1Buffer[0] = Lw_Com1EquipmentNo2; // 设备从地址
				j = 2;
			}

			// #define	w_DispWenDuDecValue		w_ParLst[148]		// 显示温度DEC值（对外给文本显示器用，ZCL 2017.11.21）
			// ZCL 正常  03指令
			// ZCL 2022.8.24 =111相对于没有 !!!
			if (j == 1) // 发往子模块
			{
				// Txd1Buffer[0]=Pw_EquipmentNo+S_Com1MSend;
				Txd1Buffer[1] = 0x03; // 功能码
				Txd1Buffer[2] = 0x01; // 开始地址（高位）
				Txd1Buffer[3] = 0x53; // 开始地址（低位）  从地址339开始读
				Txd1Buffer[4] = 0;	  // 读取个数（高位）
				Txd1Buffer[5] = 0x20; // 读取个数（低位）读32个字
				w_Txd1ChkSum = CRC16(Txd1Buffer, 6);
				Txd1Buffer[6] = w_Txd1ChkSum >> 8; // /256
				Txd1Buffer[7] = w_Txd1ChkSum;	   // 低位字节
												   //				Txd1Max = 8;
												   //				Txd1Counter = 0;
				Cw_Txd1Max = 8;
				Cw_Txd1 = 0;
				B_Com1Con = 1; // B_Com1Con=1,可以发送

				// 2010.7.4 周成磊 改成	USART_IT_TC
				USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
				USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
			}
		}

		// 3.延时操作
		if (!B_Com1Con) // =0 发送结束
		{
			if (B_Com1Send1 == 1)
				B_Com1Sd1End = 1;
			if (B_Com1Send2 == 1)
				B_Com1Sd2End = 1;
			/* 			if ( B_Com1Send3 == 1 )
							B_Com1Sd3End = 1;
						if ( B_Com1Send1 == 1 )
							B_Com1Sd4End = 1; */
		}

		//
		// 　判断接收正常后－延时发送下一条 6MS
		if (F_Com1RCVCommandOk) // 接收正常延时后，发送下一条
		{
			if (T_Com1NormalNext != SClk1Ms)
			{
				T_Com1NormalNext = SClk1Ms; //
				C_Com1NormalNext++;
				if (C_Com1NormalNext > 8) //
				{
					F_Com1RCVCommandOk = 0;
					T_Com1NormalNext = 100;
					C_Com1NormalNext = 0;
					T_Com1OvertimeNext = 100;
					C_Com1OvertimeNext = 0;
					F_Com1SendNext = 1; //
				}
			}
		}

		// 　判断发送完超时－延时发送下一条 250MS   (2400bps 最少32个) // 200
		if (!B_Com1Con && !F_Com1SendNext) // 发送完进行计时　时间到发送下一条
		{
			if (T_Com1OvertimeNext != SClk1Ms)
			{
				T_Com1OvertimeNext = SClk1Ms; //
				C_Com1OvertimeNext++;
				if (C_Com1OvertimeNext > 250) // 250  ZCL123
				{
					T_Com1OvertimeNext = 100;
					C_Com1OvertimeNext = 0;
					F_Com1SendNext = 1; //
				}
			}
		}

		//----------------------------------------------------------------
		// 判断发送完超时－延时通讯故障 4000MS   与子模块1通讯失败 // 6000
		if (B_Com1Sd1End == 1 && !B_Com1ErrWithModule1) // 发送完进行计时　时间到故障
		{
			if (T_Com1Err1 != SClk1Ms)
			{
				T_Com1Err1 = SClk1Ms; //
				C_Com1Err1++;
				if (C_Com1Err1 > 4000) //
				{
					T_Com1Err1 = 100;
					C_Com1Err1 = 0;
					B_Com1ErrWithModule1 = 1; //
				}
			}
		}

		//----------------------------------------------------------------
		// 判断发送完超时－延时通讯故障 4000MS	与子模块2通讯失败
		if (B_Com1Sd2End == 1 && !B_Com1ErrWithModule2) // 发送完进行计时　时间到故障
		{
			if (T_Com1Err2 != SClk1Ms)
			{
				T_Com1Err2 = SClk1Ms; //
				C_Com1Err2++;
				if (C_Com1Err2 > 4000) //
				{
					T_Com1Err2 = 100;
					C_Com1Err2 = 0;
					B_Com1ErrWithModule2 = 1; //
				}
			}
		}

		// 下面继续添加其他类型　周成磊 20060924
		//...
	}

	// ZCL 2023.2.28
	if (Lw_Com1EquipmentNo1 == 0)
	{
		B_Com1Sd1End = 0;
		B_Com1ErrWithModule1 = 0; //
		C_Com1Err1 = 0;
	}

	if (Lw_Com1EquipmentNo2 == 0)
	{
		B_Com1Sd2End = 0;
		B_Com1ErrWithModule2 = 0; //
		C_Com1Err2 = 0;
	}
}

u16 CRC16(u8 *pCrcData, u8 CrcDataLen)
{
	u8 CRC16Hi = 0xff; /* 高CRC字节初始化 */
	u8 CRC16Lo = 0xff; /* 低CRC字节初始化*/
	u8 Index = 0;
	u16 w_CRC16 = 0;
	while (CrcDataLen--)
	{
		Index = CRC16Hi ^ *pCrcData++;
		CRC16Hi = CRC16Lo ^ CRC_H[Index];
		CRC16Lo = CRC_L[Index];
	}
	w_CRC16 = (CRC16Hi << 8) | CRC16Lo;
	return (w_CRC16);
}

/*************************************************************************************
名  称: void Read_Stm32Data(u16 Reg, u16 Num)
功  能:	通过MODBUS读取STM32的指定个数数据.
参  数:
		Reg   -- 标记读取STM32的开始寄存器
				Num		-- 读取数据个数.
返  回:	无

编  程: ZCL
时  间: 2015-07-03
*************************************************************************************/
void Read_Stm32Data(u16 Reg, u16 Num)
{
	// ZCL 2017.6.19 这个延时很重要，防止前面的接收没有完成，就开始下一个发送
	if (S_Com1Send == 0 && T_Com1SendDelay != SClk10Ms) // ZCL 2016.6.16 加入延时
	{
		T_Com1SendDelay = SClk10Ms;
		C_Com1SendDelay++;
		if (C_Com1SendDelay > 10) // 100MS后可以再次发送
		{
			S_Com1Send = READSTM32DATA;
			C_Com1SendDelay = 0;
		}
	}
	/* 	//ZCL 2017.6.19 这里没有加延时，导致数据接收错位置，出现闪烁！
		if(S_Com1Send==0)
			S_Com1Send=READSTM32DATA; */

	if (S_Com1Send == READSTM32DATA)
	{
		S_Com1Send = READSTM32DATA + 1; // ZCL 2015.8.18 !!! 暂时删除

		Txd1Buffer[0] = 0x02;		// 设备从地址 STM32
		Txd1Buffer[1] = 0x03;		// 功能码
		Txd1Buffer[2] = (Reg >> 8); // 开始地址（高位）
		Txd1Buffer[3] = Reg;		// 开始地址（低位）  地址：9-23 设定参数查询
		Txd1Buffer[4] = (Num >> 8); // 读取个数（高位）
		Txd1Buffer[5] = Num;		// 读取个数（低位）
		Lw_Txd1ChkSum = CRC16(Txd1Buffer, 6);
		Txd1Buffer[6] = Lw_Txd1ChkSum >> 8; // /256
		Txd1Buffer[7] = Lw_Txd1ChkSum;		// 低位字节
		Cw_Txd1Max = 8;
		Cw_Txd1 = 0;
		F_Com1Send = 1;

		USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
		USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
	}
	else if (S_Com1Send == READSTM32DATA + 1)
	{
		if (F_Com1Send == 10) // 别的地方清除,代表读取正常。
		{
			F_Com1Send = 0;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// ZCL 2015.9.1  2018.5.19
			if (B_PressedStopRunKey == 0)	   // ZCL 2016.12.6 按下STOP 或者 RUN键
				S_RdWrNo++;					   // 主动发送下一条指令
			else if (B_PressedStopRunKey == 1) // ZCL 2016.12.6 按下STOP 或者 RUN键
				B_ModPar = 100;				   // ZCL 2016.12.6 =100表示不再进行参数读写操作
		}
		else if (T_Com1Send != SClk10Ms)
		{
			T_Com1Send = SClk10Ms; //
			C_Com1Send++;
			if (C_Com1Send > 80) // 	300MS发送超时
			{
				C_Com1Send = 0;
				if (B_PressedStopRunKey == 0) // ZCL 2016.12.6 未按下STOP 或者 RUN键，正常运行
				{
					S_Com1Send = READSTM32DATA; //=1可以继续发送
					S_Com1SendNoNum++;			// 发送计数器加1
				}
				else // ZCL 2016.12.6 按下STOP 或者 RUN键
				{
					S_Com1Send = 0;
					S_Com1SendNoNum = 0;
					B_ModPar = 100; // ZCL 2016.12.6 =100表示不再进行参数读写操作
				}
			}
		}
	}

	if (S_Com1SendNoNum >= 3) // 发送次数>3次，置故障标志。
	{
		S_Com1Send = 0; //=0,代表禁止发送
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
		// ZCL 2015.9.1
		// S_RdWrNo++;				//主动发送下一条指令 ZCL 2017.6.19 注释掉
	}
}

/*************************************************************************************
名  称: void Write_Stm32Data(u16 Reg, u16 Data)
功  能:	通过MODBUS向STM32写入指定数据.
参  数:
		Reg   -- 要写入的寄存器地址.
				Data  -- 要写的数据.
返  回:	无

编  程: ZCL
时  间: 2015-07-03
*************************************************************************************/
void Write_Stm32Data(u16 Reg, u16 Data)
{
	if (S_Com1Send == 0)
		S_Com1Send = WRITESTM32DATA;

	if (S_Com1Send == WRITESTM32DATA)
	{
		S_Com1Send = WRITESTM32DATA + 1;

		Txd1Buffer[0] = 0x02;		 // 设备从地址
		Txd1Buffer[1] = 0x06;		 // 功能码
		Txd1Buffer[2] = (Reg >> 8);	 // 开始地址（高位）
		Txd1Buffer[3] = Reg;		 // 开始地址（低位）  地址：9-23 设定参数查询
		Txd1Buffer[4] = (Data >> 8); // 预置数据（高位）
		Txd1Buffer[5] = Data;		 // 预置数据（低位）
		Lw_Txd1ChkSum = CRC16(Txd1Buffer, 6);
		Txd1Buffer[6] = Lw_Txd1ChkSum >> 8; // /256
		Txd1Buffer[7] = Lw_Txd1ChkSum;		// 低位字节
		Cw_Txd1Max = 8;
		Cw_Txd1 = 0;
		F_Com1Send = 1;
		// RS485_CON=1;
		// 2010.8.5 周成磊 TXE改成TC，一句改为两句
		// USART_ITConfig(USART1, USART_IT_TXE, ENABLE);				// 开始发送.
		USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
		USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
	}
	else if (S_Com1Send == WRITESTM32DATA + 1)
	{
		if (F_Com1Send == 11) // 别的地方清除,代表读取正常。
		{
			F_Com1Send = 0;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// ZCL 2015.9.1
			// S_RdWrNo++;				//主动发送下一条指令

			// ZCL 2018.5.14 原先在修改状态，指向下一条
			if (B_ModPar == 1 || B_ModPar == 2)
			{
				S_ModParStatus = 1; // 修改参数确认后，指向下一个参数
				ModParNo++;			// 修改参数序号++
				ModParBitPos = 0;	// 修改参数位位置=0
			}

			// ZCL 2018.5.19
			if (B_ModPar == 100)
				B_PressedStopRunKey = 0;
			// ZCL 2015.9.1
			// S_RdWrNo++;				//主动发送下一条指令
			B_ModPar = 0;
		}
		else if (T_Com1Send != SClk1Ms)
		{
			T_Com1Send = SClk1Ms; //
			C_Com1Send++;
			if (C_Com1Send > 800) // 	300MS发送超时
			{
				C_Com1Send = 0;
				S_Com1Send = WRITESTM32DATA; //=1可以继续发送
				S_Com1SendNoNum++;			 // 发送计数器加1
			}
		}
	}

	if (S_Com1SendNoNum >= 3) // 发送次数>3次，置故障标志。
	{
		S_Com1Send = 0; //=0,代表禁止发送
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;

		if (B_ModPar == 100)
			B_PressedStopRunKey = 0;
		// ZCL 2015.9.1
		// S_RdWrNo++;				//主动发送下一条指令
		B_ModPar = 0;

		// ZCL 2018.5.14
		// ZCL 2019.3.22 注释掉，解决：//出现问题：但一会自动退出修改状态？答：因为没有连接ARM板，导致Write_Stm32Data()中超过3次清S_ModParStatus  解决办法：注释掉S_ModParStatus=0;  还注释掉：ModParNo=0; ModParBitPos=0;

		// ZCL 2019.4.9
		//  S_ModParStatus=0;		//修改参数确认后，指向下一个参数
		//  ModParNo=0;					//修改参数序号++
		//  ModParBitPos=0;			//修改参数位位置=0
	}
}

/*************************************************************************************
名  称: void Write_Stm32MulDatas(u16 Reg, u16 Num, u16 *Data)
功  能:	通过MODBUS向STM32写入指定数据.
参  数:
		Reg   -- 要写入的寄存器地址.
				Data  -- 要写的数据.
返  回:	无

编  程: ZCL
时  间: 2015-07-03
*************************************************************************************/
void Write_Stm32MulDatas(u16 Reg, u16 Num, u16 *Data)
{
	u16 i;

	if (S_Com1Send == 0)
		S_Com1Send = WRITESTM32MULDATA;

	if (S_Com1Send == WRITESTM32MULDATA)
	{
		S_Com1Send = WRITESTM32MULDATA + 1;

		Txd1Buffer[0] = 0x02;		// 设备从地址
		Txd1Buffer[1] = 0x10;		// 功能码
		Txd1Buffer[2] = (Reg >> 8); // 开始地址（高位）
		Txd1Buffer[3] = Reg;		// 开始地址（低位）  地址：9-23 设定参数查询
		Txd1Buffer[4] = (Num >> 8); // 预置个数（高位）
		Txd1Buffer[5] = Num;		// 预置个数（低位）
		Txd1Buffer[6] = Num * 2;	// 预置字节数
		for (i = 0; i < Num; i++)
		{
			Txd1Buffer[7 + i * 2] = (Data[i] >> 8);
			Txd1Buffer[7 + i * 2 + 1] = Data[i];
		}

		Lw_Txd1ChkSum = CRC16(Txd1Buffer, 7 + i * 2);
		Txd1Buffer[7 + i * 2] = Lw_Txd1ChkSum >> 8; // /256
		Txd1Buffer[7 + i * 2 + 1] = Lw_Txd1ChkSum;	// 低位字节
		Cw_Txd1Max = 7 + i * 2 + 2;
		Cw_Txd1 = 0;
		F_Com1Send = 1;
		// RS485_CON=1;
		// 2010.8.5 周成磊 TXE改成TC，一句改为两句
		// USART_ITConfig(USART1, USART_IT_TXE, ENABLE);				// 开始发送.
		USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
		USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
	}
	else if (S_Com1Send == WRITESTM32MULDATA + 1)
	{
		if (F_Com1Send == 12) // 别的地方清除,代表读取正常。
		{
			F_Com1Send = 0;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// ZCL 2015.9.1
			// S_RdWrNo++;				//主动发送下一条指令
			B_ModPar = 0;
		}
		else if (T_Com1Send != SClk1Ms)
		{
			T_Com1Send = SClk1Ms; //
			C_Com1Send++;
			if (C_Com1Send > 800) // 	1000MS发送超时
			{
				C_Com1Send = 0;
				S_Com1Send = WRITESTM32MULDATA; //=1可以继续发送
				S_Com1SendNoNum++;				// 发送计数器加1
			}
		}
	}

	if (S_Com1SendNoNum >= 3) // 发送次数>3次，置故障标志。
	{
		S_Com1Send = 0; //=0,代表禁止发送
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
		// ZCL 2015.9.1
		// S_RdWrNo++;				//主动发送下一条指令
		B_ModPar = 0;
	}
}

// 相对地址
u16 Position(u16 *p1, u16 *p2)
{
	return p2 - p1;
}

// 加上偏移量的 绝对地址
u16 Address(u16 *p, u16 Area)
{
	B_ModYW310SZM220 = 0;	   // ZCL 2019.10.19
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Area == 0)
			Lw_Reg = Position(Pw_ParLst, p) + DNB_ADDRESS; //  2023.12.16 YLS STM32_WRPAR_ADDRESS这个常量改为50000了
		else if (Area == 1)
			Lw_Reg = Position(w_ParLst, p) + STM32_RPAR_ADDRESS;
		else if (Area == 2)
			Lw_Reg = Position(Pw_dspParLst, p) + DSP_WRPAR_ADDRESS;
		else if (Area == 3)
			Lw_Reg = Position(w_dspParLst, p) + DSP_RPAR_ADDRESS;
		// ZCL 2018.8.3 SCR本身参数  没作用，只是补上完整。
		else if (Area == 4)
			Lw_Reg = Position(w_GprsParLst, p) + SCR_WRPAR_ADDRESS; // ZCL 2018.9.13  +SCR_WRPAR_ADDRESS
		// ZCL 2019.10.19 读写YW310，SZM220的参数。进行地址变换。 注意：Com1_RcvProcess接收时，进行地址的反变换
		else if (Area == 8)
		{
			Lw_Reg = Position(w_ParLst, p);
			B_ModYW310SZM220 = 1; // ZCL 2019.10.19

			// YW310
			if (Lw_Reg == 260)
				Lw_Reg = 9;
			else if (Lw_Reg == 261)
				Lw_Reg = 23;
			else if (Lw_Reg == 262)
				Lw_Reg = 11;
			else if (Lw_Reg == 263)
				Lw_Reg = 156;
			else if (Lw_Reg == 264)
				Lw_Reg = 157;
			else if (Lw_Reg == 265)
				Lw_Reg = 142;

			else if (Lw_Reg == 256)
				Lw_Reg = 1; // ZCL 2019.10.21
			else if (Lw_Reg == 257)
				Lw_Reg = 2;
			else if (Lw_Reg == 258)
				Lw_Reg = 3;
			else if (Lw_Reg == 259)
				Lw_Reg = 4;
			else if (Lw_Reg == 266)
				Lw_Reg = 54;
			else if (Lw_Reg == 267)
				Lw_Reg = 60; // ZCL 2019.10.21

			// SZM220
			else if (Lw_Reg == 268)
				Lw_Reg = 212;
			else if (Lw_Reg == 269)
				Lw_Reg = 213;
			else if (Lw_Reg == 270)
				Lw_Reg = 218;
			else if (Lw_Reg == 271)
				Lw_Reg = 226;
			else if (Lw_Reg == 272)
				Lw_Reg = 227;
			else if (Lw_Reg == 273)
				Lw_Reg = 228;
			else if (Lw_Reg == 274)
				Lw_Reg = 229;
			else if (Lw_Reg == 275)
				Lw_Reg = 230;
			else if (Lw_Reg == 276)
				Lw_Reg = 231;
			else if (Lw_Reg == 277)
				Lw_Reg = 232;
			else if (Lw_Reg == 278)
				Lw_Reg = 233;
			else if (Lw_Reg == 279)
				Lw_Reg = 234;
			else if (Lw_Reg == 280)
				Lw_Reg = 235;
			else if (Lw_Reg == 281)
				Lw_Reg = 236;
			else if (Lw_Reg == 282)
				Lw_Reg = 237;
			else if (Lw_Reg == 283)
				Lw_Reg = 238;
			else if (Lw_Reg == 286)
				Lw_Reg = 501;
			else if (Lw_Reg == 287)
				Lw_Reg = 641;
			else if (Lw_Reg == 288)
				Lw_Reg = 640;
		}
	}
	else // 变频电机
	{
		if (Area == 0)
			Lw_Reg = Position(Pw_ParLst, p) + STM32_WRPAR_ADDRESS_BPDJ;
		else if (Area == 1)
			Lw_Reg = Position(w_ParLst, p) + STM32_RPAR_ADDRESS;
		else if (Area == 2)
			Lw_Reg = Position(Pw_dspParLst, p) + DSP_WRPAR_ADDRESS;
		else if (Area == 3)
			Lw_Reg = Position(w_dspParLst, p) + DSP_RPAR_ADDRESS;
		// ZCL 2018.8.3 SCR本身参数  没作用，只是补上完整。
		else if (Area == 4)
			Lw_Reg = Position(w_GprsParLst, p) + SCR_WRPAR_ADDRESS; // ZCL 2018.9.13  +SCR_WRPAR_ADDRESS
	}

	return Lw_Reg;
}

// 减去偏移量的 绝对地址
u16 *AddressChange(void)
{
	u16 *p;
	if (Lw_Reg < STM32_RPAR_ADDRESS)
	{
		p = Pw_ParLst;			   // 设定STM32 PAR区
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			p += Lw_Reg - DNB_ADDRESS;
		}
		else // 变频电机
		{
			p += Lw_Reg - STM32_WRPAR_ADDRESS_BPDJ;
		}
	}
	else if (Lw_Reg >= STM32_RPAR_ADDRESS && Lw_Reg < DSP_WRPAR_ADDRESS)
	{
		p = w_ParLst; // 读STM32 PAR区
		p += Lw_Reg - STM32_RPAR_ADDRESS;
	}
	else if (Lw_Reg >= DSP_WRPAR_ADDRESS && Lw_Reg < DSP_RPAR_ADDRESS)
	{
		p = Pw_dspParLst; // 设定DSP PAR区
		p += Lw_Reg - DSP_WRPAR_ADDRESS;
	}

	// else if(Lw_Reg>=DSP_RPAR_ADDRESS )  //ZCL 2019.11.22 改成下面
	else if (Lw_Reg >= DSP_RPAR_ADDRESS && Lw_Reg < SCR_WRPAR_ADDRESS)
	{
		p = w_dspParLst; // 读DSP PAR区
		p += Lw_Reg - DSP_RPAR_ADDRESS;
	}

	// ZCL 2019.11.22 加入 w_GprsParLst PAR区
	else if (Lw_Reg >= SCR_WRPAR_ADDRESS)
	{
		p = w_GprsParLst; // w_GprsParLst PAR区
		p += Lw_Reg - SCR_WRPAR_ADDRESS;
	}
	return p;
}

// 带参数的地址转换函数
u16 *AddressChange_2(u16 L_Reg)
{
	u16 *p;
	if (L_Reg < STM32_RPAR_ADDRESS)
	{
		p = Pw_ParLst; // 设定STM32 PAR区
		p += L_Reg - DNB_ADDRESS;
	}
	else if (L_Reg >= STM32_RPAR_ADDRESS && L_Reg < DSP_WRPAR_ADDRESS)
	{
		p = w_ParLst; // 读STM32 PAR区
		p += L_Reg - STM32_RPAR_ADDRESS;
	}
	else if (L_Reg >= DSP_WRPAR_ADDRESS && L_Reg < DSP_RPAR_ADDRESS)
	{
		p = Pw_dspParLst; // 设定DSP PAR区
		p += L_Reg - DSP_WRPAR_ADDRESS;
	}

	// else if(L_Reg>=DSP_RPAR_ADDRESS )  //ZCL 2019.11.22 改成下面
	else if (L_Reg >= DSP_RPAR_ADDRESS && L_Reg < SCR_WRPAR_ADDRESS)
	{
		p = w_dspParLst; // 读DSP PAR区
		p += L_Reg - DSP_RPAR_ADDRESS;
	}

	// ZCL 2019.11.22 加入 w_GprsParLst PAR区
	else if (L_Reg >= SCR_WRPAR_ADDRESS)
	{
		p = w_GprsParLst; // w_GprsParLst PAR区
		p += L_Reg - SCR_WRPAR_ADDRESS;
	}
	return p;
}

// 减去偏移量的 绝对地址 YLS 2023.12.15
// 只有双驱泵程序用到这个函数
u16 *AddressConvert(u16 L_Reg)
{
	u16 *p;
	if (L_Reg < STM32_RPAR_ADDRESS)
	{
		p = w_DNBParLst; // w_DNBParLst PAR区
		p += L_Reg - DNB_ADDRESS;
	}
	else if (L_Reg >= STM32_RPAR_ADDRESS && L_Reg < DSP_WRPAR_ADDRESS)
	{
		p = w_ParLst; // 读STM32 PAR区
		p += L_Reg - STM32_RPAR_ADDRESS;
	}
	else if (L_Reg >= DSP_WRPAR_ADDRESS && L_Reg < DSP_RPAR_ADDRESS)
	{
		p = Pw_dspParLst; // 设定DSP PAR区
		p += L_Reg - DSP_WRPAR_ADDRESS;
	}

	// else if(L_Reg>=DSP_RPAR_ADDRESS )  //ZCL 2019.11.22 改成下面
	else if (L_Reg >= DSP_RPAR_ADDRESS && L_Reg < SCR_WRPAR_ADDRESS)
	{
		p = w_dspParLst; // 读DSP PAR区
		p += L_Reg - DSP_RPAR_ADDRESS;
	}

	// ZCL 2019.11.22 加入 w_GprsParLst PAR区
	else if (L_Reg >= SCR_WRPAR_ADDRESS && L_Reg < STM32_WRPAR_ADDRESS_SQB)
	{
		p = w_GprsParLst; // w_GprsParLst PAR区
		p += L_Reg - SCR_WRPAR_ADDRESS;
	}

	// YLS 2023.12.15
	else if (L_Reg >= STM32_WRPAR_ADDRESS_SQB)
	{
		p = Pw_ParLst; // Pw_ParLst PAR区
		p += L_Reg - STM32_WRPAR_ADDRESS_SQB;
	}
	return p;
}

void USART_putchar(USART_TypeDef *USART_x, unsigned char ch)
{
	/* Write a character to the USART */
	USART_SendData(USART_x, (unsigned char)ch);
	while (USART_GetFlagStatus(USART_x, USART_FLAG_TXE) == RESET)
		;
	while (USART_GetFlagStatus(USART_x, USART_FLAG_TC) == RESET)
		;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
