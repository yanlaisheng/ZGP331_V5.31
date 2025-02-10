/**
  ******************************************************************************
  * @file    DoWith.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.27
  * @date    2014-01-03
  * @brief   数字量输入检测，数字量输出,模拟量输入检测，模拟量输出,其他本机操作
	******************************************************************************
  * @attention	2014.1.3
	1.UID密码的增加: 2013.10.26
		在加密公式中,输入参数有两个：
			uint8_t D[12];//D[12]为公式的输入数组(注意与ID[12]区分).
			uint32_t Fml_Constant; // 用户指定的32位常数
		加密公式的输出为一个数组:
			uint8_t Result[4]; // 公式计算结果输出,四个字节
		比如要读芯片的全球唯一ID值:
			uint8_t* UID=(uint8_t*)ID的起始地址;
			以后只要当数组用就可以了.比如UID[0]就是芯片最低字节的全球唯一ID.

		u32 Fml_Constant; 	//输入到公式的常数
		u8 *C= (u8*)&Fml_Constant;	//把常量转换为数组
		u16 Fml_CRC16;
	******************************************************************************
	*/

/* Includes ------------------------------------------------------------------*/
#include "GlobalConst.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "DoWith.h"
#include "spi_flash.h"
#include "com2_232.h" //ZCL 2019.3.29

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Init Structure definition */

/* Private variables ---------------------------------------------------------*/
//
uchar T_BootParLst; // 初始化参数表列
uint C_BootParLst;
uchar S_BootParLst;
uchar T_SavePar; // 保存参数
uint C_SavePar;
uchar S_SavePar;
uchar T_ForceSavPar; // 强制保存参数
uchar B_ForceSavPar; // 强制保存参数标志
uint C_ForceSavPar;	 // 强制保存参数 计数器
//
uint C_PowenOnDelay; // 上电延时后,进行其他判断或者动作
uint C_LedLight;	 // LED闪烁延时计数器

uchar T_DataLampAllLight; // 数据DATA灯总亮 2016.5.27 ZCL
uint C_DataLampAllLight;  // 数据DATA灯总亮 计数器

uint C_NetLedLightCSQ; // Net LED闪烁CSQ指示计数器 ZCL 2017.5.25

uchar T_Com2RcvReset; // COM2 接收复位定时器 ZCL 2017.5.26
uint C_Com2RcvReset;  // COM2 接收复位计数器 ZCL 2017.5.26

uchar T_Com2NoRcv; // COM2 没有接收计数器 ZCL 2017.5.30
uint C_Com2NoRcv;  // COM2 没有接收计数器 ZCL 2017.5.30

uchar T_Com3NoRcv; // COM3 没有接收网络数据计数器 ZCL 2017.5.30
uint C_Com3NoRcv;  // COM3 没有接收网络数据计数器 ZCL 2017.5.30

u8 T_LCDBackLight; // ZCL 2017.9.19 背光控制  2018.12.10
u16 C_LCDBackLight;
u32 w32_SecCouter;

u16 T_Reboot; // 定时重启计数器
u16 C_Reboot;
union
{
	float f;
	u8 b[4];
} aa; // ZCL 2019.10.18
/* Private variables extern --------------------------------------------------*/
extern uc8 Gprs_ParBootLst;
extern uint Cw_Txd3;
extern uint Cw_Txd2;
extern uchar S_M35;
extern uchar CGD0_LinkConnectOK[]; // 场景0连接标志
extern uint Cw_Rcv3;			   // 2014.11.27
extern uint Cw_Rcv2;			   // 2014.11.27

extern uchar B_Com3Cmd8B; // 查询DTU参数命令 2012.8.17
extern uchar B_Com3Cmd8D; // 设置DTU参数命令 2012.8.24
extern uchar B_Com3Cmd89; // 服务器发送数据指令 2011.8.24
extern uchar B_OtherCmd;  // 上位机通过DTU给串口2的其他指令（需发送到串口2）

extern uchar S_SendSms;	   // 发送短信步骤
extern uchar S_ReadNewSms; // 读新短信步骤
extern uchar S_SmsFull;	   // 短信满步骤

extern uchar B_GprsDataReturn;	 // GPRS 数据返回。有连接，收到串口2数据，就通过DTU发送出去
extern uchar B_HeartSendFault[]; // 发送心跳包故障标志
extern uchar B_LinkFault[];		 // 发送连接故障标志
extern uchar B_ATComStatus;		 // AT命令状态 2017.5.31
extern uchar ZhuCeOkFLAG[];
extern uchar B_SmsOrFaultHardRestart; // 短信或者故障硬件重启
/* Private function prototypes -----------------------------------------------*/
void Com0_config(void);
void Com1_config(void);
uint CRC16(uchar *pCrcData, uchar CrcDataLen);
void LCD_DLY_ms(u32 Time); // ZCL 2018.12.10

uc16 w_GprsParBootLst[50] = {
	60000, 0, 2, 1, 3,	   // 0-4		0=重启间隔时间 分钟,	1=控制台信息,	2=LORA设备通讯地址
						   //			3=主机从机选择,4=LORA模块1地址
	4, 0, 0, 0, 0,		   // 5-9		5=LORA模块2地址,6=LORA模块3地址
						   //			7=LORA模块4地址 8=设备类型0=双驱泵，1=变频电机	9=设备功率（备用）
	0, 2, 0, 0, 0,		   // 10-14		10=纬度1 11=纬度2 12=经度1
						   //			13=经度2 14=LORA模块5地址
	0, 1, 9, 0, 0,		   // 15-19		15=LORA模块6地址 16=LoRa设定，1=启用LoRa 17=Gprs 设定，9=启用GPRS
						   //			18=发射测试2 19=发射测试3
	6, 0, 0, 0, 0,		   // 20-24		20=液晶屏按键模式 21=菜单模式 22=电流小数
						   //			23=LORA模块7地址 24=LORA模块8地址
	0, 0, 0, 0, 0,		   // 25-29		25=秒 26=分 27=时
						   //			28=日 29=测试
	57600, 9600, 3, 0, 0,  // 30-34		30=串口3波特率 31=串口2波特率 32=串口2 数据位长度
						   //			33=串口2 停止位长度 34=串口2 校验位长度
						   //		 	3,  435,   20,    9,    7,	// 35-39	35=串口2 流量控制 36= 37=		ZCL 2019.9.7
	3, 435, 20, 9, 7,	   // 35-39		35=串口2 流量控制 36=Lora 频率 37=  发射功率
						   //			38=信号带宽BW 	39= 扩频因子SF
	2, 1, 0, 0, 0,		   // 40-44		40= ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8] 41=CrcOn 42=ImplicitHeaderOn
						   //			43=RxSingleOn 44=FreqHopOn  	//ZCL 2020.3.27 43项RxSingleOn改成0
	4, 128, 1000, 1000, 0, // 45-49		45=HopPeriod 46=Pw_LoRaSetPayLoadLength  47=Pw_LoRaSetTxPacketTimeOut
						   //			48=Pw_LoRaSetRxPacketTimeOut 49=特殊协议
}; // 0,   1,    2,    3,    4,	//出厂设置
   // 5,   6,    7,    8,    9,	//出厂设置

