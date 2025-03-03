/**
  ******************************************************************************
  * @file    IAP.c
  * @author  feng wu yang
  * @date    2019/10/12
  * @brief   IAP升级
	* @version V1.0.0
						:基础版本

	* @version V1.0.2
	1.修改部分注释.
	2.在升级出错但是没有备份程序时设置IAPStructValue.UpdateFlag=1;//重新更新程序
	3.修改BUG IAPStructValue.UpdateAddressCnt< FLASH_UPDATE_ADDR  改为 IAPStructValue.UpdateAddressCnt< FLASH_APP2_ADDR

	* @version V2.0.0
	1.框架整体分离,更加便于移植到各个单片机
	2.解决http数据解析bug

	* @version V2.2.0
	1.调整分段下载模式,请求完数据再去写flash, 写完flash再去请求数据

	* @version V2.2.1
	1.解决http数据解析bug

	* @version V2.2.2
	1.解决日志打印冲突bug

	* @version V2.2.3
	1.解决http数据解析bug

	* @version V3.0.0
	1.支持使用外部flash备份程序
	2.擦除完运行区再执行连接服务器(原先是连接服务器以后再执行擦除(延时可能导致TCP断开),再发送获取文件指令)
	* @version V3.0.1
	1.分段发送指令加了条件: IAPStructValue.range_stop>IAPStructValue.range_start

  ***********************************BootLoader程序里面使用*******************************************
	一,在进while前初始化
	IAPInit();

	二,把以下程序放入 1Ms累加定时器
	IAPTimerOut();//程序超时累计(放在1ms定时器)

	三,把以下程序放到主循环里面
	IAPWhile();

	四,把接收的数据传递给以下函数
	//Res-数据地址; len-数据长度
	IAPPutDatasToLoopList(unsigned char* Res,int len);

  ***********************************用户程序里面使用*******************************************
	一,初始化时可以调用以下程序
	//打印固件信息
	printf("硬件版本:%d\r\n",iap_interface_get_hardware_version());
	printf("设备型号:%s\r\n",DeviceModel);
	printf("固件版本:%s\r\n",FirmwareVersion);
	IAPInfoPathInit();//解析info.txt文件的地址

	二,把以下函数放到感觉用户程序运行没有问题的地方执行下
	IAPUpdateDispose();//处理更新状态变量

  */
#define IAP_C_
#include "IAP.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cString.h"
// #include "LoopList.h"
#include "iap_interface.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"

extern uchar S_M35;			   // s步骤：M35流程
extern uchar SendDataReturnNo; // 发送数据返回序号(串口过来的数据发到每个连接)
extern void Com2_printf(char *fmt, ...);
extern delay_ms(vu16 nCount);
extern void UART2_Output_Datas(uint8_t S_Out[], uint16_t Counter);
extern void UART3_Output_Datas(uint8_t S_Out[], uint16_t Counter);

// BootLoader程序修改
#if (IAPProgramSelect == IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram) // BootLoader  or  BootLoader+User
// PCB硬件版本(0-65535)
#define HardwareVersion 0
/*硬件版本号使用说明:(PCB硬件版本号并不是固件程序版本号,请不要混淆,PCB硬件版本号作用如下:)
假设做的第一批PCB已经投放市场,然后第二批PCB修改了部分电路(不同的电路用户程序不一样).
用户可以修改上面的硬件版为 1,然后下载BootLoader到单片机(第二批PCB的硬件版本就为1)
在用户程序里面可以使用IAPGetHardwareVersion();获取BootLoader设置的这个版本号
if(IAPGetHardwareVersion()==0)//第一批PCB硬件版本
{
	执行相应的程序
}
else if(IAPGetHardwareVersion()==1)//第二批PCB硬件版本
{
	执行相应的程序
}*/

// 产品型号(为每一个产品设置一个型号)(要和用户程序里面的型号保持一致)
char DeviceModel[50] = "ZGP331";

// 固件程序下载地址
// char IAPUrlUserBinPath[iap_interface_updata_data_len-iap_interface_updata_data_url_offset]="http://mnif.cn/ota/hardware/STM32Air724BK/user_crc.bin";
char IAPUrlUserBinPath[iap_interface_updata_data_len - iap_interface_updata_data_url_offset] = "http://ota.sanli.cn/ota/hardware/ZGP331/ZGP331.bin";
// 如果Flash没有存储固件程序下载地址,则此URL作为默认固件程序下载地址

#endif

// 用户程序修改
#if (IAPProgramSelect == IAPUserProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)
#define iap_data_offset 200 // 常量存储在偏移1024字节倍数的位置,根据自己的程序设置偏移倍数
// 产品型号(为每一个产品设置一个型号),用户根据自己的产品型号修改
// 此型号会生成到bin文件里面,升级过程中会判断bin文件里面的型号一不一致.
char DeviceModel[50] __attribute__((at(FLASH_APP1_ADDR + 1024 * iap_data_offset))) = "ZGP331";

// size:AAAAAAAAAAAAAAA: 存储固件文件大小,由编辑bin文件软件自动识别设置,用户不可修改!
//: 后面的0.0.0 代表固件版本,用户根据自己的修改,默认最长20个字符(不包含20)(可修改宏定义 FirmwareVersionLen 增加个数)
char iap_firmware_size[20 + FirmwareVersionLen + 1] __attribute__((at(FLASH_APP1_ADDR + 1024 * (iap_data_offset + 1)))) = "size:AAAAAAAAAAAAAAA:0.0.2";
// 当前固件程序的版本号
char *FirmwareVersion = &iap_firmware_size[21];

// 获取云端固件信息文件的地址
char IAPUrlUserInfoPath[iap_interface_updata_data_len - iap_interface_updata_data_url_offset] = "http://ota.sanli.cn/ota/hardware/ZGP331/info.txt";
/*info.txt文件信息如下:(JSON格式)
{"version":"0.0.1","size":15990,"url":"http://mnif.cn/ota/hardware/CH579MBK/user_crc.bin","info":"1.解决了部分BUG
2.优化了部分程序"}

version:0.0.1 解释:云端固件程序版本
url:http://mnif.cn/ota/hardware/STM32Air724L/user_crc.bin 解释:云端固件程序下载地址
info:1.解决了部分BUG 2.优化了部分程序    解释:本次固件更新的内容信息(用于APP控制升级时,APP的提示信息)
*/
#endif

IAPStruct IAPStructValue;

int tcp_http_index = 0;
uint8_t tcp_buff[1024];
uint8_t tcp_buff_copy[1024];
int tcp_len = 0;

