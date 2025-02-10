// #include "sim7600ce.h"
// #include "GlobalConst.h"
// #include "GlobalV_Extern.h"
// #include <stdio.h>
//// #include <stdlib.h>
// #include <string.h>
// #include "malloc.h"
// #include "stdarg.h"

// u8 Scan_Wtime = 0;		  // 保存扫描需要的时间
// u8 sim_ready = 0;		  // SIM卡就绪状态=1就绪，=0未找到
// u8 F_RunAgain = 1;		  // 重新运行
// u8 C_simCheckNum = 0;	  // SIM卡检查次数
// u8 T_NetworkCheck = 0;	  // 网络检查延时
// u8 C_NetworkCheck = 0;	  // 网络检查时间
// u8 F_RunCheckModular = 0; // 运行过程中检查模块标志

// u8 T_AgainLinkTime;	 // 延时接收数据的时间
// u16 C_AgainLinkTime; // 延时接收数据的次数

// u8 T_CheckLinkTime;	 // 检查GPRS网络连接时间
// u16 C_CheckLinkTime; // 检查GPRS网络连接次数

// u8 T_SendHeartbeatTime0;  // 发送心跳时间
// u32 C_SendHeartbeatTime0; // 发送心跳时间延时次数

// u8 T_SendHeartbeatTime;	 // 发送心跳时间
// u32 C_SendHeartbeatTime; // 发送心跳时间延时次数

// u8 T_SendATime;	 // 发送AT时间
// u16 C_SendATime; // 发送AT延时次数

// u8 T_GPRSLEDTime;  // GPRS指示灯状态切换延时
// u16 C_GPRSLEDTime; // GPRS指示灯状态切换延时

// u8 F_OneConnectStat = 0;	 // 第1路需要连接IP的状态
// u8 F_TWOConnectStat = 0;	 // 第2路需要连接IP的状态
// u8 F_ThreeConnectStat = 0;	 // 第3路需要连接IP的状态
// u8 F_FourConnectStat = 0;	 // 第4路需要连接IP的状态
// u8 ConnectIPNum = 0;		 // 需要连接IP的数量
// u8 NowConnectIPNum = 0;		 // 当前连接设备的数量
// u8 F_ONESendHeartbeat = 0;	 // 心跳连接1
// u8 F_TwoSendHeartbeat = 0;	 // 心跳连接2
// u8 F_ThreeSendHeartbeat = 0; // 心跳连接3
// u8 F_FourSendHeartbeat = 0;	 // 心跳连接4

// u8 F_ONESendHeartbeat0 = 0;	  // 心跳连接1
// u8 F_TwoSendHeartbeat0 = 0;	  // 心跳连接2
// u8 F_ThreeSendHeartbeat0 = 0; // 心跳连接3
// u8 F_FourSendHeartbeat0 = 0;  // 心跳连接4

// u8 Connect1Lose; // 第1路连接失败
// u8 Connect2Lose; // 第2路连接失败
// u8 Connect3Lose; // 第3路连接失败
// u8 Connect4Lose; // 第4路连接失败
// u8 Connect1OK;	 // 第1路连接成功
// u8 Connect2OK;	 // 第2路连接成功
// u8 Connect3OK;	 // 第3路连接成功
// u8 Connect4OK;	 // 第4路连接成功

// u8 NormalHeartbeat1_0; // 第1路心跳正常
// u8 NormalHeartbeat2_0; // 第2路心跳正常
// u8 NormalHeartbeat3_0; // 第3路心跳正常
// u8 NormalHeartbeat4_0; // 第4路心跳正常

// u8 NormalHeartbeat1; // 第1路心跳正常
// u8 NormalHeartbeat2; // 第2路心跳正常
// u8 NormalHeartbeat3; // 第3路心跳正常
// u8 NormalHeartbeat4; // 第4路心跳正常

// u8 Select_NetNum = 0;  // 网络选择次数
// u8 S_check_status = 0; // 模块初始化状态测试步骤号

// u8 domain_name[30] = {0};  //="";	//域名1
// u8 domain_name1[30] = {0}; //="";	//域名2
// u8 domain_name2[30] = {0}; //="";	//域名3
// u8 domain_name3[30] = {0}; //="";	//域名4

// u8 Net2G[] = {"AT+CNMP=13"};  //  GSM
// u8 Net4G[] = {"AT+CNMP=38"};  //  LTE
// u8 NetAuto[] = {"AT+CNMP=2"}; // AUTO

// u8 connectsta = 0;		  // 0,正在连接;1,连接成功;2,连接关闭;
// u8 hbeaterrcnt0 = 0;	  // 心跳错误计数器,连续8次心跳信号无应答,则则重启系统，重新连接
// u8 SendDataErrorNum0 = 0; // 发送数据错误计数器,连续8次发送数据信号无应答,则重启系统，重新连接

// u8 hbeaterrcnt = 0;		 // 心跳错误计数器,连续8次心跳信号无应答,则则重启系统，重新连接
// u8 SendDataErrorNum = 0; // 发送数据错误计数器,连续8次发送数据信号无应答,则重启系统，重新连接

// u8 Link1SendOK0 = 0; // 第1路发送成功
// u8 Link2SendOK0 = 0; // 第2路发送成功
// u8 Link3SendOK0 = 0; // 第3路发送成功
// u8 Link4SendOK0 = 0; // 第4路发送成功

// u8 Link1SendOK = 0; // 第1路发送成功
// u8 Link2SendOK = 0; // 第2路发送成功
// u8 Link3SendOK = 0; // 第3路发送成功
// u8 Link4SendOK = 0; // 第4路发送成功

// u16 PowerOnOFFWaitNum = 0; // 开关机状态次数

// u8 ModularADDIP[4]; // 模块本身的IP地址

// u8 CallerNumberbuf[11]; // 收到短信的电话号码
// u8 SMSbuf[50];			// 收到短信内容存储区

//// 下面是增加变量
////[15]:0,没有接收到数据；1接收到了一批数据
////[14:0]:接收到的数据长度
// u16 USART3_RX_STA = 0;		// 串口接收到的数据数量
// extern u8 Rcv3Buffer[];		// COM3接收缓冲区
// extern u8 Txd3Buffer[];		// COM3发送缓冲区
// extern u8 Txd2Buffer[];		// COM2发送缓冲区
// extern uint Cw_Rcv3;		// 接收计数器//
// extern uint Cw_Txd3;		// 发送计数器//
// extern uint Cw_BakRcv3;		// 接收计数器//
// extern uint Cw_Txd3Max;		// 有多少个字符需要发送//
// extern uint Lw_Txd3ChkSum;	// 发送校验和，lo,hi 两位//
// extern uint Lw_Com3RegAddr; // 串口3寄存器地址

// extern uchar B_Com3Cmd03;
// extern uchar B_Com3Cmd16;
// extern uchar B_Com3Cmd01;
// extern uchar B_Com3Cmd06;
// extern uchar B_Com3Cmd73;
// extern u8 B_LoraSendWrite; // ZCL 2021.11.16 LORA 写命令的发送
// extern u8 B_LoraSendWriteLength;

// extern uint8_t LoRaTxBuf2[]; // ZCL 2021.11.18
// extern uchar Txd3TmpBuffer[TXD3_MAX];
// extern uint Cw_Txd3TmpMax;
// extern uchar B_GprsDataReturn; // GPRS 数据返回。有连接，收到串口2数据，就通过DTU发送出去
// extern u16 Cw_Txd2Max;		   // 有多少个字符需要发送
// extern u8 Rcv2Buffer[];		   // 接收缓冲区
// extern u16 Cw_Rcv2;			   // 接收计数器
// extern u16 Cw_Txd2;			   // 发送计数器
// extern u16 Cw_BakRcv2;		   // 接收计数器备份

// u8 BakRcv2Count;
// u8 S_HongDianAcesse;
// u8 F_DTUSendRegister;
// extern uchar B_RestartNo; // ZCL 2019.4.19  重启序号，方便打印观察问题
// extern uint C_Com3NoRcv;  // COM0 没有接收网络数据计数器 ZCL 2017.5.30
// extern uc8 GPRS_ZhenTou[];
//// u8* DTURegisterNUM1;
//// u8* DTUEND;
// extern uchar *ptr, *ptr2, *ptr3; // 指针
// extern uchar B_Com3Cmd8B;		 // 查询DTU参数命令 2012.8.17
// extern uchar B_Com3Cmd8D;		 // 设置DTU参数命令 2012.8.24
// extern uchar B_Com3Cmd89;		 // 服务器发送数据指令 2011.8.24
// extern uchar B_OtherCmd;		 // 上位机通过DTU给串口2的其他指令（需发送到串口2）
// extern uchar ZhuCeOkFLAG[];		 // 注册成功标志
// extern uchar ChannelNo;			 // 应答通道,改成数值
// extern char *tcp_ip;
// extern uint8_t lte_send_cmd(char *cmd, char *res, uint32_t timeOut, uint8_t retime);
// extern void Gprs_TX_Fill(uc8 *String, u8 Length);
// extern void Gprs_TX_Byte(u8 i);
// typedef void (*pFunction)(void); // 定义一个函数类型的参数

//// 临时
//// u8 COM2_DATA;
// u8 LEDGPRS;
//// u8 PWR_LIGHT;

// void IWDG_Feed(void); // 独立看门狗喂狗
// void delay_ms(vu16 nCount);
// void SPI_FLASH_WriteEnable(void);
//// void SPI_FLASH_BufferWrite(void);
// void DTUIDInformation(void);
// void MakeInsertData(u8 *a, u16 aCount, u16 pos, u8 *b, u16 bCount);
// void SPI_FMRAM_BufferWrite(u8 *pBuffer, u16 WriteAddr, u16 NumByteToWrite);
// void UART1_Output_Datas(uint8_t S_Out[], uint8_t Counter);
// void UART2_Output_Datas(uint8_t S_Out[], uint8_t Counter);
// void UART3_Output_Datas(uint8_t S_Out[], uint16_t Counter);
// void usart3_send_string(unsigned char *str, unsigned short len);
// void lte_clear(void);
// void Fill_data(void);
// extern void LCD_DLY_ms(u32 Time);
// extern u16 CRC16(u8 *pCrcData, u8 CrcDataLen);
// extern void Com3_SlaveSend(void);
// void Com2_printf(char *fmt, ...)
//{
//	uint16_t txd_max;
//	va_list ap;
//	va_start(ap, fmt);
//	vsprintf((char *)Txd3Buffer, fmt, ap); // vsprintf   sprintf
//	va_end(ap);
//	// DMA方式发送
//	//	while(DMA1_Channel4->CNDTR!=0);	//等待通道4传输完成
//	//	UART_DMA_Enable(DMA1_Channel4,strlen((const char*)Txd3Buffer)); //启动DMA发送数据发送

//	// 中断方式发送
//	//	Cw_Txd3 = 0;
//	//	Cw_Txd3Max=strlen((const char*)Txd3Buffer);
//	//	USART_SendData(USART3, Txd3Buffer[Cw_Txd3++]);
//	//	USART_ITConfig(USART3, USART_IT_TC, ENABLE); // 开始发送.

//	// 堵塞方式发送
//	txd_max = strlen((const char *)Txd3Buffer);
//	UART3_Output_Datas(Txd3Buffer, txd_max);
//}

// unsigned int atoi(unsigned char *s, unsigned char sz)
//{
//	unsigned int i;
//	unsigned int n = 0;
//	unsigned char *p;
//	p = s;
//	if (sz != 0) // 长度不等于0
//	{
//		for (i = 0; i < sz; i++)
//		{
//			if (*(p + i) >= '0' && *(p + i) <= '9')
//			{
//				n = 10 * n + (*(p + i) - '0');
//			}
//		}
//		return n;
//	}
//	else // 否则返回为空
//		return NULL;
// }

// void reboot(void)
//{
//	pFunction Jump_To_Application;
//	u32 JumpAddress;
//	JumpAddress = *(vu32 *)(0x00000004);
//	Jump_To_Application = (pFunction)JumpAddress;
//	Jump_To_Application();
// }

// void HongDianProtocolRegister(void)
//{
//	u16 Writelink1OnlineReportTime, Writelink2OnlineReportTime, Writelink3OnlineReportTime, Writelink4OnlineReportTime; // 上电记录下心跳时间
//	if (S_HongDianAcesse == 1)																							// 注册
//	{
//		F_PowerOnRegister = 0;								   // 开始注册
//		Writelink1OnlineReportTime = Pw_link1OnlineReportTime; // 上电记录下心跳时间
//		Writelink2OnlineReportTime = Pw_link2OnlineReportTime; // 上电记录下心跳时间
//		Writelink3OnlineReportTime = Pw_link3OnlineReportTime; // 上电记录下心跳时间
//		Writelink4OnlineReportTime = Pw_link4OnlineReportTime; // 上电记录下心跳时间
//		Pw_link1OnlineReportTime = 1;						   // 上电立刻先发送心跳进行连接
//		Pw_link2OnlineReportTime = 1;						   // 上电立刻先发送心跳进行连接
//		Pw_link3OnlineReportTime = 1;						   // 上电立刻先发送心跳进行连接
//		Pw_link4OnlineReportTime = 1;						   // 上电立刻先发送心跳进行连接
//		while (!F_PowerOnRegister)
//			sim7600ce_SendHeartbeat(); // 发送模块心跳
//		if (F_PowerOnRegister == 1)
//		{
//			Pw_link1OnlineReportTime = Writelink1OnlineReportTime;
//			Pw_link2OnlineReportTime = Writelink2OnlineReportTime;
//			Pw_link3OnlineReportTime = Writelink3OnlineReportTime;
//			Pw_link4OnlineReportTime = Writelink4OnlineReportTime;
//			S_HongDianAcesse = 0;
//		}
//	}
// }

// void DTUhbeaterrPackets(void) // DTU设备心跳包（心跳包（同时也是上线注册包））
//{
//	u8 DTURegisterNUM1[4] = {0x7B, 0x01, 0x00, 0x16};
//	u8 DTUEND[1] = {0x7B};

//	DTUIDInformation();										 // 获取DTUID的内容信息
//	MakeInsertData(p_TmpBuf, 22, 0, DTURegisterNUM1, 4);	 // dtu注册前部分
//	MakeInsertData(p_TmpBuf, 22, 4, DTUIDBuf, 11);			 // dtu注册ID部分
//	MakeInsertData(p_TmpBuf, 22, 15, (u8 *)ModularADDIP, 4); // dtu注册本地IP地址部分
//	MakeInsertData(p_TmpBuf, 22, 19, local_port_Byte, 2);	 // dtu注册本地端口部分
//	MakeInsertData(p_TmpBuf, 22, 21, DTUEND, 1);			 // dtu注册后部分
//}

// void MakeInsertData(u8 *a, u16 aCount, u16 pos, u8 *b, u16 bCount)
//{
//	u16 i, j = 0;
//	u8 *p_Tmpa;
//	u8 *p_Tmpb;
//	p_Tmpa = a;
//	p_Tmpb = b;
//	for (i = pos; i <= (bCount + pos - 1); i++)
//	{
//		*(p_Tmpa + i) = *(p_Tmpb + j);
//		j++;
//		if (i > aCount)
//			break;
//	}
//	if (j >= bCount)
//		j = 0;
// }

// void DTUIDInformation(void)
//{
//	//	u8 i;
//	//	u16 hex,n=1;
//	//	hex=Pw_DTUIDNUM1;//第一个ID转换为字符
//	//	for (i=0;i<=3;i++)
//	//	{
//	//		n=n*10;
//	//		DTUIDBuf[i]=sim7600ce_hex2chr(hex/(10000/n));
//	//		hex=hex%(10000/n);
//	//	}
//	//	n=1;
//	//	hex=Pw_DTUIDNUM2;//第二个ID转换为字符
//	//	for (i=4;i<=7;i++)
//	//	{
//	//		n=n*10;
//	//		DTUIDBuf[i]=sim7600ce_hex2chr(hex/(10000/n));
//	//		hex=hex%(10000/n);
//	//	}
//	//	n=1;
//	//	hex=Pw_DTUIDNUM3;//第三个ID转换为字符
//	//	for (i=8;i<=10;i++)
//	//	{
//	//		n=n*10;
//	//		DTUIDBuf[i]=sim7600ce_hex2chr(hex/(1000/n));
//	//		hex=hex%(1000/n);
//	//	}

//	uchar m;
//	for (m = 0; m < 11; m++)
//		DTUIDBuf[m] = GprsPar[DtuNoBase + m];
//}

//// GPIO初始化
////
// void SIM7600CE_GPIO_Init(void)
//{
//	//	GPIO_InitTypeDef GPIO_InitStructure;
//	//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE); // PORTB时钟使能

//	////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;	// pb12 上电引脚；Pb13 复位引脚
//	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 					// pb12高电平>500ms，上电；pb13高电平>200ms，复位
//	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	////	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
//	//	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
//	//	GPIO_ResetBits(GPIOB, GPIO_Pin_13);

//	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Pa11 DTS
//	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	//	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
//}

//// usmart支持部分
//// 将收到的AT指令应答数据返回给电脑串口
//// mode:0,不清零USART3_RX_STA;
////      1,清零 USART3_RX_STA;
// void sim_at_response(u8 mode)
//{
//	//	if (USART3_RX_STA & 0X8000) //接收到一次数据了
//	if (usart3_rev_finish) // YLS 2023.02.08
//	{
//		//		Rcv3Buffer[USART3_RX_STA & 0X7FFF] = 0; //添加结束符
//		printf("%s", Rcv3Buffer);
//		if (mode)
//		{
//			USART3_RX_STA = 0;
//			Cw_Rcv3 = 0; // YLS 2023.02.08
//			usart3_rev_finish = 0;
//		}
//	}
// }

//// sim7600ce发送命令后,检测接收到的应答
//// str:期待的应答结果
//// 返回值:0,没有得到期待的应答结果
//// 其他,期待应答结果的位置(str的位置)
// u8 *sim7600ce_check_cmd(u8 *str)
//{
//	char *strx = 0;
//	//	if (USART3_RX_STA & 0X8000)
//	if (usart3_rev_finish) // YLS 2023.02.08
//	{
//		//		Rcv3Buffer[USART3_RX_STA & 0X7FFF] = 0; //添加结束符
//		Rcv3Buffer[Cw_BakRcv3] = 0; // 添加结束符
//		strx = strstr((const char *)Rcv3Buffer, (char *)str);
//	}
//	return (u8 *)strx;
// }
//// 向sim7600ce发送命令
////  cmd:发送的命令字符串
////  ack:期待的应答结果,如果为空,则表示不需要等待应答
////  waittime:等待时间(单位:10ms)
//// 返回值:0,发送成功(得到了期待的应答结果)
////        1,发送失败
//// u8 sim7600ce_send_cmd(u8 *cmd, u8 *ack, u16 waittime)
////{
////	u8 res = 0;
////	u8 C_Init_PwrLed_changeTime = 0; //初始化时PWRLED灯交换时间

////	USART3_RX_STA = 0;
////	C_Init_PwrLed_changeTime = 0; //初始化时PWRLED灯交换时间清“0”

//////	if ((u32)cmd <= 0XFF)
//////	{
//////		while (DMA1_Channel4->CNDTR != 0)
//////			; //等待传输完成
//////		USART3->DR = (u32)cmd;
//////	}
//////	else
//////		Com2_printf("%s\r\n", cmd); //发送命令到