/* Private functions ---------------------------------------------------------*/

void ADC_DMA_Init(void)
{
	;
	// ZCL 2021.5.4  用ST库的话，需要在ADC_CMD后面加20us左右的延时
	//...
	// Delay_US(30);					//ZCL 2021.10.26

#ifdef GD32

#else

#endif
}

u16 FilterAI(u8 SelChannel) // 过滤模拟量值
{
	u16 i;
	return i;
}

void AnologIn(void)
{
	;
}

void Variable_Init(void) //	变量初始化
{
	// ZCL 2018.8.3  w_Scr 液晶屏的参数数组，ParType:4
	w_ScrVERSION = 530; // 软件版本号 VERSION	// 2017.9.13 -3 2018.7.17

	w_ScrWriteYear = 2025; // 程序编写年
	w_ScrWriteDate = 205;  // 程序编写日期

	w_GprsSoftVer = 109;		// 软件版本号 VERSION
	C_PowenOnDelay = 0;			// 上电延时后,进行其他判断或者动作
	Pw_FastKeyBetweenMS = 1000; // 快键键间隔MS
	//
	Lw_SaveEquipOperateStatus = 1000; // ZCL 2015.9.7
	Lw_SaveEquipAlarmStatus = 1000;
	Lw_SaveDspStopStatus = 1000; // ZCL 2015.9.7
	Lw_SaveDspFaultStatus = 1000;
	//
	C_LCDBackLight = 1; // ZCL 2017.9.13 =1开始计时，会控制背光亮
	// LoRa通讯计数器清零
	w_Lora1Counter2 = 0;
	w_Lora2Counter2 = 0;
	w_Lora3Counter2 = 0;
	w_Lora4Counter2 = 0;

	F_Reset_GPS = 0; // 重启GPS标志

	F_No1_VVVFComm_Fault = 0; // 清变频器通讯故障
	F_No2_VVVFComm_Fault = 0; // 清变频器通讯故障
	F_No3_VVVFComm_Fault = 0; // 清变频器通讯故障
	F_No4_VVVFComm_Fault = 0; // 清变频器通讯故障
	F_No5_VVVFComm_Fault = 0; // 清变频器通讯故障
	F_No6_VVVFComm_Fault = 0; // 清变频器通讯故障
	F_No7_VVVFComm_Fault = 0; // 清变频器通讯故障
	F_No8_VVVFComm_Fault = 0; // 清变频器通讯故障
}

// ZCL 2018.12.11 备注：以前液晶不需要。这里需要因为有GPRS,LOAR,GPS,按键 等参数了。
void ParLst_Init(void) // RAM中参数表列 初始化 (读出)
{
	// ZCL 2019.4.4 读出液晶屏设定参数 从 FM25L16
	SPI_FLASH_CS_LOW();
	SPI_FMRAM_BufferRead((u8 *)(&w_GprsParLst), 0, SCR_PAR_SIZE * 2); // FMADD:0
	// ZCL 2019.4.4  读出GPRS(DTU)设定参数 从 FM25L16
	SPI_FLASH_CS_LOW();
	SPI_FMRAM_BufferRead(GprsPar, 256, DTU_PAR_SIZE); // FMADD:256

	// 2009.12.4  把波特率读出后保存，为了有改变时，修改波特率。
	w_GprsSaveBaudRate2 = Pw_GprsBaudRate2;

	// ZCL 2019.3.22 加入LORA射频参数
	LoRaSettings.RFFrequency = (uint32_t)Pw_LoRaSetFreq * 1000000;
	LoRaSettings.Power = Pw_LoRaSetPower;
	LoRaSettings.SignalBw = Pw_LoRaSetSignalBW; // LORA [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz,
	// 3: 20.8 kHz, 4: 31.2 kHz,5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
	LoRaSettings.SpreadingFactor = Pw_LoRaSetSpreadingFactor;	// LORA [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
	LoRaSettings.ErrorCoding = Pw_LoRaSetErrorCoding;			// LORA [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
	LoRaSettings.CrcOn = Pw_LoRaSetCrcOn;						// [0: OFF, 1: ON]
	LoRaSettings.ImplicitHeaderOn = Pw_LoRaSetImplicitHeaderOn; // [0: OFF, 1: ON]
	LoRaSettings.RxSingleOn = Pw_LoRaSetRxSingleOn;				// [0: Continuous, 1 Single]
	LoRaSettings.FreqHopOn = Pw_LoRaSetFreqHopOn;				// [0: OFF, 1: ON]
	LoRaSettings.HopPeriod = Pw_LoRaSetHopPeriod;				// Hops every frequency hopping period symbols

	LoRaSettings.TxPacketTimeout = Pw_LoRaSetTxPacketTimeOut; // ZCL 2019.9.24 再重新加上
	LoRaSettings.RxPacketTimeout = Pw_LoRaSetRxPacketTimeOut; // ZCL 2019.9.24 再重新加上

	LoRaSettings.PayloadLength = Pw_LoRaSetPayLoadLength;

	// ZCL 2019.9.12 上电RAM中参数表列 初始化时，赋值保存的经纬度
	w_ScrGpsLatAllDu1 = Pw_ScrGpsLatAllDu1;
	w_ScrGpsLatAllDu2 = Pw_ScrGpsLatAllDu2;

	w_ScrGpsLonAllDu1 = Pw_ScrGpsLonAllDu1;
	w_ScrGpsLonAllDu2 = Pw_ScrGpsLonAllDu2;
}

// w_GprsWriteSize: 字的数量
void ParArrayWrite(u16 *p_Top, u16 *p_Base, uint w_GprsWriteSize)
{
	uint i, j;

	// FLASH_PAGE_SIZE=2048（0x800）		低，中密度的STM32是1024

	// 解除FLASH 编程和擦除控制
	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	// EraseCounter 擦除计数器
	FLASH_ErasePage((u32)p_Top);

	// 把指定的数据字写入从开始地址到结束地址的FLASH
	for (i = 0; i < w_GprsWriteSize; i++) // 字符串长度
	{
		j = *p_Base++;
		FLASH_ProgramHalfWord((u32)p_Top++, j);
	}

	// 周成磊加上 2013.1.22 最后应该需要加上 锁定编程和擦除控制 ？
	FLASH_Lock();
}

