/**
  ******************************************************************************
  * @file    Xmodem.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.20
  * @date    2013.6.2
  * @brief   Xmodem协议传送文件，使用CRC校验。
	******************************************************************************
	1. 2013.9.4 重点说明，下面一句用寄存器方式好用，用库方式竟然不行，存在BUG
		//库问题是明明 USART2->SR 06位TC已经为1，
		但USART_GetFlagStatus(USART2, USART_IT_TC) 检测不到

		// Loop until the end of transmission
		//while (USART_GetFlagStatus(USART2, USART_IT_TC) == RESET);		//这句不行，存在BUG  ZCL 2013.9.4
		while((USART2->SR&0X40)==0);	//循环发送,直到发送完毕 	//换成这句 OK    ZCL 2013.9.4

	2. //Xmodem传输时需要57600较高的波特率 2013.7.3
		退出传输时，恢复成串口正常的波特率。
		Pw_GprsBaudRate2=w_GprsSaveBaudRate2;

	3. 上电就来测试文件传输，不用按空格键！
	  //检查空格键，如果有键按下，可以在上位机传送参数设置文件GP311.ini
		if(1)		//ZCL 2019.3.14 测试时
		//if( CheckSPACEInput() )

	4.  ZCL 2019.4.5
		问题：.INI GPRS配置文件超过 2048字节，导致溢出。
		解决办法：GPRS配置文件中去掉重复的汉字：GP311 TwoLink  V133 V132 2019.3.20 新服务器+临沂.ini
							RCV2_MAX		2048

	5. ZCL 2019.4.5
		问题：XmodemFileTransfer时，有时候不打印："Xmodem set end，set par item 69 times! OK" ?
		解决办法：加入延时！！！ 感觉接收没有处理完，所以必须延时。延时超过50MS，测试好用！
		w_GprsXmodemRcvLen=XmodemReceive(Rcv2Buffer,RCV2_MAX);		//ZCL 2019.4.5

		Delay_MS(500);	//ZCL 2019.4.5	很重要！OK		2000,1000,500,200,50，不加延时，打印内容69项出不来！
										//2MS不行。10,13,16,20,30MS,不行。 20MS偶尔行过
		if(w_GprsXmodemRcvLen>0)
		{
			XmodemRcvDoWith();
			w_GprsXmodemFTItem=Lw_GprsTmp;	//Xmodem文件传输 项  ZCL 2019.4.5
			B_ForceSavPar=1;								//退出设定，保存参数 2013.6.6
		}

	*/

/* Includes ------------------------------------------------------------------*/
#include "xmodem.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include "string.h"
#include <stdio.h> //加上此句可以用printf

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint LastError = 0;
uint C_ReceiveChar; // 接收字符时间计数器
uint Lw_GprsReceiveCount;
uint Lw_GprsTmp;
u8 tmp;

/* Private variables extern --------------------------------------------------*/
extern uchar Rcv2Buffer[];		 // 接收缓冲区
extern uchar *ptr, *ptr2, *ptr3; // 指针
extern uc16 CmdPst[];
extern uc8 GprsCmd2Byte[];
extern uc8 CmdNo[];
extern uchar CommandNo;		// 命令序号
extern uchar B_ForceSavPar; // 强制保存参数标志

/* Private function prototypes -----------------------------------------------*/
void Delay_US(vu16 nCount);
void Delay_MS(vu16 nCount);