//////	if (waittime == 1100) // 11s后读回串口数据(蓝牙扫描模式)
//////	{
//////		Scan_Wtime = 11; //需要定时的时间
//////						 //		 TIM4_SetARR(9999);//产生1S定时中断
//////		TIM4->ARR = 9999;
//////	}
////
////	printf("%s",cmd);
////	Com2_printf("%s\r\n", cmd); //发送命令到
////
////	if (ack && waittime) //需要等待应答
////	{
////		while (--waittime) //等待倒计时
////		{
////			if (F_RunAgain == 1)
////			{
////				COM2_DATA = 0;
////				LEDGPRS = 0;
////				C_Init_PwrLed_changeTime++;
////				if (C_Init_PwrLed_changeTime >= 40)	//快闪
////				{
////					PWR_LED_TOGGLE; //电源灯闪烁	快闪
////					C_Init_PwrLed_changeTime = 0;
////				}
////			}
////			else
////			{
////				C_Init_PwrLed_changeTime++;
////				if (C_Init_PwrLed_changeTime >= 100)	//慢闪
////				{
//////					LEDGPRS = ~LEDGPRS; //电源灯闪烁
////					PWR_LED_TOGGLE; //电源灯闪烁	慢闪
////					C_Init_PwrLed_changeTime = 0;
////					T_GPRSLEDTime = 100;
////					C_GPRSLEDTime = 0;
////				}
////			}
////			if (BakRcv2Count)
////				COM2_DATA = 0;			// COM有接收数据，关灯
////			delay_ms(10);				//每次等待倒计时进入延时10MS
////			IWDG_Feed();				//喂狗
//////			if (USART3_RX_STA & 0X8000) //接收到期待的应答结果
////			if(usart3_rev_finish)							                  // 如果串口接收完成
////			{
////				if (sim7600ce_check_cmd(ack))
////				{
////					printf("receive:%s",Rcv3Buffer);
////					lte_clear();										                // 清空缓存
////					return 0;
//////					break; //得到有效数据
////				}

////				USART3_RX_STA = 0;
////			}
////			lte_clear();										                // 清空缓存
////		}
////		if (waittime == 0) //超时喂狗
////		{
////			IWDG_Feed(); //喂狗
////			res = 1;
////		}
////	}
////	return res;
////}

//// 向sim7600ce发送命令
////  cmd:发送的命令字符串
////  ack:期待的应答结果,如果为空,则表示不需要等待应答
////  waittime:等待时间(单位:10ms)
//// 返回值:0,发送成功(得到了期待的应答结果)
////        1,发送失败
// u8 sim7600ce_send_cmd(u8 *cmd, u8 *ack, u16 waittime)
//{
//	//	uint8_t i = 0;
//	uint32_t timeout;
//	u8 C_Init_PwrLed_changeTime = 0; // 初始化时PWRLED灯交换时间
//	//	u8 *cmd1;
//	u8 cmd1[50] = {0};
//	//	for(i=0;i<retime;i++)
//	//	{

//	lte_clear();
//	timeout = waittime;
//	sprintf((char *)cmd1, "%s\r\n", cmd);
//	printf("%s", cmd1);

//	usart3_send_string((unsigned char *)cmd1, strlen((const char *)cmd1));
//	while (timeout--)
//	{
//		if (F_RunAgain == 1)
//		{
//			COM2_DATA = 0;
//			LEDGPRS = 0;
//			C_Init_PwrLed_changeTime++;
//			if (C_Init_PwrLed_changeTime >= 40) // 快闪
//			{
//				PWR_LED_TOGGLE; // 电源灯闪烁	快闪
//				C_Init_PwrLed_changeTime = 0;
//			}
//		}
//		else
//		{
//			C_Init_PwrLed_changeTime++;
//			if (C_Init_PwrLed_changeTime >= 100) // 慢闪
//			{
//				//					LEDGPRS = ~LEDGPRS; //电源灯闪烁
//				PWR_LED_TOGGLE; // 电源灯闪烁	慢闪
//				C_Init_PwrLed_changeTime = 0;
//				T_GPRSLEDTime = 100;
//				C_GPRSLEDTime = 0;
//			}
//		}
//		if (usart3_rev_finish) // 如果串口接收完成
//		{
//			if (strstr((const char *)Rcv3Buffer, (char *)ack) != NULL) // 如果检索到关键词
//			{
//				printf("receive:%s", Rcv3Buffer);
//				//					lte_clear();										                // 清空缓存
//				usart3_rev_finish = 0;
//				return 0;
//			}
//		}
//		//			delay_ms(10);
//		LCD_DLY_ms(10);
//	}
//	//		lte_clear();
//	//  }
//	return 1;
//}

///**
// * @brief  lte清空缓存
// * @param  无
// * @retval 无
// * @note   无
// */
// void lte_clear(void)
//{
//	memset(Rcv3Buffer, 0, 200);
//	//	Cw_Rcv3 = 0;
//}

///**
// * @brief  串口2发送字符串
// * @param  	str：要发送的数据
// *				    len：数据长度
// * @retval 无
// * @note   无
// */
// void usart3_send_string(unsigned char *str, unsigned short len)
//{
//	uint8_t count = 0;
//	for (; count < len; count++)
//	{
//		USART_SendData(USART3, *str++); // 发送数据
//		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
//			; // 等待发送完成
//	}
//}

//// 接收SIM7600ce返回数据（蓝牙测试模式下使用）
////  request:期待接收命令字符串
////  waittimg:等待时间(单位：10ms)
//// 返回值:0,发送成功(得到了期待的应答结果)
////        1,发送失败
// u8 sim7600ce_wait_request(u8 *request, u16 waittime)
//{
//	u8 res = 1;

//	if (request && waittime)
//	{
//		while (--waittime)
//		{
//			//			if (USART3_RX_STA & 0x8000) //接收到期待的应答结果
//			if (usart3_rev_finish) // YLS 2023.02.08
//			{
//				if (sim7600ce_check_cmd(request))
//					break; // 得到有效数据
//				USART3_RX_STA = 0;
//			}
//		}
//		if (waittime == 0)
//			res = 0;
//	}
//	return res;
//}

//// 将1个字符转换为16进制数字
////  chr:字符,0~9/A~F/a~F
//// 返回值:chr对应的16进制数值
// u8 sim7600ce_chr2hex(u8 chr)
//{
//	if (chr >= '0' && chr <= '9')
//		return chr - '0';
//	if (chr >= 'A' && chr <= 'F')
//		return (chr - 'A' + 10);
//	if (chr >= 'a' && chr <= 'f')
//		return (chr - 'a' + 10);
//	return 0;
// }
//// 将1个16进制数字转换为字符
////  hex:16进制数字,0~15;
//// 返回值:字符
// u8 sim7600ce_hex2chr(u8 hex)
//{
//	if (hex <= 9)
//		return hex + '0';
//	if (hex >= 10 && hex <= 15)
//		return (hex - 10 + 'A');
//	return '0';
// }

//// NTP网络同步时间//网络同步时间
// void ntp_update(void)
//{
//	sim7600ce_send_cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 200); // 配置承载场景1
//	sim7600ce_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"", "OK", 200);
//	sim7600ce_send_cmd("AT+SAPBR=1,1", 0, 200); // 激活一个GPRS上下文
//	delay_ms(5);
//	sim7600ce_send_cmd("AT+CNTPCID=1", "OK", 200);				   // 设置CNTP使用的CID
//	sim7600ce_send_cmd("AT+CNTP=\"202.120.2.101\",32", "OK", 200); // 设置NTP服务器和本地时区(32时区 时间最准确)
//	sim7600ce_send_cmd("AT+CNTP", "+CNTP: 1", 600);				   // 同步网络时间
// }

//// Modular(模块)信息显示(制造商,模块名称,产品序列号)
// void sim7600ce_Modular_Information(void)
//{
//	u8 *p, *p1;
//	p = (u8 *)mymalloc(50); // 申请50个内存
//	USART3_RX_STA = 0;
//	if (sim7600ce_send_cmd("AT+CGMI", "OK", 200) == 0) // 查询制造商名称
//	{
//		p1 = (u8 *)strstr((const char *)(Rcv3Buffer + 2), "\r\n");
//		p1[0] = 0; // 加入结束符
//		sprintf((char *)p, "制造商:%s", Rcv3Buffer + 2);
//		printf("%s", p); //
//		USART3_RX_STA = 0;
//		lte_clear();
//	}
//	if (sim7600ce_send_cmd("AT+CGMM", "OK", 200) == 0) // 查询模块名字
//	{
//		p1 = (u8 *)strstr((const char *)(Rcv3Buffer + 2), "\r\n");
//		p1[0] = 0; // 加入结束符
//		sprintf((char *)p, "模块型号:%s", Rcv3Buffer + 2);
//		printf("%s", p); //
//		USART3_RX_STA = 0;
//		lte_clear();
//	}
//	if (sim7600ce_send_cmd("AT+CGSN", "OK", 200) == 0) // 查询产品序列号
//	{
//		p1 = (u8 *)strstr((const char *)(Rcv3Buffer + 2), "\r\n"); // 查找回车
//		p1[0] = 0;												   // 加入结束符
//		sprintf((char *)p, "序列号:%s", Rcv3Buffer + 2);
//		printf("%s", p); //
//		USART3_RX_STA = 0;
//		lte_clear();
//	}
//	myfree(p);
// }
//// GSM信息显示(查询SIM卡,运营商名字,信号质量,电池电量,日期时间)
//// 返回值:0,正常
//// 其他,错误代码
// u8 sim7600ce_GSM_Information(void)
//{
//	u8 *p, *p1, *p2;
//	u8 res = 0;
//	u8 i;
//	p = (u8 *)mymalloc(50); // 申请50个内存
//	USART3_RX_STA = 0;

//	if (sim7600ce_send_cmd("AT+COPS?", "OK", 200) == 0) // 查询运营商名字
//	{
//		p1 = (u8 *)strstr((const char *)(Rcv3Buffer), "\"");
//		if (p1) // 有有效数据
//		{
//			p2 = (u8 *)strstr((const char *)(p1 + 1), "\"");
//			if (p2)
//				p2[0] = 0; // 加入结束符
//						   //  sprintf((char*)p,"运营商:%s",p1+1);
//						   // printf("%s",p);//
//		}
//	}
//	else
//		res |= 1 << 1;

//	lte_clear();
//	if (res == 0)
//	{
//		if (sim7600ce_send_cmd("AT+CSQ", "+CSQ:", 200) == 0) // 查询信号质量
//		{
//			p1 = (u8 *)strstr((const char *)(Rcv3Buffer), ":");
//			p2 = (u8 *)strstr((const char *)(p1), ",");
//			if (p2)
//				p2[0] = 0; // 加入结束符
//			//			sprintf((char *)CSQBuf, "%s", p1 + 2); //返回信号质量 （EDGE）
//			if (p1)
//			{
//				for (i = 0; i < 2; i++)
//				{
//					CSQBuf[i] = *(p1 + 2);
//					p1++;
//				}
//				w_GprsCSQ = atoi(CSQBuf, 2);
//				//			sprintf((char*)p,"信号质量:%s",p1+2);
//				//				printf("%s",p);//
//				printf("信号质量:%d", w_GprsCSQ);
//				USART3_RX_STA = 0;
//				lte_clear();
//			}
//			lte_clear();
//		}
//		else
//			res |= 1 << 2;
//	}
//	else
//	{
//		printf("%s", "找不到移动网络！！！"); //
//	}
//	myfree(p);
//	lte_clear();
//	return res;
//}
//// 信息显示(当前链接的网络)
// void sim7600ce_Netlink_Information(void)
//{
//	u8 CheckNet[] = {"AT+CPSI?"}; // 查询模块所处网络。
//	USART3_RX_STA = 0;

//	while (DMA1_Channel4->CNDTR != 0)
//		;						   // 等待通道4传输完成
//	Com2_printf("%s\r\n", CheckNet); // 查询模块所处网络
//	delay_ms(50);
//	//	if (USART3_RX_STA & 0X8000) //接收到应答结果
//	if (usart3_rev_finish) // YLS 2023.02.08
//	{
//		if (sim7600ce_check_cmd("+CPSI: LTE")) // 4G
//		{

//			F_NowNetworkSystem = 4; // 选择4G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于4G网络\r\n"); // 当前处于4G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: GSM")) // 2G
//		{

//			F_NowNetworkSystem = 2; // 选择2G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于2G网络\r\n"); // 当前处于2G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: CDMA")) // 2G电信
//		{

//			F_NowNetworkSystem = 2; // 选择2G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于电信2G网络\r\n"); // 当前处于2G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: WCDMA")) // 3G
//		{

//			F_NowNetworkSystem = 3; // 选择3G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于3G网络\r\n"); // 当前处于3G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: CDMA2000")) // 3G电信
//		{

//			F_NowNetworkSystem = 3; // 选择3G制式电信
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于电信3G网络\r\n"); // 当前处于3G网络	电信
//		}
//		else if (sim7600ce_check_cmd("+CPSI: NO SERVICE")) // 没有服务
//		{
//			F_NowNetworkSystem = 0; // 没有服务
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.没有查询到网络，请重试\r\n");
//		}
//		else
//		{
//			F_NowNetworkSystem = 1; // 其他网络
//		}
//		USART3_RX_STA = 0;
//	}
//}
//// 关闭网络
// u8 close_net(void)
//{
//	// 关闭网络
//	if (sim7600ce_send_cmd("AT+NETCLOSE", "+NETCLOSE:", 300) == 0)
//	{
//		if (Pw_SupportDataReceipt == 1)
//			printf("* 关闭网络 *\r\n");
//	}
//	else
//	{
//		// printf("关闭网络失败\r\n");
//		return 0;
//	}
//	return 1;
// }
//// 退出数据模式
// void exit_data_mode(void)
//{
//	unsigned char cmd[] = {"+++"}; // 发送“+++”，退出透传数据模式。

//	delay_ms(1000);
//	IWDG_Feed();			  // 喂狗
//	PWR_LED_TOGGLE;			  // 电源灯闪烁
//	Com2_printf("%s\r\n", cmd); // 发送命令到com1
//	delay_ms(1000);
//	IWDG_Feed(); // 喂狗
//}

//// 选择网络
// u8 select_net(void)
//{
//	u8 i;
//	u8 DianXinAPN[] = {"CTNET"};				 // 电信接入点
//	unsigned char CheckNet[] = {"AT+CPSI?"};	 // 查询模块所处网络。
//	unsigned char CheckNetSet[] = {"AT+CGATT?"}; // 查询模块是够选择成功。

//	// 自动选择切换网络部分
//	if (sim7600ce_send_cmd(NetAuto, "OK", 200) == 0)
//	{
//		if (Pw_SupportDataReceipt == 1)
//			printf("2.自动选择网络成功\r\n");
//	}
//	else
//	{
//		if (Pw_SupportDataReceipt == 1)
//			printf("2.自动选择网络失败\r\n");
//		return 0;
//	}
//	for (i = 0; i <= 30; i++) // 选择网络后，会注册网络，所需时间会长一点,可根据实际情况修改15s
//	{
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(500);
//		IWDG_Feed(); // 喂狗
//	}
//	// 检查网络是否选择成功
//	for (i = 0; i <= 20; i++) // 查看20次
//	{
//		if (sim7600ce_send_cmd(CheckNetSet, "+CGATT: 1", 200) == 0)
//		{
//			if (Pw_SupportDataReceipt == 1)
//				printf("2.GPRS附着网络成功\r\n");
//			i = 100; // 只要成功选择网络就跳出查看。
//			Select_NetNum = 0;
//		}
//		else
//		{
//			if (Pw_SupportDataReceipt == 1)
//				printf("2.GPRS附着网络失败\r\n");

//			PWR_LED_TOGGLE; // 电源灯闪烁
//			delay_ms(500);
//			IWDG_Feed(); // 喂狗
//						 //	Select_NetNum++;
//			if (i >= 20)
//			{
//				Select_NetNum = 0;
//				return 0;
//			}
//		}
//	}

//	// 查询模块所处网络（选择完毕后，检查模块是否处在连接选择的网络上）
//	USART3_RX_STA = 0;
//	//	while (DMA1_Channel4->CNDTR != 0)
//	//		;						   //等待通道4传输完成
//	Com2_printf("%s\r\n", CheckNet); // 查询模块所处网络

//	for (i = 0; i <= 16; i++) // 等待返回结果
//	{
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(500);
//		IWDG_Feed(); // 喂狗
//		//		if (USART3_RX_STA & 0X8000)
//		if (usart3_rev_finish) // YLS 2023.02.08
//			break;			   // 接收到应答结果,不在等待跳出循环
//	}
//	//	if (USART3_RX_STA & 0X8000) //接收到应答结果
//	if (usart3_rev_finish) // YLS 2023.02.08
//	{
//		if (sim7600ce_check_cmd("+CPSI: LTE")) // 4G
//		{

//			F_NowNetworkSystem = 4; // 选择4G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于4G网络\r\n"); // 当前处于4G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: GSM")) // 2G
//		{

//			F_NowNetworkSystem = 2; // 选择2G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于2G网络\r\n"); // 当前处于2G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: CDMA")) // 2G电信
//		{

//			F_NowNetworkSystem = 2; // 选择2G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于电信2G网络\r\n"); // 当前处于2G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: WCDMA")) // 3G
//		{

//			F_NowNetworkSystem = 3; // 选择3G制式
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于3G网络\r\n"); // 当前处于3G网络
//		}
//		else if (sim7600ce_check_cmd("+CPSI: CDMA2000")) // 3G电信
//		{
//			F_NowNetworkSystem = 3; // 选择3G制式电信
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.当前处于电信3G网络\r\n"); // 当前处于3G网络	电信
//		}
//		else if (sim7600ce_check_cmd("+CPSI: NO SERVICE")) // 没有服务
//		{
//			F_NowNetworkSystem = 0; // 没有服务
//			if (Pw_SupportDataReceipt == 1)
//				printf("3.没有查询到网络，请重试\r\n");

//			USART3_RX_STA = 0;
//			Cw_Rcv3 = 0;
//			usart3_rev_finish = 0; // YLS 2023.02.08
//			return 0;
//		}
//		else
//		{
//			F_NowNetworkSystem = 1; // 其他网络
//		}
//		USART3_RX_STA = 0;
//		Cw_Rcv3 = 0;
//		usart3_rev_finish = 0; // YLS 2023.02.08
//	}

//	// 如果电信的接入点就不进行注册了，直接连接
//	if (strstr((const char *)DianXinAPN, (const char *)APN_Name) == 0) // 说明不是电信的接入点
//	{
//		// 查询是否注册成功    （注册成功表示已经连接想要选择的网络）
//		if (sim7600ce_send_cmd("AT+CREG?", "+CREG: 0,1", 1000) == 0)
//		{
//			if (Pw_SupportDataReceipt == 1)
//				printf("4.网络注册成功\r\n");
//		}
//		else
//		{
//			if (Pw_SupportDataReceipt == 1)
//				printf("4.网络注册失败\r\n");

//			return 0;
//		}
//	}
//	return 1;
//}
//// 获得模块IP，返回1为获取IP错误。返回0为获取IP正确
////  cmd发送的字符串指令
////  waittime等到时间
//// 胡振广 2018.08.23 12：16
// u8 sim7600ce_send_Modular_IP(u8 *cmd, u16 waittime) // 获得模块IP
//{
//	u8 i, res = 0, num = 0, num1 = 0;
//	u8 cmd1[50] = {0};
//	USART3_RX_STA = 0;

//	sprintf((char *)cmd1, "%s\r", cmd);
//	printf("%s", cmd1);
//	usart3_send_string((unsigned char *)cmd1, strlen((const char *)cmd1));

//	//	Com2_printf("%s\r\n", cmd); //
//	if (waittime) // 需要等待应答
//	{
//		while (--waittime) // 等待倒计时
//		{
//			delay_ms(10); // 每次等待倒计时进入延时10MS
//			IWDG_Feed();  // 喂狗
//			//			if (USART3_RX_STA & 0X8000)
//			if (usart3_rev_finish) // YLS 2023.02.08
//			{
//				if (sim7600ce_check_cmd("ERROR"))
//				{
//					USART3_RX_STA = 0; // 检查发现返回的数据出现错误
//					Cw_Rcv3 = 0;
//					usart3_rev_finish = 0; // YLS 2023.02.08
//				}
//				else // 得到有效数据
//				{
//					for (i = 2; i <= 25; i++) // 把得到的正确数据进行转换保存
//					{
//						if ((Rcv3Buffer[i] == 0x2E) || (Rcv3Buffer[i] == 0x0D))
//						{
//							ModularADDIP[num1] = (u8)atoi(&Rcv3Buffer[i - num], num); //	转换对应的IP地址
//							if (num1 == 3)
//								break;
//							num1++; // 计算当前点（“.”）的个数
//							num = 0;
//						}
//						if (Rcv3Buffer[i] != 0x2E)
//							num++;
//					}
//					break; // 转换结束跳出while循环
//				}
//			}
//		}
//		if (waittime == 0)
//			res = 1; // 等待时间到返回错误
//	}
//	return res; // 如果未到达等待时间返回正确
//}
//// SIM7600ce初始化
////
// void SIM7600ce_Init(void)
//{
//	u8 Set_GPRS_APN[60];
//	u8 i = 0, j;