/**
 * @brief  初始化和处理更新状态
 * @warn
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
#if (IAPProgramSelect == IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram) // BootLoader  or  BootLoader+User

void IAPInit(void)
{
	// 打印更新的Flash配置信息
	Com2_printf("\r\n运行区地址: 0x%x\r\n", FLASH_APP1_ADDR);
	Com2_printf("运行区容量: 0x%x\r\n", FLASH_APP1_SIZE);

	Com2_printf("\r\n备份区地址: 0x%08x\r\n", FLASH_APP2_ADDR);
	Com2_printf("备份区容量: 0x%x\r\n", FLASH_APP2_SIZE);

	Com2_printf("\r\n存储更新相关数据地址: 0x%08x", FLASH_UPDATE_ADDR);
	Com2_printf("\r\n用户其它数据存储地址: 0x%08x\r\n", FLASH_USERDATE_ADDR);
#if (FLASH_EXTERN_BACKUP_ENABLE == 1) // 使用外部flash备份程序
	Com2_printf("\r\n用户其它数据存储地址(外部Flash): 0x%08x\r\n\r\n", FLASH_EXTERN_USERDATE_ADDR);
#endif

	// 清零所有变量;
	memset(IAPStructValue.buff, 0, sizeof(IAPStructValue.buff));
	memset(IAPStructValue.IP, 0, sizeof(IAPStructValue.IP));	 // 服务器IP/域名
	memset(IAPStructValue.Path, 0, sizeof(IAPStructValue.Path)); // Path(文件路径)

	IAPStructValue.SSLEN = 0;

#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
	IAPStructValue.range_start = 0;
	IAPStructValue.range_stop = 0;
	IAPStructValue.content_length_flag = 0; // 获取到分段数据尝试标志
	IAPStructValue.content_length = 0;
	IAPStructValue.section_download_len = 0;  // 接到到分段数据个数
	IAPStructValue.section_download_flag = 0; // 接收完分段数据,可以发送分段请求指令
#endif

	IAPStructValue.erase_run_app_flag = 0;
	IAPStructValue.SendGetFileOrderFlag = 0;
	IAPStructValue.CheckDataCnt = 0; //
	IAPStructValue.CheckValueErrFlag = 0;
	IAPStructValue.UpdateDownloadCnt = 0; // 尝试下载次数

	IAPStructValue.ReadDataEndFlag = 0;	  // 确实接收完了程序
	IAPStructValue.LoopListPutEnable = 0; // 控制是不是可以向环形队列写入数据
	IAPStructValue.LoopListWriteErr = 0;  // 环形队列写错误
	IAPStructValue.ConnectWebFlag = 0;	  // 连接web服务器标志
	IAPStructValue.UpdateFlag = 0;		  // 更新标志
	IAPStructValue.FlashWriteErrFlag = 0; // 写入Flash出错
	IAPStructValue.UpdateStatusValue = 0; // 获取更新的状态
	IAPStructValue.UpdateAddressCnt = 0;  // 用于写Flash的时候地址累加
	IAPStructValue.DownloadTimeout = 0;	  // 程序下载超时
	IAPStructValue.MainTimeout = 0;		  // 整体运行超时累加变量
	IAPStructValue.FileSizeSave = 0;	  // 固件文件大小-存储的
	IAPStructValue.FileSizeNow = 0;		  // 固件文件大小-当前接收的

	rbCreate(&IAPStructValue.LoopList, IAPStructValue.LoopListBuff, IAPLoopListBuffLen + 1); // 创建环形队列管理数组

	// 恢复出厂设置
	iap_interface_reboot();
	/*检测是不是设置了新的硬件版本*/
	IAPStructValue.HardwareVersionCopy = iap_interface_get_hardware_version(); // 获取硬件版本
	if (IAPStructValue.HardwareVersionCopy != HardwareVersion)				   // 硬件版本不一致
	{
		IAPStructValue.HardwareVersionCopy = HardwareVersion;
		iap_interface_set_hardware_version(IAPStructValue.HardwareVersionCopy); // 设置硬件版本
	}

	/*获取存储的更新程序的url地址*/
	iap_interface_get_update_url(IAPStructValue.buff); // 获取
	if (memcmp(IAPStructValue.buff, "http", 4) != 0)   // 没有更新的地址
	{
		iap_interface_set_update_url(IAPUrlUserBinPath, strlen(IAPUrlUserBinPath)); // 设置默认URL
		iap_interface_get_update_url(IAPStructValue.buff);							// 获取
	}
	Com2_printf("下载路径:%s\r\n", IAPStructValue.buff); // URL

	/*解析url地址*/
	Com2_printf("解析URL\r\n");
	if (IAPResolveUrl(IAPStructValue.buff) != 0) // 解析URL
	{
		iap_interface_set_update_state(UpdateStatus_UrlResolveErr); // 解析URL错误
	}
	Com2_printf("SSL连接 :%d\r\n", IAPStructValue.SSLEN);	   // SSL
	Com2_printf("IP地址  :%s\r\n", IAPStructValue.IP);	   // IP地址解析后存储在 IAPStructValue.IP
	Com2_printf("端口号  :%d\r\n", IAPStructValue.Port);	   // 端口号解析后存储在 IAPStructValue.Port
	Com2_printf("文件路径:%s\r\n\r\n", IAPStructValue.Path); // 文件访问路径解析后存储在 IAPStructValue.Path

#if (FLASH_EXTERN_BACKUP_ENABLE == 1)			// 使用外部flash备份程序
	if (iap_interface_flash_extern_init() != 0) // 初始化检测外部flash
	{
		iap_interface_set_update_state(UpdateStatus_FlashExternInitErr); // 初始化外部flash失败
		Com2_printf("\r\niap_interface_flash_extern_init ERR\r\n");
		iap_interface_delay_ms(100);
		iap_interface_reset_mcu(); // 重启
	}
#endif

	/*************获取更新标志位************/
	IAPStructValue.UpdateFlag = iap_interface_get_update_flage(); // 获取更新标志位

	if (IAPStructValue.UpdateFlag == 1) // 有更新标志位
	{
		iap_interface_clear_update_flage(); // 清除更新标志位
		Com2_printf("\r\n检测到更新标志\r\n");

		/**备份运行区程序**/
		if (!iap_interface_check_flash_data(FLASH_APP1_ADDR)) // 运行区有程序
		{
			Com2_printf("\r\n开始备份运行区程序\r\n");
			if (iap_interface_erase_backup_app() == 0) // 擦除备份区
			{
				Com2_printf("\r\n擦除备份区OK\r\n");
			}
			else
			{
				if (iap_interface_erase_backup_app() != 0) // 擦除备份程序
				{
					iap_interface_set_update_state(UpdateStatus_FlashEraseBackErr); // 擦除备份区失败
					Com2_printf("\r\n擦除备份区ERR\r\n");
					iap_interface_delay_ms(100); // 等待发送完成
					iap_interface_reset_mcu();	 // 重启
				}
			}

			// 把运行区的程序拷贝到备份区
			if (iap_interface_run_to_backup() != 0)
			{
				iap_interface_set_update_state(UpdateStatus_FlashBackErr); // 运行区的程序拷贝到备份区失败
				Com2_printf("\r\n备份程序失败\r\n");
				iap_interface_delay_ms(100); // 等待发送完成
				iap_interface_reset_mcu();	 // 重启
			}
			else
			{
				Com2_printf("\r\n运行区的程序拷贝到备份区OK\r\n");
				iap_interface_delay_ms(100); // 等待发送完成
			}

			/*要操作运行区的flash,先把更新标记设置为ERR*/
			iap_interface_set_update_state(UpdateStatus_Err);
		}
		else
		{
			Com2_printf("\r\n运行区无程序\r\n");
		}
	}
	else
	{
		IAPStructValue.UpdateStatusValue = iap_interface_get_update_state(); // 获取更新状态

		if ((IAPStructValue.UpdateStatusValue & 0xF0) == 0xE0 || IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage) // 已经操作了运行区,执行有错误
		{
			Com2_printf("\r\n程序更新错误:0x%02x\r\n", IAPStructValue.UpdateStatusValue);
			if (!iap_interface_check_backup_flash_data(FLASH_APP2_ADDR)) // 有备份的程序
			{
				Com2_printf("\r\n开始回滚程序\r\n");

				Com2_printf("\r\n擦除运行区.....\r\n");
				if (iap_interface_erase_run_app() != 0) // 擦除运行程序的地址
				{
					iap_interface_set_update_state(UpdateStatus_FlashEraseUserErr); // 擦除运行区失败
					Com2_printf("\r\n擦除运行区失败\r\n");
					iap_interface_delay_ms(100); // 等待发送完成
					iap_interface_reset_mcu();	 // 重启
				}

				// 把备份区的程序拷贝到运行区
				Com2_printf("\r\n拷贝备份区程序到运行区.....\r\n");
				if (iap_interface_backup_to_run() != 0)
				{
					iap_interface_set_update_state(UpdateStatus_FlashRollBackErr); // 备份区拷贝到运行区失败
					Com2_printf("\r\n备份区拷贝到运行区失败\r\n");
					iap_interface_delay_ms(100); // 等待发送完成
					iap_interface_reset_mcu();	 // 重启
				}
				else
				{
					Com2_printf("\r\n备份区拷贝到运行区OK\r\n");
					iap_interface_delay_ms(100); // 等待发送完成
				}
			}
			else
			{
				Com2_printf("\r\n无备份程序!正在让设备尝试重新升级!\r\n");
				IAPStructValue.UpdateFlag = 1; // 设置更新标志,让设备重新执行升级
			}
		}
		else
		{
			if (IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOk) // 刚升级完程序
			{
				iap_interface_set_update_state(UpdateStatus_WriteAppOkFlage); // 写入0xFE标志
			}
			Com2_printf("更新状态:0x%02x\r\n", IAPStructValue.UpdateStatusValue);
		}
	}
	iap_interface_delay_ms(1000); // 等待打印完
}