uint Str2Bcd(u8 *String);
uchar FindFirstValue(uc8 *Buffer, u8 FindValue, u8 EndValue);
uint Str2Hex(u8 *String);
void Com2_Init(void);
extern void Com2_printf(char *fmt, ...);
/***********************************************************************************
** 函数名称 : Port_OutByte
** 功能描述 : 向串口发送一个字符数据
** 入口参数 : <sendchar> 发送的数据
** 出口参数 : 无
** 返 回 值 : 无
** 其他说明 : 无
***********************************************************************************/
void Port_OutByte(u8 sendchar)
{
	RS485_CON = 1; // 2013.9.2
	Delay_US(10);  // 2013.6.5 为了减小发送校验字符C的频率
	tmp = sendchar;

	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;										  //==0正在发送，等待！
	USART_ClearITPendingBit(USART2, USART_IT_TC); // 2013.9.2	 清除USART_IT_TC标志
	USART_SendData(USART2, tmp);				  // 发送数据

	// 2013.9.4 重点说明，下面一句用寄存器方式好用，用库方式竟然不行，存在BUG
	// 库问题是明明 USART2->SR 06位TC已经为1，但USART_GetFlagStatus(USART2, USART_IT_TC) 检测不到
	/* Loop until the end of transmission */
	// while (USART_GetFlagStatus(USART2, USART_IT_TC) == RESET);		//这句不行，存在BUG  ZCL 2013.9.4
	while ((USART2->SR & 0X40) == 0)
		; // 循环发送,直到发送完毕 	//换成这句 OK    ZCL 2013.9.4

	USART_ClearITPendingBit(USART2, USART_IT_TC); // 2013.9.2
}

/***********************************************************************************
** 函数名称 : Port_InByte
** 功能描述 : 向串口接收一个字符数据
** 入口参数 : <Time> 读取数据最大延时时间(ms)
** 出口参数 : 无
** 返 回 值 : 从串口接收到的数据或者为0
** 其他说明 : 无
***********************************************************************************/
u8 Port_InByte(u16 Time)
{
	u8 ch;

	RS485_CON = 0; // 2013.9.2
	ch = 0;
	C_ReceiveChar = 0; // 接收字符时间计数器
	LastError = 0;

	while (C_ReceiveChar < Time) // C_ReceiveChar在SysTickHandler中每毫秒++
	{
		if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{
			/* Read one byte from the receive data register */
			ch = USART_ReceiveData(USART2);
			/* Clear the USART2 Receive interrupt */
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
			// if(ch>0)  //此句的加入导致错误，如果接收的数据为0，则LastError=1，出错 2013.6.4
			return ch;
		}
	}

	LastError = 1;
	return ch;
}

/********************************************************************************
** 函数名称 : Check
** 功能描述 : 校验，CRC为真则为CRC校验，否则为校验和
** 入口参数 : <crc>[in] 选择是CRC校验还是SUM校验
**			  		<buf>[in] 校验的原始数据
**			  		<sz>[in]  校验的数据长度
** 出口参数 : 无
** 返 回 值 : 校验无误返回TURE,反之返回FALSE
** 其他说明 : 无
********************************************************************************/
u16 Check(int crc, const uchar *buf, int sz)
{
	if (crc) // CRC
	{
		u16 crc = Crc16_ccitt((const char *)buf, sz);
		u16 t_crc = (buf[sz] << 8) + buf[sz + 1];
		if (crc == t_crc)
			return 1;
	}
	else // 累加和
	{
		int i;
		uchar cks = 0;
		for (i = 0; i < sz; ++i)
		{
			cks += buf[i];
		}
		if (cks == buf[sz])
			return 1;
	}

	return 0;
}

/********************************************************************************
 * 函数名称:  CheckSPACEInput(void)
 * 功能描述: 通过串口检测是否有空格键输入,没有有效输入返回0；有有效输入返回1
 * 入口参数: 无
 * 出口参数：如果有连续空格键输入，函数返回1
 * 举例应用：无
 * 特别注意：
 ********************************************************************************/
uchar CheckSPACEInput(void)
{
	uint i;

	// 长延时，可以在仿真时，鼠标点回“超级终端”界面，按空格键发送0x20;  2013.6.4
	// 正常应用时，此时间缩短！！！ 周成磊
	Delay_MS(200); // 2000
				   // i=USART_ReceiveData(USART2);  此方式不行，必须改成下面的接收方式！！！ 2013.6.5
	i = Port_InByte(DLY_1S);
	if (i == SPACE)
	{
		Delay_MS(100);
		// i=USART_ReceiveData(USART2);  此方式不行，必须改成下面的接收方式！！！ 2013.6.5
		i = Port_InByte(DLY_1S);
		if (i == SPACE)
		{
			Com2_printf("\r\n------------------------------------Begin \r\n");
			Com2_printf("Enter Xmodem set gprs par mode\r\n");
			return 1; // 检测有连续空格键，返回1
		}
	}
	return 0;
}