//	//	for (j = 0; j <= 14; j++)
//	//		APN_Name[j] = Pw_ParLst_GPRS[j + 8]; //接入点
//	for (j = 0; j <= 24; j++)
//		APN_Name[j] = GprsPar[j + AccessPointBase]; // 接入点

//	//	for (j = 0; j <= 24; j++)
//	//		domain_name[j] = Pw_ParLst_GPRS[j + 107]; //连接1 域名
//	//	for (j = 0; j <= 24; j++)
//	//		domain_name1[j] = Pw_ParLst_GPRS[j + 132]; //连接2 域名
//	//	for (j = 0; j <= 24; j++)
//	//		domain_name2[j] = Pw_ParLst_GPRS[j + 157]; //连接3 域名
//	//	for (j = 0; j <= 24; j++)
//	//		domain_name3[j] = Pw_ParLst_GPRS[j + 182]; //连接4 域名

//	for (j = 0; j <= 30; j++)
//		domain_name[j] = GprsPar[j + DomainName0Base]; // 连接1 域名
//	for (j = 0; j <= 30; j++)
//		domain_name1[j] = GprsPar[j + DomainName1Base]; // 连接2 域名
//	for (j = 0; j <= 30; j++)
//		domain_name2[j] = GprsPar[j + DomainName2Base]; // 连接3 域名
//	for (j = 0; j <= 30; j++)
//		domain_name3[j] = GprsPar[j + DomainName3Base]; // 连接4 域名

//	while (F_RunAgain == 1) // 重新运行
//	{
//		//		GPIO_SetBits(GPIOB, GPIO_Pin_13); //PB13 复位引脚，延时至少2.5秒
//		RESET_4G_ON;

//		COM2_DATA = 0;
//		LEDGPRS = 0;
//		delay_ms(400);
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(400);
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(400);
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(400);
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(400);
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(400);
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		delay_ms(400);
//		PWR_LED_TOGGLE; // 电源灯闪烁
//		IWDG_Feed();	// 喂狗

//		//		GPIO_ResetBits(GPIOB, GPIO_Pin_13); //	REST
//		RESET_4G_OFF;
//		for (i = 0; i <= 10; i++) // 延时等待模块稳定
//		{
//			PWR_LED_TOGGLE; // 电源灯闪烁
//			delay_ms(400);
//			IWDG_Feed(); // 喂狗
//		}

//		exit_data_mode(); // 退出透传模式

//		while (sim7600ce_send_cmd("AT", "OK", 30)) // 检测是否应答AT指令
//		{
//			PWR_LED_TOGGLE; // 电源灯闪烁
//			delay_ms(200);	//"尝试重新连接模块..."
//			IWDG_Feed();	// 喂狗
//			PowerOnOFFWaitNum++;
//			if (PowerOnOFFWaitNum >= 25) // 5秒
//			{
//				//				reboot(); //系统重启	YLS
//			}
//		}
//		PowerOnOFFWaitNum = 0;
//		close_net(); // 关闭网络，避免后面因为网络已经开启，导致打开网络失败
//		delay_ms(5);

//		if (Pw_SupportDataReceipt == 1) // 调试版本信息
//		{
//			printf("模组芯片版本信息:\r\n");
//			sim7600ce_send_cmd("AT+SIMCOMATI", "OK", 200); // 查询模块版本信息
//			sim_at_response(0);							   // 打印信息
//			printf("\r\n");
//		}
//		/****************************************************************************
//		可以增加以下两条指令：
//		这两条指令的意思是打开中国移动的FDD频段，因为在这之前中国移动没有FDD的牌照，
//		只有TDD的牌照，这两条指令不会让模块锁定在4G频段。
//		胡振广  2020.12.04
//		***************************************************************************/
//		if (Pw_LastPacketIdleTime == 400) // 用最后包空闲时间间隔来判断是否锁定4G，=400表示锁定
//		{
//			sim7600ce_send_cmd("AT+CNBP=,0x000001E000000095", "OK", 200);											 // 避免实验网不能连接4G，使用该指令解决，厂家给   2020.05.14 	# 屏蔽TDD频段
//			sim7600ce_send_cmd("AT+CNV=/nv/item_files/modem/lte/rrc/cap/diff_fdd_tdd_fgi_enable,0,01,1", "OK", 200); // 避免实验网不能连接，使用该指令解决，厂家给   2020.05.14
//		}
//		// 5G增加后又增加BAND41频点，所以模块要增加band41频点
//		//		sim7600ce_send_cmd("AT+CNV=/nv/item_files/modem/lte/rrc/skip_unsupported_b41_meas,0,01,1", "OK", 200);				  //                        厂家给   2022.04.06
//		//		sim7600ce_send_cmd("AT+CNV=/nv/item_files/modem/lte/rrc/skip_unsupported_band_meas,0,0000000000010000,1", "OK", 200); //  厂家给   2022.04.06
//		// 如上两个指令是兼容移动新部署5G网络时新增的band41频点,都加到代码里.//2022.4.06

//		sim7600ce_send_cmd("ATE0", "OK", 200); // 关闭回显

//		// 查询SIM卡是否在位
//		if (S_check_status == 0)
//		{
//			if (sim7600ce_send_cmd("AT+CPIN?", "+CPIN: READY", 200) == 0) // 查询SIM卡是否在位
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("1.读卡成功\r\n");
//				S_check_status = 1;
//			}
//			else
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("1.读卡失败\r\n");
//				S_check_status = 0;
//			}
//		}

//		sprintf((char *)Set_GPRS_APN, "AT+CGDCONT=1,\"IP\",\"%s\"", APN_Name);
//		// 设置APN

//		if (sim7600ce_send_cmd(Set_GPRS_APN, "OK", 200) == 0)
//		{
//			if (Pw_SupportDataReceipt == 1)
//				printf("设置APN成功\r\n");
//		}
//		else
//		{
//			if (Pw_SupportDataReceipt == 1)
//				printf("APN设置出错\r\n");
//			S_check_status = 0;
//		}
//		// 选择网络
//		if (S_check_status == 1)
//		{
//			if (!select_net()) // 第一次选择好网路后，可直接去掉该函数
//			{
//				// printf("初始化配置有误，请检查\r\n");
//				S_check_status = 0;
//			}
//			else // 初始化配置通过，可以进行功能测试
//			{
//				PWR_LIGHT = 1; // 初始化配置通过，可以进行功能配置上线
//				S_check_status = 2;
//			}
//		}
//		// 该步骤前面已经设置完毕了，可以不设置也行，程序中没有做删除。
//		sprintf((char *)Set_GPRS_APN, "AT+CGDCONT=1,\"IP\",\"%s\"", APN_Name);
//		// 设置APN
//		if (S_check_status == 2)
//		{
//			if (sim7600ce_send_cmd(Set_GPRS_APN, "OK", 200) == 0)
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("5.设置APN\r\n");
//				S_check_status = 3;
//			}
//			else
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("5.APN设置出错\r\n");
//				S_check_status = 0;
//			}
//		}
//		// 激活启动场景
//		if (S_check_status == 3)
//		{
//			if (sim7600ce_send_cmd("AT+CSOCKSETPN=1", "OK", 200) == 0)
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("6.激活移动场景\r\n");
//				S_check_status = 4;
//			}
//			else
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("6.激活移动场景失败\r\n");
//				S_check_status = 0;
//			}
//		}
//		// 设置为非透传模式
//		if (S_check_status == 4)
//		{
//			if (sim7600ce_send_cmd("AT+CIPMODE?", "+CIPMODE: 0", 200) == 0) // 这里不能直接设置透传模式，避免因为重复设置导致ERROR
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("7.设置为非透传模式\r\n");
//				S_check_status = 5;
//			}
//			else
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("7.设置为非透传模式\r\n");

//				sim7600ce_send_cmd("AT+CIPMODE=0", "OK", 200);
//				S_check_status = 5;
//			}
//		}
//		// 打开网络
//		if (S_check_status == 5)
//		{
//			if (sim7600ce_send_cmd("AT+NETOPEN", "+NETOPEN: 0", 5000) == 0) // 不能重复操作，否则会ERROR
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("8.打开网络\r\n");
//				S_check_status = 6;
//			}
//			else
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("8.打开网络失败\r\n");
//				S_check_status = 0;
//			}
//		}
//		// 获取本地IP地址
//		if (S_check_status == 6)
//		{
//			if (sim7600ce_send_Modular_IP("AT+IPADDR\r\n", 500) == 0)
//			//			if (sim7600ce_send_cmd("AT+IPADDR", "+IPADDR:", 200) == 0)
//			{
//				if (Pw_SupportDataReceipt == 1)
//					printf("9.获得IP地址\r\n");
//				// 成功获取本地IP地址
//				S_check_status = 7; // 初始化过程号
//			}
//			else
//			{
//				// 获取失败本地IP地址
//				S_check_status = 0; // 初始化过程号清“0”
//			}
//		}
//		// 网络连接配置
//		if (S_check_status == 7)
//		{
//			sim7600ce_GSM_Information();
//			if (GPRSNetConfig(4) == 0) // 开始连接
//			{
//				F_RunAgain = 0;					  // 进入主程序
//				F_RunCheckModular = 0;			  // 运行过程中检查模块标志清0
//				S_check_status = 0;				  // 初始化过程号清“0”
//				S_HongDianAcesse = 1;			  // 进入注册程序
//				hbeaterrcnt = 0;				  // 重新检测心跳
//				SendDataErrorNum = 0;			  // 重新检测发送数据应答信号次数
//				connectsta = 0;					  // 连接状态清0
//				PWR_LIGHT = 1;					  // 灭
//				sim7600ce_connectstaModularNum(); // 获得模块需要连接的IP数量
//				HongDianProtocolRegister();		  // 所有的连接完毕上电先进行注册
//			}
//			else
//			{
//				// 网络连接失败重新启动
//				S_check_status = 0; // 初始化过程号清“0”
//			}
//		}
//	}
//}

//// 网络连接配置
// const u8 *modetbl[2] = {"UDP", "TCP"}; // 连接模式//modetbl:1:TCP测试;0,UDP测试)
// u8 GPRSNetConfig(u8 link_num)
//{
//	u8 localPortBuf[5] = {0}; // 本地端口缓存
//	u8 ipbuf[15];			  // IP缓存
//	u8 portBuf[5] = {0};	  // 远端端口缓存
//	u8 *p2, *p3;
//	u8 i = 0, k = 0;

//	// if(sim7600ce_send_cmd("AT+CGATT=1","OK",500))return 3;					//附着GPRS业务

//	if (sim7600ce_send_cmd("AT+CIPHEAD=1\r\n", "OK", 500))
//		return 4; // 设置接收数据显示IP头(方便判断数据来源)

//	if (link_num == 0 || link_num >= 4)
//	{
//		// 域名解析
//		//	if (domain_name[0] + domain_name[1] + domain_name[2] + domain_name[3] != 0) //第一路域名解析内容不能为0，否则不进行解析
//		if (GprsPar[DomainName0Base] + GprsPar[DomainName0Base + 1] + GprsPar[DomainName0Base + 2] + GprsPar[DomainName0Base + 3] != 0) // 第一路域名解析内容不能为0，否则不进行解析
//		{
//			//		sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", domain_name); //把域名信息给申请的空间p_TmpBuf
//			sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", (char *)GprsPar[DomainName0Base]); // 把域名信息给申请的空间p_TmpBuf
//			if (sim7600ce_send_cmd(p_TmpBuf, "+CDNSGIP: 1", 500))
//				; // 成功解析DNS
//			//		p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name); //接收解析的IP信息
//			p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)GprsPar[DomainName0Base]); // 接收解析的IP信息
//			if (p2)																				 // 解析正确
//			{
//				p3 = (u8 *)strstr((const char *)p2, ","); //
//				sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
//				for (i = 0; i <= 3; i++)				  // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link1IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
//						GprsPar[Ip0Base] = Pw_Link1IP1;		 // YLS 2022.12.27
//						k = i + 1;							 // IP下一位的值
//						break;								 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link1IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip0Base + 1] = Pw_Link1IP2;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link1IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip0Base + 2] = Pw_Link1IP3;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x22)
//					{
//						Pw_Link1IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip0Base + 3] = Pw_Link1IP4;		 // YLS 2022.12.27
//						k = 0;									 //
//						break;									 //
//					}
//				}
//			}
//		}

//		//		if ((Pw_Link1IP1 + Pw_Link1IP2 + Pw_Link1IP3 + Pw_Link1IP4) != 0) //只有第一路可以实现透明传输
//		if ((GprsPar[Ip0Base] + GprsPar[Ip0Base + 1] + GprsPar[Ip0Base + 2] + GprsPar[Ip0Base + 3]) != 0) // 只有第一路可以实现透明传输
//		{
//			Pw_Link1localPort = (GprsPar[LoclaPort0Base] << 8) + GprsPar[LoclaPort0Base + 1]; // YLS 2022.12.27
//			sprintf((char *)localPortBuf, "%d", Pw_Link1localPort);							  // 第一路链接本地端口字转换为ascii码
//			//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link1IP1, Pw_Link1IP2, Pw_Link1IP3, Pw_Link1IP4); //存放IP
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", GprsPar[Ip0Base], GprsPar[Ip0Base + 1], GprsPar[Ip0Base + 2], GprsPar[Ip0Base + 3]); // 存放IP
//			Pw_Link1Port = (GprsPar[Port0Base] << 8) + GprsPar[Port0Base + 1];														   // YLS 2022.12.27
//			sprintf((char *)portBuf, "%d", Pw_Link1Port);																			   // 第一路链接远端端口字转换为ascii码
//			//		if (Pw_Link1TCP_UDPType == 0)															   //第一路为UDP
//			if (GprsPar[LinkTCPUDP0Base] == 0) // 第一路为UDP
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=0,\"%s\",\"%s\",%s,%s", modetbl[(u8)GprsPar[LinkTCPUDP0Base]], ipbuf, portBuf, localPortBuf); // 把网络信息给申请的空间p_TmpBuf
//				// if(sim7600ce_send_cmd(p_TmpBuf,"CONNECT",500))	return 9;		//发起第1路连接（透明传输的时候用该条配置）
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 0,0", 500))
//				{
//					CGD0_LinkConnectOK[0] = FALSE;
//					return 9; // 发起第1路连接（配置，透明传输的时候注释掉）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[0] = TRUE;
//				}
//			}
//			else ////第一路为TCP Client
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=0,\"%s\",\"%s\",%s", modetbl[(u8)GprsPar[LinkTCPUDP0Base]], ipbuf, portBuf); // 把网络信息给申请的空间p_TmpBuf
//				//				printf("AT+CIPOPEN=0,\"%s\",\"%s\",%s", modetbl[(u8)GprsPar[LinkTCPUDP0Base]], ipbuf, portBuf); //把网络信息给申请的空间p_TmpBuf
//				// if(sim7600ce_send_cmd(p_TmpBuf,"CONNECT",500))	return 9;		//发起第1路连接（透明传输的时候用该条配置）
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 0,0", 500))
//				{
//					CGD0_LinkConnectOK[0] = FALSE;
//					return 9; // 发起第1路连接（配置，透明传输的时候注释掉）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[0] = TRUE;
//				}
//			}
//		}
//	}

//	if (link_num == 1 || link_num >= 4)
//	{
//		//	if (domain_name1[0] + domain_name1[1] + domain_name1[2] + domain_name1[3] != 0) //第二路域名解析内容不能为0，否则不进行解析
//		if (GprsPar[DomainName1Base] + GprsPar[DomainName1Base + 1] + GprsPar[DomainName1Base + 2] + GprsPar[DomainName1Base + 3] != 0) // 第一路域名解析内容不能为0，否则不进行解析
//		{
//			//		sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", domain_name1); //把域名信息给申请的空间p_TmpBuf
//			sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", (char *)GprsPar[DomainName1Base]); // 把域名信息给申请的空间p_TmpBuf
//			if (sim7600ce_send_cmd(p_TmpBuf, "+CDNSGIP: 1", 500))
//				; // 成功解析DNS
//			//		p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name1); //接收解析的IP信息
//			p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)GprsPar[DomainName1Base]); // 接收解析的IP信息
//			if (p2)																				 // 解析正确
//			{
//				p3 = (u8 *)strstr((const char *)p2, ","); //
//				sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
//				for (i = 0; i <= 3; i++)				  // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link2IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
//						GprsPar[Ip1Base] = Pw_Link2IP1;		 // YLS 2022.12.27
//						k = i + 1;							 // IP下一位的值
//						break;								 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link2IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip1Base + 1] = Pw_Link2IP2;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link2IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip1Base + 2] = Pw_Link2IP3;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x22)
//					{
//						Pw_Link2IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip1Base + 3] = Pw_Link2IP4;		 // YLS 2022.12.27
//						k = 0;									 //
//						break;									 //
//					}
//				}
//			}
//		}

//		//		if ((Pw_Link2IP1 + Pw_Link2IP2 + Pw_Link2IP3 + Pw_Link2IP4) != 0)
//		if (GprsPar[DomainName1Base] + GprsPar[DomainName1Base + 1] + GprsPar[DomainName1Base + 2] + GprsPar[DomainName1Base + 3] != 0)
//		{
//			Pw_Link2localPort = (GprsPar[LoclaPort1Base] << 8) + GprsPar[LoclaPort1Base + 1]; // YLS 2022.12.27
//			sprintf((char *)localPortBuf, "%d", Pw_Link2localPort);							  // 第二路链接本地端口字转换为ascii码
//			//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link2IP1, Pw_Link2IP2, Pw_Link2IP3, Pw_Link2IP4); //存放IP
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", GprsPar[Ip1Base], GprsPar[Ip1Base + 1], GprsPar[Ip1Base + 2], GprsPar[Ip1Base + 3]); // 存放IP

//			Pw_Link2Port = (GprsPar[Port1Base] << 8) + GprsPar[Port1Base + 1]; // YLS 2022.12.27
//			sprintf((char *)portBuf, "%d", Pw_Link2Port);					   // 第一路链接远端端口字转换为ascii码
//			//		if (Pw_Link2TCP_UDPType == 0)															   //第二路为UDP
//			if (GprsPar[LinkTCPUDP1Base] == 0) // 第二路为UDP
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=1,\"%s\",\"%s\",%s,%s", modetbl[(u8)GprsPar[LinkTCPUDP1Base]], ipbuf, portBuf, localPortBuf); // 把网络信息给申请的空间p_TmpBuf
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 1,0", 500))
//				{
//					CGD0_LinkConnectOK[1] = FALSE;
//					return 10; // 发起第2路连接（配置）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[1] = TRUE;
//				}
//			}
//			else ////第二路为TCP
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=1,\"%s\",\"%s\",%s", modetbl[(u8)GprsPar[LinkTCPUDP1Base]], ipbuf, portBuf); // 把网络信息给申请的空间p_TmpBuf
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 1,0", 500))
//				{
//					CGD0_LinkConnectOK[1] = FALSE;
//					return 10; // 发起第2路连接（配置）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[1] = TRUE;
//				}
//			}
//		}
//	}