#endif

/**
 * @brief  用户程序
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
#if (IAPProgramSelect == IAPUserProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)

void IAPInfoPathInit(void)
{
	Com2_printf("解析云端固件信息的地址URL\r\n");
	IAPResolveUrl(IAPUrlUserInfoPath);					   // 解析URL
	Com2_printf("SSL连接 :%d\r\n", IAPStructValue.SSLEN);	   // SSL
	Com2_printf("IP地址  :%s\r\n", IAPStructValue.IP);	   // IP
	Com2_printf("端口号  :%d\r\n", IAPStructValue.Port);	   // Port
	Com2_printf("文件路径:%s\r\n\r\n", IAPStructValue.Path); // Path
}

/**
 * @brief  获取,清除,打印更新状态
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
uint16_t IAPUpdateDispose(void)
{
	uint16_t status;
	IAPStructValue.UpdateStatusValue = iap_interface_get_update_state(); // 获取更新状态变量
	status = IAPStructValue.UpdateStatusValue;
	if (IAPStructValue.UpdateStatusValue != 0) // 升级过程序
	{
		iap_interface_set_update_state(UpdateStatus_None);					  // 清零升级状态
		if (IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage) // 是运行的新程序
		{
			Com2_printf("运行的新程序\r\n");
		}
		else
		{
			Com2_printf("更新状态:0x%02x\r\n", IAPStructValue.UpdateStatusValue);
		}
		IAPStructValue.UpdateStatusValue = 0;
	}
	return status;
}

#endif

/**
 * @brief  把接收到的程序文件写入Flash
 * @warn
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
#if (IAPProgramSelect == IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram) // BootLoader  or  BootLoader+User
void IAPWriteData(void)
{
	int len;

#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
								//		if(IAPStructValue.section_download_flag==1 && !IAPStructValue.ReadDataEndFlag && IAPStructValue.LoopListPutEnable && rbCanWrite(&IAPStructValue.LoopList)>=IAP_RANGE)//缓存有空间存储分段数据
								//		{
								//			IAPStructValue.section_download_flag=0;
								//			IAPStructValue.range_start = IAPStructValue.range_start+IAP_RANGE;
								//			IAPStructValue.range_stop = IAPStructValue.range_start+IAP_RANGE-1;
								//			len = sprintf(IAPStructValue.buff,"GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n",IAPStructValue.Path,IAPStructValue.IP,IAPStructValue.range_start,IAPStructValue.range_stop);
								//			iap_interface_tcp_send(IAPStructValue.buff,len);
								//		}
#endif

#if (IAP_SECTION_DOWNLOAD == 1)																   // 分段下载
	if (IAPStructValue.section_download_flag == 1 && rbCanRead(&IAPStructValue.LoopList) >= 2) // 接收到更新程序
#else
	if (rbCanRead(&IAPStructValue.LoopList) >= 2) // 接收到更新程序
#endif
	{
		IAPResetDownloadTimeoutFunction(); // 重置程序下载超时
		// 读取数据
		iap_interface_disable_irq(); // 禁止中断 __disable_irq();
		rbRead(&IAPStructValue.LoopList, &IAPStructValue.LoopListReadDate[0], 2);
		iap_interface_enable_irq();									 // 允许中断 __enable_irq();
		IAPStructValue.FileSizeNow = IAPStructValue.FileSizeNow + 2; // 累加文件大小

		IAPStructValue.CheckDataCnt = IAPStructValue.CheckDataCnt + 2; // 接收的数据个数

		if (IAPStructValue.CheckDataCnt >= (IAPCheckDataBuffLen + 2)) // 到达校验个数
		{
			IAPStructValue.LoopListU16Date = IAPStructValue.LoopListReadDate[0];
			IAPStructValue.LoopListU16Date = IAPStructValue.LoopListU16Date << 8;
			IAPStructValue.LoopListU16Date = IAPStructValue.LoopListU16Date | IAPStructValue.LoopListReadDate[1]; // 拼接校验数据(高位在前低位在后)
			//			iap_interface_delay_ms(10);//写入flash加点延时再读取(部分替代芯片需要)
			iap_interface_flash_read_halfwords(IAPStructValue.UpdateAddressCnt - IAPCheckDataBuffLen, (uint16_t *)IAPStructValue.CheckDataBuff, IAPCheckDataBuffLen / 2); // 提取前CheckDataBuffLen个数据
			if (IAPStructValue.ProgramEfficient == 0)
			{
				if (memcmp(IAPStructValue.CheckDataBuff, DeviceModel, sizeof(DeviceModel)) == 0) // 判断型号
				{
					IAPStructValue.ProgramEfficient = 1; // 一致
					Com2_printf("\r\n型号一致:%s\r\n", DeviceModel);
				}
			}

			/*尝试提取bin文件里面的固件大小和固件版本*/
			if (IAPStructValue.FileSizeSave == 0) // 没有获取到文件大小
			{
				if (memcmp(IAPStructValue.CheckDataBuff, "size:", 5) == 0) // 尝试获取
				{
					if (IAPStructValue.CheckDataBuff[20] == ':')
					{
						// 固件大小
						IAPStructValue.FileSizeSave = atoi(&IAPStructValue.CheckDataBuff[5]);
						Com2_printf("\r\n固件大小:%d\r\n", IAPStructValue.FileSizeSave);
						// 固件版本
						memset(IAPStructValue.FileFirmwareVersion, 0, FirmwareVersionLen);
						memcpy(IAPStructValue.FileFirmwareVersion, &IAPStructValue.CheckDataBuff[21], FirmwareVersionLen);
						Com2_printf("\r\n固件版本:%s\r\n", IAPStructValue.FileFirmwareVersion);
						//						if(memcmp(FirmwareVersion,IAPStructValue.FileFirmwareVersion,FirmwareVersionLen) !=0)
						//						{
						//							Com2_printf("\r\n新版本\r\n");
						//						}
					}
				}
			}

			// 接收到足够的数据
			if (IAPStructValue.FileSizeSave != 0 && IAPStructValue.FileSizeNow == IAPStructValue.FileSizeSave)
			{
				IAPStructValue.ReadDataEndFlag = 1;	  // 标记接收完数据
				IAPStructValue.LoopListPutEnable = 0; // 停止向环形队列写入数据
				Com2_printf("\r\n接收到足够的数据:%d\r\n", IAPStructValue.FileSizeNow);
			}

			IAPStructValue.CheckValue = crc16_modbus((unsigned char *)IAPStructValue.CheckDataBuff, IAPCheckDataBuffLen); // 计算校验值

			if (IAPStructValue.CheckValue != IAPStructValue.LoopListU16Date) // 校验值不相等
			{
				IAPStructValue.CheckValueErrFlag = 1; // 标记校验错误
				Com2_printf("\r\nErr=%02x:%02x\r\n", IAPStructValue.CheckValue, IAPStructValue.LoopListU16Date);

				IAPStructValue.Len = 0;
				for (IAPStructValue.Len = 0; IAPStructValue.Len < IAPCheckDataBuffLen; IAPStructValue.Len++)
				{
					Com2_printf("%02x ", IAPStructValue.CheckDataBuff[IAPStructValue.Len]);
				}
			}
			else
			{
				Com2_printf("\r\nOK=%02x:%02x\r\n", IAPStructValue.CheckValue, IAPStructValue.LoopListU16Date);
			}

			IAPStructValue.CheckDataCnt = 0; // 重新开始计数,计算下一包
		}
		else
		{
			if (IAPStructValue.BuffDateCount == 0)
			{
				IAPStructValue.BuffDateCount = 1;
				IAPStructValue.BuffDate[0] = IAPStructValue.LoopListReadDate[0];
				IAPStructValue.BuffDate[1] = IAPStructValue.LoopListReadDate[1];

#if (FLASH_WRITE_SIZE == 2)
				IAPStructValue.BuffDateCount = 0;
				IAPStructValue.BuffDateFlage = 1;
#endif
			}
			else if (IAPStructValue.BuffDateCount == 1)
			{
				IAPStructValue.BuffDateCount = 2;
#if (FLASH_WRITE_SIZE == 4 || FLASH_WRITE_SIZE == 8)
				IAPStructValue.BuffDate[2] = IAPStructValue.LoopListReadDate[0];
				IAPStructValue.BuffDate[3] = IAPStructValue.LoopListReadDate[1];
#endif

#if (FLASH_WRITE_SIZE == 4)
				IAPStructValue.BuffDateCount = 0;
				IAPStructValue.BuffDateFlage = 1;
#endif
			}
			else if (IAPStructValue.BuffDateCount == 2)
			{
				IAPStructValue.BuffDateCount = 3;
#if (FLASH_WRITE_SIZE == 8)
				IAPStructValue.BuffDate[4] = IAPStructValue.LoopListReadDate[0];
				IAPStructValue.BuffDate[5] = IAPStructValue.LoopListReadDate[1];
#endif
			}
			else if (IAPStructValue.BuffDateCount == 3)
			{
				IAPStructValue.BuffDateCount = 4;
#if (FLASH_WRITE_SIZE == 8)
				IAPStructValue.BuffDate[6] = IAPStructValue.LoopListReadDate[0];
				IAPStructValue.BuffDate[7] = IAPStructValue.LoopListReadDate[1];
#endif

#if (FLASH_WRITE_SIZE == 8)
				IAPStructValue.BuffDateCount = 0;
				IAPStructValue.BuffDateFlage = 1;
#endif
			}

#if (FLASH_EXTERN_BACKUP_ENABLE != 1)																	// 不使用外部flash备份程序
			if (IAPStructValue.UpdateAddressCnt < FLASH_APP2_ADDR && IAPStructValue.BuffDateFlage == 1) // 不能超过备份区
#else
			if (IAPStructValue.UpdateAddressCnt < FLASH_UPDATE_ADDR && IAPStructValue.BuffDateFlage == 1) // 不能超过备份区
#endif
			{
				IAPStructValue.BuffDateFlage = 0;
				if (!IAPStructValue.FlashWriteErrFlag) // 写Flash没有错误
				{
					if (iap_interface_write_flash(IAPStructValue.UpdateAddressCnt, &IAPStructValue.BuffDate[0]) != 0)
					{
						IAPStructValue.FlashWriteErrFlag = 1; // 写Flash错误
					}
				}
				IAPStructValue.UpdateAddressCnt += FLASH_WRITE_SIZE; // 地址增加
			}
		}

