/**
 ******************************************************************************
 * @file    Gprs_at.c
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.27
 * @date    2014-01-03
 * @brief   AT COMMAND
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "Gprs_at.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include "com3_232.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "math.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private variables Extern---------------------------------------------------*/
extern uchar Txd3Buffer[];     // 发送缓冲区
extern uint Cw_Txd3Max;        // 有多少个字符需要发送//
extern uchar StringBuffer[12]; // BCD码转成字符串缓存。主要对IP地址和长度进行转换
extern uchar S_M35;            // s步骤：M35流程
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern void Com2_printf(char *fmt, ...);

/*通用AT指令 */
void At_AT(void) // 发送AT字符用于同步波特率
{
  // Com2_printf("AT");
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT"); // YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ATE(u8 i) // 命令回显模式  =1启用回显，默认；=0不回显(设置)
{
  // Com2_printf("ATE0");
  Cw_Txd3Max = 0;
  Gprs_TX_String("ATE"); // YLS 相同命令
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CSQ(void) // 检测信号强度
{
  // Com2_printf("AT+CSQ");
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CSQ"); // YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CREG(void) // 查询GSM注册状态  返回1,5 已注册
{
  // Com2_printf("AT+CREG?");
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CREG?"); // 0D,0A, +CREG: 0,1		YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ThreePlusSign(void) // 退出透传，进入命令模式 +++
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("+++"); // YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ATO0(void) // 退出命令，进入数据透传模式
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+ATO0");
  Gprs_TX_String("AT+ATO");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ATI(void) // 显示产品信息
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("ATI"); // YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ATQ0(void) // 设置结果码显示模式  0=向TE发送结果码(默认)；1=不发送
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("ATQ0");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ATAndF(void) // 设置所有TA参数为出厂设置
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT&F");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ATAndW(void) // 保存用户配置参数
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT&W");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_ATV1(void) // 设置TA响应返回内容格式。 =0数字码；=1详细码(默认)
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("ATV1");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QPOWD(void) // 正常关机
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QPOWD=1");
  Gprs_TX_String("AT+CPOF"); // 关机
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_IPR(void) // 设置波特率，若=0，为自适应波特率
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+IPR=115200");
  // Gprs_TX_String("AT+IPR=57600&W");
  //	Gprs_TX_String("AT+IPRX=57600");	//YLS 2022.12.25
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

/*	SMS操作函数	*/
void At_CPMS(void) // 选择短消息存储器
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CPMS="); // AT+CPMS="SM","SM","SM" 	//YLS 相同命令
  Gprs_TX_Byte('"');
  Gprs_TX_String("SM");
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(',');
  Gprs_TX_Byte('"');
  Gprs_TX_String("SM");
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(',');
  Gprs_TX_Byte('"');
  Gprs_TX_String("SM");
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CMGF(u8 i) // 配置短消息模式 =0 PDU； =1 文本
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CMGF="); // YLS 相同命令
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CSCS(void) // 选择 TE字符集
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CSCS="); // YLS 相同命令
  Gprs_TX_Byte('"');
  Gprs_TX_String("GSM");
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CNMI(void) // 短信提示设置
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CNMI=2,1,0,0,0"); // YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CSMP(void) // 设置文本格式参数
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CSMP=17,169,0,241"); // YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CSDH(void) // 显示文本格式参数 =0显示部分参数 =1显示全部参数
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CSDH=0"); // YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CPMSInq(void) // 确认短信息存储器,并查询容量
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CPMS?"); // AT+CPMS?	//YLS 相同命令
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CMGR(uint i) // 读取短消息,指定存储位置
{
  uchar j, k;
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CMGR="); // YLS 相同命令
  j = Bcd2Str(i);             // 1个字转10进制字符串 如:0xDE 转成 222
  for (k = 0; k < j; k++)
  {
    Txd3Buffer[Cw_Txd3Max++] = StringBuffer[k];
  }
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CMGD(uchar i) // 删除短消息 AT+CMGD=1,4(删除当前存储器中全部短信)
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CMGD=1,"); // 周成磊注：1,4不好用，改成0,4才好用  2013.5.14 !!!
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QSCLK(u8 i) // 慢时钟配置  =1启用慢时钟
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QSCLK=");
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

/*	GPRS操作函数	 TCP单路透传测试 */
void At_CFUN(void) // 设置模块所有功能开启。	第二个参数=0，不复位立即有效；
{
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CFUN=1"); // 第一个参数=0，最小功能；=1最大功能(默认)；=4，关闭射频
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CPIN(void) // 确保PIN码已解。ZCL:一般不需要
{
  // Com2_printf("AT+CPIN=?");
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CPIN=?");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_CGATT(void) // GPRS附着
{
  // Com2_printf("AT+CGATT=1");
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+CGATT=1");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QIFGCNT(u8 i) // 配置前置场景 设置值: 0-1  P142
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QIFGCNT=");
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QICSGP(void) // 设置GPRS连接方式 0:CSD连接  1:GPRS连接
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QICSGP=1,");
  Gprs_TX_Byte('"');
  Gprs_TX_String("CMNET");
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QIDNSIP(u8 i) // 设置服务器地址方式 =0 IP访问; =1 域名访问
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QIDNSIP=");
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QINDI(void) // 查看数据接收方式1: 需要命令提取数据
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QINDI?");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QISHOWRA(u8 i) // 查看数据显示格式 1: 加上IP及端口号
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QISHOWRA=");
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QIHEAD(u8 i) // 查看数据显示格式 1: 加上IPD<Len>
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QIHEAD=");
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QISHOWPT(u8 i) // 查看数据显示格式 1: 加上协议类型
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QISHOWPT=");
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QIMUX(u8 i) // 0:单路连接   1: 多路连接
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QIMUX=");
  // Gprs_TX_String("AT+CMUX=");
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QIMODE(u8 i) //=1使用透传模式
{
  // Com2_printf("AT+CIPMODE=0");
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QIMODE=");
  Gprs_TX_String("AT+CIPMODE="); // YLS 2022.12.24
  Gprs_TX_Byte(i);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QITCFG(void) // 收到串口数据后等待600ms发送，或串口数据达到512字节后，
{                    // 立即发送。 <配置透明传输模式 >
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QITCFG=3,6,512,1");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QIMuxOPEN(u8 i) // 建立多路 TCP连接或注册UDP端口号  (0-5个连接)
{                       //"TCP", "222.173.103.226",7500		公司
  uchar j, k, m;
  uint w = 0;
  const u8 *modetbl[2] = {"UDP", "TCP"}; // 连接模式//modetbl:1:TCP测试;0,UDP测试)
  u8 p_TmpBuf[512] = {0};                // 缓存1
  u8 localPortBuf[5] = {0};              // 本地端口缓存
  u8 ipbuf[15];                          // IP缓存
  u8 portBuf[5] = {0};                   // 远端端口缓存

  Pw_Link1localPort = (GprsPar[LoclaPort0Base + i * 2] << 8) + GprsPar[LoclaPort0Base + i * 2 + 1]; // YLS 2022.12.27
  sprintf((char *)localPortBuf, "%d", Pw_Link1localPort);                                           // 第一路链接本地端口字转换为ascii码
  //			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link1IP1, Pw_Link1IP2, Pw_Link1IP3, Pw_Link1IP4); //存放IP
  sprintf((char *)ipbuf, "%d.%d.%d.%d", GprsPar[Ip0Base + i * 4], GprsPar[Ip0Base + i * 4 + 1], GprsPar[Ip0Base + i * 4 + 2], GprsPar[Ip0Base + i * 4 + 3]); // 存放IP
  Pw_Link1Port = (GprsPar[Port0Base + i * 2] << 8) + GprsPar[Port0Base + i * 2 + 1];                                                                         // YLS 2022.12.27
  sprintf((char *)portBuf, "%d", Pw_Link1Port);                                                                                                              // 第一路链接远端端口字转换为ascii码
  if (GprsPar[LinkTCPUDP0Base + i] == 0)                                                                                                                     // 第二路为UDP
  {
    sprintf((char *)p_TmpBuf, "AT+CIPOPEN=0,\"%s\",\"%s\",%s,%s", modetbl[(u8)GprsPar[LinkTCPUDP0Base + i]], ipbuf, portBuf, localPortBuf);
  }
  else
  {
    sprintf((char *)p_TmpBuf, "AT+CIPOPEN=0,\"%s\",\"%s\",%s", modetbl[(u8)GprsPar[LinkTCPUDP0Base + i]], ipbuf, portBuf); // 把网络信息给申请的空间p_TmpBuf
  }
  // Com2_printf("%s\r\n", p_TmpBuf);
  Cw_Txd3Max = 0;

  //  Gprs_TX_String("AT+QIOPEN=");		//"TCP", "119.75.218.77",80		百度
  Gprs_TX_String("AT+CIPOPEN="); //"TCP", "119.75.218.77",80		百度	YLS 2022.12.24
  Gprs_TX_Byte(i + 0x30);        // 连接号
  Gprs_TX_Byte(',');

  Gprs_TX_Byte('"');
  if (GprsPar[LinkTCPUDP0Base + i] == 0) // 连接类型
    Gprs_TX_String("UDP");
  else
    Gprs_TX_String("TCP");
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(',');

  Gprs_TX_Byte('"');

  // 总述：地址不为0，IP地址，否则为域名
  // IP地址，BCD码转成字符串，并填充
  if (GprsPar[Ip0Base + i * 4]) // IP地址
  {
    for (m = 0; m < 4; m++) // 4个字节 （DE AD 67 E2）2012.8.20
    {
      w = GprsPar[Ip0Base + i * 4 + m]; // 取出一个值(字节)
      j = Bcd2Str(w);                   // 1个字转10进制字符串 如:0xDE 转成 222
      for (k = 0; k < j; k++)
      {
        Txd3Buffer[Cw_Txd3Max++] = StringBuffer[k];
      }
      if (m < 3)
        Txd3Buffer[Cw_Txd3Max++] = '.';
    }
  }
  // 域名内容，填充
  // else
  // {
  //   k = 0;
  //   while (GprsPar[DomainName0Base + i * 31 + k] != 0)
  //   {
  //     Txd3Buffer[Cw_Txd3Max++] = GprsPar[DomainName0Base + i * 31 + k];
  //     k++;
  //   }
  // }
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(',');

  // 端口2个字节需要合成整数，BCD码转成字符串 2012.8.21
  w = GprsPar[Port0Base + i * 2]; // ！周成磊 2012.9.11 这里是2 注意
  w = (w << 8) + GprsPar[Port0Base + i * 2 + 1];
  j = Bcd2Str(w); // 1个字转10进制字符串 如:0x1D4C 转成 7500
  for (k = 0; k < j; k++)
  {
    Txd3Buffer[Cw_Txd3Max++] = StringBuffer[k];
  }

  // UDP模式，填充本地端口
  if (GprsPar[LinkTCPUDP0Base + i] == 0) // 连接类型
  {
    Gprs_TX_Byte(',');
    // 端口2个字节需要合成整数，BCD码转成字符串 2012.8.21
    w = GprsPar[LoclaPort0Base + i * 2]; // ！周成磊 2012.9.11 这里是2 注意
    w = (w << 8) + GprsPar[LoclaPort0Base + i * 2 + 1];
    j = Bcd2Str(w); // 1个字转10进制字符串 如:0x1D4C 转成 7500
    for (k = 0; k < j; k++)
    {
      Txd3Buffer[Cw_Txd3Max++] = StringBuffer[k];
    }
  }

  // 加入回车符
  Gprs_TX_Byte(0x0d);
  // 启动发送
  Gprs_TX_Start();
}

// 发送域名查询命令
void At_DomainName(u8 i)
{
  u8 k;

  Gprs_TX_String("AT+CDNSGIP ="); //"TCP", "119.75.218.77",80		百度	YLS 2022.12.24
  Gprs_TX_Byte('"');
  k = 0;
  while (GprsPar[DomainName0Base + i * 31 + k] != 0)
  {
    Txd3Buffer[Cw_Txd3Max++] = GprsPar[DomainName0Base + i * 31 + k];
    k++;
  }
  Gprs_TX_Byte('"');
  // 加入回车符
  Gprs_TX_Byte(0x0d);
  // 启动发送
  Gprs_TX_Start();

  Com2_printf("Send Data：%s\r\n", Txd3Buffer);
}

/* void At_QISEND(u8 LinkNo)			//DTU内容发送,不带长度最后使用(CTRL+Z)结束 实际为：0x1A
{
  Gprs_TX_String("AT+QISEND=");
  Gprs_TX_Byte(LinkNo+0x30);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
} */

void At_QISENDAccordingLength(u8 LinkNo, uint Length) // DTU 内容发送按照长度
{
  uchar k;
  u8 ipbuf[15];              // IP缓存
  u8 portBuf[5] = {0};       // 远端端口缓存
  u8 p_SendTmpBuf[50] = {0}; // 缓存
  u16 LinkIP1;
  u16 LinkIP2;
  u16 LinkIP3;
  u16 LinkIP4;
  u16 LinkPort;
  u8 LinkTCP_UDPType;
  // 服务器(DSC)的IP地址
  if (LinkNo == 0)
  {
    LinkIP1 = GprsPar[Ip0Base];
    LinkIP2 = GprsPar[Ip0Base + 1];
    LinkIP3 = GprsPar[Ip0Base + 2];
    LinkIP4 = GprsPar[Ip0Base + 3];
    LinkPort = (GprsPar[Port0Base] << 8) + GprsPar[Port0Base + 1];
    LinkTCP_UDPType = GprsPar[LinkTCPUDP0Base];
  }
  else if (LinkNo == 1)
  {
    LinkIP1 = GprsPar[Ip1Base];
    LinkIP2 = GprsPar[Ip1Base + 1];
    LinkIP3 = GprsPar[Ip1Base + 2];
    LinkIP4 = GprsPar[Ip1Base + 3];
    LinkPort = (GprsPar[Port1Base] << 8) + GprsPar[Port1Base + 1];
    LinkTCP_UDPType = GprsPar[LinkTCPUDP1Base];
  }
  if (LinkNo == 2)
  {
    LinkIP1 = GprsPar[Ip2Base];
    LinkIP2 = GprsPar[Ip2Base + 1];
    LinkIP3 = GprsPar[Ip2Base + 2];
    LinkIP4 = GprsPar[Ip2Base + 3];
    LinkPort = (GprsPar[Port2Base] << 8) + GprsPar[Port2Base + 1];
    LinkTCP_UDPType = GprsPar[LinkTCPUDP2Base];
  }
  if (LinkNo == 3)
  {
    LinkIP1 = GprsPar[Ip3Base];
    LinkIP2 = GprsPar[Ip3Base + 1];
    LinkIP3 = GprsPar[Ip3Base + 2];
    LinkIP4 = GprsPar[Ip3Base + 3];
    LinkPort = (GprsPar[Port3Base] << 8) + GprsPar[Port3Base + 1];
    LinkTCP_UDPType = GprsPar[LinkTCPUDP3Base];
  }

  Cw_Txd3Max = 0;

  // Gprs_TX_String("AT+CIPSEND="); // YLS 2022.12.24
  // Gprs_TX_Byte(LinkNo + 0x30);
  // Gprs_TX_Byte(',');
  // // 发送长度，转成字符串
  // j = Bcd2Str(Length); // 1个字转10进制字符串 如:0xDE 转成 222
  // for (k = 0; k < j; k++)
  // {
  //   Txd3Buffer[Cw_Txd3Max++] = StringBuffer[k];
  // }

  sprintf((char *)ipbuf, "%d.%d.%d.%d", LinkIP1, LinkIP2, LinkIP3, LinkIP4); // 存放IP
  sprintf((char *)portBuf, "%d", LinkPort);                                  // 第一路链接远端端口字转换为ascii码
  if (LinkTCP_UDPType == 0)                                                  // 第一路为UDP
  {
    sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=%d,%d,\"%s\",%s", LinkNo, Length, ipbuf, portBuf); // 把网络信息给申请的空间p_LinkTmpBuf
  }
  else if (LinkTCP_UDPType == 1) ////第一路为TCP Client
  {
    sprintf((char *)p_SendTmpBuf, "AT+CIPSEND=%d,%d", LinkNo, Length);
  }
  // Com2_printf("%s\r\n", p_SendTmpBuf);
  for (k = 0; k < strlen((char *)p_SendTmpBuf); k++)
  {
    Txd3Buffer[Cw_Txd3Max++] = p_SendTmpBuf[k];
  }

  // 回车符
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QISACK(void) // 检测数据是否发送成功  +QISACK: 4,4,0 说明发送长度和已确认
{                    // 发送成功的数据都是4，发送完毕；0表示未发送成功的数据个数
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QISACK");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QICLOSE(u8 i) // 关闭当前指定的连接
{
  // printf("S_M35=%d\r\n", S_M35);
  // Com2_printf("AT+CIPCLOSE=");
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QICLOSE=");
  Gprs_TX_String("AT+CIPCLOSE="); // YLS 2022.12.24
  Gprs_TX_Byte(i + 0x30);
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_NETOPEN(void) // 打开网络
{
  // printf("S_M35=%d\r\n", S_M35);
  // Com2_printf("AT+NETOPEN");
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+NETOPEN"); // YLS 2022.12.24
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_NETCLOSE(void) // 关闭网络
{
  // printf("S_M35=%d\r\n", S_M35);
  // Com2_printf("AT+NETCLOSE");
  Cw_Txd3Max = 0;
  Gprs_TX_String("AT+NETCLOSE"); // YLS 2022.12.24
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QIDEACT(void) // 关闭GPRS场景；之后才能透传，单路等参数，否则出现错误
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QIDEACT");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

void At_QISTAT(void) // 查询网络状态
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QISTAT");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

// ZCL 2018.12.10
void At_QIURC(void) // 初始化完成提示上报
{
  Cw_Txd3Max = 0;
  //  Gprs_TX_String("AT+QIURC=1");
  Gprs_TX_Byte(0x0d);
  Gprs_TX_Start();
}

/****END OF FILE****/
