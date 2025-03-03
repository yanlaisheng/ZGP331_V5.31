#ifndef __IAP_H__
#define __IAP_H__

#ifndef IAP_C_ // 如果没有定义
#define IAP_Ex_ extern
#else
#define IAP_Ex_
#endif

#include <stdint.h>
// #include "LoopList.h"
#include "flash_extern.h"
#include "iap_interface.h"

#if 1 // 是否打印日志
#define Iap_printf printf
#else
#define Com2_printf(...)
#endif

/*选择IAP程序作为什么程序使用*/
#define IAPBootLoaderProgram 0		  // 作为BootLoader使用
#define IAPUserProgram 1			  // 作为用户程序使用
#define IAPBootLoaderAndUserProgram 2 // BootLoader + User

#define IAPProgramSelect IAPUserProgram /*选择IAP程序作为什么程序使用*/

#define UpdateStatus_None 0x00				 // 无
#define UpdateStatus_WriteAppOk 0x01		 // 上次成功写入了更新程序
#define UpdateStatus_UrlResolveErr 0x02		 // 解析URL错误
#define UpdateStatus_FlashEraseBackErr 0x03	 // Flash 擦除失败(备份地址)
#define UpdateStatus_FlashBackErr 0x04		 // 备份失败
#define UpdateStatus_FlashExternInitErr 0x05 // 初始化外部flash失败

#define UpdateStatus_DataAddressError 0xE0	// Flash的高位地址不是0x08 或者 RAM的高位地址不是0x20
#define UpdateStatus_LoopListWriteErr 0xE1	// 写缓存错误
#define UpdateStatus_DownloadTimeout 0xE2	// 程序下载超时
#define UpdateStatus_MainTimeout 0xE3		// 整个程序运行的时间
#define UpdateStatus_FlashWriteErr 0xE4		// Flash 写错误
#define UpdateStatus_FlashEraseUserErr 0xE5 // Flash 擦除失败(运行地址)
#define UpdateStatus_FlashRollBackErr 0xE6	// 备份区拷贝到运行区失败

#define UpdateStatus_ReceiveDataLost 0xE7 // 数据接不完整

#define UpdateStatus_ProgramInvalid 0xE8 // 程序无效

#define UpdateStatus_CheckErr 0xED // 校验错误

#define UpdateStatus_Err 0xEF			  // 进入更新标记ERR
#define UpdateStatus_WriteAppOkFlage 0xFE // 如果获取的状态是 0x01 则写入该值(更新状态是该值说明运行用户程序失败)

/******************************************/
#define IAP_SECTION_DOWNLOAD 1  //1:使能分段下载 0:不使用分段下载
#define IAP_RANGE 2048         //分段下载每次请求的数据个数

#define IAPCheckDataBuffLen 128 // 校验的数据个数

#if (IAP_SECTION_DOWNLOAD == 1)
#define IAPLoopListBuffLen (IAP_RANGE + IAPCheckDataBuffLen + 2) // 环形队列接收数据缓存大小
#else
	#define IAPLoopListBuffLen 8192 //环形队列接收数据缓存大小
#endif

#define IAPReadDataIdleTimer 60000 // 在该时间内接收不到数据就认为接收完了数据(ms)

#define IAPUpdateDownloadNumber 3 // 尝试下载程序的次数(默认3,最大不得超过255)

#define IAPMainTimeoutCompare 120000 // 整个程序强制超时时间(ms),超时重启

// 固件版本号长度
#define FirmwareVersionLen 20
// 产品型号
IAP_Ex_ char DeviceModel[50];
// 固件版本
IAP_Ex_ char *FirmwareVersion;

