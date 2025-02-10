/**
 ******************************************************************************
 * @file    GlobalV_Extern.h
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.27
 * @date    2014-01-03
 * @brief   全局变量声明
 ******************************************************************************
 */

#ifndef __GLOBALV_EXTERN_H
#define __GLOBALV_EXTERN_H

#include "sx1276-LoRa.h"

// (定义变量)
extern u8 SoftClock[];     // 辅助时钟
extern u16 w_ParLst[];     // 参数字列表区
extern u8 DInb[];          // 开关量输入参数列表区
extern u8 DOutb[];         // 开关量输出参数列表区
extern u16 w_AI[];         // 模拟量输入参数列表区
extern u8 Tmp1;            // 暂存值
extern u8 Tmp2;            // 暂存值
extern u8 Tmp3;            // 暂存值
extern u8 Tmp4;            // 暂存值
extern u8 Tmp5;            // 暂存值
extern u16 Temp1;          // 暂存值
extern u16 Temp2;          // 暂存值
extern u16 Temp3;          // 暂存值
extern u16 Temp4;          // 暂存值
extern u8 Value[5];        // 数组
extern u16 sysTickMsValue; // 系统时钟MS值

extern u16 w_ParLst[];  // 参数字列表区
extern u16 Pw_ParLst[]; // 参数字列表区

extern u16 w_dspParLst[];  // DSP参数字列表区 只读 2015.7.1
extern u16 Pw_dspParLst[]; // DSP参数字列表区 设定 2015.7.1
extern u16 w_ScrParLst[];  // ZCL 2018.8.3 SCR本身参数

extern u8 B_ModPar;
extern u16 Lw_Reg;
extern u16 Lw_SaveSetValue;
extern u16 Lw_DCM200SCR_VERSION;
extern u16 Lw_SaveEquipOperateStatus; // ZCL 2015.9.7
extern u16 Lw_SaveEquipAlarmStatus;
extern u16 Lw_SaveDspStopStatus;
extern u16 Lw_SaveDspFaultStatus;
extern u16 Lw_SaveKglInStatus;  // ZCL 2019.8.31 2019.9.11
extern u16 Lw_SaveKglOutStatus; // ZCL 2019.8.31 2019.9.11

//-----------GPRS  ZCL 2018.12.10
extern u16 w_GprsParLst[]; // 参数字列表区 读写
extern u8 B_RunGprsModule; // 运行GPRS模块标志
extern u8 GprsPar[];       // Gprs参数
extern u8 Com1_LedStatus;  // COM1指示灯状态
extern u8 Com0_LedStatus;  // COM0指示灯状态

//----------LoRa 主机（或者 单个从机） 参数保存区
extern u16 w_dsp1ParLst[]; // 参数字列表区 读写 ZCL 2019.3.7
extern u16 w_dsp2ParLst[]; // 参数字列表区 读写 ZCL 2019.3.7
extern u16 w_dsp3ParLst[]; // 参数字列表区 读写 ZCL 2019.3.7
extern u16 w_dsp4ParLst[]; // 参数字列表区 读写 ZCL 2019.3.7

extern tLoRaSettings LoRaSettings; // ZCL 2019.3.22

extern u16 w_DNBParLst[]; // 参数字列表区  ZCL 2022.5.19

extern u8 F_ModeParLora; // ZCL 2023.2.10  修改LORA参数标志

// 下面是胡振广定义的参数
extern u8 APN_Name[]; // 接入点

extern u16 Pw_ParLst_GPRS[256]; // 设定参数字列表区

extern u8 ServerCodeSrc[8];      // 服务器代码字符串
extern u8 AccessPointNameSrc[5]; // 接入点名称字符串

extern u8 S_XmodemConfigProcess; // S_Xmodem配置步骤
extern u8 F_STOPTransmit;        // Xmodem停止传输标志