void Flushinput(void)
{
	// while (port_inbyte(((DLY_1S)*3)>>1) >= 0)
	;
}

/********************************************************************************
 * 函数名称:  XmodemFileTransfer(void)
 * 功能描述: 使用Xmodem协议进行文件传输 （文件传输：File Transfer ）
 * 入口参数: 无
 * 出口参数：无
 * 举例应用：无
 * 特别注意：
 ********************************************************************************/
u8 B_Xmodem;				  // ZCL 2020.4.17
void XmodemFileTransfer(void) // ZCL 2013.5.29 	Xmodem协议进行文件传输 （文件传输：File Transfer ）
{
	// Xmodem传输时需要57600较高的波特率 2013.7.3
	u8 i = 0;
	w_GprsSaveBaudRate2 = Pw_GprsBaudRate2;
	w_GprsSaveDataBitLen2 = Pw_GprsDataBitLen2;
	w_GprsSaveStopBitLen2 = Pw_GprsStopBitLen2;
	w_GprsSaveParityBitLen2 = Pw_GprsParityBitLen2;
	w_GprsSaveFlowControl2 = Pw_GprsFlowControl2;
	Pw_GprsBaudRate2 = 57600;	 // 57600:较高的波特率
	Pw_GprsDataBitLen2 = 0x03;	 // 0x03:8位数据位
	Pw_GprsStopBitLen2 = 0x00;	 // 0x00:1位停止位
	Pw_GprsParityBitLen2 = 0x00; // 0x00:无校验
	Pw_GprsFlowControl2 = 0x03;	 // 0x03:无流控

	Com2_Init();

	B_Xmodem = 1;				 // ZCL 2020.4.17
	Com2_printf("  Xmodem\r\n"); // ZCL 2018.12.10

	w_GprsXmodemFTItem = 555; // 没有文件传输的值；Xmodem文件传输 项  ZCL 2019.4.5
	// 检查空格键，如果有键按下，可以在上位机传送参数设置文件GP311.ini
	// if(1)		//ZCL 2019.3.14 测试时
	if (CheckSPACEInput())
	{

		w_GprsXmodemRcvLen = XmodemReceive(Rcv2Buffer, RCV2_MAX); // ZCL 2019.4.5

		Delay_MS(500); // ZCL 2019.4.5	很重要！OK		2000,1000,500,200,50，不加延时，打印内容69项出不来！
					   // 2MS不行。10,13,16,20,30MS,不行。 20MS偶尔行过
		if (w_GprsXmodemRcvLen > 0)
		{
			XmodemRcvDoWith();
			w_GprsXmodemFTItem = Lw_GprsTmp; // Xmodem文件传输 项  ZCL 2019.4.5
			B_ForceSavPar = 1;				 // 退出设定，保存参数 2013.6.6
		}

		// 打印参数，方便观看程序的运行情况（特别是独立看门狗的运行） 2013.7.3
		Com2_printf("Exit Xmodem , Enter normal run mode. \r\n");
		Com2_printf("-------------------------------------End \r\n");
		i = 1;
	}

	// Delay_MS(100);		//ZCL 2019.4.5

	// 退出Xmodem时，需重设波特率  2013.7.3
	Pw_GprsBaudRate2 = w_GprsSaveBaudRate2; // 进入Xmodem模式，使用57600较高的波特率
	Pw_GprsDataBitLen2 = w_GprsSaveDataBitLen2;
	Pw_GprsStopBitLen2 = w_GprsSaveStopBitLen2;
	Pw_GprsParityBitLen2 = w_GprsSaveParityBitLen2;
	Pw_GprsFlowControl2 = w_GprsSaveFlowControl2;
	Com2_Init();

	if (i == 0)
		// 打印参数，方便观看程序的运行情况（特别是独立看门狗的运行） 2013.7.3
		// Com2_printf("\r\nExit Xmodem , Enter normal run mode (2) \r\n");	  //ZCL 2020.4.17 有这句，没有焊接GPRS模块的时候，跳不过去
		B_Xmodem = 2; // ZCL 2020.4.17
}