//	if (link_num == 2 || link_num >= 4)
//	{
//		//	if (domain_name2[0] + domain_name2[1] + domain_name2[2] + domain_name2[3] != 0) //第三路域名解析内容不能为0，否则不进行解析
//		if (GprsPar[DomainName2Base] + GprsPar[DomainName2Base + 1] + GprsPar[DomainName2Base + 2] + GprsPar[DomainName2Base + 3] != 0) // 第三路域名解析内容不能为0，否则不进行解析
//		{
//			//		sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", domain_name2); //把域名信息给申请的空间p_TmpBuf
//			sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", (char *)GprsPar[DomainName2Base]); // 把域名信息给申请的空间p_TmpBuf
//			if (sim7600ce_send_cmd(p_TmpBuf, "+CDNSGIP: 1", 500))
//				; // 成功解析DNS
//			//		p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name2); //接收解析的IP信息
//			p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)GprsPar[DomainName2Base]); // 接收解析的IP信息
//			if (p2)																				 // 解析正确
//			{
//				p3 = (u8 *)strstr((const char *)p2, ","); //
//				sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
//				for (i = 0; i <= 3; i++)				  // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link3IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
//						GprsPar[Ip2Base] = Pw_Link3IP1;		 // YLS 2022.12.27
//						k = i + 1;							 // IP下一位的值
//						break;								 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link3IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip2Base + 1] = Pw_Link3IP2;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link3IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip2Base + 2] = Pw_Link3IP3;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x22)
//					{
//						Pw_Link3IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip2Base + 3] = Pw_Link3IP4;		 // YLS 2022.12.27
//						k = 0;									 //
//						break;									 //
//					}
//				}
//			}
//		}

//		//		if ((Pw_Link3IP1 + Pw_Link3IP2 + Pw_Link3IP3 + Pw_Link3IP4) != 0)
//		if (GprsPar[DomainName2Base] + GprsPar[DomainName2Base + 1] + GprsPar[DomainName2Base + 2] + GprsPar[DomainName2Base + 3] != 0)
//		{
//			Pw_Link3localPort = (GprsPar[LoclaPort2Base] << 8) + GprsPar[LoclaPort2Base + 1]; // YLS 2022.12.27
//			sprintf((char *)localPortBuf, "%d", Pw_Link3localPort);							  // 第三路链接本地端口字转换为ascii码
//			//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link3IP1, Pw_Link3IP2, Pw_Link3IP3, Pw_Link3IP4); //存放IP
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", GprsPar[Ip2Base], GprsPar[Ip2Base + 1], GprsPar[Ip2Base + 2], GprsPar[Ip2Base + 3]); // 存放IP
//			Pw_Link3Port = (GprsPar[Port2Base] << 8) + GprsPar[Port2Base + 1];														   // YLS 2022.12.27
//			sprintf((char *)portBuf, "%d", Pw_Link3Port);																			   // 第三路链接远端端口字转换为ascii码
//			if (Pw_Link3TCP_UDPType == 0)																							   // 第三路为UDP
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=2,\"%s\",\"%s\",%s,%s", modetbl[(u8)Pw_Link3TCP_UDPType], ipbuf, portBuf, localPortBuf); // 把网络信息给申请的空间p_TmpBuf
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 2,0", 500))
//				{
//					CGD0_LinkConnectOK[2] = FALSE;
//					return 11; // 发起第3路连接（配置）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[2] = TRUE;
//				}
//			}
//			else ////第三路为TCP
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=2,\"%s\",\"%s\",%s", modetbl[(u8)Pw_Link3TCP_UDPType], ipbuf, portBuf); // 把网络信息给申请的空间p_TmpBuf
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 2,0", 500))
//				{
//					CGD0_LinkConnectOK[2] = FALSE;
//					return 11; // 发起第3路连接（配置）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[2] = TRUE;
//				}
//			}
//		}
//	}

//	if (link_num == 3 || link_num >= 4)
//	{
//		//	if (domain_name3[0] + domain_name3[1] + domain_name3[2] + domain_name3[3] != 0) //第四路域名解析内容不能为0，否则不进行解析
//		if (GprsPar[DomainName3Base] + GprsPar[DomainName3Base + 1] + GprsPar[DomainName3Base + 2] + GprsPar[DomainName3Base + 3] != 0) // 第一路域名解析内容不能为0，否则不进行解析
//		{
//			//		sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", domain_name3); //把域名信息给申请的空间p_TmpBuf
//			sprintf((char *)p_TmpBuf, "AT+CDNSGIP =\"%s\"", (char *)GprsPar[DomainName3Base]); // 把域名信息给申请的空间p_TmpBuf
//			if (sim7600ce_send_cmd(p_TmpBuf, "+CDNSGIP: 1", 500))
//				; // 成功解析DNS
//			//		p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name3); //接收解析的IP信息
//			p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)GprsPar[DomainName3Base]); // 接收解析的IP信息
//			if (p2)																				 // 解析正确
//			{
//				p3 = (u8 *)strstr((const char *)p2, ","); //
//				sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
//				for (i = 0; i <= 3; i++)				  // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link4IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
//						GprsPar[Ip3Base] = Pw_Link4IP1;		 // YLS 2022.12.27
//						k = i + 1;							 // IP下一位的值
//						break;								 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link4IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip3Base + 1] = Pw_Link4IP2;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x2E)
//					{
//						Pw_Link4IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip3Base + 2] = Pw_Link4IP3;		 // YLS 2022.12.27
//						k = i + 1;								 // IP下一位的值
//						break;									 //
//					}
//				}
//				for (i = k; i <= 3 + k; i++) // 转化为16进制数
//				{
//					if (p_TmpBuf[i] == 0x22)
//					{
//						Pw_Link4IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
//						GprsPar[Ip3Base + 3] = Pw_Link4IP4;		 // YLS 2022.12.27
//						k = 0;									 //
//						break;									 //
//					}
//				}
//			}
//		}

//		//		if ((Pw_Link4IP1 + Pw_Link4IP2 + Pw_Link4IP3 + Pw_Link4IP4) != 0)
//		if (GprsPar[DomainName3Base] + GprsPar[DomainName3Base + 1] + GprsPar[DomainName3Base + 2] + GprsPar[DomainName3Base + 3] != 0)
//		{
//			Pw_Link4localPort = (GprsPar[LoclaPort3Base] << 8) + GprsPar[LoclaPort3Base + 1]; // YLS 2022.12.27
//			sprintf((char *)localPortBuf, "%d", Pw_Link4localPort);							  // 第四路链接本地端口字转换为ascii码
//			//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link4IP1, Pw_Link4IP2, Pw_Link4IP3, Pw_Link4IP4); //存放IP
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", GprsPar[Ip3Base], GprsPar[Ip3Base + 1], GprsPar[Ip3Base + 2], GprsPar[Ip3Base + 3]); // 存放IP

//			Pw_Link4Port = (GprsPar[Port3Base] << 8) + GprsPar[Port3Base + 1]; // YLS 2022.12.27
//			sprintf((char *)portBuf, "%d", Pw_Link4Port);					   // 第四路链接远端端口字转换为ascii码
//			if (Pw_Link4TCP_UDPType == 0)									   // 第四路为UDP
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=3,\"%s\",\"%s\",%s,%s", modetbl[(u8)Pw_Link4TCP_UDPType], ipbuf, portBuf, localPortBuf); // 把网络信息给申请的空间p_TmpBuf
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 3,0", 500))
//				{
//					CGD0_LinkConnectOK[3] = FALSE;
//					return 12; // 发起第4路连接（配置）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[3] = TRUE;
//				}
//			}
//			else ////第四路为TCP
//			{
//				sprintf((char *)p_TmpBuf, "AT+CIPOPEN=3,\"%s\",\"%s\",%s", modetbl[(u8)Pw_Link4TCP_UDPType], ipbuf, portBuf); // 把网络信息给申请的空间p_TmpBuf
//				if (sim7600ce_send_cmd(p_TmpBuf, "+CIPOPEN: 3,0", 500))
//				{
//					CGD0_LinkConnectOK[3] = FALSE;
//					return 12; // 发起第4路连接（配置）
//				}
//				else
//				{
//					CGD0_LinkConnectOK[3] = TRUE;
//				}
//			}
//		}
//	}

//	return 0;
//	//
//}

//// SIM7600ce运行过程中每隔一段时间检查网络 ，如果网络不正常重启动模块
////
// void sim7600ce_NetworkCheck(void)
//{
//	//	u8 res = 0;

//	//	if (F_RunAgain == 0 && T_NetworkCheck != SClkSecond) // 运行过程中每个一段时间检查网络
//	//	{
//	//		T_NetworkCheck = SClkSecond; //
//	//		C_NetworkCheck++;
//	//		if (C_NetworkCheck > Pw_Link1NetWorkCheckTime) // 运行过程中每隔20秒检查网络
//	//		{
//	//			T_NetworkCheck = 100;
//	//			C_NetworkCheck = 0;

//	//			if (sim7600ce_send_cmd("AT+CPIN?", "+CPIN: READY", 200))
//	//				res |= 1 << 0; // 查询SIM卡是否在位

//	//			if (sim7600ce_send_cmd("AT+COPS?", "OK", 200))
//	//				res |= 1 << 1; // 查询运营商名字
//	//			if (res != 0)
//	//			{
//	//				F_RunAgain = 1;		   // 重新运行模块
//	//				F_RunCheckModular = 1; // 运行过程中检查模块标志
//	//			}
//	//		}
//	//	}
//	//	else if (F_RunAgain == 1 && F_RunCheckModular == 1)
//	//	{
//	//		SIM7600ce_Init(); // sim7600ce初始化
//	//	}
//	if (F_RunAgain == 1 && F_RunCheckModular == 1)
//	{
//		SIM7600ce_Init(); // sim7600ce初始化
//	}
//}
//// sim7600ce GPRS数据发送与加载
//// tcp/udp数据发送与加载
//// 带心跳功能,以维持连接
//// SendDateBuf:发送的数据的BUF或字符串
//// SendBufLen:发送的数据长度
//// Star_OR_hex:发送的是16进制数还是字符串，=1是十六进制数；=0是字符串
//// 特别说明定长发送，数据中可以包含0x03，0x1A，0x1B
//// 胡振广 2019.05.14  10：49
// void sim7600ceGPRSUpLoaded(u8 *SendDateBuf, u16 SendBufLen, u8 Star_OR_hex)
//{
//	u8 ipbuf[15];			   // IP缓存
//	u8 portBuf[5] = {0};	   // 远端端口缓存
//	u8 p_SendTmpBuf[50] = {0}; // 缓存
//	u8 F_SendData = 0;		   // 发送数据标志

//	if (hbeaterrcnt <= 8 && !F_GPRSSendFinish) // 心跳<=8（正常的时候）且没有发送完成标志
//	{
//		if (F_AcklinkNum == 1)
//		{
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link1IP1, Pw_Link1IP2, Pw_Link1IP3, Pw_Link1IP4); // 存放IP
//			sprintf((char *)portBuf, "%d", Pw_Link1Port);											   // 第一路链接远端端口字转换为ascii码
//			if ((Pw_Link1TCP_UDPType == 0) && (F_UDP_OR_TCP == 0))									   // 第一路为UDP
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=0,%d,\"%s\",%s", SendBufLen, ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第1路发送
//			}
//			else if ((Pw_Link1TCP_UDPType == 1) && (F_UDP_OR_TCP == 1)) ////第一路为TCP Client
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=0,%d", SendBufLen);
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第1路发送
//			}

//			if (F_SendData == 1) // 发送数据
//			{
//				if (!F_DTUSendRegister)
//					COM2_DATA = 1; // 发送开始点亮指示灯(不是注册包)
//				if (Star_OR_hex)   //=1是十六进制数；=0是字符串
//				{
//					memcpy(Txd3Buffer, SendDateBuf, SendBufLen);
//					//					while(DMA1_Channel4->CNDTR!=0);	//等待传输完成
//					//					UART_DMA_Enable(DMA1_Channel4,SendBufLen); //发送出去
//					UART3_Output_Datas(Txd3Buffer, SendBufLen); // YLS 2023.02.10
//				}
//				else
//					Com2_printf("%s\r\n", SendDateBuf); // 发送
//				delay_ms(20);						  // 延时需要加
//				F_SendData = 0;						  // 发送标志清0
//				// 定长度发送不需要0X1A,"OK"或者“<Ctrl+Z>”
//				//	if(sim7600ce_send_cmd((u8*)0X1A,"OK",1000)==0)//检查第1路是否发生成功，为真就是数据发送成功!发送数据到远端服务, CTRL+Z (0x1a)发送
//				COM2_DATA = 0; // 发送结束关闭指示灯
//							   //	else //否则数据发送失败!
//				COM2_DATA = 0; // 发送结束关闭指示灯

//				Link1SendOK = 1;		  // 第1路发送成功。
//				C_SendHeartbeatTime0 = 0; // 心跳计数清零
//				C_SendHeartbeatTime = 0;  // 心跳计数清零
//			}
//			else // 数据发送失败
//			{
//				SendDataErrorNum++; // 超过8次，系统重启
//				Link1SendOK = 0;	// 第1路发送失败。
//			}
//		}
//		if (F_AcklinkNum == 2)
//		{
//			delay_ms(10);																			   // 延时需要加
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link2IP1, Pw_Link2IP2, Pw_Link2IP3, Pw_Link2IP4); // 存放IP
//			sprintf((char *)portBuf, "%d", Pw_Link2Port);											   // 第2路链接远端端口字转换为ascii码
//			if ((Pw_Link2TCP_UDPType == 0) && (F_UDP_OR_TCP == 0))									   // 第2路为UDP
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=1,%d,\"%s\",%s", SendBufLen, ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第2路发送
//			}
//			else if ((Pw_Link2TCP_UDPType == 1) && (F_UDP_OR_TCP == 1)) // 第2路为TCP Client
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=1,%d", SendBufLen);
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第2路发送
//			}
//			if (F_SendData == 1) // 发送数据
//			{
//				COM2_DATA = 1; // 发送开始点亮指示灯
//				if (Star_OR_hex)
//				{

//					memcpy(Txd3Buffer, SendDateBuf, SendBufLen);

//					//					while(DMA1_Channel4->CNDTR!=0);	//等待传输完成
//					//					UART_DMA_Enable(DMA1_Channel4,SendBufLen); //发送出去
//					UART3_Output_Datas(Txd3Buffer, SendBufLen); // YLS 2023.02.10
//				}
//				else
//					Com2_printf("%s\r\n", SendDateBuf); // 发送命令

//				delay_ms(20);	// 延时需要加
//				F_SendData = 0; // 发送标志清0
//				// 定长度发送不需要0X1A,"OK"
//				//	if(sim7600ce_send_cmd((u8*)0X1A,"OK",1000)==0)//检查第2路是否发生成功，为真就是数据发送成功!发送数据到远端服务, CTRL+Z (0x1a)发送
//				COM2_DATA = 0; // 发送结束关闭指示灯
//							   //	else //否则数据发送失败!
//				COM2_DATA = 0; // 发送结束关闭指示灯

//				Link2SendOK = 1;		  // 第2路发送成功。
//				C_SendHeartbeatTime0 = 0; // 心跳计数清零
//				C_SendHeartbeatTime = 0;  // 心跳计数清零
//			}
//			else // 数据发送失败
//			{
//				SendDataErrorNum++; // 超过8次，系统重启
//				Link2SendOK = 0;	// 第2路发送失败。
//			}
//		}
//		if (F_AcklinkNum == 3)
//		{
//			delay_ms(10);																			   // 延时需要加
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link3IP1, Pw_Link3IP2, Pw_Link3IP3, Pw_Link3IP4); // 存放IP
//			sprintf((char *)portBuf, "%d", Pw_Link3Port);											   // 第3路链接远端端口字转换为ascii码
//			if ((Pw_Link3TCP_UDPType == 0) && (F_UDP_OR_TCP == 0))									   // 第3路为UDP
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=2,%d,\"%s\",%s", SendBufLen, ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第3路发送
//			}
//			else if ((Pw_Link3TCP_UDPType == 1) && (F_UDP_OR_TCP == 1)) // 第3路为TCP Client
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=2,%d", SendBufLen);
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第3路发送
//			}
//			if (F_SendData == 1) // 发送数据
//			{
//				COM2_DATA = 1; // 发送开始点亮指示灯
//				if (Star_OR_hex)
//				{
//					memcpy(Txd3Buffer, SendDateBuf, SendBufLen);

//					//					while(DMA1_Channel4->CNDTR!=0);	//等待传输完成
//					//					UART_DMA_Enable(DMA1_Channel4,SendBufLen); //发送出去
//					UART3_Output_Datas(Txd3Buffer, SendBufLen); // YLS 2023.02.10
//				}
//				else
//					Com2_printf("%s\r\n", SendDateBuf); // 发送命令字

//				delay_ms(20);	// 延时需要加
//				F_SendData = 0; // 发送标志清0
//				// 定长度发送不需要0X1A,"OK"
//				//  if(sim7600ce_send_cmd((u8*)0X1A,"OK",1000)==0)//检查第3路是否发生成功，为真就是数据发送成功!发送数据到远端服务, CTRL+Z (0x1a)发送
//				COM2_DATA = 0; // 发送结束关闭指示灯
//				// else //否则数据发送失败!
//				COM2_DATA = 0; // 发送结束关闭指示灯

//				Link3SendOK = 1;		  // 第3路发送成功。
//				C_SendHeartbeatTime0 = 0; // 心跳计数清零
//				C_SendHeartbeatTime = 0;  // 心跳计数清零
//			}
//			else // 数据发送失败
//			{
//				SendDataErrorNum++; // 超过8次，系统重启
//				Link3SendOK = 0;	// 第3路发送失败。
//			}
//		}
//		if (F_AcklinkNum == 4)
//		{
//			delay_ms(10);																			   // 延时需要加
//			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link4IP1, Pw_Link4IP2, Pw_Link4IP3, Pw_Link4IP4); // 存放IP
//			sprintf((char *)portBuf, "%d", Pw_Link4Port);											   // 第4路链接远端端口字转换为ascii码
//			if ((Pw_Link4TCP_UDPType == 0) && (F_UDP_OR_TCP == 0))									   // 第4路为UDP
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=3,%d,\"%s\",%s", SendBufLen, ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第4路发送
//			}
//			else if ((Pw_Link4TCP_UDPType == 1) && (F_UDP_OR_TCP == 1)) // 第4路为TCP Client
//			{
//				sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=3,%d", SendBufLen);
//				if (sim7600ce_send_cmd(p_SendTmpBuf, ">", 200) == 0)
//					F_SendData = 1; // 发起第4路发送
//			}
//			if (F_SendData == 1) // 发送数据
//			{
//				COM2_DATA = 1; // 发送开始点亮指示灯
//				if (Star_OR_hex)
//				{
//					memcpy(Txd3Buffer, SendDateBuf, SendBufLen);

//					//					while(DMA1_Channel4->CNDTR!=0);	//等待传输完成
//					//					UART_DMA_Enable(DMA1_Channel4,SendBufLen); //发送出去
//					UART3_Output_Datas(Txd3Buffer, SendBufLen); // YLS 2023.02.10
//				}
//				else
//					Com2_printf("%s\r\n", SendDateBuf); // 发送命令

//				delay_ms(20);	// 延时需要加
//				F_SendData = 0; // 发送标志清0
//				// 定长度发送不需要0X1A,"OK"
//				//  if(sim7600ce_send_cmd((u8*)0X1A,"OK",1000)==0)//检查第4路是否发生成功，为真就是数据发送成功!发送数据到远端服务, CTRL+Z (0x1a)发送
//				COM2_DATA = 0; // 发送结束关闭指示灯
//				// else //否则数据发送失败!
//				COM2_DATA = 0; // 发送结束关闭指示灯

//				Link4SendOK = 1;		  // 第4路发送成功。
//				C_SendHeartbeatTime0 = 0; // 心跳计数清零
//				C_SendHeartbeatTime = 0;  // 心跳计数清零
//			}
//			else // 数据发送失败
//			{
//				SendDataErrorNum++; // 超过8次，系统重启
//				Link4SendOK = 0;	// 第4路发送失败。
//			}
//		}
//		if (Link1SendOK + Link2SendOK + Link3SendOK + Link4SendOK == ConnectIPNum) // 所有需要连接的都发送成功了
//		{
//			if (SendDataErrorNum != 0)
//				SendDataErrorNum--;
//		}
//		F_GPRSSendFinish = 1; // 发送完成，准备下次发送
//	}
//}
//// GPRS模块连接失败后重新连接
// void sim7600ce_ReconnectNet(void)
//{
//	if (((hbeaterrcnt > 4) || (SendDataErrorNum > 4)) && (T_AgainLinkTime != SClk10Ms)) // 数据发送错误应答10次,或者连续8次心跳没有正确发送成功,则200MS重新连接
//	{
//		T_AgainLinkTime = SClk10Ms; // 延时接收数据的时间
//		C_AgainLinkTime++;
//		if (C_AgainLinkTime > Pw_link1ReconnectTimeBetwen) // 2S
//		{
//			T_AgainLinkTime = 100;
//			C_AgainLinkTime = 0;

