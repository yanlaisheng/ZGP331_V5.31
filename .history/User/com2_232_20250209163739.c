/**
  ******************************************************************************
  * @file    com2_232.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.10
  * @date    2015-04-13
  * @brief   Com1 USE USART2
	******************************************************************************
	* @note	2015.04.13
			COM2: 透传输出调试；Xmodem协议传送文件
	*/

/* Includes ------------------------------------------------------------------*/
#include "com1_232.h"
#include "com2_232.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include <stdio.h> //加上此句可以用printf

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 Txd2Buffer[TXD2_MAX];	  // 发送缓冲区
u8 Rcv2Buffer[RCV2_MAX];	  // 接收缓冲区
u8 Txd2Buffer_TEMP[TXD2_MAX]; // 发送缓冲区
u16 Cw_Rcv2;				  // 接收计数器
u16 Cw_Txd2;				  // 发送计数器
u16 Cw_BakRcv2;				  // 接收计数器备份
u16 Cw_Txd2Max;				  // 有多少个字符需要发送
u16 Lw_Txd2ChkSum;			  // 发送校验和，lo,hi 两位
u16 Lw_Com2RegAddr;			  // 串口1寄存器地址
//
u8 B_Com2Cmd03;
u8 B_Com2Cmd16;
u8 B_Com2Cmd01;
u8 B_Com2Cmd06;
u8 B_Com2Cmd73;
u8 T_NoRcv2; // 没有接收计数器
u16 C_NoRcv2;

u8 F_ModDspPar; // 修改DSP参数

// B_PcSetGprs，=1时，再接收到数据则置=2，借用Sms_SetupInq()函数处理
u8 B_PcSetGprs; // PC设置GPRS标志 2013.6.5

/* Private variables extern --------------------------------------------------*/
extern u16 C_ForceSavPar;	// 强制保存参数计数器
extern uint8_t LoRaTxBuf[]; // ZCL 2018.11.12
extern u8 B_LoRaSendMaster;

extern u8 Txd3Buffer[TXD3_MAX]; // 发送缓冲区
extern u8 Rcv3Buffer[RCV3_MAX]; // 接收缓冲区
extern u16 Cw_Rcv3;				// 接收计数器
extern u16 Cw_Txd3;				// 发送计数器
extern u16 Cw_BakRcv3;			// 接收计数器备份
extern u16 Cw_Txd3Max;			// 有多少个字符需要发送
extern u16 Lw_Txd3ChkSum;		// 发送校验和，lo,hi 两位
extern u16 Lw_Com3RegAddr;		// 串口1寄存器地址

extern u8 B_GprsDataReturn; // GPRS 数据返回。有连接，收到串口2数据，就通过DTU发送出去
extern u16 C_GprsDataSend;
extern u8 B_SmsRunStop; // 短信控制停止（设备停机）
extern u8 S_M35;
extern u16 C_DelayRunGprsModule; // 延时运行GPRS模块标志
extern u8 CGD0_LinkConnectOK[];	 // 场景0连接标志
extern u8 B_ATComStatus;		 // AT命令状态 2013.5.20
extern u8 B_ForceSavPar;		 // 强制保存参数标志
extern uchar B_RestartNo;		 // ZCL 2019.4.19  重启序号，方便打印观察问题

// ZCL 2019.11.29
extern u8 Txd4Buffer[];
extern u8 Rcv4Buffer[];
extern u16 Cw_Txd4;
extern u16 Cw_Rcv4;
extern u16 C_NoRcv4;
extern u16 Cw_Txd4Max;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Com2_Configuration(void); // GPIO配置
void Com2_config(void);
u16 CRC16(u8 *pCrcData, u8 CrcDataLen);
void Delay_MS(vu16 nCount);
extern void Com2_printf(char *fmt, ...);
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : GPIO_Com2_Configuration
 * Description    : Configures the different GPIO ports.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Com2_Configuration(void) // GPIO配置
{
	GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体

	/* Configure Com2(使用硬件:USART2) Rx (PA.3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure Com2(使用硬件:USART2) Tx (PA.2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure 485 控制端 PC.15 */
	/* PC13，PC14和PC15引脚通过电源开关进行供电，因此这三个引脚作为输出引脚时有以下限制：
	作为输出脚时只能工作在2MHz模式下,最大驱动负载为30pF, 同一时间，三个引脚中只有一个引脚
	能作为输出引脚。
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);	 */
	// Com2控制端
	// RS485_CON=0;
}