// w_GprsReadSize: 字的数量
void ParArrayRead(uint *p_Top, uc16 *p_Base, uint w_GprsReadSize)
{
	uint i;
	for (i = 0; i < w_GprsReadSize; i++)
	{
		*p_Top++ = *p_Base++; // zcl 地址自动加2！
	}
}

// w_GprsReadSize: 字的数量
void GprsArrayRead(uchar *p_Top, uc8 *p_Base, uint w_GprsReadSize)
{
	uint i, j;
	for (i = 0; i < w_GprsReadSize; i++)
	{
		j = *p_Base++; // zcl 地址自动加2！
		*p_Top++ = j;  // 高字节
					   //*p_Top++ = (j>>8);		//低字节
	}
}

// ZCL 2018.12.11 备注：以前液晶不需要。这里需要因为有GPRS,LOAR,GPS,按键 等参数了。
void Boot_ParLst(void) // 初始化设定参数
{
	uchar i = 0;

	u16 nw_i, nw_j, nw_k, nw_l; // ZCL 2019.9.11

	// w_GprsModPar=2000,可以修改参数	// w_GprsParInitial==4321 初始化参数
	if (w_GprsModPar == 2000 && w_GprsParInitial == 4321 && S_BootParLst == 0) // ZCL 2019.3.12
	{
		i = 1;
	}
	else if (Pw_LoRaMasterSlaveSel > 2000 && Pw_LoRaEquipmentNo > 2000 && S_BootParLst == 0)
	{
		i = 2;
		B_ForceSavPar = 1; // 2013.10.1
	}
	else if (Pw_LoRaMasterSlaveSel == 0 && Pw_LoRaEquipmentNo == 0 && // 2013.5.22 初始化
			 Pw_LoRaSetSpreadingFactor == 0 && Pw_LoRaSetErrorCoding == 0 && S_BootParLst == 0)
	{
		i = 3;
		B_ForceSavPar = 1; // 2013.10.1
	}
	// ZCL 2018.12.22
	else if (Pw_GprsBaudRate2 > 57600 && S_BootParLst == 0)
	{
		i = 4;
		B_ForceSavPar = 1; // 2013.10.1
	}

	if (i > 0)
	{

		// ZCL 2016.5.26 取消Gprs_ParBootLst参数的读，Gprs参数不再初始化
		// 2016.5.24 dtuid号，初始化的时候不修改DTU号功能。 保存DTU号
		uchar m, DtuID[15];
		for (m = 0; m < 11; m++)
			DtuID[m] = GprsPar[DtuNoBase + m];

		// 2012.7.23 读出GPRS初始化参数 ZCL 2019.4.4  改掉：(uc16 *)(&Gprs_ParBootLst)
		// ZCL 2020.4.5 取消初始化GPRS参数 OK
		GprsArrayRead(GprsPar, &Gprs_ParBootLst, DTU_PAR_SIZE); // GPRS初始化参数

		// 2016.5.24 dtuid号，初始化的时候不修改DTU号。恢复DTU号
		for (m = 0; m < 11; m++)
			GprsPar[DtuNoBase + m] = DtuID[m];
		// ZCL 2017.5.27 上面仿真调试的时候，打开！

		// ZCL 2019.9.11 这几个用于供水画面的SCR参数，不进行初始化，只能手动修改。
		nw_i = Pw_ScrKeyMode;
		nw_j = Pw_ScrMenuMode;
		nw_k = Pw_ScrCurrentBits;
		nw_l = Pw_ScrEquipPower;

		ParArrayRead(w_GprsParLst, w_GprsParBootLst, SCR_PAR_SIZE); // 读出初始化参数

		Pw_ScrKeyMode = nw_i;
		Pw_ScrMenuMode = nw_j;
		Pw_ScrCurrentBits = nw_k;
		Pw_ScrEquipPower = nw_l;

		// ZCL 2019.3.22  赋值测试用！
		/* 		Pw_LoRaSetFreq=LoRaSettings.RFFrequency/1000000;
				Pw_LoRaSetPower=LoRaSettings.Power;
				Pw_LoRaSetSignalBW=LoRaSettings.SignalBw;     // LORA [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz,
				// 3: 20.8 kHz, 4: 31.2 kHz,5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
				Pw_LoRaSetSpreadingFactor=LoRaSettings.SpreadingFactor;  // LORA [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
				Pw_LoRaSetErrorCoding=LoRaSettings.ErrorCoding;      // LORA [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
				Pw_LoRaSetCrcOn=LoRaSettings.CrcOn;                         // [0: OFF, 1: ON]
				Pw_LoRaSetImplicitHeaderOn=LoRaSettings.ImplicitHeaderOn;   // [0: OFF, 1: ON]
				Pw_LoRaSetRxSingleOn=LoRaSettings.RxSingleOn;               // [0: Continuous, 1 Single]
				Pw_LoRaSetFreqHopOn=LoRaSettings.FreqHopOn;                 // [0: OFF, 1: ON]
				Pw_LoRaSetHopPeriod=LoRaSettings.HopPeriod;                 // Hops every frequency hopping period symbols

				Pw_LoRaSetTxPacketTimeOut=LoRaSettings.TxPacketTimeout;			//ZCL 2019.9.24 再重新加上
				Pw_LoRaSetRxPacketTimeOut=LoRaSettings.RxPacketTimeout;			//ZCL 2019.9.24 再重新加上

				Pw_LoRaSetPayLoadLength=LoRaSettings.PayloadLength;
		 */

		// ZCL 2016.5.26 加在这里，否则ParArrayRead一读，就都给恢复成初始值了
		if (i == 1)
			Pw_GprsSetTest = 991; // ZCL 2019.3.22
		else if (i == 2)
			Pw_GprsSetTest = 992; // ZCL 2016.5.26 加上当成一个标志
		else if (i == 3)
			Pw_GprsSetTest = 993; // ZCL 2016.5.26 加上当成一个标志
		i = 0;

		// 电话号码不初始化
		// 暂不保存参数，当w_GprsModPar==5000，可以保存参数到FLASH
		if (B_ForceSavPar == 0)
		{
			w_GprsModPar = 2000;
			w_GprsParInitial = 4321;
			S_BootParLst = 1;
		}
		else
		{
			S_SavePar = 0;
			w_GprsParInitial = 1000; // 2013.10.12 用1000代表上电初始化后，强制保存的参数
			SavePar_Prompt();		 // 2013.10.12 自动初始化后立即保存
		}
	}

	//
	if (T_BootParLst != SClkSecond && S_BootParLst != 0) // 用于MD304L提示状态，人性化设计 ZCL
	{
		T_BootParLst = SClkSecond; // 现在w_GprsParInitial=4321
		C_BootParLst++;
		if (C_BootParLst > 2 && S_BootParLst == 1)
		{
			S_BootParLst = 2;
			w_GprsParInitial = 6000;
		}
		else if (C_BootParLst > 4 && S_BootParLst == 2)
		{
			T_BootParLst = 100;
			C_BootParLst = 0;
			S_BootParLst = 0;
			w_GprsParInitial = 0;
		}
	}
}

