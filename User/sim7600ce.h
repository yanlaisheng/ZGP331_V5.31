#ifndef __SIM7600CE_H__
#define __SIM7600CE_H__
#include "GlobalConst.h"
#include "GlobalV_Extern.h"

extern u8 Scan_Wtime;
extern u8 sim_ready;      // SIM卡就绪状态=1就绪，=0未找到
extern u8 F_RunAgain;     // 重新运行标志
extern u8 p_TmpBuf[];     // 缓存1
extern u8 p1_TmpBuf[];    // 缓存2
extern u8 ModularADDIP[]; // 模块本身的IP地址
extern u8 domain_name[];
extern u8 domain_name1[];
extern u8 domain_name2[];
extern u8 domain_name3[];

void sim_at_response(u8 mode);
u8 *sim7600ce_check_cmd(u8 *str);
u8 sim7600ce_send_cmd(u8 *cmd, u8 *ack, u16 waittime);
u8 sim7600ce_wait_request(u8 *request, u16 waittime);
u8 sim7600ce_chr2hex(u8 chr);
u8 sim7600ce_hex2chr(u8 hex);
void sim7600ce_unigbk_exchange(u8 *src, u8 *dst, u8 mode);

void sim7600ce_connectstaModularNum(void);                                   // 获得模块需要连接的IP数量
u8 sim7600ce_send_Modular_IP(u8 *cmd, u16 waittime);                         // 获得模块IP
u8 GPRSNetConfig(u8 link_num);                                               // 网络配置信息
void sim7600ceGPRSUpLoaded(u8 *SendDateBuf, u16 SendBufLen, u8 Star_OR_hex); // GPRS数据加载
void sim7600ce_Modular_Information(void);                                    // 模块信息显示
u8 sim7600ce_GSM_Information(void);                                          // 显示GSM模块信息
void sim7600ce_Netlink_Information(void);                                    // 显示网络连接信息
void ntp_update(void);                                                       // 网络同步时间
void sim7600ce_GPRSRUN(void);                                                // SIM7600ce主运行函数
void SIM7600CE_GPIO_Init(void);                                              // SIM7600ce的GPIO初始化
void SIM7600ce_Init(void);                                                   // SIM7600ce初始化
void SIM7600ce_NetworkCheck(void);                                           // SIM7600ce运行过程中每个一段时间检查网络
void sim7600ce_ReconnectNet(void);                                           // GPRS模块连接失败后重新连接
void sim7600ce_SendHeartbeat(void);                                          // GPRS模块发送心跳函数
void sim7600ce_ReceiveData(void);                                            // GPRS模块数据接收函数
void sim7600ce_sms_ReciveInit(void);                                         // 短信接收初始化函数
void sim7600ce_sms_send(u8 *SendDateBuf);                                    // SIM7600ce发短信
void sim7600ce_smsread_UpLoaded(void);                                       // 短信读数据加载
void sim7600ce_smsWrite_UpLoaded(void);                                      // 短信写数据加载
void usart3_send_string(unsigned char *str, unsigned short len);
void lte_clear(void);
u8 sim7600ce_send_cmd(u8 *cmd, u8 *ack, u16 waittime);
void sim7600ce_DoWithData(void);
#endif
