/**
 ******************************************************************************
 * @file    GlobalV.h
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.27
 * @date    2014-01-03
 * @brief   全局变量声明
 ******************************************************************************
 */

#ifndef __GLOBALV_H
#define __GLOBALV_H

#include "GlobalConst.h"

// (定义变量)
u8 SoftClock[9]; // 辅助时钟

u8 DInb[6];         // 开关量输入参数列表区
u8 DOutb[6];        // 开关量输出参数列表区
u16 w_AI[8];        // 模拟量输入参数列表区
u8 Tmp1;            // 暂存值
u8 Tmp2;            // 暂存值
u8 Tmp3;            // 暂存值
u8 Tmp4;            // 暂存值
u8 Tmp5;            // 暂存值
u16 Temp1;          // 暂存值
u16 Temp2;          // 暂存值
u16 Temp3;          // 暂存值
u16 Temp4;          // 暂存值
u8 Value[5];        // 数组
u16 sysTickMsValue; // 系统时钟MS值

u16 Pw_ParLst[500]; // 参数字列表区 设定		//0区访问
u16 w_ParLst[1000]; // 参数字列表区 只读		//1区访问

u16 Pw_dspParLst[100]; // DSP参数字列表区 设定 2015.7.1			//2区访问
u16 w_dspParLst[150];  // DSP参数字列表区 只读 2015.7.1	ZCL 2018.9.21 数量100太少			//3区访问

// u16  	w_ScrParLst[50];			// ZCL 2018.8.3 SCR本身参数  			ZCL 2018.9.21 数量10太少
//			w_GprsParLst[]

u8 B_ModPar;
u16 Lw_Reg;
u16 Lw_SaveSetValue;
u16 Lw_DCM200SCR_VERSION;
u16 Lw_SaveEquipOperateStatus;
u16 Lw_SaveEquipAlarmStatus;

u16 Lw_SaveDspStopStatus;  // ZCL 2018.4.25
u16 Lw_SaveDspFaultStatus; // ZCL 2018.4.25

u16 Lw_SaveKglInStatus;  // ZCL 2019.8.31 2019.9.11
u16 Lw_SaveKglOutStatus; // ZCL 2019.8.31 2019.9.11

//-----------GPRS  ZCL 2018.12.10 ；ZCL 2019.9.11 150
u16 w_GprsParLst[280]; // 参数字列表区 读写 ZCL 2018.12.10		//4区访问，这个本身参数，实际不用4访问

u8 B_RunGprsModule; // 运行GPRS模块标志(没有与文本通讯，4秒后此值=1)
u8 GprsPar[288];    // Gprs参数
u8 Com1_LedStatus;  // COM1指示灯状态
u8 Com0_LedStatus;  // COM0指示灯状态

//----------LoRa 主机（或者 单个从机） 参数保存区
// u16  	w_dsp1ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7			//5区访问，这个本身参数，实际不用5访问
// u16  	w_dsp2ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7			//5区访问，这个本身参数，实际不用5访问
// u16  	w_dsp3ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7			//5区访问，这个本身参数，实际不用5访问
// u16  	w_dsp4ParLst[50];				// 参数字列表区 读写 ZCL 2019.3.7			//5区访问，这个本身参数，实际不用5访问

u16 w_DNBParLst[1000]; // 参数字列表区  ZCL 2022.5.19

u8 F_ModeParLora; // ZCL 2023.2.10  修改LORA参数标志

// 下面是胡振广定义的参数
u8 APN_Name[31]; // 接入点

u16 Pw_ParLst_GPRS[256]; // 设定参数字列表区

u8 ServerCodeSrc[8];      // 服务器代码字符串
u8 AccessPointNameSrc[5]; // 接入点名称字符串

u8 S_XmodemConfigProcess; // S_Xmodem配置步骤
u8 F_STOPTransmit;        // Xmodem停止传输标志

u16 ReceiveGPRSBufOneDateLen;   // 接收第1路GPRSBuf的数据长度
u16 ReceiveGPRSBufTwoDateLen;   // 接收第2路GPRSBuf的数据长度
u16 ReceiveGPRSBufThreeDateLen; // 接收第3路GPRSBuf的数据长度
u16 ReceiveGPRSBufFourDateLen;  // 接收第4路GPRSBuf的数据长度
u8 ReceiveGPRSBuf[RCV3_MAX];    // GPRS接收到的数据放到GPRSBUF当中
u8 F_GPRSSendFinish;            // GPRS发送完成标志
u8 F_UDP_OR_TCP;                //=0为UDP，=1为TCP
u8 F_PowerOnRegister;           // 宏电协议上电注册标志
u8 F_AcklinkNum = 0;
;                      // 接受数据后，记录是哪一路发过来的数据，返回数据的时候返回给哪一路。
u8 F_NowNetworkSystem; // 当前网络制式