// ZCL 2018.12.11 备注：以前液晶不需要。这里需要因为有GPRS,LOAR,GPS,按键 等参数了。
void SavePar_Prompt(void) // 保存参数+状态提示
{
	// uchar i=0;

	if (B_ForceSavPar == 1 && S_SavePar == 0) // 强制保存参数
	{
		w_GprsModPar = 0; // 防止把 w_GprsModPar==5000 保存到FMRAM

		SPI_FLASH_CS_LOW();
		// ZCL 2019.4.4  保存液晶屏设定参数 到 FM25L16
		SPI_FMRAM_BufferWrite((u8 *)(&w_GprsParLst), 0, SCR_PAR_SIZE * 2);
		// ZCL 2019.4.4  保存GPRS(DTU)设定参数 到 FM25L16
		SPI_FMRAM_BufferWrite(GprsPar, 256, DTU_PAR_SIZE);

		w_GprsModPar = 17;
		// 再加修改端子配置 ZCL
		S_SavePar = 1;
		B_ForceSavPar = 0;
		//

		// ZCL 2019.4.4 以后再添加 过程详细记录 和 GPRS故障，重启等到FM25L16
		// SPI_FLASH_Record();
	}

	if ((w_GprsModPar == 5000 || w_ModPar == 5000)
		// ZCL 2019.9.11 加上 供水的修改参数w_ModPar=5000，也保存 液晶屏SCR的参数！OK
		&& S_SavePar == 0)
	{
		w_GprsModPar = 0; // 防止把 w_GprsModPar==5000 保存到FMRAM

		SPI_FLASH_CS_LOW();
		// ZCL 2019.4.4  保存液晶屏设定参数 到 FM25L16
		SPI_FMRAM_BufferWrite((u8 *)(&w_GprsParLst), 0, SCR_PAR_SIZE * 2);
		// ZCL 2019.4.4  保存GPRS(DTU)设定参数 到 FM25L16
		SPI_FMRAM_BufferWrite(GprsPar, 256, DTU_PAR_SIZE);

		w_GprsModPar = 5000;
		// 再加修改端子配置 ZCL
		S_SavePar = 1;
		//

		// ZCL 2019.4.4 以后再添加 过程详细记录 和 GPRS故障，重启等到FM25L16
		// SPI_FLASH_Record();
	}

	//
	if (T_SavePar != SClk10Ms && S_SavePar != 0) // 用于MD304L提示状态，人性化设计 ZCL
	{
		T_SavePar = SClk10Ms; //
		C_SavePar++;
		if (C_SavePar > 150 && S_SavePar == 1)
		{
			S_SavePar = 2;
			w_GprsModPar = 6000;
		}
		else if (C_SavePar > 300 && S_SavePar == 2)
		{
			T_SavePar = 100;
			C_SavePar = 0;
			S_SavePar = 0;
			w_GprsModPar = 0;
		}
	}
}

// ZCL 2018.12.11 备注：以前液晶不需要。这里需要因为有GPRS,LOAR,GPS,按键 等参数了。
void ForceSavePar(void) // 强制保存参数
{						// w_GprsModPar=2000,规定时间内没有修改和保存参数,强制保存参数
	if (w_GprsModPar == 2000 && T_ForceSavPar != SClkSecond)
	{
		T_ForceSavPar = SClkSecond; // 上电延时时间到后，可以检测变频器报警
		C_ForceSavPar++;
		if (C_ForceSavPar > 900) // 15分钟
		{
			T_ForceSavPar = 100;
			C_ForceSavPar = 0;
			//
			B_ForceSavPar = 1;
		}
	}
}