//			// 连接中断了,或者连续8次心跳没有正确发送成功,则重新连接
//			F_RunAgain = 1;		   // 重新运行模块
//			F_RunCheckModular = 1; // 运行过程中检查模块标志

//			connectsta = 0;	 // 连接状态清0
//			hbeaterrcnt = 0; // 心跳数量清0
//							 //			reboot();		 //系统重启
//		}
//	}
//}

//// GPRS模块发送心跳函数
// void sim7600ce_SendHeartbeat(void)
//{
//	u8 ipbuf[15];																					// IP缓存
//	u8 portBuf[5] = {0};																			// 远端端口缓存
//	u8 p_LinkTmpBuf[50] = {0};																		// 缓存
//	u8 F_HeartData = 0;																				// 心跳的内容标志
//	u16 local_port;																					// 本地ip端口暂存
//	u32 AllTotalTime;																				// 总时间
//	u16 link1OnlineReportTime, link2OnlineReportTime, link3OnlineReportTime, link4OnlineReportTime; // 报告时间

//	if (!usart3_rev_flag && connectsta == 0 && T_SendHeartbeatTime != SClk10Ms) // 连接正常的时候,每30秒发送一次心跳
//	{
//		T_SendHeartbeatTime = SClk10Ms; // 发送心跳时间
//		C_SendHeartbeatTime++;
//		// 30S

//		// 第1路
//		if (F_OneConnectStat == 1 && (C_SendHeartbeatTime >= (Pw_link1OnlineReportTime + (NowConnectIPNum * 200))))
//		{
//			if (!F_ONESendHeartbeat)
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link1IP1, Pw_Link1IP2, Pw_Link1IP3, Pw_Link1IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link1Port);											   // 第一路链接远端端口字转换为ascii码

//				NowConnectIPNum++; // 当前要发送到数+1

//				if (Pw_Link1TCP_UDPType == 0) // 第一路为UDP
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPSEND=0,22,\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, ">", 200) == 0)
//						F_HeartData = 1; // 发起第1路发送心跳
//				}
//				else ////第一路为TCP Client
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPOPEN=0,\"TCP\",\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间p_TmpBuf
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, "+CIPOPEN: 0,0", 30))
//						; // 发起第1路连接（配置，透明传输的时候注释掉）
//					// 上面内容防止TCP服务器端关闭，不能够打开网络
//					if (sim7600ce_send_cmd("AT+CIPSEND=0,22", ">", 100) == 0)
//						F_HeartData = 1; // 发起第1路发送心跳
//				}
//				if (F_HeartData == 1) // 发送数据
//				{
//					local_port = Pw_Link1localPort;			  /*定义本地1端口*/
//					local_port_Byte[0] = local_port >> 8;	  /*定义本地1端口字节*/
//					local_port_Byte[1] = local_port & 0x00FF; /*定义本地1端口字节*/
//					DTUhbeaterrPackets();					  // 获取心跳包内容
//					memcpy(Txd3Buffer, p_TmpBuf, 22);		  //
//															  //					while(DMA1_Channel4->CNDTR!=0);	//传输完成
//															  //					UART_DMA_Enable(DMA1_Channel4,22); //发送出去
//					UART3_Output_Datas(Txd3Buffer, 22);		  // YLS 2023.02.05
//															  //					UART1_Output_Datas(Txd3Buffer, 22);

//					F_HeartData = 0; // 发送心跳标志清0
//					delay_ms(40);	 // 必须要加延时

//					NormalHeartbeat1 = 1; // 第1路心跳正常
//					USART3_RX_STA = 0;
//					Cw_Rcv3 = 0;
//					usart3_rev_finish = 0; // YLS 2023.02.08
//					C_SendHeartbeatTime = 0;
//				}
//				else
//				{
//					NormalHeartbeat1 = 0; // 第1路心跳不正常
//					hbeaterrcnt++;
//				}
//				F_ONESendHeartbeat = 1;
//			}
//		}

//		// 第2路
//		if (F_TWOConnectStat == 1 && (C_SendHeartbeatTime >= (Pw_link2OnlineReportTime + (NowConnectIPNum * 200))))
//		{
//			if (!F_TwoSendHeartbeat)
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link2IP1, Pw_Link2IP2, Pw_Link2IP3, Pw_Link2IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link2Port);											   // 第2路链接远端端口字转换为ascii码

//				NowConnectIPNum++; // 当前要发送到数+1

//				if (Pw_Link2TCP_UDPType == 0) // 第2路为UDP
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPSEND=1,22,\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, ">", 200) == 0)
//						F_HeartData = 1; // 发起第2路发送心跳
//				}
//				else ////第2路为TCP Client
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPOPEN=1,\"TCP\",\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, "+CIPOPEN: 1,0", 30))
//						; // 发起第2路连接（配置，透明传输的时候注释掉）
//					// 上面内容防止TCP服务器端关闭，不能够打开网络
//					if (sim7600ce_send_cmd("AT+CIPSEND=1,22", ">", 100) == 0)
//						F_HeartData = 1; // 发起第2路发送心跳
//				}

//				if (F_HeartData == 1) // 发送数据
//				{
//					local_port = Pw_Link2localPort;			  /*定义本地2端口*/
//					local_port_Byte[0] = local_port >> 8;	  /*定义本地2端口字节*/
//					local_port_Byte[1] = local_port & 0x00FF; /*定义本地2端口字节*/
//					DTUhbeaterrPackets();					  // 获取心跳包内容
//					memcpy(Txd3Buffer, p_TmpBuf, 22);
//					//					while(DMA1_Channel4->CNDTR!=0);	//传输完成
//					//					UART_DMA_Enable(DMA1_Channel4,22); //发送出去
//					UART3_Output_Datas(Txd3Buffer, 22); // YLS 2023.02.05

//					F_HeartData = 0; // 发送心跳标志清0
//					delay_ms(40);	 // 必须要加延时

//					NormalHeartbeat2 = 1; // 第2路心跳正常
//					USART3_RX_STA = 0;
//					Cw_Rcv3 = 0;
//					usart3_rev_finish = 0; // YLS 2023.02.08
//					C_SendHeartbeatTime = 0;
//				}
//				else
//				{
//					NormalHeartbeat2 = 0; // 第2路心跳不正常
//					hbeaterrcnt++;
//				}
//				F_TwoSendHeartbeat = 1;
//			}
//		}

//		// 第3路
//		if (F_ThreeConnectStat == 1 && (C_SendHeartbeatTime >= (Pw_link3OnlineReportTime + (NowConnectIPNum * 200))))
//		{
//			if (!F_ThreeSendHeartbeat)
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link3IP1, Pw_Link3IP2, Pw_Link3IP3, Pw_Link3IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link3Port);											   // 第3路链接远端端口字转换为ascii码

//				NowConnectIPNum++; // 当前要发送到数+1

//				if (Pw_Link3TCP_UDPType == 0) // 第3路为UDP
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPSEND=2,22,\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, ">", 200) == 0)
//						F_HeartData = 1; // 发起第3路发送心跳
//				}
//				else ////第3路为TCP Client
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPOPEN=2,\"TCP\",\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, "+CIPOPEN: 2,0", 30))
//						; // 发起第3路连接（配置，透明传输的时候注释掉）
//					// 上面内容防止TCP服务器端关闭，不能够打开网络
//					if (sim7600ce_send_cmd("AT+CIPSEND=2,22", ">", 100) == 0)
//						F_HeartData = 1; // 发起第3路发送心跳
//				}
//				if (F_HeartData == 1) // 发送数据
//				{
//					local_port = Pw_Link3localPort;			  /*定义本地3端口*/
//					local_port_Byte[0] = local_port >> 8;	  /*定义本地3端口字节*/
//					local_port_Byte[1] = local_port & 0x00FF; /*定义本地3端口字节*/
//					DTUhbeaterrPackets();					  // 获取心跳包内容
//					memcpy(Txd3Buffer, p_TmpBuf, 22);
//					//					while(DMA1_Channel4->CNDTR!=0);	//传输完成
//					//					UART_DMA_Enable(DMA1_Channel4,22); //发送出去
//					UART3_Output_Datas(Txd3Buffer, 22); // YLS 2023.02.05

//					F_HeartData = 0; // 发送心跳标志清0
//					delay_ms(40);	 // 必须要加延时

//					NormalHeartbeat3 = 1; // 第3路心跳正常
//					USART3_RX_STA = 0;
//					Cw_Rcv3 = 0;
//					usart3_rev_finish = 0; // YLS 2023.02.08
//					C_SendHeartbeatTime = 0;
//				}
//				else
//				{
//					NormalHeartbeat3 = 0; // 第3路心跳不正常
//					hbeaterrcnt++;
//				}
//				F_ThreeSendHeartbeat = 1;
//			}
//		}

//		// 第4路
//		if (F_FourConnectStat == 1 && (C_SendHeartbeatTime >= (Pw_link4OnlineReportTime + (NowConnectIPNum * 200))))
//		{
//			if (!F_FourSendHeartbeat)
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link4IP1, Pw_Link4IP2, Pw_Link4IP3, Pw_Link4IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link4Port);											   // 第4路链接远端端口字转换为ascii码

//				NowConnectIPNum++; // 当前要发送到数+1

//				if (Pw_Link4TCP_UDPType == 0) // 第4路为UDP
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPSEND=3,22,\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, ">", 200) == 0)
//						F_HeartData = 1; // 发起第3路发送心跳
//				}
//				else ////第4路为TCP Client
//				{
//					sprintf((char *)p_LinkTmpBuf, "AT+CIPOPEN=3,\"TCP\",\"%s\",%s", ipbuf, portBuf); // 把网络信息给申请的空间
//					if (sim7600ce_send_cmd(p_LinkTmpBuf, "+CIPOPEN: 3,0", 30))
//						; // 发起第4路连接（配置，透明传输的时候注释掉）
//					// 上面内容防止TCP服务器端关闭，不能够打开网络
//					if (sim7600ce_send_cmd("AT+CIPSEND=3,22", ">", 100) == 0)
//						F_HeartData = 1; // 发起第4路发送心跳
//				}
//				if (F_HeartData == 1) // 发送数据
//				{
//					local_port = Pw_Link4localPort;			  /*定义本地4端口*/
//					local_port_Byte[0] = local_port >> 8;	  /*定义本地4端口字节*/
//					local_port_Byte[1] = local_port & 0x00FF; /*定义本地4端口字节*/
//					DTUhbeaterrPackets();					  // 获取心跳包内容
//					memcpy(Txd3Buffer, p_TmpBuf, 22);
//					//					while(DMA1_Channel4->CNDTR!=0);	//传输完成
//					//					UART_DMA_Enable(DMA1_Channel4,22); //发送出去
//					UART3_Output_Datas(Txd3Buffer, 22); // YLS 2023.02.05

//					F_HeartData = 0; // 发送心跳标志清0
//					delay_ms(40);	 // 必须要加延时

//					NormalHeartbeat4 = 1; // 第4路心跳正常
//					USART3_RX_STA = 0;
//					Cw_Rcv3 = 0;
//					usart3_rev_finish = 0; // YLS 2023.02.08
//					C_SendHeartbeatTime = 0;
//				}
//				else
//				{
//					NormalHeartbeat4 = 0; // 第4路心跳不正常
//					hbeaterrcnt++;
//				}
//				F_FourSendHeartbeat = 1;
//			}
//		}
//		if (NormalHeartbeat1 + NormalHeartbeat2 + NormalHeartbeat3 + NormalHeartbeat4 == ConnectIPNum) // 需要连接的都正常反馈
//		{
//			if (NowConnectIPNum == ConnectIPNum) // 说明已经发送完毕了
//			{
//				T_SendHeartbeatTime = 100; // 所有心跳都正常清0重新发送心跳
//				C_SendHeartbeatTime = 0;
//				NowConnectIPNum = 0;
//				hbeaterrcnt = 0;
//				F_PowerOnRegister = 1; // 上电注册完毕
//				F_ONESendHeartbeat = 0;
//				F_TwoSendHeartbeat = 0;
//				F_ThreeSendHeartbeat = 0;
//				F_FourSendHeartbeat = 0;
//			}
//		}
//		// 计算总时间

//		if (F_OneConnectStat == 1)
//			link1OnlineReportTime = Pw_link1OnlineReportTime;
//		else
//			link1OnlineReportTime = 0;
//		if (F_TWOConnectStat == 1)
//			link2OnlineReportTime = Pw_link2OnlineReportTime;
//		else
//			link2OnlineReportTime = 0;
//		if (F_ThreeConnectStat == 1)
//			link3OnlineReportTime = Pw_link3OnlineReportTime;
//		else
//			link3OnlineReportTime = 0;
//		if (F_FourConnectStat == 1)
//			link4OnlineReportTime = Pw_link4OnlineReportTime;
//		else
//			link4OnlineReportTime = 0;
//		AllTotalTime = link1OnlineReportTime + link2OnlineReportTime + link3OnlineReportTime + link4OnlineReportTime;

//		if (((NormalHeartbeat1 + NormalHeartbeat2 + NormalHeartbeat3 + NormalHeartbeat4) != ConnectIPNum) &&
//			(C_SendHeartbeatTime >= (AllTotalTime + (ConnectIPNum * 200))))
//		{
//			T_SendHeartbeatTime = 100; // 心跳不正常，延时最大时间仍然不正常则清0重新发送心跳
//			C_SendHeartbeatTime = 0;
//			NowConnectIPNum = 0;
//			F_PowerOnRegister = 1; // 上电注册完毕
//			F_ONESendHeartbeat = 0;
//			F_TwoSendHeartbeat = 0;
//			F_ThreeSendHeartbeat = 0;
//			F_FourSendHeartbeat = 0;
//		}
//	}
//}

//// GPRS模块数据接收函数
// void sim7600ce_ReceiveData(void)
//{
//	u8 RecNumBit, IP1NumBit, IP2NumBit, IP3NumBit, IP4NumBit, PortNumBit; // 接收到的数据长度是几位的
//	u16 j = 0, n, m, i;
//	u8 *p2, *p3;
//	// u8 *p1;
//	u8 ipbuf[15];			   // IP缓存
//	u8 portBuf[5] = {0};	   // 远端端口缓存
//	u8 p_RECV_IPBuf[40] = {0}; // 接收缓存

//	//	delay_ms(1);
//	//	COM2_DATA = 0;				//接受完毕数据关闭指示灯
//	//	if (USART3_RX_STA & 0X8000) //接收数据(所有的接受数据处理)
//	if (usart3_rev_finish) // YLS 2023.02.07
//	{
//		// USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
//		//		Rcv3Buffer[USART3_RX_STA & 0X7FFF] = 0; //添加结束符
//		if (Cw_BakRcv3 <= RCV3_MAX - 1)
//		{
//			Rcv3Buffer[Cw_BakRcv3] = 0; // 添加结束符
//		}
//		else
//		{
//			Rcv3Buffer[RCV3_MAX - 1] = 0; // 添加结束符
//		}

//		// p2 = (u8 *)strstr((const char *)Rcv3Buffer, "+CIPSEND:"); // 接收到TCP/UDP数据
//		// if (p2)													  // 接收到TCP/UDP数据
//		// {
//		// 	usart3_rev_finish = 0;
//		// 	return;
//		// }

//		//		if (Pw_SupportDataReceipt == 1)
//		//			printf("%s", Rcv3Buffer);			//这个地方不能用显示字符串的方式，而是应该发送二进制数据，因为数据中可能存在0,就截断了
//		//			UART1_Output_Datas(Rcv3Buffer, Cw_BakRcv3);
//		//		delay_ms(5);

//		p2 = (u8 *)strstr((const char *)Rcv3Buffer, "RECV FROM:"); // 接收到TCP/UDP数据
//		if (p2)													   // 接收到TCP/UDP数据
//		{
//			COM2_DATA = 1; // 接收到数据开打指示灯

//			// 第1路
//			if ((Pw_Link1IP1 + Pw_Link1IP2 + Pw_Link1IP3 + Pw_Link1IP4) != 0) // 第一路的IP地址
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link1IP1, Pw_Link1IP2, Pw_Link1IP3, Pw_Link1IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link1Port);											   // 第一路链接远端端口字转换为ascii码
//				sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
//				if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第1路发来的数据
//				{
//					p3 = (u8 *)strstr((const char *)p2, "+IPD");
//					//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第1路数据长度字符 类似这样的指针指向的字符串向数组传值，不知道能传几个数，很容易出现溢出，
//					for (i = 0; i < 15; i++)
//					{
//						p1_TmpBuf[i] = *(p3 + 4);
//						p3++;
//					}
//					for (i = 0; i <= 5; i++) // 把接收到的第1路数据长度字符转化为16进制数
//					{
//						if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
//						{
//							ReceiveGPRSBufOneDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

//							ReceiveGPRSBufTwoDateLen = 0;
//							ReceiveGPRSBufThreeDateLen = 0;
//							ReceiveGPRSBufFourDateLen = 0;
//							break; //
//						}
//					}
//					for (i = 0; i <= 5; i++)
//						p1_TmpBuf[i] = 0; // 把接收的第1路字符缓存长度数清0，为下一次计算
//					//					for (i = 0; i <RCV3_MAX; i++)
//					//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
//					memset(ReceiveGPRSBuf, 0, RCV3_MAX);
//					m = ReceiveGPRSBufOneDateLen;
//					for (RecNumBit = 0; m != 0; RecNumBit++)
//						m /= 10;
//					m = Pw_Link1IP1;
//					for (IP1NumBit = 0; m != 0; IP1NumBit++)
//						m /= 10;
//					m = Pw_Link1IP2;
//					for (IP2NumBit = 0; m != 0; IP2NumBit++)
//						m /= 10;
//					m = Pw_Link1IP3;
//					for (IP3NumBit = 0; m != 0; IP3NumBit++)
//						m /= 10;
//					m = Pw_Link1IP4;
//					for (IP4NumBit = 0; m != 0; IP4NumBit++)
//						m /= 10;
//					m = Pw_Link1Port;
//					for (PortNumBit = 0; m != 0; PortNumBit++)
//						m /= 10;
//					n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
//					for (i = 24 + n; i <= (ReceiveGPRSBufOneDateLen + 24 + n); i++)
//					{
//						ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第1路数据给ReceiveGPRSBuf
//						j++;
//					}
//					if (j >= ReceiveGPRSBufOneDateLen)
//						j = 0;
//					ChannelNo = 0; // 通道号，第几路通道收到数据
//					F_AcklinkNum = 1;
//				}
//			}

//			// 第2路
//			if ((Pw_Link2IP1 + Pw_Link2IP2 + Pw_Link2IP3 + Pw_Link2IP4) != 0)
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link2IP1, Pw_Link2IP2, Pw_Link2IP3, Pw_Link2IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link2Port);											   // 第一路链接远端端口字转换为ascii码
//				sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
//				if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第2路发来的数据
//				{
//					p3 = (u8 *)strstr((const char *)p2, "+IPD");
//					//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第2路数据长度字符
//					for (i = 0; i < 15; i++)
//					{
//						p1_TmpBuf[i] = *(p3 + 4);
//						p3++;
//					}
//					for (i = 0; i <= 5; i++) // 把接收到的第2路数据长度字符转化为16进制数
//					{
//						if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
//						{
//							ReceiveGPRSBufTwoDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