/********************************************************************************
** 函数名称 : XmodemReceive
** 功能描述 : xmodem协议接收文件
** 入口参数 : <checkType>[in] 接收文件的校验方式，'C':crc校验，NAK:累加和校验
** 出口参数 : 无
** 返 回 值 : 接收文件操作时的相关错误代码
** 其他说明 : 无
********************************************************************************/
// destination 目标		sz:size:尺寸，长度
int XmodemReceive(uchar *dest, int destsz) // xmodem协议接收文件
{
	uchar xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	uchar *p;
	int bufsz, crc = 0;
	uchar checkchar = 'C';
	uchar packetno = 1;
	int i, c, len = 0;
	int retry, retrans = MAXRETRANS;

	for (;;)
	{
		for (retry = 0; retry < 16; ++retry) //? 16 所有的超时及错误事件至少重试10次
		{
			if (checkchar)
			{
				Delay_MS(200);			 // 2013.6.5 为了减小发送校验字符C的频率
				Port_OutByte(checkchar); // 发送：校验模式字符
			}
			c = Port_InByte((DLY_1S) << 1); // 接收：在2S时间内接收到的字符
			if (LastError == 0)
			{
				switch (c)
				{
				case SOH: // SOH标准Xmodem协议（每个数据包含有128字节数据）
					bufsz = 128;
					goto start_recv;
				case STX: // STX 1k-Xmodem（每个数据包含有1024字节数据）
					bufsz = 1024;
					goto start_recv;
				case EOT: // EOT:结束
					Flushinput();
					Port_OutByte(ACK); // 接收方用 ACK 进行确认
					//*(dest+len)=0;
					return len; /* normal end */
				case CAN_X:		// CAN_X:取消传输
					c = Port_InByte(DLY_1S);

					if (c == CAN_X)
					{
						Flushinput();
						Port_OutByte(ACK);
						return -1; /* canceled by remote */
					}
					break;
				default:
					break;
				}
			}
		}

		if (checkchar == 'C')
		{
			checkchar = NAK;
			continue;
		}

		Flushinput();
		Port_OutByte(CAN_X);
		Port_OutByte(CAN_X);
		Port_OutByte(CAN_X);
		return -2; /* sync error */

	start_recv:
		if (checkchar == 'C') // CRC校验
			crc = 1;
		checkchar = 0;									  // 校验模式字符清零，不再发送校验模式
		p = xbuff;										  // 指针
		*p++ = c;										  // 保存第一个字符
		for (i = 0; i < (bufsz + (crc ? 1 : 0) + 3); ++i) // 循环接收其他字符
		{
			c = Port_InByte(DLY_1S); // 字符间1秒延时
			Lw_GprsReceiveCount = i;

			if (LastError != 0) // 有错误跳出，发送NAK
				goto reject;
			*p++ = c; // 没有错误保存字符
		}

		// 上面接收完一帧数据
		// 下面进行序号，序号补码，校验
		if (xbuff[1] == (uchar)(~xbuff[2]) && (xbuff[1] == packetno || xbuff[1] == (uchar)packetno - 1) && Check(crc, &xbuff[3], bufsz))
		{
			if (xbuff[1] == packetno)
			{
				int count = destsz - len;
				if (count > bufsz)
					count = bufsz;
				if (count > 0)
				{
					memcpy(&dest[len], &xbuff[3], count);
					// strcpy (&dest[len], &xbuff[3]);
					len += count;
				}
				++packetno; // 包序号加1
				retrans = MAXRETRANS + 1;
			}
			if (--retrans <= 0)
			{
				Flushinput();
				Port_OutByte(CAN_X);
				Port_OutByte(CAN_X);
				Port_OutByte(CAN_X);
				return -3; /* too many retry error */
			}

			Port_OutByte(ACK);
			continue;
		}

	reject:
		Flushinput();
		Port_OutByte(NAK);
	}
}