void DoWith(void) // 一些数据,记录的处理
{
	uint i, j, HeartSendFault, LinkFault;
	// uint nw_Qian,nw_Bai,nw_Shi,nw_Ge;				//ZCL 2019.4.4
	//	u32 m;			//ZCL 2019.10.18 测试
	// ZCL 2019.10.18 温度的变化，变化值进行显示
	s16 nw_Wendu;
	u8 ConnectNum;

	float f_m, f_n, f_mul;

	switch (Pw_LoRaSetSignalBW)
	{
	case 0:
		f_m = 7.8;
		break;
	case 1:
		f_m = 10.4;
		break;
	case 2:
		f_m = 15.6;
		break;
	case 3:
		f_m = 20.8;
		break;
	case 4:
		f_m = 31.2;
		break;
	case 5:
		f_m = 41.6;
		break;
	case 6:
		f_m = 62.5;
		break;
	case 7:
		f_m = 125;
		break;
	case 8:
		f_m = 250;
		break;
	case 9:
		f_m = 500;
		break;

	default:
		f_m = 125;
	}

	switch (Pw_LoRaSetErrorCoding)
	{
	case 0:
		f_n = (float)4 / 4;
		break; // ZCL 2019.9.24  必须加(float)
	case 1:
		f_n = (float)4 / 5;
		break;
	case 2:
		f_n = (float)4 / 6;
		break;
	case 3:
		f_n = (float)4 / 7;
		break;
	case 4:
		f_n = (float)4 / 8;
		break;

	default:
		f_n = (float)4 / 6;
	}

	// pow(2,20);  //2的20次幂
	f_mul = (float)Pw_LoRaSetSpreadingFactor;
	f_mul = f_mul * f_m * f_n;
	i = (1 << Pw_LoRaSetSpreadingFactor);
	f_mul = f_mul / i * 100; // *1000/10  变成 *100；  /10 为了显示时少一个小数点，2.280kbps 显示成 2.28kbps
	w_LoRaDateRate = (uint)f_mul;

	// ZCL 2018.12.20 限制LoRa Gprs不能同时设定为透传到COM2（再加GPS  Pw_GprsSet.4位）
	if (F_LoRaToCom) // LoRa优先输出到COM2		2019.1.19
	{
		F_GprsTransToCom = 0; // 1111 1111 1110 1101
							  // F_GpsToCom=0;						// 1111 1111 1110 1111
							  // F_GprsMasterNotToCom=0;	//ZCL 2019.3.11
	}
	// else if(F_GprsTransToCom)	//GPRS优先GPS输出到COM2
	// {
	// F_GpsToCom=0;						// 1111 1111 1110 1111
	// F_GprsMasterNotToCom=0;	//ZCL 2019.3.11
	// }

	if (F_GprsMasterNotToCom)  // ZCL 2019.3.11  F_GprsMasterNotToCom不输出到串口；
		F_GprsMasterToCom = 0; // ZCL 2019.3.11  F_GprsMasterToCom输出到串口；

	// ZCL 2019.3.7
	if (Pw_LoRaMasterSlaveSel == 0) // LoRa从机。 LoRa从机的时候，把值放到指定区域，便于LoRa主机查询
	{
		// 10个字
		w_dsp1SoftVersion = w_dspSoftVersion;
		w_dsp1NowHz = w_dspNowHz;
		w_dsp1APhaseCurrent = w_dspAPhaseCurrent;
		w_dsp1OutVoltage = w_dspOutVoltage;
		w_dsp1NowTemperature = w_dspNowTemperature;
		// w_dsp1BusBarVoltage = w_dspBusBarVoltage;
		// w_dsp1TargetHz 			= w_dspTargetHz;
		w_dsp1Counter3 = w_dspCounter3;
		w_dsp1SysErrorCode1 = w_dspSysErrorCode1;
		w_dsp1SysRunStatus1 = w_dspSysRunStatus1;
		w_dsp1AI1ADCValue = w_dspAI1ADCValue;
		w_dsp1AI2ADCValue = w_dspAI2ADCValue;

		// ZCL 2019.9.11  再加压力和状态 6个字
		w_1InPDec = w_InPDec;
		w_1OutPDec = w_OutPDec;
		w_1PIDCalcP = w_PIDCalcP;
		w_1EquipOperateStatus = w_EquipOperateStatus;
		w_1EquipAlarmStatus = w_EquipAlarmStatus;
		w_1ScrEquipPower = Pw_ScrEquipPower; // 文本内的参数，4区
	}

	// ZCL 2017.9.19 背光控制  ZCL 2018.12.10
	if (C_LCDBackLight > 0)
	{
		Qb_LcdBacklight = 1; // 背光亮
		if (T_LCDBackLight != SClk10Ms)
		{
			T_LCDBackLight = SClk10Ms;
			C_LCDBackLight++;
			if (C_LCDBackLight > 12000) // 最大65535！
			{
				C_LCDBackLight = 0;
				Qb_LcdBacklight = 0; // 背光灭
			}
		}
	}

	// 2017.5.25  -- 2017.5.31 DoWith中  根据如下条件重启：
	// ZCL 2017.5.26  HeartSendFault
	HeartSendFault = 0;
	for (i = 0; i < 4; i++)
	{
		j = GprsPar[MaxLoginDSCNum0Base + i];
		if (B_HeartSendFault[i] >= j)
			HeartSendFault++;
	}

	// ZCL 2017.5.26  LinkFault
	LinkFault = 0;
	for (i = 0; i < 4; i++)
	{
		if (B_LinkFault[i] >= 10)
			LinkFault++;
	}

	// 80S内没有恢复接收。重启		ZCL123 2019.3.11 F_GprsMasterToCom
	if (F_GprsTransToCom || F_GprsMasterToCom) // Pw_GprsSet.1=1  GPRS输出到COM2
	{
		// ZCL 2017.5.26 COM2禁止接收后，80S内没有恢复接收。重启
		// USART2->CR1 .3位  =0 禁止接收
		if ((USART2->CR1 & 0x0004) == 0 //???YLS 2023.01.27有问题？ 原来&&
			|| B_Com3Cmd8B + B_Com3Cmd8D + B_GprsDataReturn + B_ATComStatus + B_Com3Cmd89 + B_OtherCmd + HeartSendFault + LinkFault + S_SendSms + S_ReadNewSms + S_SmsFull > 0)
		{
			if (T_Com2RcvReset != SClkSecond)
			{
				T_Com2RcvReset = SClkSecond;
				C_Com2RcvReset++;
				if (C_Com2RcvReset > 80) // 80S内没有恢复接收。重启
				{
					T_Com2RcvReset = 100;
					C_Com2RcvReset = 0;

					USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
					USART2->CR1 |= 0x0004; //.3位  运行接收

					B_Com3Cmd8B = 0;
					B_Com3Cmd8D = 0;
					B_GprsDataReturn = 0;

					for (i = 0; i < 4; i++)
					{
						B_HeartSendFault[i] = 0;
					}
					for (i = 0; i < 4; i++)
					{
						B_LinkFault[i] = 0;
					}

					//
					B_SmsOrFaultHardRestart = 0;
					S_M35 = 0x01;	  // 模块关机，重启
					POWEROFF_4G;	  // 4G模块断电
					LCD_DLY_ms(3000); // 此时间内没有喂狗，设备复位！
					POWERON_4G;		  // 4G模块上电
				}
			}
		}
		else
		{
			C_Com2RcvReset = 0;
		}
	}

	//	// ZCL 2017.5.30  com2接收不到数据，660S定时重启
	//	if (S_M35 == 0x1B && T_Com2NoRcv != SClkSecond)
	//	{
	//		T_Com2NoRcv = SClkSecond;
	//		C_Com2NoRcv++;
	//		if (C_Com2NoRcv > 1800)
	//		{
	//			C_Com2NoRcv = 0;
	//			//
	//			S_M35 = 0x01;	  // 模块关机，重启
	//			LCD_DLY_ms(3000); // 此时间内没有喂狗，设备复位！
	//		}
	//	}

	// ZCL 2017.5.30  com3接收不到网络数据，660S定时重启
	// if (S_M35 == 0x1B && T_Com3NoRcv != SClkSecond)
	if (F_GprsEn && T_Com3NoRcv != SClkSecond) // 放宽重启的条件，不管任何状态，只要20分钟收不到数据就重启
	{
		T_Com3NoRcv = SClkSecond;
		C_Com3NoRcv++;
		if (C_Com3NoRcv > 1200)
		{
			C_Com3NoRcv = 0;
			//
			B_SmsOrFaultHardRestart = 0; // 短信或者故障硬件重启
			S_M35 = 0x01;				 // 模块关机，重启
			POWEROFF_4G;				 // YLS 2023.12.25 关闭4G模块电源
			LCD_DLY_ms(3000);			 // 此时间内没有喂狗，设备复位！
			POWERON_4G;					 // YLS 2023.12.25 打开4G模块电源
		}
	}

	// 强制定时重启
	if (Pw_RebootInterval != 60000 && T_Reboot != SClkMinute) // 如果间隔时间==60000，就取消定时重启功能
	{
		T_Reboot = SClkMinute;
		C_Reboot++;
		if (C_Reboot >= Pw_RebootInterval)
		{
			C_Reboot = 0;
			//
			B_SmsOrFaultHardRestart = 0; // 短信或者故障硬件重启
			S_M35 = 0x01;				 // 模块关机，重启
			POWEROFF_4G;				 // YLS 2023.12.25 关闭4G模块电源
			LCD_DLY_ms(3000);			 // 此时间内没有喂狗，设备复位！
			POWERON_4G;					 // YLS 2023.12.25 打开4G模块电源
		}
	}

	// 串口2波特率设置程序　2013.1.17
	j = GprsPar[BaudRateBase + 2];
	Pw_GprsBaudRate2 = (j << 8) + GprsPar[BaudRateBase + 3];

	Pw_GprsDataBitLen2 = GprsPar[BaudRateBase + 4];	  // 2013.11.15
	Pw_GprsStopBitLen2 = GprsPar[BaudRateBase + 5];	  // 2013.11.15
	Pw_GprsParityBitLen2 = GprsPar[BaudRateBase + 6]; // 2013.11.15
	Pw_GprsFlowControl2 = GprsPar[BaudRateBase + 7];  // 2013.11.15

	// 2013.11.15
	if (Pw_GprsBaudRate2 != w_GprsSaveBaudRate2 || Pw_GprsDataBitLen2 != w_GprsSaveDataBitLen2 || Pw_GprsStopBitLen2 != w_GprsSaveStopBitLen2 || Pw_GprsParityBitLen2 != w_GprsSaveParityBitLen2 || Pw_GprsFlowControl2 != w_GprsSaveFlowControl2)
	{
		w_GprsSaveBaudRate2 = Pw_GprsBaudRate2;
		w_GprsSaveDataBitLen2 = Pw_GprsDataBitLen2;
		w_GprsSaveStopBitLen2 = Pw_GprsStopBitLen2;
		w_GprsSaveParityBitLen2 = Pw_GprsParityBitLen2;
		w_GprsSaveFlowControl2 = Pw_GprsFlowControl2;

		Com2_config();
	}

	// ZCL 2017.5.25 信号质量不好的时候，闪烁
	if (C_NetLedLightCSQ > 300 && (w_GprsCSQ < 8 || w_GprsCSQ == 99))
	{
		C_NetLedLightCSQ = 0;
		if (NET_STA > 0)
			NET = 0; //=1,灭；=0,亮
		else
			NET = 1; //=1,灭；=0,亮
	}
	//	else if(w_GprsCSQ>=8 && w_GprsCSQ!=99)
	//		NET=0;					//=1,灭；=0,亮

	// GPRS模块的：PWR.DATA.NET 指示灯的动作 2013.9.1
	// S_M35来控制PWR.DATA.NET灯（因为不同的阶段，指示灯的闪烁有不同的含义）
	if (S_M35 < 0x11)
	{
		PWR_LIGHT = 1; //=1,灭；=0,亮
		// NET=1;					//=1,灭；=0,亮   ZCL 2017.5.25  用NET指示灯来闪烁指示信号不好的时候

		// 1. DATA先闪烁，代表在找网络
		if (C_LedLight > 300)
		{
			C_LedLight = 0;
			if (COM2_DATASTA == 1)
				COM2_DATA = 0; //=1,灭；=0,亮
			else
				COM2_DATA = 1; //=1,灭；=0,亮
		}
	}
	else if (S_M35 >= 0x011 && S_M35 < 0x01B)
	{
		// 2. NET常亮
		NET = 0; //=1,灭；=0,亮
		// 2.1 找到网络，DATA停止闪烁
		COM2_DATA = 1; //=1,灭；=0,亮
	}

	if (S_M35 >= 0x01B)
	{
		// 3.1 PWR灯
		if (CGD0_LinkConnectOK[0] + CGD0_LinkConnectOK[1] + CGD0_LinkConnectOK[2] + CGD0_LinkConnectOK[3] == 0)
		{
			// 3.1.1 PWR  闪烁较快  表示未能与GPRS网络联接
			if (C_LedLight > 50)
			{
				C_LedLight = 0;
				if (PWR_STA > 0)
					PWR_LIGHT = 0; //=1,灭；=0,亮
				else
					PWR_LIGHT = 1; //=1,灭；=0,亮
			}
		}
		else
		{
			ConnectNum = 0;
			for (i = 0; i < 4; i++)
			{
				if (GprsPar[Ip0Base + i * 4] + GprsPar[DomainName0Base + i * 31] != 0)
					ConnectNum++;
			}

			if (ConnectNum == (ZhuCeOkFLAG[0] + ZhuCeOkFLAG[1] + ZhuCeOkFLAG[2] + ZhuCeOkFLAG[3]))
			{
				// 3.1.2 PWR  闪烁较慢  表示成功与GPRS网络连接
				if (C_LedLight > 1000)
				{
					C_LedLight = 0;
					if (PWR_STA > 0)
						PWR_LIGHT = 0; //=1,灭；=0,亮
					else
						PWR_LIGHT = 1; //=1,灭；=0,亮
				}
			}
			else
			{
				// 3.1.2 PWR  闪烁较快  表示只有部分通道成功与GPRS网络连接
				if (C_LedLight > 300)
				{
					C_LedLight = 0;
					if (PWR_STA > 0)
						PWR_LIGHT = 0; //=1,灭；=0,亮
					else
						PWR_LIGHT = 1; //=1,灭；=0,亮
				}
			}
		}

		// 3.2 COM2_DATA灯
		if (Cw_Rcv2 + Cw_Txd2 > 0)
		{
			// COM2_DATA 翻转  2014.11.27修改
			// ZCL 2020.3.26			GPIO_WriteBit(GPIOC, GPIO_Pin_12,
			//		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_12))));		//ZCL 2018.12.4
			COM2_DATA_TOGGLE;
		}
		else //=1,灭
			COM2_DATA = 1;
	}

	// TXD和RXD都是低电平有效，平时高电平 2013.8.30
	// 2014.11.27修改 COM3_DATA 翻转
	// 2014.12.11 COM3_DATA不受S_M35控制，根据数据翻转
	if (Cw_Rcv3 + Cw_Txd3 > 0)
	{
		// COM3_DATA 翻转 2014.11.27修改
		// ZCL 2020.3.26		GPIO_WriteBit(GPIOD, GPIO_Pin_5,
		//  								 (BitAction)((1-GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_5))));		//ZCL 2018.12.4	 凑得，没有这个脚
		COM3_DATA_TOGGLE;
	}
	else //=1,灭
		COM3_DATA = 1;

	// 2016.5.27 加入DATA灯如果一直亮10秒，看门狗重启。2016.5.27
	if (T_DataLampAllLight != SClk10Ms)
	{
		if (COM2_DATASTA == 0)
		{
			T_DataLampAllLight = SClk10Ms;
			C_DataLampAllLight++;
			if (C_DataLampAllLight > 2000)
			{
				C_DataLampAllLight = 0;

				S_M35 = 0x01;	  // 模块关机，重启
				LCD_DLY_ms(3000); // 此时间内没有喂狗，设备复位！ 2013.7.3
			}
		}
		else if (C_DataLampAllLight > 0)
			C_DataLampAllLight--;
	}

	// ZCL 2019.10.18 温度的变化，变化值进行显示
	// ZCL 2022.10.5
	// 电机1 温度值变换
	nw_Wendu = (s16)(w_Lora1WenDu1);
	if (nw_Wendu < 0)
	{
		w_Pump1DispWenDu1DecValue = -nw_Wendu;
		w_Pump1DispWenDu1DecValue = w_Pump1DispWenDu1DecValue + 10000; // 10000是负号
	}
	else
		w_Pump1DispWenDu1DecValue = w_Lora1WenDu1;

	nw_Wendu = (s16)(w_Lora1WenDu2);
	if (nw_Wendu < 0)
	{
		w_Pump1DispWenDu2DecValue = -nw_Wendu;
		w_Pump1DispWenDu2DecValue = w_Pump1DispWenDu2DecValue + 10000; // 10000是负号
	}
	else
		w_Pump1DispWenDu2DecValue = w_Lora1WenDu2;

	// 电机1 温度值变换
	nw_Wendu = (s16)(w_Lora2WenDu1);
	if (nw_Wendu < 0)
	{
		w_Pump2DispWenDu1DecValue = -nw_Wendu;
		w_Pump2DispWenDu1DecValue = w_Pump2DispWenDu1DecValue + 10000; // 10000是负号
	}
	else
		w_Pump2DispWenDu1DecValue = w_Lora2WenDu1;

	nw_Wendu = (s16)(w_Lora2WenDu2);
	if (nw_Wendu < 0)
	{
		w_Pump2DispWenDu2DecValue = -nw_Wendu;
		w_Pump2DispWenDu2DecValue = w_Pump2DispWenDu2DecValue + 10000; // 10000是负号
	}
	else
		w_Pump2DispWenDu2DecValue = w_Lora2WenDu2;

	// 电机1 温度值变换
	nw_Wendu = (s16)(w_Lora3WenDu1);
	if (nw_Wendu < 0)
	{
		w_Pump3DispWenDu1DecValue = -nw_Wendu;
		w_Pump3DispWenDu1DecValue = w_Pump3DispWenDu1DecValue + 10000; // 10000是负号
	}
	else
		w_Pump3DispWenDu1DecValue = w_Lora3WenDu1;

	nw_Wendu = (s16)(w_Lora3WenDu2);
	if (nw_Wendu < 0)
	{
		w_Pump3DispWenDu2DecValue = -nw_Wendu;
		w_Pump3DispWenDu2DecValue = w_Pump3DispWenDu2DecValue + 10000; // 10000是负号
	}
	else
		w_Pump3DispWenDu2DecValue = w_Lora3WenDu2;

	// 电机1 温度值变换
	nw_Wendu = (s16)(w_Lora4WenDu1);
	if (nw_Wendu < 0)
	{
		w_Pump4DispWenDu1DecValue = -nw_Wendu;
		w_Pump4DispWenDu1DecValue = w_Pump4DispWenDu1DecValue + 10000; // 10000是负号
	}
	else
		w_Pump4DispWenDu1DecValue = w_Lora4WenDu1;

	nw_Wendu = (s16)(w_Lora4WenDu2);
	if (nw_Wendu < 0)
	{
		w_Pump4DispWenDu2DecValue = -nw_Wendu;
		w_Pump4DispWenDu2DecValue = w_Pump4DispWenDu2DecValue + 10000; // 10000是负号
	}
	else
		w_Pump4DispWenDu2DecValue = w_Lora4WenDu2;
}