//							ReceiveGPRSBufOneDateLen = 0;
//							ReceiveGPRSBufThreeDateLen = 0;
//							ReceiveGPRSBufFourDateLen = 0;
//							break; //
//						}
//					}
//					for (i = 0; i <= 5; i++)
//						p1_TmpBuf[i] = 0; // 把接收的第2路字符缓存长度数清0，为下一次计算
//					//					for (i = 0; i <RCV3_MAX; i++)
//					//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
//					memset(ReceiveGPRSBuf, 0, RCV3_MAX);
//					m = ReceiveGPRSBufTwoDateLen;
//					for (RecNumBit = 0; m != 0; RecNumBit++)
//						m /= 10;
//					m = Pw_Link2IP1;
//					for (IP1NumBit = 0; m != 0; IP1NumBit++)
//						m /= 10;
//					m = Pw_Link2IP2;
//					for (IP2NumBit = 0; m != 0; IP2NumBit++)
//						m /= 10;
//					m = Pw_Link2IP3;
//					for (IP3NumBit = 0; m != 0; IP3NumBit++)
//						m /= 10;
//					m = Pw_Link2IP4;
//					for (IP4NumBit = 0; m != 0; IP4NumBit++)
//						m /= 10;
//					m = Pw_Link2Port;
//					for (PortNumBit = 0; m != 0; PortNumBit++)
//						m /= 10;
//					n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
//					for (i = 24 + n; i <= (ReceiveGPRSBufTwoDateLen + 24 + n); i++)
//					{
//						ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第2路数据给ReceiveGPRSBuf
//						j++;
//					}
//					if (j >= ReceiveGPRSBufTwoDateLen)
//						j = 0;
//					ChannelNo = 1; // 通道号，第几路通道收到数据
//					F_AcklinkNum = 2;
//				}
//			}

//			// 第3路
//			if ((Pw_Link3IP1 + Pw_Link3IP2 + Pw_Link3IP3 + Pw_Link3IP4) != 0)
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link3IP1, Pw_Link3IP2, Pw_Link3IP3, Pw_Link3IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link3Port);											   // 第一路链接远端端口字转换为ascii码
//				sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
//				if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第3路发来的数据
//				{
//					p3 = (u8 *)strstr((const char *)p2, "+IPD");
//					//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第3路数据长度字符
//					for (i = 0; i < 15; i++)
//					{
//						p1_TmpBuf[i] = *(p3 + 4);
//						p3++;
//					}
//					for (i = 0; i <= 5; i++) // 把接收到的第3路数据长度字符转化为16进制数
//					{
//						if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
//						{
//							ReceiveGPRSBufThreeDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

//							ReceiveGPRSBufOneDateLen = 0;
//							ReceiveGPRSBufTwoDateLen = 0;
//							ReceiveGPRSBufFourDateLen = 0;
//							break; //
//						}
//					}
//					for (i = 0; i <= 5; i++)
//						p1_TmpBuf[i] = 0; // 把接收的第3路字符缓存长度数清0，为下一次计算
//					//					for (i = 0; i <RCV3_MAX; i++)
//					//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
//					memset(ReceiveGPRSBuf, 0, RCV3_MAX);
//					m = ReceiveGPRSBufThreeDateLen;
//					for (RecNumBit = 0; m != 0; RecNumBit++)
//						m /= 10;
//					m = Pw_Link3IP1;
//					for (IP1NumBit = 0; m != 0; IP1NumBit++)
//						m /= 10;
//					m = Pw_Link3IP2;
//					for (IP2NumBit = 0; m != 0; IP2NumBit++)
//						m /= 10;
//					m = Pw_Link3IP3;
//					for (IP3NumBit = 0; m != 0; IP3NumBit++)
//						m /= 10;
//					m = Pw_Link3IP4;
//					for (IP4NumBit = 0; m != 0; IP4NumBit++)
//						m /= 10;
//					m = Pw_Link3Port;
//					for (PortNumBit = 0; m != 0; PortNumBit++)
//						m /= 10;
//					n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
//					for (i = 24 + n; i <= (ReceiveGPRSBufThreeDateLen + 24 + n); i++)
//					{
//						ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第3路数据给ReceiveGPRSBuf
//						j++;
//					}
//					if (j >= ReceiveGPRSBufThreeDateLen)
//						j = 0;
//					ChannelNo = 2; // 通道号，第几路通道收到数据
//					F_AcklinkNum = 3;
//				}
//			}

//			// 第4路
//			if ((Pw_Link4IP1 + Pw_Link4IP2 + Pw_Link4IP3 + Pw_Link4IP4) != 0)
//			{
//				sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link4IP1, Pw_Link4IP2, Pw_Link4IP3, Pw_Link4IP4); // 存放IP
//				sprintf((char *)portBuf, "%d", Pw_Link4Port);											   // 第一路链接远端端口字转换为ascii码
//				sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
//				if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第4路发来的数据
//				{
//					p3 = (u8 *)strstr((const char *)p2, "+IPD");
//					//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第4路数据长度字符
//					for (i = 0; i < 15; i++)
//					{
//						p1_TmpBuf[i] = *(p3 + 4);
//						p3++;
//					}
//					for (i = 0; i <= 5; i++) // 把接收到的第4路数据长度字符转化为16进制数
//					{
//						if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
//						{
//							ReceiveGPRSBufFourDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

//							ReceiveGPRSBufOneDateLen = 0;
//							ReceiveGPRSBufTwoDateLen = 0;
//							ReceiveGPRSBufThreeDateLen = 0;
//							break; //
//						}
//					}
//					for (i = 0; i <= 5; i++)
//						p1_TmpBuf[i] = 0; // 把接收的第4路字符缓存长度数清0，为下一次计算
//					//					for (i = 0; i <RCV3_MAX; i++)
//					//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
//					memset(ReceiveGPRSBuf, 0, RCV3_MAX);
//					m = ReceiveGPRSBufFourDateLen;
//					for (RecNumBit = 0; m != 0; RecNumBit++)
//						m /= 10;
//					m = Pw_Link4IP1;
//					for (IP1NumBit = 0; m != 0; IP1NumBit++)
//						m /= 10;
//					m = Pw_Link4IP2;
//					for (IP2NumBit = 0; m != 0; IP2NumBit++)
//						m /= 10;
//					m = Pw_Link4IP3;
//					for (IP3NumBit = 0; m != 0; IP3NumBit++)
//						m /= 10;
//					m = Pw_Link4IP4;
//					for (IP4NumBit = 0; m != 0; IP4NumBit++)
//						m /= 10;
//					m = Pw_Link4Port;
//					for (PortNumBit = 0; m != 0; PortNumBit++)
//						m /= 10;
//					n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
//					for (i = 24 + n; i <= (ReceiveGPRSBufFourDateLen + 24 + n); i++)
//					{
//						ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第4路数据给ReceiveGPRSBuf
//						j++;
//					}
//					if (j >= ReceiveGPRSBufFourDateLen)
//						j = 0;
//					ChannelNo = 3; // 通道号，第几路通道收到数据
//					F_AcklinkNum = 4;
//				}
//			}

//			delay_ms(50);
//			sim7600ce_DoWithData(); // 处理接收到的数据
//		}

//		// 接收到短息
//		p2 = (u8 *)strstr((const char *)Rcv3Buffer, "+CMTI:"); // 接收到短息
//		if (p2)												   // 接收短信内容
//		{
//			if (strstr((const char *)p2, "+CMTI:")) // 收到短信数据
//			{
//				p3 = (u8 *)strstr((const char *)p2, ",");			   // 收到短信数量
//				p2[0] = 0;											   // 加入结束符
//				sprintf((char *)p1_TmpBuf, "AT+CMGR=%s\r\n", p3 + 1);  // 要发送的读取指令
//				sim7600ce_send_cmd(p1_TmpBuf, "+CMGR:", 200);		   // 读短信
//				delay_ms(40);										   // 必须要加延时
//				if (sim7600ce_send_cmd(p1_TmpBuf, "+CMGR:", 200) == 0) // 读取短信
//				{
//					p3 = (u8 *)strstr((const char *)(Rcv3Buffer), ",");
//					p2 = (u8 *)strstr((const char *)(p3 + 2), "\"");
//					p2[0] = 0;										// 加入结束符
//					sprintf((char *)CallerNumberbuf, "%s", p3 + 5); // 接收到短息的电话号码

//					p3 = (u8 *)strstr((const char *)(p2 + 1), "\r"); // 寻找回车符
//					sprintf((char *)SMSbuf, "%s", p3 + 2);			 // 把接收到短信的内容给p1_TmpBuf
//				}
//			}
//		}
//		USART3_RX_STA = 0;
//		Cw_Rcv3 = 0;
//		usart3_rev_finish = 0; // YLS 2023.02.08
//							   // USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 打开接收中断
//	}
//}
//// 获得模块需要连接的IP数量
//// 胡振广 2018.08.23  15：27
// void sim7600ce_connectstaModularNum(void) // 获得模块需要连接的IP数量
//{

//	if ((Pw_Link1IP1 + Pw_Link1IP2 + Pw_Link1IP3 + Pw_Link1IP4) != 0)
//	{
//		if (!F_OneConnectStat)
//		{
//			ConnectIPNum++;		  // 需要连接IP的数量
//			F_OneConnectStat = 1; // 第1路需要连接IP
//		}
//	}
//	else
//	{
//		if (F_OneConnectStat)
//		{
//			ConnectIPNum--;		  // 需要连接IP的数量
//			F_OneConnectStat = 0; // 第1路不需要连接IP
//		}
//	}
//	if ((Pw_Link2IP1 + Pw_Link2IP2 + Pw_Link2IP3 + Pw_Link2IP4) != 0)
//	{
//		if (!F_TWOConnectStat)
//		{
//			ConnectIPNum++;		  // 需要连接IP的数量
//			F_TWOConnectStat = 1; // 第2路需要连接IP
//		}
//	}
//	else
//	{
//		if (F_TWOConnectStat)
//		{
//			ConnectIPNum--;		  // 需要连接IP的数量
//			F_TWOConnectStat = 0; // 第2路不需要连接IP
//		}
//	}
//	if ((Pw_Link3IP1 + Pw_Link3IP2 + Pw_Link3IP3 + Pw_Link3IP4) != 0)
//	{
//		if (!F_ThreeConnectStat)
//		{
//			ConnectIPNum++;			// 需要连接IP的数量
//			F_ThreeConnectStat = 1; // 第3路需要连接IP
//		}
//	}
//	else
//	{
//		if (F_ThreeConnectStat)
//		{
//			ConnectIPNum--;			// 需要连接IP的数量
//			F_ThreeConnectStat = 0; // 第3路不需要连接IP
//		}
//	}
//	if ((Pw_Link4IP1 + Pw_Link4IP2 + Pw_Link4IP3 + Pw_Link4IP4) != 0)
//	{
//		if (!F_FourConnectStat)
//		{
//			ConnectIPNum++;		   // 需要连接IP的数量
//			F_FourConnectStat = 1; // 第4路需要连接IP
//		}
//	}
//	else
//	{
//		if (F_FourConnectStat)
//		{
//			ConnectIPNum--;		   // 需要连接IP的数量
//			F_FourConnectStat = 0; // 第4路不需要连接IP
//		}
//	}
//}

//// 短信接收初始化函数
// void sim7600ce_sms_ReciveInit(void)
//{
//	sim7600ce_send_cmd("AT+CMGF=1", "OK", 200);		  // 设置为文本模式
//	sim7600ce_send_cmd("AT+CSCS=\"GSM\"", "OK", 200); // 设置GSM字符集
//	sim7600ce_send_cmd("AT+CNMI=2,1", "OK", 200);	  // 设置新消息提示
// }

//// SendDateBuf要发送短信的内容，内容是ASC码字符
//// sim7600ce发短信
// void sim7600ce_sms_send(u8 *SendDateBuf)
//{
//	u8 i;
//	u8 sms_SendBuf[21]; // 发送数据缓存
//	u8 phonebuf[12];	// 号码缓存

//	if (CallerNumberbuf[0] + CallerNumberbuf[1] + CallerNumberbuf[2] != 0) // 手机号码前3位不是0的时候可以发送短信
//	{
//		memcpy(phonebuf, CallerNumberbuf, 11); // 复制号码
//		phonebuf[11] = '\0';				   // 结束字符
//		sprintf((char *)sms_SendBuf, "AT+CMGS=\"%s\"", phonebuf);
//		sim7600ce_send_cmd("AT+CSCS=\"GSM\"", "OK", 200);	// 设置GSM字符集
//		sim7600ce_send_cmd("AT+CMGF=1", "OK", 200);			// 设置为文本模式
//		if (sim7600ce_send_cmd(sms_SendBuf, ">", 200) == 0) // 发送要发送手机的电话号码
//		{
//			delay_ms(200);									//
//			Com2_printf("%s", SendDateBuf);					// 发送短信内容
//			delay_ms(500);									// 必须延时，否则不能发送短信
//			sim7600ce_send_cmd((u8 *)0X1A, "+CMGS:", 2000); // 发送结束符,等待发送完成(最长等待20秒钟,因为短信长了的话,等待时间会长一些)
//			delay_ms(100);									//
//			sim7600ce_sms_ReciveInit();						// 设置为接受短信模式
//		}
//		for (i = 0; i <= 49; i++)
//			SMSbuf[i] = 0; // 把收到的信息清空
//		for (i = 0; i <= 11; i++)
//			CallerNumberbuf[i] = 0; // 把需要发送的手机号清0，为下一次发送
//		USART3_RX_STA = 0;
//		Cw_Rcv3 = 0;
//		usart3_rev_finish = 0; // YLS 2023.02.08
//	}
//}

//// 短信读数据加载
// const u8 *modetb2[2] = {"UDP", "TCP"}; // 连接模式//mode:1:TCP测试;0,UDP测试)
// void sim7600ce_smsread_UpLoaded(void)
//{
//	//	u8 *p1, *p2;
//	// 查询信息

//	if (strstr((const char *)SMSbuf, "GPR+01;")) // 获取第1路的IP和端口。01检查写的是第1路的IP和端口。举例(GPR+01;)
//	{
//		sprintf((char *)p1_TmpBuf, "%s:%d.%d.%d.%d:%d", modetb2[(u8)Pw_Link1TCP_UDPType], Pw_Link1IP1, Pw_Link1IP2, Pw_Link1IP3, Pw_Link1IP4, Pw_Link1Port); // 获取第1路的IP和端口。
//		sprintf((char *)p_TmpBuf, "GPR+01, IP1:\r\n\"%s\"", p1_TmpBuf);																						 // 写要返回的DTU内容。
//		sim7600ce_sms_send(p_TmpBuf);
//	}
//	else if (strstr((const char *)SMSbuf, "GPR+02;")) // 获取第2路的IP和端口。02举例(GPR+02;)
//	{
//		sprintf((char *)p1_TmpBuf, "%s:%d.%d.%d.%d:%d", modetb2[(u8)Pw_Link2TCP_UDPType], Pw_Link2IP1, Pw_Link2IP2, Pw_Link2IP3, Pw_Link2IP4, Pw_Link2Port); // 获取第2路的IP和端口。
//		sprintf((char *)p_TmpBuf, "GPR+02, IP2:\r\n\"%s\"", p1_TmpBuf);																						 // 写要返回的DTU内容。
//		sim7600ce_sms_send(p_TmpBuf);
//	}
//	else if (strstr((const char *)SMSbuf, "GPR+03;")) // 获取第3路的IP和端口。03举例(GPR+03;)
//	{
//		sprintf((char *)p1_TmpBuf, "%s:%d.%d.%d.%d:%d", modetb2[(u8)Pw_Link3TCP_UDPType], Pw_Link3IP1, Pw_Link3IP2, Pw_Link3IP3, Pw_Link3IP4, Pw_Link3Port); // 获取第3路的IP和端口。
//		sprintf((char *)p_TmpBuf, "GPR+03, IP3:\r\n\"%s\"", p1_TmpBuf);																						 // 写要返回的DTU内容。
//		sim7600ce_sms_send(p_TmpBuf);
//	}
//	else if (strstr((const char *)SMSbuf, "GPR+04;")) // 获取第4路的IP和端口。04举例(GPR+04;)
//	{
//		sprintf((char *)p1_TmpBuf, "%s:%d.%d.%d.%d:%d", modetb2[(u8)Pw_Link4TCP_UDPType], Pw_Link4IP1, Pw_Link4IP2, Pw_Link4IP3, Pw_Link4IP4, Pw_Link4Port); // 获取第4路的IP和端口。
//		sprintf((char *)p_TmpBuf, "GPR+04, IP4:\r\n\"%s\"", p1_TmpBuf);																						 // 写要返回的DTU内容。
//		sim7600ce_sms_send(p_TmpBuf);
//	}
//	else if (strstr((const char *)SMSbuf, "GPR+05;")) // 获取无线模块的地址。05举例(GPR+05;)
//	{
//		sprintf((char *)p1_TmpBuf, "%d", Pw_RFModularAddress);			   // 获取无线模块的地址。
//		sprintf((char *)p_TmpBuf, "GPR+05, RF-Address:\"%s\"", p1_TmpBuf); // 写要返回的DTU内容。
//		sim7600ce_sms_send(p_TmpBuf);
//	}
//	else if (strstr((const char *)SMSbuf, "GPR+06;")) // 获取DTU号码。06举例(GPR+06;)
//	{
//		sprintf((char *)p1_TmpBuf, "%d%d%d", Pw_DTUIDNUM1, Pw_DTUIDNUM2, Pw_DTUIDNUM3); // 取出DTU号码。
//		//		sprintf((char *)p1_TmpBuf, "%s", &GprsPar[DtuNoBase]);		// 取出DTU号码。	//YLS 2023.02.08
//		sprintf((char *)p_TmpBuf, "GPR+06, DTU:\"%s\"", p1_TmpBuf); // 写要返回的DTU内容。
//		sim7600ce_sms_send(p_TmpBuf);
//	}
//}
//// 短信写数据加载
// const u8 SMS_SetOk[] = {"SMS SETOK!"};		 //
// const u8 SMS_SetERROR[] = {"SMS SETERROR!"}; //
// u8 IPDotNUM = 0;							 // IP点的数量
// void sim7600ce_smsWrite_UpLoaded(void)
//{
//	u8 i, j = 0, NowWriteOK = 0;
//	// 写信息
//	if (strstr((const char *)SMSbuf, "GPW+01,")) // 检查写的是第1路的IP和端口。举例(GPW+01,UDP:222.173.103.226:5008;)
//	{
//		if (SMSbuf[6] == ',' && SMSbuf[10] == ':')
//		{
//			j = 0;			// 检查用
//			NowWriteOK = 0; // 检查用
//			IPDotNUM = 0;	// 开始计数值
//			if (SMSbuf[7] == 'T')
//				Pw_Link1TCP_UDPType = 1; // 协议类型
//			else
//				Pw_Link1TCP_UDPType = 0;
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip1的第1位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip1的第1位数据长度字符串内容没有了
//					{
//						Pw_Link1IP1 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP1的第2个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+01,IP1:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip1的第2位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip1的第2位数据长度字符串内容没有了
//					{
//						Pw_Link1IP2 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP1的第3个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+01,IP1:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip1的第3位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip1的第3位数据长度字符串内容没有了
//					{
//						Pw_Link1IP3 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP1的第4个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+01,IP1:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip1的第4位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ':') // 说明ip1的第4位数据长度字符串内容没有了
//					{
//						Pw_Link1IP4 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP1的端口数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+01,IP1:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 5; i++) // ip1的端口取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ';') // 说明ip1的端口数据长度字符串内容没有了
//					{
//						Pw_Link1Port = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = 0;									// 找到下一个IP1数组中存的位置清0
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+01,IP1:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				SPI_FMRAM_BufferWrite((u8 *)(&Pw_ParLst_GPRS[0]), 0, FLASH_PAR_SIZE * 2); // FLASH_PAR_SIZE 256（设定参数一个扇区）
//				sprintf((char *)p_TmpBuf, "GPW+01,IP1:\"%s\"", SMS_SetOk);				  // 写要返回的DTU内容。