/********************************************************************************
** 函数名称 : XmodemRcvDoWith
** 功能描述 : xmodem协议接收的数据进行处理
** 入口参数 : 无
** 出口参数 : 无
** 返 回 值 : 无
** 其他说明 : 程序按照短信设置、查询程序
********************************************************************************/
void XmodemRcvDoWith(void) // xmodem协议接收的数据进行处理			//ZCL 2013.5.30
{
	uchar i, r, s, err = 0;
	uint ip[4], w1, n = 0;
	// char *ptr;

	ptr = Rcv2Buffer;
	Lw_GprsTmp = 0;

	// 一、设置指令W 或者 w
	while (1)
	{
		ptr = (u8 *)strstr((char *)ptr, ")=");
		if (ptr != NULL)
		{
			if (Lw_GprsTmp == 0) // 打印个换行符，好分辨信息
				Com2_printf("\r\n-------------------Result info\r\n");
			// 1. 合成命令，找命令，查找相应字符位置以利于下一步使用
			// 把收到的字符串 如：GPW+26,42;		0x32和0x36合成16进制指令值0x26
			CommandNo = Str2Hex(ptr - 2);
			if (CommandNo <= 0x63)
				n = FindFirstValue(CmdNo, CommandNo, 0x00); // n:找到指令值在GPRS 参数保存序列的位置
			i = 1;											// 查找第一个'='
			r = FindFirstValue(ptr, 0x0D, 0x00);			// 0x0D=回车		//查找结束符';'

			// 2.错误指令的处理; ZCL 添加 2012.9.23
			// n==255 没有找到此命令；CommandNo>0x90 此命令超过存在的命令
			if (n == 255 || i == 255 || r == 255 || CommandNo > 0x90)
			{
				// 写的错误号从1开始 （可以1-9）
				// 置错误标志 err=1
				err = 1;
				Com2_printf("CommandNo Error!\r\n");
				// break;  在后面检测err后，break跳出
			}

			// 周成磊 2013.2.2 	有错误跳过
			ptr = ptr + 2; // 指令内容的地址
			if (err)
				;

			// 5. IP地址: 发送过来如222.173.103.226 因为有'.'需要去掉
			// 0x20,0x30,0x40,0x50 是IP地址, 0x62是DNS地址
			else if (CommandNo == 0x20 || CommandNo == 0x30 || CommandNo == 0x40 || CommandNo == 0x50 || CommandNo == 0x62)
			{
				// 把收到的字符串 如：0x32,0x32,0x32=222，合成10进制值222(在内存中保存为0xDE)
				for (s = 0; s < 4; s++)
				{
					ip[s] = Str2Bcd(ptr); // 下一个位置
					if (s == 0 && ip[s] == 0)
					{
						ip[1] = 0;
						ip[2] = 0;
						ip[3] = 0;
						s = 3; // 2013.6.4 解决IP设置为空时显示出错问题
					}
					if (s == 3)
						break; // 最后面没有'.'了
					// 查找第一二三个'.'
					i = FindFirstValue(ptr, 0x2E, 0x00); // 0x2E=.
					ptr = ptr + i + 1;
				}

				// 如果正确则进行保存；如果错误则返回错误信息
				for (s = 0; s < 4; s++)
				{
					if (ip[s] <= 255)
					{
						// n: 指令值在GPRS 参数保存序列的位置
						// 正确保存，把IP地址写到GprsPar参数表
						GprsPar[CmdPst[n] + s] = ip[s];
					}
					else
					{
						// 错误处理 标记：err=2
						err = 2;
						Com2_printf("IP Address Set Error!\r\n");
						// 2013.1.24 周成磊 退出for循环
						break;
					}
				}
			}

			// 6. 字符串的设置（不是数值） 如：0x05：CMNET
			// 服务代码:01		接入点名称:05		DTU身份识别码:06		域名:21.31.41.51
			else if (CommandNo == 0x01 || CommandNo == 0x05 || CommandNo == 0x06 || CommandNo == 0x21 || CommandNo == 0x31 || CommandNo == 0x41 || CommandNo == 0x51)
			{
				if (CommandNo == 0x01 && r - i - 1 > 8)
					err = 3;
				else if (CommandNo == 0x05 && r - i - 1 > 24)
					err = 3;
				else if (CommandNo == 0x06 && r - i - 1 > 11)
					err = 3;
				else if (CommandNo == 0x21 && r - i - 1 > 30)
					err = 3;
				else if (CommandNo == 0x31 && r - i - 1 > 30)
					err = 3;
				else if (CommandNo == 0x41 && r - i - 1 > 30)
					err = 3;
				else if (CommandNo == 0x51 && r - i - 1 > 30)
					err = 3;
				if (err == 3)
				{
					// 填充参数错误的短信内容，准备发送短信
					// 错误参数的处理
					Com2_printf("Par Item 0x%x: String Length Overlong Error! \r\n", CommandNo);
				}
				else
				{
					// r(0x0D=回车 位置)  i(=位置)
					for (s = 0; s < (r - i - 1); s++)
					{
						GprsPar[CmdPst[n] + s] = *(ptr + s);
					}
					// 2012.9.20 周成磊 字符串的末尾加0
					GprsPar[CmdPst[n] + s] = 0;
				}
			}

			// 8. 串口的设置，0x0D指令：两个字  0x00,0x00,0x25,0x80 = 9600
			//  目前程序只支持最后一个字的波特率设置，只支持到0-65535
			else if (CommandNo == 0x0D)
			{
				// 8.1 波特率
				// 把收到的字符串 如：0x39,0x36,0x30,0x30=9600，合成10进制值(在内存中保存为0x25,0x80)
				// 过来的值不需要换算,合并起来就行
				w1 = Str2Bcd(ptr); // 下一个位置
				if (w1 == 1200 || w1 == 2400 || w1 == 4800 || w1 == 9600 || w1 == 19200 || w1 == 38400 || w1 == 57600)
				{
					GprsPar[CmdPst[n] + 2] = w1 >> 8; // 高字节
					GprsPar[CmdPst[n] + 3] = w1;	  // 低字节
					// 在Com1_BaudRateSet()中修改Pw_GprsBaudRate2
				}
				else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
				{
					;
				}
				else
				{
					// 错误参数的处理
					err = 4;
					Com2_printf("Com1 Baudrate Set Error!\r\n");
				}

				// 8.2 数据位 数据位为8位（00-5， 01-6， 02-7， 03-8）	过来的值5,6,7,8要换算成 0,1,2,3
				// 查找下一个个',' [0x2C:',']
				i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
				if (i == 255)
				{
					w1 = 255;
					goto Xmodem_SetBaudItemLack_END; // 2013.12.18
				}
				else
				{
					ptr = ptr + i + 1;
					w1 = Str2Bcd(ptr); // 下一个位置
				}

				if (w1 <= 8 && w1 >= 5)
				{
					GprsPar[CmdPst[n] + 4] = w1 - 5;
				}
				else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
				{
					;
				}
				else
				{
					// 错误参数的处理
					err = 5;
					Com2_printf("Com1 DataBitLength Set Error!\r\n");
				}

				// 8.3 停止位 停止位为1位（00-1，04-2）	过来的值1,2要换算成 0,0x04
				// 查找下一个个',' [0x2C:',']
				i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
				if (i == 255)
				{
					w1 = 255;
					goto Xmodem_SetBaudItemLack_END; // 2013.12.18
				}
				else
				{
					ptr = ptr + i + 1;
					w1 = Str2Bcd(ptr); // 下一个位置
				}
				if (w1 == 1 || w1 == 2)
				{
					GprsPar[CmdPst[n] + 5] = (w1 - 1) * 4;
				}
				else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
				{
					;
				}
				else
				{
					// 错误参数的处理
					err = 6;
					Com2_printf("Com1 StopBitLength Set Error!\r\n");
				}

				// 8.4 奇偶校验 校验位-无（00-无0，08-奇1，18-偶2）	过来的值0,1,2要换算成 0，0x08，0x18
				// 查找下一个个',' [0x2C:',']
				i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
				if (i == 255)
				{
					w1 = 255;
					goto Xmodem_SetBaudItemLack_END; // 2013.12.18
				}
				else
				{
					ptr = ptr + i + 1;
					w1 = Str2Bcd(ptr); // 下一个位置
				}
				if (w1 <= 2)
				{
					if (w1 == 0)
						GprsPar[CmdPst[n] + 6] = 0;
					else if (w1 == 1)
						GprsPar[CmdPst[n] + 6] = 0x08;
					else if (w1 == 2)
						GprsPar[CmdPst[n] + 6] = 0x18;
				}
				else
				{
					// 错误参数的处理
					err = 7;
					Com2_printf("Com1 ParityBitLength Set Error!\r\n");
				}

				// 8.5 流控-无流控（01-Xon/Xoff1，02-硬流控2，03-无流控3，04-半双工485 4，05-全双工422 5）
				// 过来的值不需要换算
				// 查找下一个个',' [0x2C:',']
				i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
				if (i == 255)
				{
					w1 = 255;
					goto Xmodem_SetBaudItemLack_END; // 2013.12.18
				}
				else
				{
					ptr = ptr + i + 1;
					w1 = Str2Bcd(ptr); // 下一个位置
				}
				if (w1 <= 5 && w1 > 0)
				{
					GprsPar[CmdPst[n] + 7] = w1;
				}
				else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
				{
					;
				}
				else
				{
					// 错误参数的处理
					err = 8;
					Com2_printf("Com1 FlowControl Set Error!\r\n");
				}

			Xmodem_SetBaudItemLack_END: // 设置波特率中设置项缺少错误。GPW+0D,9600,8,1,2,3;
				if (w1 == 255 && err == 0)
				{
					// 错误参数的处理	//设置串行口项缺少错误
					err = 9;
					Com2_printf("Serial Port Item Lack Error!\r\n");
				}
			}

			// 9. 双字节参数和单字节参数：数值的设置 0-65535范围内
			else
			{
				// 查找指令是不是双字节参数
				s = FindFirstValue(GprsCmd2Byte, CommandNo, 0x00);
				// 参数的值
				w1 = Str2Bcd(ptr); // 下一个位置
				// 双字节参数
				if (s != 255) // s>0 && 		//2013.6.4这个不应该要
				{
					GprsPar[CmdPst[n] + 0] = w1 >> 8;
					GprsPar[CmdPst[n] + 1] = w1;
				}
				// 单字节参数
				else
				{
					GprsPar[CmdPst[n] + 0] = w1;
				}
			}

			// 周成磊 发现错误立即跳出，不再检测下一项参数
			if (err)
			{
				Com2_printf("Xmodem File Transfer err= %d  !!!\r\n", err);
				Com2_printf("Please restart File Transfer !!!\r\n");
				break;
			}
			else
				Lw_GprsTmp++;
		}

		// 没有搜索到")="，结束循环 2013.6.4
		else
		{
			Com2_printf("Xmodem set end，set par item %d times! OK\r\n", Lw_GprsTmp);
			break;
		}
	}
}

/* CRC16 implementation acording to CCITT standards */
u16 Crc16_ccitt(const char *buf, int len) // Crc16		ZCL: 应该同串口的CRC16校验，写法不太一样
{
	int counter;
	u16 crc = 0;
	for (counter = 0; counter < len; counter++)
		crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *(char *)buf++) & 0x00FF];
	return crc;
}