/* USART2  configuration ------------------------------------------------------*/
/* USART  configured as follow:
		- BaudRate = 9600 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
				RTS/CTS协议(Request To Send/Clear To Send)即请求发送/允许发送协议
*/
void Com2_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	// 2014.1.5 下面这句非常重要，复位寄存器，否则在MDK中寄存器没有复位，会出错。
	USART_DeInit(USART2);					// 复位 Usart3
	USART_StructInit(&USART_InitStructure); // 把USART_InitStruct中的每一个参数按缺省值填入

	// 1. 00 00 25 80 串口速率9600（25 80是其十六进制数）
	USART_InitStructure.USART_BaudRate = Pw_GprsBaudRate2; // 9600;
	// 2. 03 数据位为8位（00-5， 01-6， 02-7， 03-8）；（这里只能设置03）
	// STM32: 只有USART_WordLength_8b 、USART_WordLength_9b ，没有5位，6位，7位的长度
	// STM32: 此处在有奇偶校验时需要9个数据位，无奇偶校验时8个数据位。通过奇偶校验位来设置数据位长度
	if (Pw_GprsParityBitLen2 == 0x00)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		Pw_GprsDataBitLen2 = 0x03;
	}
	else if (Pw_GprsParityBitLen2 == 0x08 || Pw_GprsParityBitLen2 == 0x18)
	{
		Pw_GprsDataBitLen2 = 0x03;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	}
	// 3. 00 停止位为1位（00-1，04-2）；（这里可以设置00、04）
	// STM32: USART_StopBits_1 、USART_StopBits_0.5 、USART_StopBits_2 、USART_StopBits_1.5
	if (Pw_GprsStopBitLen2 == 0x00)
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
	else if (Pw_GprsStopBitLen2 == 0x04)
		USART_InitStructure.USART_StopBits = USART_StopBits_2;
	else
	{
		Pw_GprsStopBitLen2 = 0x00;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
	}
	// 4. 00 校验位-无（00-无，08-奇，18-偶）；（这里可以设置00、08、18）
	// STM32: USART_Parity_No  	USART_Parity_Odd 奇模式		USART_Parity_Even 偶模式
	if (Pw_GprsParityBitLen2 == 0x00)
		USART_InitStructure.USART_Parity = USART_Parity_No;
	else if (Pw_GprsParityBitLen2 == 0x08)
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
	else if (Pw_GprsParityBitLen2 == 0x18)
		USART_InitStructure.USART_Parity = USART_Parity_Even;
	else
	{
		Pw_GprsParityBitLen2 = 0x00;
		USART_InitStructure.USART_Parity = USART_Parity_No;
	}
	// 5. 03 流控-无流控（01-Xon/Xoff，02-硬流控，03-无流控，04-半双工485，05-全双工422）
	//				（这里可以设置03、02）
	// STM32: 		USART_HardwareFlowControl_None  		硬件流控制失能
	// USART_HardwareFlowControl_RTS  			发送请求 RTS使能
	// USART_HardwareFlowControl_CTS  			清除发送 CTS使能
	// USART_HardwareFlowControl_RTS_CTS  	RTS和 CTS使能
	if (Pw_GprsFlowControl2 == 0x03)
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	else if (Pw_GprsFlowControl2 == 0x02)
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
	else
	{
		Pw_GprsFlowControl2 = 0x03;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	}
	// 6. 发送使能和接收使能，不用控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure Com1-USART2 */
	USART_Init(USART2, &USART_InitStructure);
	/* Enable USART2 Receive interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);
}

/*******************************************************************************
 * Function Name  : Com2_Init
 * Description    : Initializes the peripherals used by the Usart driver.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Com2_Init(void)
{
	GPIO_Com2_Configuration();
	Com2_config();
	//
	Cw_Txd2 = 0;
	Cw_Rcv2 = 0;
}

// 接收处理程序 校验程序
void Com2_RcvProcess(void)
{
	u8 k, i = 0, s; // 临时变量 k,s,   ZCL 2019.11.21 s
	u16 j;
	// 作为主机,指定接收时间到了,就可以处理接收到的字符串了
	//  在没收到串行字符的时间超过设定时，可以对接收缓存进行处理了
	//  **********************************rcv_counter<>0,收到字符才能处理
	if (Cw_Rcv2 > 0 && T_NoRcv2 != SClk1Ms)
	{ // 接收处理过程
		// RS485_CON=0;
		T_NoRcv2 = SClk1Ms; //
		C_NoRcv2++;
		if (C_NoRcv2 > NORCVMAXMS) //
		{
			//
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // 禁止UART1串口中断,防止在处理期间更改
			Cw_BakRcv2 = Cw_Rcv2;							// 把 Cw_Rcv2 保存
			C_NoRcv2 = 0;									// 清没有接收计数器

			//
			if (Cw_BakRcv2 <= RCV2_MAX) // 接收长度正确,继续处理.
			{
				// ZCL 2019.11.21 取消这个if的注释，让串口2可以访问
				if (F_LoRaToCom == 0 && F_GprsTransToCom == 0 && F_GpsToCom == 0) // ZCL 2019.11.29 加 && F_GpsToCom==0
				{
					// ZCL 2018.11.13 注释掉，改成串口2接收到，LoRa发出
					if (Rcv2Buffer[0] == Pw_LoRaEquipmentNo) // 从地址检测－接收到的上位机查询指令
					{
						j = CRC16(Rcv2Buffer, Cw_BakRcv2 - 2); // CRC 校验
						k = j >> 8;
						s = j;
						if (k == Rcv2Buffer[Cw_BakRcv2 - 2] && s == Rcv2Buffer[Cw_BakRcv2 - 1])
						{							// CRC校验正确
							if (Rcv2Buffer[1] == 3) // 03读取保持寄存器
							{
								B_Com2Cmd03 = 1;
								j = Rcv2Buffer[2];
								Lw_Com2RegAddr = (j << 8) + Rcv2Buffer[3];
							}
							else if (Rcv2Buffer[1] == 16) // 16预置多寄存器
							{
								C_ForceSavPar = 0; // 强制保存参数计数器=0
								B_Com2Cmd16 = 1;
								j = Rcv2Buffer[2];
								Lw_Com2RegAddr = (j << 8) + Rcv2Buffer[3];
							}
							else if (Rcv2Buffer[1] == 1) // 01读取线圈状态
							{
								B_Com2Cmd01 = 1;
							}
							else if (Rcv2Buffer[1] == 6) // 06预置单寄存器
							{
								C_ForceSavPar = 0; // 强制保存参数计数器=0
								B_Com2Cmd06 = 1;
								j = Rcv2Buffer[2];
								Lw_Com2RegAddr = (j << 8) + Rcv2Buffer[3];
							}
							else
								i = 1;
						}
						else
							i = 2;
					}
					else
						i = 3;
				}

				// ZCL 2018.12.20 Pw_LoRaSet .1位 =1  转发到LORA COM2输出到LORA (透传使用)
				else if (F_LoRaToCom)
				{
					for (j = 0; j < Cw_BakRcv2; j++)
					{
						LoRaTxBuf[j] = Rcv2Buffer[j];

						B_LoRaSendMaster = 2;
					}
				}

				// Pw_GprsSet .1位 =1   GPRS输出到COM2
				else if (F_GprsTransToCom)
				{
					if (Cw_BakRcv2 <= RCV2_MAX) // 接收长度正确,继续处理.
					{
						// ZCL 2019.4.19 测试，可以打印和看门狗重启！ 2019.4.25
						if (Rcv2Buffer[0] == '@' && Rcv2Buffer[1] == '9' && Rcv2Buffer[2] == '9' && Rcv2Buffer[3] == '#' && Cw_BakRcv2 == 4)
						{
							Com2_printf("Test RestartNo=%d\r\n", B_RestartNo); // ZCL 2019.4.19 重启序号，方便打印观察问题
							Delay_MS(3000);									   // 此时间内没有喂狗，设备复位！ 2019.4.11
						}

						// 1.1 有无连接都可，进入AT指令状态
						if (Rcv2Buffer[0] == '@' && Rcv2Buffer[1] == '1' && Rcv2Buffer[2] == '1' && Rcv2Buffer[3] == '#' && Cw_BakRcv2 == 4)
						{
							B_ATComStatus = 1;
						}
						// 1.2 有无连接都可，退出AT指令状态
						else if (Rcv2Buffer[0] == '@' && Rcv2Buffer[1] == '2' && Rcv2Buffer[2] == '2' && Rcv2Buffer[3] == '#' && Cw_BakRcv2 == 4)
						{
							B_ATComStatus = 2; // 这里直接为0,@22#不会回发给PC,所以在后面发送后才清零
						}

						// 2.1 有无连接都可，进入设定GPRS参数状态 2013.6.6
						if (Rcv2Buffer[0] == '@' && Rcv2Buffer[1] == '3' && Rcv2Buffer[2] == '3' && Rcv2Buffer[3] == '#' && Cw_BakRcv2 == 4)
						{
							B_PcSetGprs = 1;
							Com2_printf("\r\nEnter PC set gprs par mode\r\n");
						}
						// 2.2 有无连接都可，退出设定GPRS参数状态 2013.6.6
						else if (Rcv2Buffer[0] == '@' && Rcv2Buffer[1] == '4' && Rcv2Buffer[2] == '4' && Rcv2Buffer[3] == '#' && Cw_BakRcv2 == 4)
						{
							B_PcSetGprs = 0;
							B_ForceSavPar = 1; // 退出设定，保存参数 2013.6.6
							// SavePar_Prompt();						//保存参数 2012.10.11
							Com2_printf("\r\nQuit PC set gprs par mode, Set end!\r\n");
						}

						// 3 B_PcSetGprs=1，在用PC串口设定GPRS参数状态下，这时再接收到数据，则置B_PcSetGprs=2，
						// 借用Sms_SetupInq()函数处理
						else if (B_PcSetGprs == 1) //(=1，PC串口设定GPRS参数状态下)		3放到2.1前会出问题
						{
							B_PcSetGprs = 2; //=2做为标志，在main中跳转到Sms_SetupInq()处理：设定GPRS参数
											 // 处理完后再立即让B_PcSetGprs=1，然后再等待接收处理下一条信息
						}

						// 5. 有连接时，就把接收的数据，发送给GPRS模块，赋值B_GprsDataReturn
						else if (CGD0_LinkConnectOK[0] + CGD0_LinkConnectOK[1] + CGD0_LinkConnectOK[2] + CGD0_LinkConnectOK[3] != 0 && !B_ATComStatus)
						{
							// 2012.10.12 短信控制设备启停，不用往网络连接上发送
							if (B_SmsRunStop == 2 || B_SmsRunStop == 3)
							{
								if (Rcv2Buffer[2] == 0x01 && Rcv2Buffer[3] == 0x3D)
								{
									B_SmsRunStop = 3; // 接收到设备发送回来的信息，可以发短信了
								}
							}
							else
							{
								B_GprsDataReturn = 1; // 此标志置1，在com0_232.c中回送数据
								C_GprsDataSend = 0;
							}
						}

						// 6. 有无连接都可，是AT指令，直接把串口2接收到的数据发送到串口3(GPRS)
						if (B_ATComStatus) // 注意：去掉了else
						{
							// 把串口2接收到的数据发送到串口3
							for (k = 0; k < Cw_BakRcv2; k++)
							{
								Txd3Buffer[k] = Rcv2Buffer[k];
							}
							Cw_Txd3Max = Cw_BakRcv2;
							Cw_Txd3 = 0;
							RS485_CON = 1;
							// 2010.8.5 周成磊 TXE改成TC，一句改为两句
							USART_ITConfig(USART3, USART_IT_TC, ENABLE); // 开始发送.
							USART_SendData(USART3, Txd3Buffer[Cw_Txd3++]);

							// 下面这个发送调试用，把串口2接收到的，再回发给串口2，让PC观看用
							// 2013.5.22 把接收到的数据回发到串口2，因为开着回显，会返回两句指令，正常。
							for (k = 0; k < Cw_BakRcv2; k++)
							{
								Txd2Buffer[k] = Rcv2Buffer[k];
							}
							Cw_Txd2Max = Cw_BakRcv2;
							Cw_Txd2 = 0;
							RS485_CON = 1;
							USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
							USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);

							// 2013.5.23 收到@22#，退出AT状态，在这里进行
							if (B_ATComStatus == 2)
								B_ATComStatus = 0;
						}
					}

					// 2014.12.9 加上条件，数据转发时禁止接收。
					// 2016.7.1 ZCL 因为这个条件的加入，必须后面在给每路链接发送完毕后，还得打开接收。！！！
					if (B_GprsDataReturn == 0)
					{
						USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
						USART2->CR1 |= 0x0004; //.3位  运行接收
					}
				}

				// ZCL 2019.11.29 Pw_GprsSet .4位 =1  F_GpsToCom  增加: COM2接收到电脑指令，转发到GPS口(COM4)
				else if (F_GpsToCom)
				{
					for (j = 0; j < Cw_BakRcv2; j++)
					{
						Txd4Buffer[j] = Rcv2Buffer[j];
					}

					// 准备发送
					Cw_Txd4Max = Cw_BakRcv2;
					Cw_Txd4 = 0;

					// 2010.8.5 周成磊 TXE改成TC，一句改为两句
					USART_SendData(UART4, Txd4Buffer[Cw_Txd4++]);
					USART_ITConfig(UART4, USART_IT_TC, ENABLE); // 开始发送.
				}
			}
			else
				i = 4;
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			Cw_Rcv2 = 0; // 准备下次接收到缓存开始
		}
	}

	if (i > 0)
	{
		for (j = 0; j < 20; j++)
		{
			Rcv2Buffer[j] = 0;
		}
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}
}

void Com2_SlaveSend(void) // 串口2从机发送
{
	u16 m, n, j = 0;
	u8 k;
	u16 *p_wRead;
	u8 *p_bMove;
	u16 *p_wTarget;
	u16 Lw_Com2Addr;
	u8 *p_bGen;

	if (Pw_EquipmentType == 0) // 双驱泵
	{
		//
		if (B_Com2Cmd03) // 读取保持寄存器
		{
			Txd2Buffer[0] = Pw_LoRaEquipmentNo; // 设备从地址Pw_EquipmentNo
			Txd2Buffer[1] = Rcv2Buffer[1];		// 功能码
			Txd2Buffer[2] = Rcv2Buffer[5] * 2;	// Rcv2Buffer[5]=字数 　

			// 转化地址
			p_wRead = AddressConvert(Lw_Com2RegAddr); // 2023.12.16 YLS

			p_bMove = Txd2Buffer;
			for (k = 0; k < Rcv2Buffer[5]; k++) // 填充查询内容
			{
				// m = *(p_wRead + Lw_Com2RegAddr + k);// 2023.12.16 YLS
				m = *(p_wRead + k); // 2023.12.16 YLS
				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
			Lw_Txd2ChkSum = CRC16(Txd2Buffer, Txd2Buffer[2] + 3);
			Txd2Buffer[Txd2Buffer[2] + 3] = Lw_Txd2ChkSum >> 8; // /256
			Txd2Buffer[Txd2Buffer[2] + 4] = Lw_Txd2ChkSum;		// 低位字节
			Cw_Txd2Max = Txd2Buffer[2] + 5;
			//
			//
			B_Com2Cmd03 = 0;
			Cw_Txd2 = 0;
			// RS485_CON=1;
			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
			// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
			USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
			USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
		}
		//
		else if (B_Com2Cmd16 || B_Com2Cmd06) // 16预置多寄存器
		{

			if (B_Com2Cmd06) // 预置单个
			{
				j = Rcv2Buffer[2];
				Lw_Com2Addr = (j << 8) + Rcv2Buffer[3];
				// 添加修改ZGP331本身参数功能
				if (Lw_Com2Addr >= STM32_RPAR_ADDRESS) // 如果地址在10000以上，就修改ZGP331本身的参数
				{
					// 这是预置本机的 设定参数；
					p_wTarget = AddressConvert(Lw_Com2Addr);
					m = Rcv2Buffer[4];
					// w_ParLst[Lw_Com3RegAddr] = (m << 8) + Txd2Buffer[5];
					*p_wTarget = (m << 8) + Rcv2Buffer[5];
					B_ForceSavPar = 1; // 保存参数
				}
				else
				{
					// m = Rcv2Buffer[2];
					// m = (m << 8) + Rcv2Buffer[3];
					w_ZhuanFaAdd = Lw_Com2Addr;
					m = Rcv2Buffer[4];
					m = (m << 8) + Rcv2Buffer[5];
					//				*(p_wTarget + Lw_Com2RegAddr) = m;
					w_ZhuanFaData = m;
					F_ModeParLora = 1;
				}
			}

			else if (B_Com2Cmd16) // 预置多个
			{
				if (Rcv2Buffer[6] == 2)
				{
					m = Rcv2Buffer[2];
					m = (m << 8) + Rcv2Buffer[3];
					w_ZhuanFaAdd = m;

					m = Rcv2Buffer[7];
					m = (m << 8) + Rcv2Buffer[8];
					w_ZhuanFaData = m;

					F_ModeParLora = 1;
				}
				else
					F_ModeParLora = 0;
			}

			// -------------------------
			// 返回数据
			// 2015.7.11 加入判断，因为设定DSP参数时，如果原先已经有设定在运行，丢弃。
			if (B_Com2Cmd16 || B_Com2Cmd06)
			{
				Txd2Buffer[0] = Pw_LoRaEquipmentNo; // 设备从地址
				Txd2Buffer[1] = Rcv2Buffer[1];		// 功能码
				Txd2Buffer[2] = Rcv2Buffer[2];		// 开始地址高位字节
				Txd2Buffer[3] = Rcv2Buffer[3];		// 开始地址低位字节
				Txd2Buffer[4] = Rcv2Buffer[4];		// 寄存器数量高位
				Txd2Buffer[5] = Rcv2Buffer[5];		// 寄存器数量低位
				/*
				if (j == 0)					   // 如果不能被正常预置，返回FFFF zcl
				{
					Txd2Buffer[4] = 0xff; // 寄存器数量高位、预置数据
					Txd2Buffer[5] = 0xff; // 寄存器数量低位、预置数据
				}
				*/
				Lw_Txd2ChkSum = CRC16(Txd2Buffer, 6);
				Txd2Buffer[6] = Lw_Txd2ChkSum >> 8; // /256
				Txd2Buffer[7] = Lw_Txd2ChkSum;		// 低位字节
				Cw_Txd2Max = 8;
				B_Com2Cmd16 = 0;
				B_Com2Cmd06 = 0;
				Cw_Txd2 = 0;
				// RS485_CON=1;
				// 2010.8.5 周成磊 TXE改成TC，一句改为两句
				// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
				USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
				USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
			}
		} // 06、16预置寄存器 结束
	}
	else // 变频电机
	{
		//
		if (B_Com2Cmd03) // 读取保持寄存器
		{
			Txd2Buffer[0] = 2;				   // 设备从地址Pw_EquipmentNo
			Txd2Buffer[1] = Rcv2Buffer[1];	   // 功能码
			Txd2Buffer[2] = Rcv2Buffer[5] * 2; // Rcv2Buffer[5]=字数 　

			// 转化地址 ZCL 2015.7.11
			if (Lw_Com2RegAddr < STM32_RPAR_ADDRESS)
			{
				p_wRead = Pw_ParLst; // 设定STM32 PAR区
				Lw_Com2RegAddr = Lw_Com2RegAddr - STM32_WRPAR_ADDRESS_BPDJ;
			}
			else if (Lw_Com2RegAddr < DSP_WRPAR_ADDRESS)
			{
				p_wRead = w_ParLst;									  // 读STM32 PAR区
				Lw_Com2RegAddr = Lw_Com2RegAddr - STM32_RPAR_ADDRESS; // 10000
			}
			else if (Lw_Com2RegAddr < DSP_RPAR_ADDRESS)
			{
				p_wRead = Pw_dspParLst;								 // 设定DSP PAR区
				Lw_Com2RegAddr = Lw_Com2RegAddr - DSP_WRPAR_ADDRESS; // 20000
			}
			// else   //ZCL 2019.11.22
			else if (Lw_Com2RegAddr < SCR_WRPAR_ADDRESS)
			{
				p_wRead = w_dspParLst;								// 读DSP PAR区
				Lw_Com2RegAddr = Lw_Com2RegAddr - DSP_RPAR_ADDRESS; // 30000
			}

			// ZCL 2019.11.22 加入 w_GprsParLst PAR区
			else
			{
				p_wRead = w_GprsParLst; // w_GprsParLst PAR区
				Lw_Com2RegAddr = Lw_Com2RegAddr - SCR_WRPAR_ADDRESS;
			}

			p_bMove = Txd2Buffer;
			for (k = 0; k < Rcv2Buffer[5]; k++) // 填充查询内容
			{
				m = *(p_wRead + Lw_Com2RegAddr + k);
				*(p_bMove + 3 + k * 2) = m >> 8;
				*(p_bMove + 3 + k * 2 + 1) = m;
			}
			Lw_Txd2ChkSum = CRC16(Txd2Buffer, Txd2Buffer[2] + 3);
			Txd2Buffer[Txd2Buffer[2] + 3] = Lw_Txd2ChkSum >> 8; // /256
			Txd2Buffer[Txd2Buffer[2] + 4] = Lw_Txd2ChkSum;		// 低位字节
			Cw_Txd2Max = Txd2Buffer[2] + 5;
			//
			//
			B_Com2Cmd03 = 0;
			Cw_Txd2 = 0;
			// RS485_CON=1;
			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
			// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
			USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
			USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
		}
		//
		else if (B_Com2Cmd16 || B_Com2Cmd06) // 16预置多寄存器
		{

			j = 1;

			// 修改参数单元
			if (j)
			{
				// 转化地址 ZCL 2015.7.11
				if (Lw_Com2RegAddr < STM32_RPAR_ADDRESS)
				{
					p_wTarget = Pw_ParLst; // 设定STM32 PAR区
					Lw_Com2RegAddr = Lw_Com2RegAddr - STM32_WRPAR_ADDRESS_BPDJ;
				}
				else if (Lw_Com2RegAddr < DSP_WRPAR_ADDRESS)
				{
					p_wTarget = w_ParLst; // 读STM32 PAR区
					Lw_Com2RegAddr = Lw_Com2RegAddr - STM32_RPAR_ADDRESS;
				}
				else if (Lw_Com2RegAddr < DSP_RPAR_ADDRESS) // 原先有修改参数标志，不再保存
				{
					p_wTarget = Pw_dspParLst; // 设定DSP PAR区
					Lw_Com2RegAddr = Lw_Com2RegAddr - DSP_WRPAR_ADDRESS;
					// 表示要修改DSP参数，需要在COM1中转发这个指令  ZCL 2015.7.11
					if (!F_ModDspPar)
						F_ModDspPar = 1;
					else // 原先有修改参数标志，则清除此次的修改指令。
					{
						B_Com2Cmd16 = 0;
						B_Com2Cmd06 = 0;
					}
				}

				/* 			else
							{
								p_wTarget=w_dspParLst;			// 读DSP PAR区
								Lw_Com2RegAddr=Lw_Com2RegAddr-DSP_RPAR_ADDRESS;
							}				 */

				// else   // 06 16 指令 写只读的DSP PAR区，不需要！//ZCL 2019.11.22
				else if (Lw_Com2RegAddr < SCR_WRPAR_ADDRESS)
				{
					// p_wTarget=w_dspParLst;			// 读DSP PAR区
					// Lw_Com2RegAddr=Lw_Com2RegAddr-DSP_RPAR_ADDRESS;
					;
				}

				// ZCL 2019.11.22 加入 w_GprsParLst PAR区
				else
				{
					p_wTarget = w_GprsParLst; // w_GprsParLst PAR区
					Lw_Com2RegAddr = Lw_Com2RegAddr - SCR_WRPAR_ADDRESS;
				}

				if (B_Com2Cmd06) // 预置单个
				{
					m = Rcv2Buffer[4];
					m = (m << 8) + Rcv2Buffer[5];
					*(p_wTarget + Lw_Com2RegAddr) = m;

					// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
					if (F_ModDspPar)
					{
						w_ModDspParAdd = Lw_Com2RegAddr;
						w_ModDspParValue = m;
					}
				}

				else if (B_Com2Cmd16) // 预置多个
				{
					if (Rcv2Buffer[5] < 90)
					{
						p_bGen = Rcv2Buffer;
						for (k = 0; k < Rcv2Buffer[5]; k++) // Rcv2Buffer[5]=字数
						{
							m = *(p_bGen + 7 + k * 2);
							n = *(p_bGen + 7 + k * 2 + 1);
							m = (m << 8) + n;
							*(p_wTarget + Lw_Com2RegAddr + k) = m;
						}

						// 因为要转发到DSP，把地址和值保存起来。 ZCL  2015.7.11
						// 这里暂时只适合预置一个DSP参数  ZCL  2015.7.11
						if (F_ModDspPar)
						{
							w_ModDspParAdd = Lw_Com2RegAddr;
							w_ModDspParValue = m;
						}
					}
				}
			}

			// -------------------------
			// 返回数据
			// 2015.7.11 加入判断，因为设定DSP参数时，如果原先已经有设定在运行，丢弃。
			if (B_Com2Cmd16 || B_Com2Cmd06)
			{
				Txd2Buffer[0] = 2;			   // 设备从地址
				Txd2Buffer[1] = Rcv2Buffer[1]; // 功能码
				Txd2Buffer[2] = Rcv2Buffer[2]; // 开始地址高位字节
				Txd2Buffer[3] = Rcv2Buffer[3]; // 开始地址低位字节
				Txd2Buffer[4] = Rcv2Buffer[4]; // 寄存器数量高位
				Txd2Buffer[5] = Rcv2Buffer[5]; // 寄存器数量低位
				if (j == 0)					   // 如果不能被正常预置，返回FFFF zcl
				{
					Txd2Buffer[4] = 0xff; // 寄存器数量高位、预置数据
					Txd2Buffer[5] = 0xff; // 寄存器数量低位、预置数据
				}
				Lw_Txd2ChkSum = CRC16(Txd2Buffer, 6);
				Txd2Buffer[6] = Lw_Txd2ChkSum >> 8; // /256
				Txd2Buffer[7] = Lw_Txd2ChkSum;		// 低位字节
				Cw_Txd2Max = 8;
				B_Com2Cmd16 = 0;
				B_Com2Cmd06 = 0;
				Cw_Txd2 = 0;
				// RS485_CON=1;
				USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
				USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
			}
		} // 06、16预置寄存器 结束
	}
}

// 作为主模块时程序
void Com2_MasterSend(void) // 串口2主发送程序
{
}

/*******************************************************************************
 * Function Name  : fputc
 * Description    : Retargets the C library printf function to the USART.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	RS485_CON = 1;
	//  USART_SendData(USART1, (u8) ch);
	USART_SendData(USART2, (u8)ch);

	/* Loop until the end of transmission */
	//  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
	{
	}
	RS485_CON = 0;
	return ch;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