//				F_RunAgain = 1;		   // 重新运行模块
//				sim_ready = 0;		   // 检查sim卡准备就绪
//				F_RunCheckModular = 1; // 运行过程中检查模块标志
//			}
//			if (j == 1)
//			{
//				Pw_Link1IP1 = 0;
//				Pw_Link1IP2 = 0;
//				Pw_Link1IP3 = 0;
//				Pw_Link1IP4 = 0;
//				Pw_Link1Port = 0;
//				j = 0; // 下个参数进行检查用
//			}
//		}
//		else
//		{
//			sprintf((char *)p_TmpBuf, "GPW+01,IP1:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//		}
//		sim7600ce_sms_send(p_TmpBuf); // 返回短信
//	}
//	else if (strstr((const char *)SMSbuf, "GPW+02,")) // 检查写的是第2路的IP和端口。举例(GPW+02,UDP:222.173.103.226:5008;)
//	{
//		if (SMSbuf[6] == ',' && SMSbuf[10] == ':')
//		{
//			j = 0;			// 检查用
//			NowWriteOK = 0; // 检查用
//			IPDotNUM = 0;	// 开始计数值
//			if (SMSbuf[7] == 'T')
//				Pw_Link2TCP_UDPType = 1;
//			else
//				Pw_Link2TCP_UDPType = 0;
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip2的第1位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip2的第1位数据长度字符串内容没有了
//					{
//						Pw_Link2IP1 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP2的第2个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+02,IP2:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip2的第2位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip2的第2位数据长度字符串内容没有了
//					{
//						Pw_Link2IP2 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP2的第3个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+02,IP2:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip2的第3位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip2的第3位数据长度字符串内容没有了
//					{
//						Pw_Link2IP3 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP2的第4个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+02,IP2:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip2的第4位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ':') // 说明ip2的第4位数据长度字符串内容没有了
//					{
//						Pw_Link2IP4 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP2的端口数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+02,IP2:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 5; i++) // ip2的端口取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ';') // 说明ip2的端口数据长度字符串内容没有了
//					{
//						Pw_Link2Port = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = 0;									// 找到下一个IP2数组中存的位置清0
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+02,IP2:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				SPI_FMRAM_BufferWrite((u8 *)(&Pw_ParLst_GPRS[0]), 0, FLASH_PAR_SIZE * 2); // FLASH_PAR_SIZE 256（设定参数一个扇区）
//				sprintf((char *)p_TmpBuf, "GPW+02,IP2:\"%s\"", SMS_SetOk);				  // 写要返回的DTU内容。

//				F_RunAgain = 1;		   // 重新运行模块
//				sim_ready = 0;		   // 检查sim卡准备就绪
//				F_RunCheckModular = 1; // 运行过程中检查模块标志
//			}
//			if (j == 1)
//			{
//				Pw_Link2IP1 = 0;
//				Pw_Link2IP2 = 0;
//				Pw_Link2IP3 = 0;
//				Pw_Link2IP4 = 0;
//				Pw_Link2Port = 0;
//				j = 0; // 下个参数进行检查用
//			}
//		}
//		else
//		{
//			sprintf((char *)p_TmpBuf, "GPW+02,IP2:\"%s\"", SMS_SetERROR); // 写要返回的IP2内容错误。
//		}
//		sim7600ce_sms_send(p_TmpBuf); // 返回短信
//	}
//	else if (strstr((const char *)SMSbuf, "GPW+03,")) // 检查写的是第3路的IP和端口。举例(GPW+03,UDP:222.173.103.226:5008;)
//	{
//		if (SMSbuf[6] == ',' && SMSbuf[10] == ':')
//		{
//			j = 0;			// 检查用
//			NowWriteOK = 0; // 检查用
//			IPDotNUM = 0;	// 开始计数值
//			if (SMSbuf[7] == 'T')
//				Pw_Link3TCP_UDPType = 1;
//			else
//				Pw_Link3TCP_UDPType = 0;
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip3的第1位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip3的第1位数据长度字符串内容没有了
//					{
//						Pw_Link3IP1 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP3的第2个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+03,IP3:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip3的第2位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip3的第2位数据长度字符串内容没有了
//					{
//						Pw_Link3IP2 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP3的第3个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+03,IP3:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip3的第3位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip3的第3位数据长度字符串内容没有了
//					{
//						Pw_Link3IP3 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP3的第4个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+03,IP3:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip3的第4位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ':') // 说明ip3的第4位数据长度字符串内容没有了
//					{
//						Pw_Link3IP4 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP3的端口数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+03,IP3:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 5; i++) // ip3的端口取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ';') // 说明ip3的端口数据长度字符串内容没有了
//					{
//						Pw_Link3Port = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = 0;									// 找到下一个IP3数组中存的位置清0
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+03,IP3:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				SPI_FMRAM_BufferWrite((u8 *)(&Pw_ParLst_GPRS[0]), 0, FLASH_PAR_SIZE * 2); // FLASH_PAR_SIZE 256（设定参数一个扇区）
//				sprintf((char *)p_TmpBuf, "GPW+03,IP3:\"%s\"", SMS_SetOk);				  // 写要返回的DTU内容。

//				F_RunAgain = 1;		   // 重新运行模块
//				sim_ready = 0;		   // 检查sim卡准备就绪
//				F_RunCheckModular = 1; // 运行过程中检查模块标志
//			}
//			if (j == 1)
//			{
//				Pw_Link3IP1 = 0;
//				Pw_Link3IP2 = 0;
//				Pw_Link3IP3 = 0;
//				Pw_Link3IP4 = 0;
//				Pw_Link3Port = 0;
//				j = 0; // 下个参数进行检查用
//			}
//		}
//		else
//		{
//			sprintf((char *)p_TmpBuf, "GPW+03,IP3:\"%s\"", SMS_SetERROR); // 写要返回的IP3内容错误。
//		}
//		sim7600ce_sms_send(p_TmpBuf); // 返回短信
//	}
//	else if (strstr((const char *)SMSbuf, "GPW+04,")) // 检查写的是第4路的IP和端口。举例(GPW+04,UDP:222.173.103.226:5008;)
//	{
//		if (SMSbuf[6] == ',' && SMSbuf[10] == ':')
//		{
//			j = 0;			// 检查用
//			NowWriteOK = 0; // 检查用
//			IPDotNUM = 0;	// 开始计数值
//			if (SMSbuf[7] == 'T')
//				Pw_Link4TCP_UDPType = 1;
//			else
//				Pw_Link4TCP_UDPType = 0;
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip4的第1位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip4的第1位数据长度字符串内容没有了
//					{
//						Pw_Link4IP1 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP4的第2个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+04,IP4:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip4的第2位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip3的第2位数据长度字符串内容没有了
//					{
//						Pw_Link4IP2 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP4的第3个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+04,IP4:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip4的第3位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == '.') // 说明ip4的第3位数据长度字符串内容没有了
//					{
//						Pw_Link4IP3 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP4的第4个IP数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+04,IP4:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 4; i++) // ip4的第4位取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ':') // 说明ip4的第4位数据长度字符串内容没有了
//					{
//						Pw_Link4IP4 = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = IPDotNUM + i + 1;				   // 找到下一个IP4的端口数组中存的位置
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+04,IP4:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				for (i = 0; i <= 5; i++) // ip4的端口取出16进制数
//				{
//					if (SMSbuf[11 + IPDotNUM + i] == ';') // 说明ip4的端口数据长度字符串内容没有了
//					{
//						Pw_Link4Port = atoi(&SMSbuf[11 + IPDotNUM], i); // 转换为16进制
//						IPDotNUM = 0;									// 找到下一个IP4数组中存的位置清0
//						NowWriteOK = 1;
//						break; //
//					}
//				}
//				if (NowWriteOK == 0) // 没有写进去
//				{
//					sprintf((char *)p_TmpBuf, "GPW+04,IP4:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//					j = 1;
//				}
//				NowWriteOK = 0; // 下一个变量判断用
//			}
//			if (j == 0)
//			{
//				SPI_FMRAM_BufferWrite((u8 *)(&Pw_ParLst_GPRS[0]), 0, FLASH_PAR_SIZE * 2); // FLASH_PAR_SIZE 256（设定参数一个扇区）
//				sprintf((char *)p_TmpBuf, "GPW+04,IP4:\"%s\"", SMS_SetOk);				  // 写要返回的DTU内容。

//				F_RunAgain = 1;		   // 重新运行模块
//				sim_ready = 0;		   // 检查sim卡准备就绪
//				F_RunCheckModular = 1; // 运行过程中检查模块标志
//			}
//			if (j == 1)
//			{
//				Pw_Link4IP1 = 0;
//				Pw_Link4IP2 = 0;
//				Pw_Link4IP3 = 0;
//				Pw_Link4IP4 = 0;
//				Pw_Link4Port = 0;
//				j = 0; // 下个参数进行检查用
//			}
//		}
//		else
//		{
//			sprintf((char *)p_TmpBuf, "GPW+04,IP4:\"%s\"", SMS_SetERROR); // 写要返回的IP4内容错误。
//		}
//		sim7600ce_sms_send(p_TmpBuf); // 返回短信
//	}
//	else if (strstr((const char *)SMSbuf, "GPW+05,")) // 检查写的是RF的地址。举例(GPW+05,24;)
//	{
//		if (SMSbuf[7] >= '0' && SMSbuf[7] <= '9' && SMSbuf[6] == ',')
//		{
//			j = 0;					 // 检查用
//			NowWriteOK = 0;			 // 检查用
//			for (i = 0; i <= 4; i++) // RF的地址取出16进制数
//			{
//				if (SMSbuf[7 + i] == ';') // 说明RF地址数据长度字符串内容没有了
//				{
//					Pw_RFModularAddress = atoi(&SMSbuf[7], i); // RF地址转换为16进制
//					NowWriteOK = 1;
//					break; //
//				}
//			}
//			if (NowWriteOK == 0) // 没有写进去
//			{
//				sprintf((char *)p_TmpBuf, "GPW+05,RF Address:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//				j = 1;
//			}
//			NowWriteOK = 0; // 下一个变量判断用
//			if (j == 0)
//			{
//				SPI_FMRAM_BufferWrite((u8 *)(&Pw_ParLst_GPRS[0]), 0, FLASH_PAR_SIZE * 2); // FLASH_PAR_SIZE 256（设定参数一个扇区）
//				sprintf((char *)p_TmpBuf, "GPW+05,RF Address:\"%s\"", SMS_SetOk);		  // 写要返回的DTU内容。

//				F_RunAgain = 1;		   // 重新运行模块
//				sim_ready = 0;		   // 检查sim卡准备就绪
//				F_RunCheckModular = 1; // 运行过程中检查模块标志
//			}
//			j = 0;
//		}
//		else
//		{
//			sprintf((char *)p_TmpBuf, "GPW+05,RF Address:\"%s\"", SMS_SetERROR); // 写要返回的宏电传输协议内容错误。
//		}
//		sim7600ce_sms_send(p_TmpBuf); // 返回短信
//	}
//	else if (strstr((const char *)SMSbuf, "GPW+06,")) // 检查写的是DTU号
//	{
//		if (SMSbuf[7] >= '0' && SMSbuf[7] <= '9' && SMSbuf[18] == ';')
//		{
//			Pw_DTUIDNUM1 = atoi(&SMSbuf[7], 4);										  // 保存Pw_DTUIDNUM1
//			Pw_DTUIDNUM2 = atoi(&SMSbuf[11], 4);									  // 保存Pw_DTUIDNUM2
//			Pw_DTUIDNUM3 = atoi(&SMSbuf[15], 3);									  // 保存Pw_DTUIDNUM3
//			SPI_FMRAM_BufferWrite((u8 *)(&Pw_ParLst_GPRS[0]), 0, FLASH_PAR_SIZE * 2); // FLASH_PAR_SIZE 256（设定参数一个扇区）
//			sprintf((char *)p_TmpBuf, "GPW+06,DTU:\"%s\"", SMS_SetOk);				  // 写要返回的DTU内容。

//			F_RunAgain = 1;		   // 重新运行模块
//			sim_ready = 0;		   // 检查sim卡准备就绪
//			F_RunCheckModular = 1; // 运行过程中检查模块标志
//		}
//		else
//		{
//			sprintf((char *)p_TmpBuf, "GPW+06,DTU:\"%s\"", SMS_SetERROR); // 写要返回的DTU内容错误。
//		}
//		sim7600ce_sms_send(p_TmpBuf); // 返回短信
//	}
//	else if (strstr((const char *)SMSbuf, "GPW+07,")) // 检查写的是透传还是宏电传输协议。=1为宏电协议，=0为普通透传。举例(GPW+07,01;)
//	{
//		if (SMSbuf[7] >= '0' && SMSbuf[7] <= '9' && SMSbuf[6] == ',')
//		{
//			j = 0;					 // 检查用
//			NowWriteOK = 0;			 // 检查用
//			for (i = 0; i <= 4; i++) // 透传还是宏电传输协议模式取出16进制数
//			{
//				if (SMSbuf[7 + i] == ';') // 说明透传还是宏电传输协议模式数据长度字符串内容没有了
//				{
//					Pw_GPRSRTModeSelect = atoi(&SMSbuf[7], i); // 透传还是宏电传输协议模式转换为16进制
//					NowWriteOK = 1;
//					break; //
//				}
//			}
//			if (NowWriteOK == 0) // 没有写进去
//			{
//				sprintf((char *)p_TmpBuf, "GPW+07,HongDianMode:\"%s\"", SMS_SetERROR); // 写要返回的IP1内容错误。
//				j = 1;
//			}
//			NowWriteOK = 0; // 下一个变量判断用
//			if (j == 0)
//			{
//				SPI_FMRAM_BufferWrite((u8 *)(&Pw_ParLst_GPRS[0]), 0, FLASH_PAR_SIZE * 2); // FLASH_PAR_SIZE 256（设定参数一个扇区）
//				sprintf((char *)p_TmpBuf, "GPW+07,HongDianMode:\"%s\"", SMS_SetOk);		  // 写要返回的DTU内容。

//				F_RunAgain = 1;		   // 重新运行模块
//				sim_ready = 0;		   // 检查sim卡准备就绪
//				F_RunCheckModular = 1; // 运行过程中检查模块标志
//			}
//			j = 0; // 下个参数进行检查用
//		}
//		else
//		{
//			sprintf((char *)p_TmpBuf, "GPW+07,HongDianMode:\"%s\"", SMS_SetERROR); // 写要返回的宏电传输协议内容错误。
//		}
//		sim7600ce_sms_send(p_TmpBuf); // 返回短信
//	}
//}

//// GPRS指示灯运行过程中的状态
////
// void sim7600ce_GPRSLEDRUN(void)
//{
//	//	if (T_GPRSLEDTime != SClk10Ms) // GPRS指示灯运行过程中的状态,每0.5秒切换一次状态
//	//	{
//	//		T_GPRSLEDTime = SClk10Ms; //
//	//		C_GPRSLEDTime++;
//	//		if (C_GPRSLEDTime >= 40) // 0.4S
//	//		{
//	//			T_GPRSLEDTime = 100;
//	//			C_GPRSLEDTime = 0;
//	//			LEDGPRS = ~LEDGPRS; //电源灯闪烁
//	//		}
//	//	}

//	// ZCL 2017.5.25 信号质量不好的时候，闪烁
//	if (C_NetLedLightCSQ > 300 && (w_GprsCSQ < 8 || w_GprsCSQ == 99))
//	{
//		C_NetLedLightCSQ = 0;
//		if (NET_STA > 0)
//			NET = 0; //=1,灭；=0,亮
//		else
//			NET = 1; //=1,灭；=0,亮
//	}
//	else if (w_GprsCSQ >= 8 && w_GprsCSQ != 99)
//		NET = 1; //=1,灭；=0,亮

//	// GPRS模块的：PWR.DATA.NET 指示灯的动作 2013.9.1
//	// S_M35来控制PWR.DATA.NET灯（因为不同的阶段，指示灯的闪烁有不同的含义）
//	//	if(S_M35<0x11)
//	if (F_RunAgain == 1)
//	{
//		if (S_check_status < 7)
//		{
//			PWR_LIGHT = 1; //=1,灭；=0,亮
//			// NET=1;					//=1,灭；=0,亮   ZCL 2017.5.25  用NET指示灯来闪烁指示信号不好的时候

//			// 1. DATA先闪烁，代表在找网络
//			if (C_LedLight > 300)
//			{
//				C_LedLight = 0;
//				if (COM2_DATASTA == 1)
//					COM2_DATA = 0; //=1,灭；=0,亮
//				else
//					COM2_DATA = 1; //=1,灭；=0,亮
//			}
//		}
//		else
//		{
//			// 2. NET常亮
//			NET = 0; //=1,灭；=0,亮
//			// 2.1 找到网络，DATA停止闪烁
//			COM2_DATA = 1; //=1,灭；=0,亮
//		}
//	}
//	else
//	{
//		// 3.1 PWR灯
//		if (CGD0_LinkConnectOK[0] + CGD0_LinkConnectOK[1] + CGD0_LinkConnectOK[2] + CGD0_LinkConnectOK[3] == 0)
//		{
//			// 3.1.1 PWR  闪烁较快  表示未能与GPRS网络联接
//			if (C_LedLight > 50)
//			{
//				C_LedLight = 0;
//				if (PWR_STA > 0)
//					PWR_LIGHT = 0; //=1,灭；=0,亮
//				else
//					PWR_LIGHT = 1; //=1,灭；=0,亮
//			}
//		}
//		else
//		{
//			// 3.1.2 PWR  闪烁较慢  表示成功与GPRS网络连接
//			if (C_LedLight > 1000)
//			{
//				C_LedLight = 0;
//				if (PWR_STA > 0)
//					PWR_LIGHT = 0; //=1,灭；=0,亮
//				else
//					PWR_LIGHT = 1; //=1,灭；=0,亮
//			}
//		}

//		// 3.2 COM2_DATA灯
//		if (Cw_Rcv2 + Cw_Txd2 > 0)
//		{
//			// COM2_DATA 翻转  2014.11.27修改
//			// ZCL 2020.3.26			GPIO_WriteBit(GPIOC, GPIO_Pin_12,
//			//		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_12))));		//ZCL 2018.12.4
//			COM2_DATA = 0;
//		}
//		else //=1,灭
//			COM2_DATA = 1;

//		/*TXD和RXD都是低电平有效，平时高电平 2013.8.30 */
//		// 2014.11.27修改 COM3_DATA 翻转
//		// 2014.12.11 COM3_DATA不受S_M35控制，根据数据翻转
//		if (Cw_Rcv3 + Cw_Txd3 > 0)
//		{
//			// COM3_DATA 翻转 2014.11.27修改
//			// ZCL 2020.3.26		GPIO_WriteBit(GPIOD, GPIO_Pin_5,
//			//  								 (BitAction)((1-GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_5))));		//ZCL 2018.12.4	 凑得，没有这个脚
//			COM3_DATA = 0;
//		}
//		else //=1,灭
//			COM3_DATA = 1;

//		// 2016.5.27 加入DATA灯如果一直亮10秒，看门狗重启。2016.5.27
//		if (T_DataLampAllLight != SClk10Ms)
//		{
//			if (COM2_DATASTA == 0)
//			{
//				T_DataLampAllLight = SClk10Ms;
//				C_DataLampAllLight++;
//				if (C_DataLampAllLight > 2000)
//				{
//					C_DataLampAllLight = 0;

//					//				S_M35 = 0x01;						//模块关机，重启
//					LCD_DLY_ms(3000); // 此时间内没有喂狗，设备复位！ 2013.7.3
//				}
//			}
//			else if (C_DataLampAllLight > 0)
//				C_DataLampAllLight--;
//		}
//	}
//}

//// 处理GPRS模块接收到的数据函数
// void sim7600ce_DoWithData(void)
//{
//	//	u8 i=0;       						// 临时变量 k,s,
//	u16 j, m;
//	uchar s; // ttp:transparent protocol 透明协议
//	uint k, len1, len2;
//	u16 *p_wRead;
//	u8 *p_bMove;

//	//=1 DDP协议 GprsPar[EnDDP0Base+ChannelNo]
//	if (GprsPar[EnDDP0Base + ChannelNo])
//	{
//		ptr2 = (uchar *)strstr((char *)ReceiveGPRSBuf, "{"); //{的位置 =0x7B
//		if (ptr2 != NULL)
//		{
//			// 1. DTU注册	 7B后面的字节是功能码，0x81心跳包注册功能码
//			if (*(ptr2 + 1) == 0x81)
//			{
//				ZhuCeOkFLAG[*(ptr + 10) - 0x30] = 1; // ptr+10
//			}
//			// 2. 查询DTU参数
//			else if (*(ptr2 + 1) == 0x8B)
//				B_Com3Cmd8B = 1;
//			// 3. 设置DTU参数
//			else if (*(ptr2 + 1) == 0x8D)
//				B_Com3Cmd8D = 1;
//			// 4. 服务器分离终端，DTU重启指令
//			else if (*(ptr2 + 1) == 0x82)
//			{
//				//			S_M35 = 0x01;
//				F_RunAgain = 1;
//				F_RunCheckModular = 1;
//				B_RestartNo = 1; // ZCL 2019.4.19 重启序号，方便打印观察问题
//								 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
//			}