void SPI_FLASH_Record(void) // SPI-FLASH 记录函数
{
	//
}

void ParLimit(void) // 参数限制
{
	// 波特率限制
	if (Pw_GprsBaudRate2 != 2400 && Pw_GprsBaudRate2 != 4800 && Pw_GprsBaudRate2 != 9600 && Pw_GprsBaudRate2 != 19200 && Pw_GprsBaudRate2 != 38400 && Pw_GprsBaudRate2 != 57600)
	{
		Pw_GprsBaudRate2 = 9600; // 串口1波特率
	}

	// ZCL 2018.12.10
	if (Pw_ScrKeyMode >= 3) // 对液晶屏按键模式值进行限制
		Pw_ScrKeyMode = 0;

	// ZCL 2019.3.23
	if (Pw_ScrMenuMode != 0 && Pw_ScrMenuMode != 1 && Pw_ScrMenuMode != 2)
		Pw_ScrMenuMode = 0;

	if (Pw_ScrCurrentBits >= 3 || Pw_ScrCurrentBits < 1) // 对值进行限制
		Pw_ScrCurrentBits = 2;

	// ZCL 2019.9.11  Pw_ScrEquipPower 变频功率  是个变值，不太好限制

	if (Pw_LoRaModule1Add >= 1000)
		Pw_LoRaModule1Add = 0;
	if (Pw_LoRaModule2Add >= 1000)
		Pw_LoRaModule2Add = 0;
	if (Pw_LoRaModule3Add >= 1000)
		Pw_LoRaModule3Add = 0;
	if (Pw_LoRaModule4Add >= 1000)
		Pw_LoRaModule4Add = 0;

	// ZCL 2019.3.22		LORA数值的限制
	if (Pw_LoRaSetFreq < 137 || Pw_LoRaSetFreq > 1020)
		Pw_LoRaSetFreq = 434;
	if (Pw_LoRaSetPower == 20)
		;
	else if (Pw_LoRaSetPower < 7 || Pw_LoRaSetPower > 14)
		Pw_LoRaSetPower = 20;

	if (Pw_LoRaSetSignalBW > 9) // 信号带宽BW
		Pw_LoRaSetSignalBW = 7; // ZCL 2019.9.24  9

	if (Pw_LoRaSetSpreadingFactor < 6 || Pw_LoRaSetSpreadingFactor > 12) // 扩频因子 SF
		Pw_LoRaSetSpreadingFactor = 7;

	if (Pw_LoRaSetErrorCoding < 1 || Pw_LoRaSetErrorCoding > 4) // 编码率
		Pw_LoRaSetErrorCoding = 2;

	if (Pw_LoRaSetCrcOn > 1)
		Pw_LoRaSetCrcOn = 1;

	if (Pw_LoRaSetImplicitHeaderOn > 1)
		Pw_LoRaSetImplicitHeaderOn = 0;

	if (Pw_LoRaSetRxSingleOn > 1)
		Pw_LoRaSetRxSingleOn = 0; // ZCL 2020.3.26 限制由1改成0

	if (Pw_LoRaSetFreqHopOn > 1)
		Pw_LoRaSetFreqHopOn = 0;

	// ZCL 2019.9.24 发送超时
	if (Pw_LoRaSetTxPacketTimeOut < 10 || Pw_LoRaSetTxPacketTimeOut > 6000)
		Pw_LoRaSetTxPacketTimeOut = 1000;
	// ZCL 2019.9.24 接收超时
	if (Pw_LoRaSetRxPacketTimeOut < 10 || Pw_LoRaSetRxPacketTimeOut > 6000)
		Pw_LoRaSetRxPacketTimeOut = 1000;

	// ZCL 2020.4.8 进行限制
	if (Pw_LoRaMasterSlaveSel > 1)
		Pw_LoRaMasterSlaveSel = 0;

	// ZCL 2019.4.2
	if (Pw_LoRaMasterSlaveSel == 1)
	{
		// Pw_LoRaSet=1;			//ZCL 2019.11.21 注释掉
		// Pw_GprsSet=9;			//ZCL 2019.11.21 注释掉
		// Pw_LoRaEquipmentNo = 2;
		// if (Pw_LoRaModule1Add < 3)
		// 	Pw_LoRaModule1Add = 3;
	}
	else if (Pw_LoRaMasterSlaveSel == 0)
	{
		// Pw_LoRaSet=1;			//ZCL 2019.11.21 注释掉
		// Pw_GprsSet=0;			//ZCL 2019.11.21 注释掉
		if (Pw_LoRaEquipmentNo < 3)
			Pw_LoRaEquipmentNo = 3;
	}
	// 强制重启时间间隔
	if (Pw_RebootInterval < 5)
	{
		Pw_RebootInterval = 480;
	}
}