#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
		IAPStructValue.section_download_len = IAPStructValue.section_download_len + 2;
		if (IAPStructValue.section_download_len == IAPStructValue.content_length)
		{
			IAPStructValue.section_download_len = 0;
			IAPStructValue.section_download_flag = 0;
			IAPStructValue.range_start = IAPStructValue.range_start + IAP_RANGE;
			IAPStructValue.range_stop = IAPStructValue.range_start + IAP_RANGE - 1;

			if (IAPStructValue.FileSizeSave > 0)
			{
				// 分段限制不能越界,有些服务器越界会出问题
				if (IAPStructValue.range_stop >= (IAPStructValue.FileSizeSave - 1))
				{
					IAPStructValue.range_stop = (IAPStructValue.FileSizeSave - 1);
				}
			}
#if (HttpContentRangeEnable == 1)
			else if (IAPStructValue.ContentRangeLen > 0)
			{
				// 分段限制不能越界,有些服务器越界会出问题
				if (IAPStructValue.range_stop >= (IAPStructValue.ContentRangeLen - 1))
				{
					IAPStructValue.range_stop = (IAPStructValue.ContentRangeLen - 1);
				}
			}
#endif
			// 没有接收完数据, 同时 range_stop > range_start
			if (IAPStructValue.ReadDataEndFlag == 0 && IAPStructValue.range_stop > IAPStructValue.range_start)
			{
				len = sprintf(IAPStructValue.buff, "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n", IAPStructValue.Path, IAPStructValue.IP, IAPStructValue.range_start, IAPStructValue.range_stop);
				iap_interface_tcp_send(IAPStructValue.buff, len);
			}
		}
#endif

#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
								//			IAPStructValue.section_download_len= IAPStructValue.section_download_len+2;//接到到分段数据个数
								//			if(IAPStructValue.section_download_len == IAP_RANGE)
								//			{
								//				IAPStructValue.section_download_flag=1;
								//				IAPStructValue.HttpDataStartFlage = 0;
								//				IAPStructValue.section_download_len=0;
								//				Com2_printf("\r\nsection_download_len\r\n");
								//			}
#endif
	}
	else // 环形队列里面没有数据了.并不证明接收完了数据,可能写入环形队列慢,读的快
	{
		if (IAPStructValue.ReadDataEndFlag) // 接收出现空闲/接收完成
		{
			IAPStructValue.ReadDataEndFlag = 0;	  // 清零接收完更新程序标志
			IAPStructValue.LoopListPutEnable = 0; // 停止向环形队列写入数据

			if (IAPStructValue.FlashWriteErrFlag == 1) // Flash写错误
			{
				IAPStructValue.FlashWriteErrFlag = 0;
				iap_interface_set_update_state(UpdateStatus_FlashWriteErr); // Flash写错误
			}
			else if (IAPStructValue.LoopListWriteErr) // 环形队列写错误
			{
				IAPStructValue.LoopListWriteErr = 0;
				iap_interface_set_update_state(UpdateStatus_LoopListWriteErr); // 写缓存错误
			}
			else if (IAPStructValue.CheckValueErrFlag) // 校验错误
			{
				IAPStructValue.CheckValueErrFlag = 0;
				iap_interface_set_update_state(UpdateStatus_CheckErr); // 数据校验错误
			}
			// 没接收到数据
			else if (IAPStructValue.FileSizeNow == 0) // 接收的数据个数不对
			{
				iap_interface_set_update_state(UpdateStatus_ReceiveDataLost); // 数据接收不完整
			}
			// 接收到数据,但是和bin文件里面记录的不一样
			else if (IAPStructValue.FileSizeSave != 0 && IAPStructValue.FileSizeNow != IAPStructValue.FileSizeSave) // 接收的数据个数不对
			{
				IAPStructValue.FileSizeNow = 0;
				IAPStructValue.FileSizeSave = 0;
				iap_interface_set_update_state(UpdateStatus_ReceiveDataLost); // 数据接收不完整
			}
			else if (IAPStructValue.ProgramEfficient == 0) // 文件无效
			{
				iap_interface_set_update_state(UpdateStatus_ProgramInvalid); // 写入文件无效标记
				/*如果不写下面的重启,则程序会尝试下载,默认3次*/
				Com2_printf("\r\n程序设备型号不一致,正在重启执行回滚!\r\n");
				iap_interface_delay_ms(10);
				iap_interface_reset_mcu(); // 重启
			}
			else
			{
				if (!iap_interface_check_flash_data(FLASH_APP1_ADDR)) // 检测某些位置的Flash的高位地址是不是0x08		//RAM的高位地址是不是0x20
				{
					iap_interface_set_update_state(UpdateStatus_WriteAppOk); // 写入0x01标志
					Com2_printf("\r\n程序下载成功,写入0x01标志,正在重启\r\n");
					iap_interface_delay_ms(500);
					iap_interface_reset_mcu(); // 重启
				}
				else
				{
					iap_interface_set_update_state(UpdateStatus_DataAddressError); // 数据错误
				}
			}

			IAPStructValue.SendGetFileOrderFlag = 0; // 清除发送过获取文件命令标志
			IAPResetDownloadTimeoutFunction();

			IAPStructValue.ConnectWebFlag = 0;				   // 设置重新连接Web服务器
			IAPStructValue.UpdateAddressCnt = FLASH_APP1_ADDR; // 重置用于写Flash的时候地址累加
			IAPStructValue.CheckDataCnt = 0;				   // 清零校验数据个数累加变量
			iap_interface_delay_ms(10);

			// 超过下载次数还么有下载成功,则重启.
			IAPStructValue.UpdateDownloadCnt = IAPStructValue.UpdateDownloadCnt + 1; // 下载次数加一
			if (IAPStructValue.UpdateDownloadCnt >= IAPUpdateDownloadNumber)
			{
				Com2_printf("\r\n尝试次数到达设定次数\r\n");
				iap_interface_delay_ms(500);
				iap_interface_reset_mcu(); // 重启
			}
			else
			{
				Com2_printf("\r\n程序下载出错(错误码:0x%02x),正在重新下载\r\n", iap_interface_get_update_state());
			}
		}
	}
}