//			// 2. 上位机通过DTU给串口2的指令(可能不需要返回数据)
//			// UDP: RECV FROM:112.125.89.8:34491 +IPD19 7B 89 00 10 31 33 39 31 32 33 34 35 36 37 38 7B 31 32 33
//			// TCP: RECV FROM:112.125.89.8:34491 +IPD19 7B 89 00 13 31 33 39 31 32 33 34 35 36 37 38 31 32 33 7B
//			else if (*(ptr2 + 1) == 0x89) //&& *(ptr+i+19)==0x7B
//			{
//				B_Com3Cmd89 = 1;
//			}
//			else
//				B_OtherCmd = 1; //=1，上位机通过DTU给串口2的其他指令，以后再加具体指令

//			C_Com3NoRcv = 0; // ZCL 2017.5.30 COM0 没有接收网络数据计数器
//		}

//		// if(B_Com3Cmd89 || B_OtherCmd) 2014.12.5 移到后面
//	}

//	// 1.连接上，是B_Com3Cmd89.B_OtherCmd命令转发(DDP协议的需要解析,透明协议的直接传输)
//	if (B_Com3Cmd89 || B_OtherCmd)
//	{
//		B_Com3Cmd89 = 0;
//		B_OtherCmd = 0;

//		// 接收到的数值长度，DDP协议下需要减16，正常情况下不用减
//		len1 = ReceiveGPRSBufOneDateLen + ReceiveGPRSBufTwoDateLen + ReceiveGPRSBufThreeDateLen + ReceiveGPRSBufFourDateLen;
//		if (GprsPar[EnDDP0Base + ChannelNo])
//		{
//			if (len1 >= 16) // ZCL 2019.4.16 加上条件
//				len2 = len1 - 16;
//			else
//				len2 = 0;
//		}
//		else
//			len2 = len1;

//		// 2012.10.15 加入DDP协议(TCP.UDP数据格式不同)和透明协议(TCP.UDP一样)
//		// 2012.10.10 加入TCP协议(=1)和UDP协议(=0)
//		// UDP:7B 89 00 10 31 33 39 31 32 33 34 35 36 37 38 7B 31 32 33
//		// UDP说明：7B(起始标志，固定值) 89(包类型，固定值) 00 10(包长度，2字节，固定值) 31 33 39 31 32 33 34 35 36 37 38(DTU身份识别码，11字节) 7B(结束标志，固定值) 31 32 33(用户数据，≤1024字节)
//		// TCP:7B 89 00 13 31 33 39 31 32 33 34 35 36 37 38 31 32 33 7B
//		// TCP说明:7B(起始标志，固定值) 89(包类型，固定值) 00 13(包长度，2字节，减0x10为实际值) 31 33 39 31 32 33 34 35 36 37 38(DTU身份识别码，11字节) 31 32 33(用户数据，≤1024字节) 7B(结束标志，固定值)
//		for (k = 0; k < len2; k++) // 串口3(GPRS模块)接收到的发送到串口2
//		{
//			//=1 DDP协议 GprsPar[EnDDP0Base+ChannelNo]
//			if (GprsPar[EnDDP0Base + ChannelNo])
//			{
//				//=0,UDP
//				if (GprsPar[LinkTCPUDP0Base + ChannelNo] == 0)
//					Txd2Buffer[k] = *(ptr2 + 16 + k); // 去掉UDP的包头，存入Txd2Buffer
//				//=1,TCP
//				else
//					Txd2Buffer[k] = *(ptr2 + 15 + k); // 去掉TCP的包头，存入Txd2Buffer
//			}
//			//=0 透明协议 GprsPar[EnDDP0Base+ChannelNo]
//			else
//			{
//				Txd2Buffer[k] = ReceiveGPRSBuf[k]; // 直接存入Txd2Buffer
//			}
//		}

//		// #define	F_GprsTransToCom			MEM_ADDR(BITBAND( (u32)&Pw_GprsSet, 1 ))	// GPRS透传模式，输出到COM2		0/1

//		// ZCL 2019.3.11 主机GPRS模式下，默认不输出信息到串口	0/1
//		// #define	F_GprsMasterNotToCom	MEM_ADDR(BITBAND( (u32)&Pw_GprsSet, 2 ))	// MasterNotToCom

//		// ZCL 2019.3.11 主机GPRS模式，设定输出信息到串口	 2019.3.11
//		// #define	F_GprsMasterToCom			MEM_ADDR(BITBAND( (u32)&Pw_GprsSet, 3 ))	// MasterToCom

//		// ZCL 2019.3.12 主机GPRS模式，处理MODBUS命令，然后自己返回数据
//		// 注：这里使用的是Txd2Buffer，上面填充的。代替Rcv3Buffer
//		if (F_GprsMasterNotToCom || F_GprsMasterToCom)
//		{
//			if (Txd2Buffer[0] == 2) // 从地址检测－接收到的上位机查询指令
//			{
//				j = CRC16(Txd2Buffer, len2 - 2); // CRC 校验
//				k = j >> 8;
//				s = j;
//				if (k == Txd2Buffer[len2 - 2] && s == Txd2Buffer[len2 - 1])
//				{							// CRC校验正确
//					if (Txd2Buffer[1] == 3) // 03读取保持寄存器
//					{
//						B_Com3Cmd03 = 1;
//						j = Txd2Buffer[2];
//						Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];
//					}
//					else if (Txd2Buffer[1] == 6) // 06预置单寄存器
//					{
//						// C_ForceSavPar=0;		// 强制保存参数计数器=0
//						B_Com3Cmd06 = 1;
//						j = Txd2Buffer[2];
//						Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];

//						// ZCL 2021.7.10  处理要修改的数据  ICCARD 取水机
//						// ZCL 2021.11.17

//						// 这是预置本机的 设定参数； 这里我们需要的是预置 LORA从机的参数，需要转发的！
//						/* 									m=Txd2Buffer[4];
//										w_ParLst[Lw_Com3RegAddr]=(m<<8)+Txd2Buffer[5];	 */

//						// ZCL 2021.11.17 转发指令
//						B_LoraSendWrite = 1; // ZCL 2021.11.17
//					}

//					// ZCL 2021.11.17
//					else if (Txd2Buffer[1] == 16) // 16预置多寄存器
//					{
//						// C_ForceSavPar=0;		// 强制保存参数计数器=0
//						B_Com3Cmd16 = 1;
//						j = Txd2Buffer[2];
//						Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];

//						// 这是预置本机的 设定参数； 这里我们需要的是预置 LORA从机的参数，需要转发的！
//						/* 									if ( Txd2Buffer[5] < 30 )			//ZCL 2021.11.17  限制下
//										{
//											p_bGen = Txd2Buffer;
//											p_wTarget = w_ParLst;
//											for ( k = 0; k < Txd2Buffer[5] ; k++ )		// Rcv0Buffer[5]=字数
//											{
//												m = *( p_bGen + 7 + k * 2 );
//												n = *( p_bGen + 7 + k * 2 + 1 );
//												*( p_wTarget + Lw_Com3RegAddr + k ) = ( m << 8 ) + n;
//											}
//										} */

//						// ZCL 2021.11.17 转发指令
//						B_LoraSendWrite = 1; // ZCL 2021.11.17
//						B_LoraSendWriteLength = len2;
//						if (B_LoraSendWriteLength > BUFFER_SIZE)
//							B_LoraSendWriteLength = BUFFER_SIZE;	// YLS 限制大小，防止数组溢出
//						for (j = 0; j < B_LoraSendWriteLength; j++) // Rcv0Buffer[5]=字数 这个地方要加限制接收的字长，防止超出数组长度
//						{
//							LoRaTxBuf2[j] = Txd2Buffer[j];
//						}
//					}
//				}
//			}
//		}

//		// ZCL 2019.3.12  主机GPRS模式下，解析出指令，准备返回！
//		if (B_Com3Cmd03)
//		{
//			Txd3TmpBuffer[0] = 2;				  // 设备从地址Pw_EquipmentNo
//			Txd3TmpBuffer[1] = Txd2Buffer[1];	  // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
//			Txd3TmpBuffer[2] = Txd2Buffer[5] * 2; // Rcv2Buffer[5]=字数 　

//			// 转化地址 ZCL 2015.7.11

//			if (Txd2Buffer[5] > 100)
//				Txd2Buffer[5] = 100; // ZCL 2019.4.26 限制大小，防止数组溢出

//			p_wRead = w_GprsParLst; // 读w_GprsParLst PAR区	2019.3.14
//			p_bMove = Txd3TmpBuffer;
//			for (k = 0; k < Txd2Buffer[5]; k++) // 填充查询内容
//			{
//				m = *(p_wRead + Lw_Com3RegAddr + k);
//				*(p_bMove + 3 + k * 2) = m >> 8;
//				*(p_bMove + 3 + k * 2 + 1) = m;
//			}
//			Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, Txd3TmpBuffer[2] + 3);
//			Txd3TmpBuffer[Txd3TmpBuffer[2] + 3] = Lw_Txd3ChkSum >> 8; // /256
//			Txd3TmpBuffer[Txd3TmpBuffer[2] + 4] = Lw_Txd3ChkSum;	  // 低位字节
//			Cw_Txd3TmpMax = Txd3TmpBuffer[2] + 5;
//			//
//			//
//			B_Com3Cmd03 = 0;
//			Cw_Txd3 = 0;

//			// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
//			B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络

//			// RS485_CON=1;
//			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
//			// USART_SendData(USART2,Txd2Buffer[Cw_Txd2++]);
//			// USART_ITConfig(USART2, USART_IT_TC, ENABLE);				// 开始发送.

//			// UART2_Output_Datas(Txd3TmpBuffer, Cw_Txd3TmpMax);
//		}

//		// ZCL 2019.3.12 今后添加其他指令  2021.11.17
//		else if (B_Com3Cmd06) // ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
//		{
//			B_Com3Cmd06 = 0;
//			//...
//			// ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
//			Txd3TmpBuffer[0] = 2;			  // 设备从地址Pw_EquipmentNo
//			Txd3TmpBuffer[1] = Txd2Buffer[1]; // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
//			Txd3TmpBuffer[2] = Txd2Buffer[2]; // 　
//			Txd3TmpBuffer[3] = Txd2Buffer[3]; //
//			Txd3TmpBuffer[4] = Txd2Buffer[4]; //
//			Txd3TmpBuffer[5] = Txd2Buffer[5]; //

//			Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, 6);
//			Txd3TmpBuffer[6] = Lw_Txd3ChkSum >> 8; // /256
//			Txd3TmpBuffer[7] = Lw_Txd3ChkSum;	   // 低位字节
//			Cw_Txd3TmpMax = 8;
//			//
//			//
//			B_Com3Cmd06 = 0;
//			Cw_Txd3 = 0;

//			// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
//			B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络

//			// RS485_CON=1;
//			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
//			// USART_SendData(USART2,Txd2Buffer[Cw_Txd2++]);
//			// USART_ITConfig(USART2, USART_IT_TC, ENABLE);				// 开始发送.
//		}

//		// ZCL 2019.3.12 今后添加其他指令  2021.11.17
//		else if (B_Com3Cmd16) // ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
//		{
//			B_Com3Cmd16 = 0;
//			//...
//			// ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
//			Txd3TmpBuffer[0] = 2;			  // 设备从地址Pw_EquipmentNo
//			Txd3TmpBuffer[1] = Txd2Buffer[1]; // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
//			Txd3TmpBuffer[2] = Txd2Buffer[2]; // 　
//			Txd3TmpBuffer[3] = Txd2Buffer[3]; //
//			Txd3TmpBuffer[4] = Txd2Buffer[4]; //
//			Txd3TmpBuffer[5] = Txd2Buffer[5]; //

//			Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, 6);
//			Txd3TmpBuffer[6] = Lw_Txd3ChkSum >> 8; // /256
//			Txd3TmpBuffer[7] = Lw_Txd3ChkSum;	   // 低位字节
//			Cw_Txd3TmpMax = 8;
//			//
//			//
//			B_Com3Cmd16 = 0;
//			Cw_Txd3 = 0;

//			// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
//			B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络

//			// RS485_CON=1;
//			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
//			// USART_SendData(USART2,Txd2Buffer[Cw_Txd2++]);
//			// USART_ITConfig(USART2, USART_IT_TC, ENABLE);				// 开始发送.
//		}

//		// ZCL 2019.3.12 	对接收到的数据，进行串口输出
//		if (F_GprsTransToCom || F_GprsMasterToCom)
//		{
//			Cw_Txd2Max = len2;
//			Cw_Txd2 = 0;

//			// RS485_CON=1;		//2013.9.2
//			//  2010.8.5 周成磊 TXE改成TC，一句改为两句
//			// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
//			USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
//			USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
//			//			UART2_Output_Datas(Txd2Buffer, Cw_Txd2Max); // 发送到COM2
//		}
//		//		F_GPRSSendFinish = 0;
//		//		sim7600ceGPRSUpLoaded(Txd3TmpBuffer, Cw_Txd3TmpMax, 1);
//	}

//	// 2.1 (有连接，非B_Com3Cmd89.B_OtherCmd命令，串口1接收转发到串口2) 目前没有用
//	// 2.2 无连接，一些AT指令返回结果，串口1接收转发到串口2 2013.5.20
//	// ZCL 2019.4.23 必须等数据串口转发完，再进行printf信息输出
//	else if (GprsPar[ConsoleInfoTypeBase]) // 控制台信息类型,=1输出调试信息
//	{
//		if (Cw_BakRcv3 >= 512)
//			Cw_BakRcv3 = 500; // ZCL 2019.4.26 限制大小，防止数组溢出

//		for (k = 0; k < Cw_BakRcv3; k++)
//		{
//			Txd2Buffer[k] = Rcv3Buffer[k]; // 串口3(GPRS模块)接收到的发送到串口2
//		}
//		Cw_Txd2Max = Cw_BakRcv3;
//		Cw_Txd2 = 0;
//		RS485_CON = 1; // 2013.9.2
//		// 2010.8.5 周成磊 TXE改成TC，一句改为两句
//		// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
//		USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
//		USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
//		//		UART2_Output_Datas(Txd2Buffer, Cw_Txd2Max); // 发送到COM2
//	}

//	// 2.数据发送
//	Cw_Txd3Max = 0; // 2014.12.8
//	// 2012.10.4 加入DDP协议和透明协议的选择
//	//=1，DDP协议
//	if (GprsPar[EnDDP0Base + ChannelNo]) //=1，DDP协议；=0，透明协议
//	{
//		Gprs_TX_Fill(GPRS_ZhenTou, 16); // DDP协议
//		// 修改DTU身份识别码
//		for (k = 0; k < 11; k++)
//			Txd3Buffer[4 + k] = GprsPar[DtuNoBase + k];

//		// 2012.10.10 加入TCP协议和UDP协议的区别
//		// UDP: 7B 09 00 10 31 33 39 31 32 33 34 35 36 37 38 7B 61 62 63
//		// TCP: 7B 09 00 13 31 33 39 31 32 33 34 35 36 37 38 61 62 63 7B

//		// TCP
//		if (GprsPar[LinkTCPUDP0Base + ChannelNo]) //=1，TCP协议；=0，UDP协议
//		{
//			Cw_Txd3Max = 15; // 修改Cw_Txd3Max值从16改成15！ 2012.10.10

//			// ZCL 2019.3.12 加限制，透传是原先程序；新主机GPRS模式是新程序，填充数据不一样。
//			if (F_GprsTransToCom)
//			{
//				Gprs_TX_Fill(Rcv2Buffer, Cw_BakRcv2); // 填充数据
//				Gprs_TX_Byte(0x7B);
//				// 2012.10.10 修改发送长度（使用TCP协议，则长度为所有发送数据总长度！ 周成磊）
//				Txd3Buffer[3] = 16 + Cw_BakRcv2;
//			}

//			// ZCL 2019.3.12
//			else if (F_GprsMasterNotToCom || F_GprsMasterToCom)
//			{
//				Gprs_TX_Fill(Txd3TmpBuffer, Cw_Txd3TmpMax); // 填充数据 ZCL 2019.3.12

//				Gprs_TX_Byte(0x7B);
//				// 2012.10.10 修改发送长度（使用TCP协议，则长度为所有发送数据总长度！ 周成磊）
//				Txd3Buffer[3] = 16 + Cw_Txd3TmpMax;
//			}
//		}

//		// UDP时
//		else
//		{
//			// Gprs_TX_Fill(Rcv2Buffer,Cw_BakRcv2);		//填充数据
//			// ZCL 2019.3.12 加限制，透传是原先程序；新主机GPRS模式是新程序，填充数据不一样。
//			if (F_GprsTransToCom)
//				Gprs_TX_Fill(Rcv2Buffer, Cw_BakRcv2); // 填充数据

//			else if (F_GprsMasterNotToCom || F_GprsMasterToCom)
//				Gprs_TX_Fill(Txd3TmpBuffer, Cw_Txd3TmpMax); // 填充数据 ZCL 2019.3.12
//		}
//	}
//	//=0，透明协议
//	else // 透明协议不用加头，也不分TCP,UDP区别
//	{
//		// Gprs_TX_Fill(Rcv2Buffer,Cw_BakRcv2);		//不分TCP,UDP区别

//		// ZCL 2019.3.12 加限制，透传是原先程序；新主机GPRS模式是新程序，填充数据不一样。
//		if (F_GprsTransToCom)
//			Gprs_TX_Fill(Rcv2Buffer, Cw_BakRcv2); // 填充数据

//		else if (F_GprsMasterNotToCom || F_GprsMasterToCom)
//			Gprs_TX_Fill(Txd3TmpBuffer, Cw_Txd3TmpMax); // 填充数据 ZCL 2019.3.12
//	}

//	F_GPRSSendFinish = 0;
//	sim7600ceGPRSUpLoaded(Txd3Buffer, Cw_Txd3Max, 1);

//	memset(ReceiveGPRSBuf, 0, RCV3_MAX);
//	memset(Rcv3Buffer, 0, RCV3_MAX);
//}

// void UART1_Output_Datas(uint8_t S_Out[], uint8_t Counter)
//{
//	uint8_t i;
//	for (i = 0; i < Counter; i++)
//	{
//		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
//			; // 等待发送结束
//		USART_SendData(USART1, S_Out[i]);
//	}
// }

// void UART2_Output_Datas(uint8_t S_Out[], uint8_t Counter)
//{
//	uint8_t i;
//	for (i = 0; i < Counter; i++)
//	{
//		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET)
//			; // 等待发送结束
//		USART_SendData(USART2, S_Out[i]);
//	}
// }

// void UART3_Output_Datas(uint8_t S_Out[], uint16_t Counter)
//{
//	uint8_t i;
//	for (i = 0; i < Counter; i++)
//	{
//		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
//			; // 等待发送结束
//		USART_SendData(USART3, S_Out[i]);
//	}
//	//        uart_putchar(databuf[i]); //LINK TO UART
// }

//// sim7600ce主运行程序
////
// void sim7600ce_GPRSRUN(void)
//{
//	Fill_data();
//	sim7600ce_NetworkCheck();		  // 运行过程中检查网络是否正常，不正常重新启动
//	sim7600ce_connectstaModularNum(); // 获得模块需要连接的IP数量
//									  //	sim7600ce_SendHeartbeat0();		  // 发送模块心跳
//	sim7600ce_SendHeartbeat();		  // 发送模块心跳
//	sim7600ce_ReconnectNet();		  // 检查模块是否需要重新连接
//	sim7600ce_ReceiveData();		  // 接收GPRS发送来的数据
//									  //	Com3_SlaveSend();
//	sim7600ce_GPRSLEDRUN();			  // GPRS指示灯切换
//	sim7600ce_smsread_UpLoaded();	  ////短信读数据加载
//	sim7600ce_smsWrite_UpLoaded();	  // 短信写数据加载
// }