void Time_Output(void) // 软件时钟输出	 2008.10.21
{
	// 软件时钟，非真实时钟
	if (SClkSecond >= 60) // 秒
	{
		SClkSecond = 0;
		SClkMinute++;
		Pw_GprsSetMinute = SClkMinute;
		if (SClkMinute >= 60) // 分
		{
			SClkMinute = 0;
			SClkHour++;
			Pw_GprsSetHour = SClkHour;
			if (SClkHour >= 24) // 时
			{
				SClkHour = 0;
				SClkDay++;
				Pw_GprsSetDay = SClkDay;
				if (SClkDay > 30) // 日
				{
					SClkDay = 1;
					SClkMonth++;
					// Pw_GprsSetMonth=Pw_GprsSetMonth;
					if (SClkMonth > 12) // 月
					{
						SClkMonth = 1;
						SClkYear++;
					}
				}
			}
		}
	}

	Pw_GprsSetSecond = SClkSecond;
	Pw_GprsSetMinute = SClkMinute;
	Pw_GprsSetHour = SClkHour;
}

/* static u32 CpuID[3];
static u32 Lock_Code; */

/* void GetLockCode(void)		// 2013.10.26 得到锁定密码
{
	//获取CPU唯一ID
	CpuID[0]=*(vu32*)(0x1ffff7e8);
	CpuID[1]=*(vu32*)(0x1ffff7ec);
	CpuID[2]=*(vu32*)(0x1ffff7f0);
	//加密算法,很简单的加密算法
	Lock_Code=(CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}
 */