extern u16 ReceiveGPRSBufOneDateLen;   // 接收第1路GPRSBuf的数据长度
extern u16 ReceiveGPRSBufTwoDateLen;   // 接收第2路GPRSBuf的数据长度
extern u16 ReceiveGPRSBufThreeDateLen; // 接收第3路GPRSBuf的数据长度
extern u16 ReceiveGPRSBufFourDateLen;  // 接收第4路GPRSBuf的数据长度
extern u8 ReceiveGPRSBuf[];            // GPRS接收到的数据放到GPRSBUF当中
extern u8 F_GPRSSendFinish;            // GPRS发送完成标志
extern u8 F_UDP_OR_TCP;                //=0为UDP，=1为TCP
extern u8 F_PowerOnRegister;           // 宏电协议上电注册标志
extern u8 F_AcklinkNum;
;                             // 接受数据后，记录是哪一路发过来的数据，返回数据的时候返回给哪一路。
extern u8 F_NowNetworkSystem; // 当前网络制式

extern u8 CSQBuf[2];                  // 信号质量
extern u8 local_port_Byte[2];         // 本地端口临时存储去
extern u8 DTUIDBuf[11];               // 保存的DTUID号
extern u16 T_link1OnlineReportTimeMS; // 链接1心跳时间MS
extern u16 T_link2OnlineReportTimeMS; // 链接2心跳时间MS
extern u16 T_link3OnlineReportTimeMS; // 链接3心跳时间MS
extern u16 T_link4OnlineReportTimeMS; // 链接4心跳时间MS

extern uint8_t usart3_rev_cnt;
extern uint8_t usart3_rev_flag;
extern uint8_t usart3_rev_finish; // 串口3接收计数，串口3接收标志串口接收完成标志

extern uint8_t lte_power_on_flag, lte_init_flag, lte_net_flag, lte_connect_flag;
extern uint8_t err_cnt, err_num;
extern uint8_t heart_beat_flag; // 心跳标志

extern uchar T_DataLampAllLight; // 数据DATA灯总亮 2016.5.27 ZCL
extern uint C_DataLampAllLight;  // 数据DATA灯总亮 计数器
extern uint C_LedLight;          // LED闪烁延时计数器
extern uint C_NetLedLightCSQ;    // Net LED闪烁CSQ指示计数器 ZCL 2017.5.25
extern u16 Cw_Rcv2;              // 接收计数器
extern u16 Cw_Txd2;              // 发送计数器
extern u8 CGD0_LinkConnectOK[];  // 场景0连接标志
extern u8 F_RunCheckModular;

extern u8 B_Com1ErrWithModule1; // 标志 与子模块1通讯失败
extern u8 B_Com1ErrWithModule2; // 标志 与子模块2通讯失败
extern u8 F_RemoteQuery;        // 通过GPRS远程查询参数标志
extern u8 F_Reset_GPS;          // 重启GPS标志
extern u16 T_Reset_GPS;
extern u16 C_Reset_GPS;
extern u16 T_PowerOnOff_GPS;
extern u16 C_PowerOnOfft_GPS;

extern u8 B_LoRaErrWithModule1; // 标志 与子模块1通讯失败
extern u8 B_LoRaErrWithModule2; // 标志 与子模块2通讯失败
extern u8 B_LoRaErrWithModule3; // 标志 与子模块3通讯失败
extern u8 B_LoRaErrWithModule4; // 标志 与子模块4通讯失败
extern u8 B_LoRaErrWithModule5; // 标志 与子模块5通讯失败
extern u8 B_LoRaErrWithModule6; // 标志 与子模块6通讯失败
extern u8 B_LoRaErrWithModule7; // 标志 与子模块7通讯失败
extern u8 B_LoRaErrWithModule8; // 标志 与子模块8通讯失败