/**
 * @brief  发送指令获取程序文件
 * @warn
 * @param  None
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
void IAPGetProgramFile(void)
{
	int len = 0;
	if (!IAPStructValue.SendGetFileOrderFlag && IAPStructValue.UpdateFlag == 1) // 没有发送过获取文件命令
	{
		if (IAPStructValue.erase_run_app_flag == 0)
		{
			Com2_printf("\r\n擦除运行区.........\r\n");
			if (iap_interface_erase_run_app() != 0) // 擦除运行区地址
			{
				iap_interface_set_update_state(UpdateStatus_FlashEraseUserErr); // Flash 擦除失败
				Com2_printf("\r\n擦除运行区失败(in Get File)\r\n");
				iap_interface_delay_ms(100); // 等待发送完成
				iap_interface_reset_mcu();	 // 重启
			}
			IAPStructValue.erase_run_app_flag = 1;
		}

		if (IAPStructValue.ConnectWebFlag) // 连接上Web服务器
		{
			rbCreate(&IAPStructValue.LoopList, IAPStructValue.LoopListBuff, IAPLoopListBuffLen + 1); // 初始化缓存

// 发送请求程序文件指令
#if (IAP_SECTION_DOWNLOAD == 1) // 使能分段下载
			len = sprintf(IAPStructValue.buff, "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n", IAPStructValue.Path, IAPStructValue.IP, 0, IAP_RANGE - 1);
#else
			len = sprintf(IAPStructValue.buff, "GET %s HTTP/1.1\r\nHost: %s\r\nContent-Type: text/html\r\n\r\n", IAPStructValue.Path, IAPStructValue.IP);
#endif
			if (IAPStructValue.SSLEN == 0) // http
			{
				Com2_printf("http get file\r\n");
				iap_interface_tcp_send(IAPStructValue.buff, len);
			}
			else // https
			{
				Com2_printf("https get file\r\n");
				iap_interface_tcp_send(IAPStructValue.buff, len);
			}

// 清零各种标志
#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
			IAPStructValue.range_start = 0;
			IAPStructValue.range_stop = 0;
			IAPStructValue.content_length = 0;
			IAPStructValue.content_length_flag = 0; // 获取到分段数据尝试标志
			IAPStructValue.section_download_len = 0;
			IAPStructValue.section_download_flag = 0;
#if (HttpContentRangeEnable == 1)
			IAPStructValue.ContentRangeLen = 0;
			IAPStructValue.ContentRangeLenCopy = 0;
			IAPStructValue.ContentRangeCnt1 = 0;
			IAPStructValue.ContentRangeCnt1Copy = 0;
			IAPStructValue.ContentRangeCnt2 = 0;
			IAPStructValue.ContentRangeCnt2Copy = 0;
			IAPStructValue.ContentRangeCnt3 = 0;
			IAPStructValue.ContentRangeCnt3Copy = 0;
			IAPStructValue.ContentRangeSign1 = 0;
			IAPStructValue.ContentRangeSign2 = 0;
#endif
#endif

			IAPStructValue.erase_run_app_flag = 0;
			IAPStructValue.BuffDateCount = 0;
			IAPStructValue.BuffDateFlage = 0;
			IAPStructValue.UpdateAddressCnt = FLASH_APP1_ADDR; // 重置用于写Flash的时候地址累加
			IAPStructValue.LoopListPutEnable = 1;			   // 允许向环形队列中写入数据
			IAPStructValue.ProgramEfficient = 0;			   // 清零程序无效变量
			IAPStructValue.FileSizeNow = 0;					   // 清除累加文件大小变量
			IAPStructValue.HttpHeadOK = 0;					   // 接收到http 头
			IAPStructValue.HttpHeadEndOK = 0;				   // 接收完http头
			IAPStructValue.HttpHeadCnt = 0;					   // 用于判断http头的累加变量
			IAPStructValue.HttpDataStartFlage = 0;			   // 清零获取http消息体标志
			IAPStructValue.SendGetFileOrderFlag = 1;		   // 标记发送获取文件指令
		}
	}
}

/**
* @brief  判断到开始接收消息体,设置 IAPStructValue.HttpDataStartFlage=1
* @warn
* @param  Res:web服务器返回的数据
* @param  None
* @param  None
* @retval None
* @example
HTTP/1.1 206 Partial Content
Server: openresty/1.15.8.2
Date: Fri, 10 Sep 2021 05:16:55 GMT
Content-Type: application/octet-stream
Content-Length: 500
Last-Modified: Fri, 10 Sep 2021 04:26:21 GMT
Connection: keep-alive
ETag: "613ade6d-4182"
Content-Range: bytes 500-999/16770
**/
void IAPHttpHead(char Res)
{
#if (IAP_SECTION_DOWNLOAD == 1) // 分段下载
	if (IAPStructValue.HttpHeadOK == 0 && IAPStructValue.HttpDataStartFlage == 0 && IAPStructValue.LoopListPutEnable == 1 && IAPStructValue.section_download_flag == 0)
	{
		if (Res == 'H' && IAPStructValue.HttpHeadCnt == 0)
		{
			IAPStructValue.HttpHeadCnt++;
		}
		else if (Res == 'T' && IAPStructValue.HttpHeadCnt == 1)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'T' && IAPStructValue.HttpHeadCnt == 2)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'P' && IAPStructValue.HttpHeadCnt == 3)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '/' && IAPStructValue.HttpHeadCnt == 4)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '1' && IAPStructValue.HttpHeadCnt == 5)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '.' && IAPStructValue.HttpHeadCnt == 6)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '1' && IAPStructValue.HttpHeadCnt == 7)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == ' ' && IAPStructValue.HttpHeadCnt == 8)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '2' && IAPStructValue.HttpHeadCnt == 9)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '0' && IAPStructValue.HttpHeadCnt == 10)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '6' && IAPStructValue.HttpHeadCnt == 11)
		// V2.2.3有些服务器不会返回Partial参数
		//			IAPStructValue.HttpHeadCnt++;
		//			else if(Res==' ' && IAPStructValue.HttpHeadCnt==12)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='P' && IAPStructValue.HttpHeadCnt==13)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='a' && IAPStructValue.HttpHeadCnt==14)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='r' && IAPStructValue.HttpHeadCnt==15)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='t' && IAPStructValue.HttpHeadCnt==16)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='i' && IAPStructValue.HttpHeadCnt==17)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='a' && IAPStructValue.HttpHeadCnt==18)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='l' && IAPStructValue.HttpHeadCnt==19)IAPStructValue.HttpHeadCnt++;
		//			else if(Res==' ' && IAPStructValue.HttpHeadCnt==20)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='C' && IAPStructValue.HttpHeadCnt==21)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='o' && IAPStructValue.HttpHeadCnt==22)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='n' && IAPStructValue.HttpHeadCnt==23)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='t' && IAPStructValue.HttpHeadCnt==24)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='e' && IAPStructValue.HttpHeadCnt==25)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='n' && IAPStructValue.HttpHeadCnt==26)IAPStructValue.HttpHeadCnt++;
		//			else if(Res=='t' && IAPStructValue.HttpHeadCnt==27)
		{
			IAPStructValue.HttpHeadOK = 1;
			IAPStructValue.HttpHeadCnt = 0;
		}
		else
		{
			IAPStructValue.HttpHeadCnt = 0;
		}
	}

	/*获取分段数据长度*/ // Content-Length: 404
	if (IAPStructValue.HttpHeadOK == 1 && IAPStructValue.content_length_flag == 0)
	{
		if (Res == 'C' && IAPStructValue.HttpHeadCnt == 0)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'o' && IAPStructValue.HttpHeadCnt == 1)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'n' && IAPStructValue.HttpHeadCnt == 2)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 't' && IAPStructValue.HttpHeadCnt == 3)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'e' && IAPStructValue.HttpHeadCnt == 4)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'n' && IAPStructValue.HttpHeadCnt == 5)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 't' && IAPStructValue.HttpHeadCnt == 6)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '-' && IAPStructValue.HttpHeadCnt == 7)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'L' && IAPStructValue.HttpHeadCnt == 8)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'e' && IAPStructValue.HttpHeadCnt == 9)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'n' && IAPStructValue.HttpHeadCnt == 10)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'g' && IAPStructValue.HttpHeadCnt == 11)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 't' && IAPStructValue.HttpHeadCnt == 12)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'h' && IAPStructValue.HttpHeadCnt == 13)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == ':' && IAPStructValue.HttpHeadCnt == 14)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == ' ' && IAPStructValue.HttpHeadCnt == 15)
			IAPStructValue.HttpHeadCnt++;
		else if (IAPStructValue.HttpHeadCnt == 16 && Res >= '0' && Res <= '9')
		{
			IAPStructValue.content_length = Res - '0';
			IAPStructValue.HttpHeadCnt++;
		}
		else if (IAPStructValue.HttpHeadCnt >= 17 && IAPStructValue.HttpHeadCnt <= 19)
		{
			if (Res == 0x0D) // 获取完成
			{
				IAPStructValue.content_length_flag = 1;
				IAPStructValue.HttpHeadCnt = 0;
			}
			else
			{
				if (Res >= '0' && Res <= '9')
				{
					IAPStructValue.content_length = IAPStructValue.content_length * 10;
					IAPStructValue.content_length = IAPStructValue.content_length + (Res - 0x30);
				}
			}
		}
		else
		{
			IAPStructValue.HttpHeadCnt = 0;
		}
	}