typedef struct
{
	char *Str;								  // 通用变量
	long Len;								  // 通用变量
	char buff[iap_interface_updata_data_len]; // 通用变量

	uint16_t UpdateStatusValue; // 获取更新的状态变量

	// Url相关
	char SSLEN;																		 // 1:SSL  0:非SSL
	char IP[256];																	 // 服务器IP/域名
	uint16_t Port;																	 // 端口号
	uint16_t HardwareVersionCopy;													 // 硬件版本:0-65534
	char Path[iap_interface_updata_data_len - iap_interface_updata_data_url_offset]; // Path(文件路径)

	/*处理HTTP数据*/
	char HttpHeadCnt;
	char HttpHeadOK;		 // 接收到正常的http数据
	char HttpHeadEndOK;		 // http的heap接收完成,后面发过来的是数据
	char HttpDataStartFlage; // 下次传进来的是消息体
	//

#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
	int range_start;
	int range_stop;
	int content_length_flag;   // 获取到分段数据尝试标志
	int content_length;		   // 获取到的分段数据长度
	int section_download_len;  // 接到到分段数据个数
	int section_download_flag; // 接收完分段数据,可以发送分段请求指令
#endif

#if (IAPProgramSelect == IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram) // BootLoader  or  BootLoader+User

	char SendGetFileOrderFlag; // 发送完获取文件指令标志;1-发送完获取指令

	char ReadDataEndFlag; // 接收完了程序;确认接收完程序以后设置该变量为1

	char UpdateFlag;		// 更新标志;1:有更新;0:无更新
	char ConnectWebFlag;	// 连接Web服务器标志;1-连接; 0-未连接
	char FlashWriteErrFlag; // 写入Flash出错标志;1:写入出错;0:无错误
	char UpdateDownloadCnt; // 下载程序的次数

	// 校验
	char CheckDataBuff[IAPCheckDataBuffLen + 2]; // 校验数据
	uint32_t CheckDataCnt;
	uint16_t CheckValue;
	char CheckValueErrFlag; // 1:crc校验错误  0:无错误
	uint32_t FileSizeNow;	// 固件文件大小-当前接收的
	uint32_t FileSizeSave;	// 固件文件大小-存储在flash的
	// 固件版本:固件里面的
	char FileFirmwareVersion[FirmwareVersionLen];
	// Flash地址
	uint32_t UpdateAddressCnt; // 用于写Flash的时候地址累加

	// 备份/回滚变量
	uint32_t UserDataSize;	   // 用户程序大小
	uint32_t UserRunAdddrCnt;  // 用户程序运行首地址
	uint32_t UserBackAdddrCnt; // 用户程序备份首地址
	uint16_t ReadDate;
	// 超时
	uint32_t DownloadTimeout; // 程序下载超时累加变量
	uint32_t MainTimeout;	  // 整体运行超时累加变量

	// 人为操作更新
	uint32_t PeopleControlCnt; // 人为操作更新累加变量

	// 环形队列
	rb_t LoopList;
	char LoopListBuff[IAPLoopListBuffLen + 1]; // 接收程序数据缓存数组
	char LoopListPutEnable;					   // 控制是不是可以向环形队列写入数据
	char LoopListWriteErr;					   // 环形队列写错误
	char LoopListReadDate[2];				   // 从环形队列读取数据使用
	uint16_t LoopListU16Date;				   // 从环形队列读取数据,组合成16位数据

	// 缓存2,4,8字节数据
	char BuffDate[FLASH_WRITE_SIZE]; // 缓存数据
	char BuffDateCount;				 // 缓存计数
	char BuffDateFlage;				 // 缓存完成

	// 标记文件是否有效;1:有效
	char ProgramEfficient;
	// 擦除运行区标志
	char erase_run_app_flag;
/*获取实际http协议字段中的数据大小*/
#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
#define HttpContentRangeEnable 0
	uint32_t ContentRangeLen;
	uint32_t ContentRangeLenCopy;
	char ContentRangeCnt1;
	char ContentRangeCnt1Copy;
	char ContentRangeCnt2;
	char ContentRangeCnt2Copy;
	char ContentRangeCnt3;
	char ContentRangeCnt3Copy;
	char ContentRangeSign1;
	char ContentRangeSign2;
#endif
#endif
} IAPStruct;

IAP_Ex_ IAPStruct IAPStructValue;

// 获取更新的信息
#if (IAPProgramSelect == IAPUserProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)
void IAPInfoPathInit(void);		 // 用户程序调用
uint16_t IAPUpdateDispose(void); // 处理更新
#endif

#if (IAPProgramSelect == IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram) // BootLoader  or  BootLoader+User

void IAPInit(void);							/*初始化*/
void IAPWriteData(void);					// 写数据
void IAPGetProgramFile(void);				// 发送指令获取程序文件
void IAPHttpHead(char Res);					// 判断到开始接收消息体,设置 IAPStructValue.HttpDataStartFlage=1
void IAPPutDataToLoopList(char Res);		// 写入数据到环形队列
void IAPTimerOut(void);						// 程序超时累计(放在1ms定时器)
void IAPResetDownloadTimeoutFunction(void); // 重置程序下载超时
void IAPMainTimeoutFunction(void);			// 整体运行超时,超时重启;
void IAPResetIAPMainTimeoutFunction(void);	// 重置整体运行超时
void IAPLoadAPPProgram(void);				// 没有更新的状态下加载用户程序

int crc16_modbus(unsigned char *modbusdata, int length); // CRC 计算
#endif

void IAPWhile(void);
// 在确认接收完数据的地方调用一次这个函数
void IAPAfterRecvData(void);
// 写入数据到环形队列
void IAPPutDatasToLoopList(unsigned char *Res, int len);
////解析Url
int IAPResolveUrl(char *ch);

// 检查是否需要升级
void Check_OTA(void);
void tcp_http_into_data(char *data, int len);
#endif