extern u16 T_Judge_No1_VVVF_Comm; // 判断1#泵变频器通讯故障计时
extern u16 C_Judge_No1_VVVF_Comm; // 判断1#泵变频器通讯故障计数
extern u16 T_Judge_No2_VVVF_Comm; // 判断2#泵变频器通讯故障计时
extern u16 C_Judge_No2_VVVF_Comm; // 判断2#泵变频器通讯故障计数
extern u16 T_Judge_No3_VVVF_Comm; // 判断3#泵变频器通讯故障计时
extern u16 C_Judge_No3_VVVF_Comm; // 判断3#泵变频器通讯故障计数
extern u16 T_Judge_No4_VVVF_Comm; // 判断4#泵变频器通讯故障计时
extern u16 C_Judge_No4_VVVF_Comm; // 判断4#泵变频器通讯故障计数
extern u16 T_Judge_No5_VVVF_Comm; // 判断5#泵变频器通讯故障计时
extern u16 C_Judge_No5_VVVF_Comm; // 判断5#泵变频器通讯故障计数
extern u16 T_Judge_No6_VVVF_Comm; // 判断6#泵变频器通讯故障计时
extern u16 C_Judge_No6_VVVF_Comm; // 判断6#泵变频器通讯故障计数
extern u16 T_Judge_No7_VVVF_Comm; // 判断7#泵变频器通讯故障计时
extern u16 C_Judge_No7_VVVF_Comm; // 判断7#泵变频器通讯故障计数
extern u16 T_Judge_No8_VVVF_Comm; // 判断8#泵变频器通讯故障计时
extern u16 C_Judge_No8_VVVF_Comm; // 判断8#泵变频器通讯故障计数

extern u8 F_No1_VVVFComm_Fault; // 1#泵变频器通讯异常标志
extern u8 F_No2_VVVFComm_Fault; // 2#泵变频器通讯异常标志
extern u8 F_No3_VVVFComm_Fault; // 3#泵变频器通讯异常标志
extern u8 F_No4_VVVFComm_Fault; // 4#泵变频器通讯异常标志
extern u8 F_No5_VVVFComm_Fault; // 5#泵变频器通讯异常标志
extern u8 F_No6_VVVFComm_Fault; // 6#泵变频器通讯异常标志
extern u8 F_No7_VVVFComm_Fault; // 7#泵变频器通讯异常标志
extern u8 F_No8_VVVFComm_Fault; // 8#泵变频器通讯异常标志

extern u16 Count_Save1_No1_VVVF_Comm; // 1#泵变频器通讯计数器1
extern u16 Count_Save2_No1_VVVF_Comm; // 1#泵变频器通讯计数器2
extern u16 Count_Save1_No2_VVVF_Comm; // 2#泵变频器通讯计数器1
extern u16 Count_Save2_No2_VVVF_Comm; // 2#泵变频器通讯计数器2
extern u16 Count_Save1_No3_VVVF_Comm; // 3#泵变频器通讯计数器1
extern u16 Count_Save2_No3_VVVF_Comm; // 3#泵变频器通讯计数器2
extern u16 Count_Save1_No4_VVVF_Comm; // 4#泵变频器通讯计数器1
extern u16 Count_Save2_No4_VVVF_Comm; // 4#泵变频器通讯计数器2
extern u16 Count_Save1_No5_VVVF_Comm; // 5#泵变频器通讯计数器1
extern u16 Count_Save2_No5_VVVF_Comm; // 5#泵变频器通讯计数器2
extern u16 Count_Save1_No6_VVVF_Comm; // 6#泵变频器通讯计数器1
extern u16 Count_Save2_No6_VVVF_Comm; // 6#泵变频器通讯计数器2
extern u16 Count_Save1_No7_VVVF_Comm; // 7#泵变频器通讯计数器1
extern u16 Count_Save2_No7_VVVF_Comm; // 7#泵变频器通讯计数器2
extern u16 Count_Save1_No8_VVVF_Comm; // 8#泵变频器通讯计数器1
extern u16 Count_Save2_No8_VVVF_Comm; // 8#泵变频器通讯计数器2

extern u8 B_Com1Con; // 串口1发送控制
#endif               /* __GLOBALV_EXTERN_H */