#if (HttpContentRangeEnable == 1)
	/*获取实际http文件大小 Content-Range: bytes 1500100-19000/10000*/
	if (IAPStructValue.HttpHeadOK == 1)
	{
		if (IAPStructValue.ContentRangeSign2 == 1)
		{
			if (IAPStructValue.ContentRangeCnt3 == 0 && Res >= '0' && Res <= '9')
			{
				IAPStructValue.ContentRangeLenCopy = Res - '0';
				IAPStructValue.ContentRangeCnt3Copy = 1;
				IAPStructValue.ContentRangeCnt3++;
			}
			else if (IAPStructValue.ContentRangeCnt3 >= 1 && IAPStructValue.ContentRangeCnt3 <= IAPStructValue.ContentRangeCnt3Copy)
			{
				if (Res >= '0' && Res <= '9')
				{
					IAPStructValue.ContentRangeLenCopy = IAPStructValue.ContentRangeLenCopy * 10;
					IAPStructValue.ContentRangeLenCopy = IAPStructValue.ContentRangeLenCopy + (Res - 0x30);

					IAPStructValue.ContentRangeCnt3++;
					IAPStructValue.ContentRangeCnt3Copy++;
				}
				else
				{
					if (Res != 0x0D)
					{
						IAPStructValue.ContentRangeLenCopy = 0;
					}
					else
					{
						IAPStructValue.ContentRangeLen = IAPStructValue.ContentRangeLenCopy;
					}
					IAPStructValue.ContentRangeSign2 = 0;
					IAPStructValue.ContentRangeCnt3 = 0;
					IAPStructValue.ContentRangeCnt3Copy = 0;
				}
			}
			else
			{
				IAPStructValue.ContentRangeSign2 = 0;
				IAPStructValue.ContentRangeCnt3 = 0;
				IAPStructValue.ContentRangeCnt3Copy = 0;
			}
		}

		if (IAPStructValue.ContentRangeSign1 == 1)
		{
			if (IAPStructValue.ContentRangeCnt2 == 0 && Res >= '0' && Res <= '9')
			{
				IAPStructValue.ContentRangeCnt2Copy = 1;
				IAPStructValue.ContentRangeCnt2++;
			}
			else if (IAPStructValue.ContentRangeCnt2 >= 1 && IAPStructValue.ContentRangeCnt2 <= IAPStructValue.ContentRangeCnt2Copy)
			{ // 找到 /
				if (Res >= '0' && Res <= '9')
				{
					IAPStructValue.ContentRangeCnt2++;
					IAPStructValue.ContentRangeCnt2Copy++;
				}
				else
				{
					if (Res == '/')
					{
						IAPStructValue.ContentRangeSign2 = 1;
						IAPStructValue.ContentRangeCnt2 = 0;
						IAPStructValue.ContentRangeSign1 = 0;
						IAPStructValue.ContentRangeCnt3 = 0;
						IAPStructValue.ContentRangeLen = 0;
					}
					else
					{
						IAPStructValue.ContentRangeCnt2 = 0;
						IAPStructValue.ContentRangeSign1 = 0;
					}
				}
			}
			else
			{
				IAPStructValue.ContentRangeCnt2 = 0;
				IAPStructValue.ContentRangeSign1 = 0;
			}
		}

		if (Res == 'C' && IAPStructValue.ContentRangeCnt1 == 0)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'o' && IAPStructValue.ContentRangeCnt1 == 1)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'n' && IAPStructValue.ContentRangeCnt1 == 2)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 't' && IAPStructValue.ContentRangeCnt1 == 3)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'e' && IAPStructValue.ContentRangeCnt1 == 4)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'n' && IAPStructValue.ContentRangeCnt1 == 5)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 't' && IAPStructValue.ContentRangeCnt1 == 6)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == '-' && IAPStructValue.ContentRangeCnt1 == 7)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'R' && IAPStructValue.ContentRangeCnt1 == 8)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'a' && IAPStructValue.ContentRangeCnt1 == 9)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'n' && IAPStructValue.ContentRangeCnt1 == 10)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'g' && IAPStructValue.ContentRangeCnt1 == 11)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'e' && IAPStructValue.ContentRangeCnt1 == 12)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == ':' && IAPStructValue.ContentRangeCnt1 == 13)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == ' ' && IAPStructValue.ContentRangeCnt1 == 14)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'b' && IAPStructValue.ContentRangeCnt1 == 15)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'y' && IAPStructValue.ContentRangeCnt1 == 16)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 't' && IAPStructValue.ContentRangeCnt1 == 17)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 'e' && IAPStructValue.ContentRangeCnt1 == 18)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == 's' && IAPStructValue.ContentRangeCnt1 == 19)
			IAPStructValue.ContentRangeCnt1++;
		else if (Res == ' ' && IAPStructValue.ContentRangeCnt1 == 20)
			IAPStructValue.ContentRangeCnt1++;
		else if (IAPStructValue.ContentRangeCnt1 == 21 && Res >= '0' && Res <= '9')
		{
			IAPStructValue.ContentRangeCnt1Copy = IAPStructValue.ContentRangeCnt1 + 1;
			IAPStructValue.ContentRangeCnt1++;
		}
		// 找到 -
		else if (IAPStructValue.ContentRangeCnt1 >= 22 && IAPStructValue.ContentRangeCnt1 <= IAPStructValue.ContentRangeCnt1Copy)
		{
			if (Res >= '0' && Res <= '9')
			{
				IAPStructValue.ContentRangeCnt1++;
				IAPStructValue.ContentRangeCnt1Copy++;
			}
			else
			{
				if (Res == '-')
				{
					IAPStructValue.ContentRangeSign1 = 1;
					IAPStructValue.ContentRangeSign2 = 0;
					IAPStructValue.ContentRangeCnt2 = 0;
					IAPStructValue.ContentRangeCnt1 = 0;
				}
				else
				{
					IAPStructValue.ContentRangeCnt1 = 0;
				}
			}
		}
		else
		{
			if (Res == 'C' && IAPStructValue.ContentRangeCnt1 == 1)
				IAPStructValue.ContentRangeCnt1 = 1;
			else
				IAPStructValue.ContentRangeCnt1 = 0;
		}
	}