u8 CSQBuf[2];                  // 信号质量
u8 local_port_Byte[2];         // 本地端口临时存储区
u8 DTUIDBuf[11];               // 保存的DTUID号
u16 T_link1OnlineReportTimeMS; // 链接1心跳时间MS
u16 T_link2OnlineReportTimeMS; // 链接2心跳时间MS
u16 T_link3OnlineReportTimeMS; // 链接3心跳时间MS
u16 T_link4OnlineReportTimeMS; // 链接4心跳时间MS

uint8_t usart3_rev_cnt = 0;
uint8_t usart3_rev_flag = 0;
uint8_t usart3_rev_finish = 0; // 串口3接收计数，串口3接收标志串口接收完成标志

u8 B_Com1ErrWithModule1; // 标志 与子模块1通讯失败
u8 B_Com1ErrWithModule2; // 标志 与子模块2通讯失败

u8 F_RemoteQuery; // 通过GPRS远程查询参数标志
u8 F_Reset_GPS;   // 重启GPS标志
u16 T_Reset_GPS;
u16 C_Reset_GPS;
u16 T_PowerOnOff_GPS;
u16 C_PowerOnOfft_GPS;

u16 T_Judge_No1_VVVF_Comm; // 判断1#泵变频器通讯故障计时
u16 C_Judge_No1_VVVF_Comm; // 判断1#泵变频器通讯故障计数
u16 T_Judge_No2_VVVF_Comm; // 判断2#泵变频器通讯故障计时
u16 C_Judge_No2_VVVF_Comm; // 判断2#泵变频器通讯故障计数
u16 T_Judge_No3_VVVF_Comm; // 判断3#泵变频器通讯故障计时
u16 C_Judge_No3_VVVF_Comm; // 判断3#泵变频器通讯故障计数
u16 T_Judge_No4_VVVF_Comm; // 判断4#泵变频器通讯故障计时
u16 C_Judge_No4_VVVF_Comm; // 判断4#泵变频器通讯故障计数
u16 T_Judge_No5_VVVF_Comm; // 判断5#泵变频器通讯故障计时
u16 C_Judge_No5_VVVF_Comm; // 判断5#泵变频器通讯故障计数
u16 T_Judge_No6_VVVF_Comm; // 判断6#泵变频器通讯故障计时
u16 C_Judge_No6_VVVF_Comm; // 判断6#泵变频器通讯故障计数
u16 T_Judge_No7_VVVF_Comm; // 判断7#泵变频器通讯故障计时
u16 C_Judge_No7_VVVF_Comm; // 判断7#泵变频器通讯故障计数
u16 T_Judge_No8_VVVF_Comm; // 判断8#泵变频器通讯故障计时
u16 C_Judge_No8_VVVF_Comm; // 判断8#泵变频器通讯故障计数

u8 F_No1_VVVFComm_Fault; // 1#泵变频器通讯异常标志
u8 F_No2_VVVFComm_Fault; // 2#泵变频器通讯异常标志
u8 F_No3_VVVFComm_Fault; // 3#泵变频器通讯异常标志
u8 F_No4_VVVFComm_Fault; // 4#泵变频器通讯异常标志
u8 F_No5_VVVFComm_Fault; // 5#泵变频器通讯异常标志
u8 F_No6_VVVFComm_Fault; // 6#泵变频器通讯异常标志
u8 F_No7_VVVFComm_Fault; // 7#泵变频器通讯异常标志
u8 F_No8_VVVFComm_Fault; // 8#泵变频器通讯异常标志

u16 Count_Save1_No1_VVVF_Comm; // 1#泵变频器通讯计数器1
u16 Count_Save2_No1_VVVF_Comm; // 1#泵变频器通讯计数器2
u16 Count_Save1_No2_VVVF_Comm; // 2#泵变频器通讯计数器1
u16 Count_Save2_No2_VVVF_Comm; // 2#泵变频器通讯计数器2
u16 Count_Save1_No3_VVVF_Comm; // 3#泵变频器通讯计数器1
u16 Count_Save2_No3_VVVF_Comm; // 3#泵变频器通讯计数器2
u16 Count_Save1_No4_VVVF_Comm; // 4#泵变频器通讯计数器1
u16 Count_Save2_No4_VVVF_Comm; // 4#泵变频器通讯计数器2
u16 Count_Save1_No5_VVVF_Comm; // 5#泵变频器通讯计数器1
u16 Count_Save2_No5_VVVF_Comm; // 5#泵变频器通讯计数器2
u16 Count_Save1_No6_VVVF_Comm; // 6#泵变频器通讯计数器1
u16 Count_Save2_No6_VVVF_Comm; // 6#泵变频器通讯计数器2
u16 Count_Save1_No7_VVVF_Comm; // 7#泵变频器通讯计数器1
u16 Count_Save2_No7_VVVF_Comm; // 7#泵变频器通讯计数器2
u16 Count_Save1_No8_VVVF_Comm; // 8#泵变频器通讯计数器1
u16 Count_Save2_No8_VVVF_Comm; // 8#泵变频器通讯计数器2
#endif                         /* __GLOBALV_H */