/* void GetLockCode(void)		// 2013.10.26 得到锁定密码
{
	//获取CPU唯一ID
	//使用指针指向UID的起始地址。
	//以后只要当数组用就可以了.比如UID[0]就是芯片最低字节的全球唯一ID.
	u32* UID=(u32*)0x1ffff7e8;		//

	//加密算法,很简单的加密算法
	Lock_Code=(UID[0]>>1)+(UID[1]>>2)+(UID[2]>>3);
}
 */

u8 D[12];						 // D[12]为公式的输入数组(注意与UID[12]区分).
u8 *UID = (u8 *)0x1ffff7e8;		 // 全球唯一ID号
u8 *LOCKCODE = (u8 *)0x0801E000; // 锁定密码
u32 Fml_Constant;				 // 用户指定的32位常数，输入到公式的常数
u8 Result[4];					 // 公式计算结果输出,四个字节；加密公式的输出为一个数组
u8 *C = (u8 *)&Fml_Constant;	 // 把常量转换为数组
u16 Fml_CRC16;					 // CRC16校验值
u8 B_LockCodeOK;				 // 锁定密码正确标志

void GetLockCode(void) // 2013.10.26 得到锁定密码
{
	// 在加密公式中,输入参数有两个： 1：是常数；2：是变化了顺序的UID值
	Fml_Constant = 0x1100FF;

	// 利用UID[0..11]，可以直接取用读出的唯一ID号
	D[0] = UID[2];
	D[1] = UID[3];
	D[2] = UID[6];
	D[3] = UID[7];
	D[4] = UID[4];
	D[5] = UID[5];
	D[6] = UID[10];
	D[7] = UID[11];
	D[8] = UID[0];
	D[9] = UID[1];
	D[10] = UID[8];
	D[11] = UID[9];

	Formula_12(D, Result); // 根据自定义算法，算出密码
	if (Result[0] == LOCKCODE[0] && Result[1] == LOCKCODE[1] && Result[2] == LOCKCODE[2] && Result[3] == LOCKCODE[3])
		B_LockCodeOK = 1;
	else
		B_LockCodeOK = 0;
}

/********************************************************************
函数功能：公式
入口参数：D
备  注：
********************************************************************/
void Formula_12(u8 *D, u8 *Result)
{
	Result[0] = C[0] + D[0] - D[4] + D[8];
	Result[1] = C[1] & D[1] ^ D[5] | D[9];
	Result[2] = C[2] | D[2] ^ D[6] ^ D[10];
	Result[3] = C[3] - D[3] + D[7] & D[11];
}

/********************************************************************
函数功能：公式
入口参数：D
备  注：
********************************************************************/
void Formula_113(u8 *D, u8 *Result)
{
	u8 *crc;
	u16 dat;
	crc = (u8 *)&dat;
	Fml_CRC16 = 0;
	dat = CRC16(D, 12);
	Result[0] = crc[0];
	Result[1] = crc[1];
	Result[2] = C[2] ^ D[2] ^ D[1];
	Result[3] = C[3] ^ D[3] ^ D[0];
}

void Test(void) // 输出
{
}
u32 FtoU32(u16 w1, u16 w2) // 四字节的浮点数据转换为乘了10的长整形数据
{
	u32 tmpL;
	aa.b[3] = (w1 >> 8);
	aa.b[2] = w1;
	aa.b[1] = (w2 >> 8);
	aa.b[0] = w2;
	// tmpL=(u32)(aa.f*100.0);		//2位小数点
	tmpL = (u32)(aa.f * 10.0); // 1位小数点
	return tmpL;
}