#endif

	if (IAPStructValue.content_length_flag == 1 && IAPStructValue.HttpHeadOK && !IAPStructValue.HttpHeadEndOK)
	{ // 0D 0A 0D 0A
		if (Res == 0x0D && IAPStructValue.HttpHeadCnt == 0)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 0x0A && IAPStructValue.HttpHeadCnt == 1)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 0x0D && IAPStructValue.HttpHeadCnt == 2)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 0x0A && IAPStructValue.HttpHeadCnt == 3)
		{
			IAPStructValue.content_length_flag = 0;
			IAPStructValue.HttpHeadOK = 0;
			IAPStructValue.HttpHeadEndOK = 1;
		}
		else
			IAPStructValue.HttpHeadCnt = 0;
	}

	if (IAPStructValue.HttpHeadEndOK == 1) // http数据的head已经过去,后面的是真实数据
	{
		IAPStructValue.HttpHeadEndOK = 0;
		IAPStructValue.HttpHeadCnt = 0;
		IAPStructValue.HttpDataStartFlage = 1; // 标记后面是真实数据
	}

#else
	// HTTP/1.1 200 OK
	if (!IAPStructValue.HttpHeadOK && !IAPStructValue.HttpDataStartFlage && IAPStructValue.LoopListPutEnable)
	{
		if (Res == 'H' && IAPStructValue.HttpHeadCnt == 0)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'T' && IAPStructValue.HttpHeadCnt == 1)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'T' && IAPStructValue.HttpHeadCnt == 2)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'P' && IAPStructValue.HttpHeadCnt == 3)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '/' && IAPStructValue.HttpHeadCnt == 4)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '1' && IAPStructValue.HttpHeadCnt == 5)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '.' && IAPStructValue.HttpHeadCnt == 6)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '1' && IAPStructValue.HttpHeadCnt == 7)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == ' ' && IAPStructValue.HttpHeadCnt == 8)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '2' && IAPStructValue.HttpHeadCnt == 9)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '0' && IAPStructValue.HttpHeadCnt == 10)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == '0' && IAPStructValue.HttpHeadCnt == 11)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == ' ' && IAPStructValue.HttpHeadCnt == 12)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'O' && IAPStructValue.HttpHeadCnt == 13)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 'K' && IAPStructValue.HttpHeadCnt == 14)
		{
			IAPStructValue.HttpHeadOK = 1;
			IAPStructValue.HttpHeadCnt = 0;
		}
		else
		{
			IAPStructValue.HttpHeadCnt = 0;
		}
	}

	if (IAPStructValue.HttpHeadOK && !IAPStructValue.HttpHeadEndOK)
	{ // 0D 0A 0D 0A
		if (Res == 0x0D && IAPStructValue.HttpHeadCnt == 0)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 0x0A && IAPStructValue.HttpHeadCnt == 1)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 0x0D && IAPStructValue.HttpHeadCnt == 2)
			IAPStructValue.HttpHeadCnt++;
		else if (Res == 0x0A && IAPStructValue.HttpHeadCnt == 3)
		{
			IAPStructValue.HttpHeadOK = 0;
			IAPStructValue.HttpHeadEndOK = 1;
		}
		else
			IAPStructValue.HttpHeadCnt = 0;
	}

	if (IAPStructValue.HttpHeadEndOK == 1) // http数据的head已经过去,后面的是真实数据
	{
		IAPStructValue.HttpHeadEndOK = 0;
		IAPStructValue.HttpHeadCnt = 0;
		IAPStructValue.HttpDataStartFlage = 1; // 标记后面是真实数据
	}
#endif
}

/**
 * @brief  写入数据到环形队列
 * @warn
 * @param  Res:数据地址
 * @param  len:数据个数
 * @param  None
 * @retval None
 * @example
 **/
void IAPPutDatasToLoopList(unsigned char *Res, int len)
{
	int i = 0;
	if (IAPStructValue.LoopListPutEnable == 1)
	{
		for (i = 0; i < len; i++)
		{
			IAPPutDataToLoopList(Res[i]);
		}
	}
}

/**
 * @brief  写入数据到环形队列
 * @warn
 * @param  Res:数据
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
void IAPPutDataToLoopList(char Res)
{
	if (IAPStructValue.HttpDataStartFlage == 1 && IAPStructValue.LoopListPutEnable && (IAPStructValue.LoopListPutEnable ^ IAPStructValue.LoopListWriteErr))
	{
		if (PutData(&IAPStructValue.LoopList, &Res, 1) != 1) // 写一个字节到环形队列
		{
			IAPStructValue.LoopListWriteErr = 1; // 环形队列写错误
												 //			Com2_printf("环形队列错误!*************************************\r\n");
		}
		else
		{
#if (IAP_SECTION_DOWNLOAD == 1)															   // 分段下载
			IAPStructValue.section_download_len = IAPStructValue.section_download_len + 1; // 接到到分段数据个数
			if (IAPStructValue.section_download_len == IAPStructValue.content_length)
			{
				IAPStructValue.section_download_flag = 1; // 接收完分段数据
				IAPStructValue.HttpDataStartFlage = 0;
				IAPStructValue.section_download_len = 0;
				//				Com2_printf("\r\nsection_download_len\r\n");
			}
#endif
		}
	}
	IAPHttpHead(Res); // 解析http数据
}

/**
 * @brief  程序超时累计(放在1ms定时器)
 * @warn
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
void IAPTimerOut(void)
{
	/*发送完获取文件指令以后,持续检查是否下载超时*/
	if (IAPStructValue.SendGetFileOrderFlag)
	{
		IAPStructValue.DownloadTimeout++;

		if (IAPStructValue.DownloadTimeout > IAPReadDataIdleTimer)
		{
			IAPStructValue.DownloadTimeout = 0;
			IAPStructValue.ReadDataEndFlag = 1;	  // 认为接收完程序文件
			IAPStructValue.LoopListPutEnable = 0; // 禁止写数据到环形队列
		}
	}

	/*整体运行超时累加变量*/
	IAPStructValue.MainTimeout++;
}

/**
 * @brief IAPWhile(放到主while循环里面)
 * @param None
 * @param None
 * @param
 * @retval None
 * @example
 **/
void IAPWhile(void)
{
	IAPLoadAPPProgram();	  // 尝试加载用户程序
	IAPMainTimeoutFunction(); // 整体运行超时,超时重启;
	IAPWriteData();			  // 把接收到的程序文件写入Flash
	IAPGetProgramFile();	  // 发送get指令获取程序文件

	if (IAPStructValue.UpdateFlag == 1 && IAPStructValue.ConnectWebFlag != 1)
	{
		if (iap_interface_tcp_connect() == 1)
		{
			IAPStructValue.ConnectWebFlag = 1;
		}
	}
}

/**
 * @brief 在确认接收完数据的地方调用一次这个函数
 * @param None
 * @param None
 * @param
 * @retval None
 * @example
 **/
void IAPAfterRecvData(void)
{
	IAPStructValue.ReadDataEndFlag = 1;	  // 标记接收完成
	IAPStructValue.LoopListPutEnable = 0; // 禁止写数据到环形队列
}

/**
 * @brief calculate CRC
 * @param *modbusdata: Source data address
 * @param length: data length
 * @param
 * @retval CRC16 Value
 * @example
 **/
