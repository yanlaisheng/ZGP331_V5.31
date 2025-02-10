/**
 ******************************************************************************
 * @file    Gprs_at.h
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.27
 * @date    2014-01-03
 * @brief   AT COMMAND
 ******************************************************************************
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __GPRS_AT_H
#define __GPRS_AT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void At_AT(void);            // 发送AT字符用于同步波特率
void At_ATE(u8 i);           // 命令回显模式  =1启用回显，默认；=0不回显(设置);
void At_CSQ(void);           // 检测信号强度
void At_CREG(void);          // 查询GSM注册状态  返回1,5 已注册
void At_ThreePlusSign(void); // 退出透传，进入命令模式 +++
void At_ATO0(void);          // 退出命令，进入数据透传模式
void At_ATI(void);           // 显示产品信息
void At_ATQ0(void);          // 设置结果码显示模式  0=向TE发送结果码(默认);1=不发送
void At_ATAndF(void);        // 设置所有TA参数为出厂设置
void At_ATAndW(void);        // 保存用户配置参数
void At_ATV1(void);          // 设置TA响应返回内容格式。 =0数字码；=1详细码(默认);
void At_QPOWD(void);         //=0立即关机；=1正常关机
void At_IPR(void);           // 设置波特率，若=0，为自适应波特率
void At_CPMS(void);          // 选择短消息存储器
void At_CMGF(u8 i);          // 配置短消息模式 =0 PDU； =1 文本
void At_CSCS(void);          // 选择 TE字符集
void At_CNMI(void);          // 短信提示设置
void At_CSMP(void);          // 设置文本格式参数
void At_CSDH(void);          // 显示文本格式参数 =0显示部分参数 =1显示全部参数
void At_CPMSInq(void);       // 确认短信息存储器,并查询容量
void At_CMGR(uint i);        // 读取短消息,指定存储位置
void At_CMGD(uchar i);       // 读取短消息,指定存储位置
void At_QSCLK(u8 i);         // 慢时钟配置  =1启用慢时钟
void At_CFUN(void);          // 设置模块所有功能开启。	第二个参数=0，不复位立即有效；
void At_CPIN(void);          // 确保PIN码已解。ZCL:一般不需要
void At_CGATT(void);         // GPRS附着
void At_QIFGCNT(u8 i);       // 配置前置场景 设置值: 0-1  P142
void At_QICSGP(void);        // 设置GPRS连接方式 0:CSD连接  1:GPRS连接
void At_QIDNSIP(u8 i);       // 设置服务器地址方式 =0 IP访问; =1 域名访问
void At_QINDI(void);         // 查看数据接收方式1: 需要命令提取数据
void At_QISHOWRA(u8 i);      // 查看数据显示格式 1: 加上IP及端口号
void At_QIHEAD(u8 i);        // 查看数据显示格式 1: 加上IPD<Len>
void At_QISHOWPT(u8 i);      // 查看数据显示格式 1: 加上协议类型
void At_QIMUX(u8 i);         // 0:单路连接   1: 多路连接
void At_QIMODE(u8 i);        //=1使用透传模式
void At_QITCFG(void);        // 收到串口数据后等待600ms发送，或串口数据达到512字节后，
                      // 立即发送。 <配置透明传输模式 >
void At_QIMuxOPEN(u8 i); // 建立多路 TCP连接或注册UDP端口号  (0-5个连接)
                         //"TCP", "222.173.103.226",7500		公司
void At_QISENDAccordingLength(u8 LinkNo, uint Length); // DTU 内容发送按照长度
void At_QISACK(void);                                  // 检测数据是否发送成功  +QISACK: 4,4,0 说明发送长度和已确认
void At_QICLOSE(u8 i);                                 // 关闭当前指定的连接
void At_NETCLOSE(void);                                // 关闭网络
void At_NETOPEN(void);                                 // 打开网络
void At_QIDEACT(void);                                 // 关闭GPRS场景；之后才能透传，单路等参数，否则出现错误
void At_QISTAT(void);                                  // 查询网络状态

void At_QIURC(void); // 初始化完成提示上报  ZCL 2018.12.10

// 向发送缓存区写入一个字节
void Gprs_TX_Byte(u8 i);
// 向发送缓存区写入一个字符串
void Gprs_TX_String(u8 *String);
void Gprs_TX_Fill(uc8 *String, u8 Length);
// 启动发送
void Gprs_TX_Start(void);
// 计算字符串长度
uchar Gprs_String_Length(u8 *String);
void At_DomainName(u8 i);
#endif /* __GPRS_AT_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