int crc16_modbus(unsigned char *modbusdata, int length)
{
	int i, j;
	int crc = 0xffff; // 0xffff or 0
	for (i = 0; i < length; i++)
	{
		crc ^= modbusdata[i];
		for (j = 0; j < 8; j++)
		{
			if ((crc & 0x01) == 1)
			{
				crc = (crc >> 1) ^ 0xa001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return crc;
}

/**
 * @brief  重置程序下载超时
 * @warn
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
void IAPResetDownloadTimeoutFunction(void)
{
	IAPStructValue.DownloadTimeout = 0;
}

/**
 * @brief  整体运行超时,超时重启;
 * @warn
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
void IAPMainTimeoutFunction(void)
{
	if (IAPStructValue.MainTimeout > IAPMainTimeoutCompare)
	{
		IAPStructValue.MainTimeout = 0;
		iap_interface_set_update_state(UpdateStatus_MainTimeout); // 整个程序运行超时
		iap_interface_reset_mcu();								  // 重启
	}
}

/**
 * @brief  重置整体运行超时
 * @warn
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
void IAPResetIAPMainTimeoutFunction(void)
{
	IAPStructValue.MainTimeout = 0;
}

/**
 * @brief  尝试加载用户程序
 * @warn
 * @param
 * @param  None
 * @param  None
 * @retval None
 * @example
 **/
void IAPLoadAPPProgram(void)
{
	if (IAPStructValue.UpdateFlag != 1 && IAPStructValue.LoopListPutEnable != 1) // 没有更新标志  且   没有操作升级
	{
		if (!iap_interface_check_flash_data(FLASH_APP1_ADDR)) // 检测某些位置的Flash前8位数据
		{
			iap_interface_close_all_interrupt();
			iap_interface_load_app(FLASH_APP1_ADDR); // 运行用户程序
		}
		else //
		{
			//			iap_interface_set_update_flage();//设置更新标志
			//			//清空flash记录的更新地址,重新加载会使用默认地址
			//			iap_interface_clear_update_url();
			//			iap_interface_delay_ms(10);
			//			//重启
			//			iap_interface_reset_mcu();
		}
	}
}

#endif

// 解析Url;返回值:0 success
// http://mnif.cn/ota/hardware/STM32ESP8266PP/
// https://mnif.cn/ota/hardware/STM32ESP8266PP/
// http://mnif.cn:80/ota/hardware/STM32ESP8266PP/
// https://mnif.cn:443/ota/hardware/STM32ESP8266PP/
// http://ota.sanli.cn/ota/hardware/ZGP331/
int IAPResolveUrl(char *ch)
{
	if (ch != NULL && (strlen(ch) > 5))
	{
		// http or https
		if (memcmp(ch, "https", 5) == 0)
		{
			IAPStructValue.SSLEN = 1;
			IAPStructValue.Len = 5;
		}
		else if (memcmp(ch, "http", 4) == 0)
		{
			IAPStructValue.SSLEN = 0;
			IAPStructValue.Len = 4;
		}
		else
		{
			return -2;
		}

		// IP
		IAPStructValue.Str = StrBetwString(ch + IAPStructValue.Len, "://", ":");
		if (IAPStructValue.Str != NULL)
		{ // 带端口号
			memset(IAPStructValue.IP, 0, sizeof(IAPStructValue.IP));
			memcpy(IAPStructValue.IP, IAPStructValue.Str, strlen(IAPStructValue.Str));
			IAPStructValue.Len = IAPStructValue.Len + 3 + strlen(IAPStructValue.IP); // 14/15
			cStringRestore();

			// Port
			IAPStructValue.Str = StrBetwString(ch + IAPStructValue.Len, ":", "/");
			if (IAPStructValue.Str != NULL)
			{
				if (strlen(IAPStructValue.Str) < 6 && atoi(IAPStructValue.Str) != 0)
				{
					IAPStructValue.Port = atoi(IAPStructValue.Str);
					IAPStructValue.Len = IAPStructValue.Len + 1 + strlen(IAPStructValue.Str); // 18/19

					cStringRestore();
					// Path
					memcpy(IAPStructValue.Path, ch + IAPStructValue.Len, sizeof(IAPStructValue.Path) - IAPStructValue.Len);
				}
				else
				{
					return -4;
				}
			}
			else
			{
				cStringRestore();
				return -5;
			}
		}
		else
		{ // 不带端口号
			cStringRestore();
			if (IAPStructValue.SSLEN)
			{
				IAPStructValue.Port = 443;
			}
			else
			{
				IAPStructValue.Port = 80;
			}

			// IP
			IAPStructValue.Str = StrBetwString(ch + IAPStructValue.Len, "://", "/");
			if (IAPStructValue.Str != NULL)
			{
				memset(IAPStructValue.IP, 0, sizeof(IAPStructValue.IP));
				memcpy(IAPStructValue.IP, IAPStructValue.Str, strlen(IAPStructValue.Str));
				IAPStructValue.Len = IAPStructValue.Len + 3 + strlen(IAPStructValue.IP); // 14/15
			}
			else
			{
				return -6;
			}
			cStringRestore();
			// Path
			memcpy(IAPStructValue.Path, ch + IAPStructValue.Len, sizeof(IAPStructValue.Path) - IAPStructValue.Len);
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

// 检查是否需要升级
void Check_OTA(void)
{
	// test123
//	int len = 0;
//	int length = 0;
////	char *str;
//	if (Pw_CheckUpgrade == 1)
//	{
//		if (CGD0_LinkConnectOK[3] == TRUE && GprsPar[LinkTCPUDP0Base + 3] == 1) // 只有第4路通道连上才能升级，只能通过第4路通道升级，并且必须为TCP连接
//		{
//			// 组合 get 指令
//			len = sprintf((char *)tcp_buff, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", IAPStructValue.Path, IAPStructValue.IP);			
//			length = sprintf((char *)tcp_buff_copy, "AT+CIPSEND=%d,%d\r\n", 3, len); // 只能从第4路通道发送数据
//			UART3_Output_Datas(tcp_buff_copy, length);								 // 发送数据命令
//			delay_ms(20);
//			UART2_Output_Datas(tcp_buff_copy, length);								 // 发送数据命令
//			delay_ms(20);

//			UART3_Output_Datas(tcp_buff, len); // 发送数据给服务器
//			Ota_upgrading_flag = 1;			   // 置正在升级标志
//			Pw_CheckUpgrade = 0;			   // 清检测升级标志
////			delay_ms(20);
////			UART2_Output_Datas(tcp_buff, len); // 发送数据给服务器
//		}
//	}

	if (Pw_CheckUpgrade == 1)
	{
		Pw_CheckUpgrade=0;
		iap_interface_set_update_flage();//设置更新标志
		iap_interface_delay_ms(10);
		//重启
		iap_interface_reset_mcu();		
	}
}

/**
 * @brief   接收到模组数据以后,把数据传递给此函数
 * @param   None
 * @retval  None
 * @warning None
 * @example
 **/
void tcp_http_into_data(char *data, int len)
{
	char *str;
	// 获取版本号
	str = StrBetwString(data, "\"version\":\"", "\",\"");
	if (str != NULL)
	{
		len = strlen(str); // 获取版本号长度
		if (len < FirmwareVersionLen)
		{ // 版本号长度正确
			Com2_printf("\r\n获取到版本号\r\n");
			if (memcmp(str, FirmwareVersion, len) != 0)
			{ // 版本号不一样
				Com2_printf("有新版本:%s\r\n", str);
				cStringRestore();
				// 获取下载地址 URL
				str = StrBetwString(data, "url\":\"", "\",\"");
				// URL 缓存到数组
				memset(IAPStructValue.buff, 0, sizeof(IAPStructValue.buff));
				sprintf(IAPStructValue.buff, "%s", str);
				// 解析 URL
				if (IAPResolveUrl(IAPStructValue.buff) == 0)
				{
					// 存储 url 到 flash
					iap_interface_set_update_url(IAPStructValue.buff, strlen(IAPStructValue.buff));
					cStringRestore();
					iap_interface_set_update_flage(); // 设置更新标志
					iap_interface_reset_mcu();		  // 重启
				}
			}
			else
			{
				Com2_printf("\r\n版本已是最新,无需更新!\r\n");
			}
		}
	}
}
