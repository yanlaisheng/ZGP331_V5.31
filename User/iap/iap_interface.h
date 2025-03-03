#ifndef IAP_INTERFACE_H_
#define IAP_INTERFACE_H_

#ifndef IAP_INTERFACE_C_//如果没有定义
#define IAP_INTERFACE_Cx_ extern
#else
#define IAP_INTERFACE_Cx_
#endif

#include <stdint.h>
#include "flash_extern.h"
#include <stm32f10x.h>

#if 1 //是否打印日志
#define iap_interface_printf  printf
#else
#define iap_interface_printf(...)
#endif


/*设置Flash*/
#define FLASH_MCU_BASE 0x08000000   //Flash基地址
#define FLASH_MCU_SIZE 512 	 		   //所选MCU的FLASH容量大小(单位为KB)

#if (FLASH_MCU_SIZE<256)
#define FLASH_SECTOR_SIZE 1024 //每页Flash大小(单位为字节)
#else 
#define FLASH_SECTOR_SIZE	2048 //每页Flash大小(单位为字节)
#endif	 	   

#define FLASH_IAP_SIZE 26          //BootLoader所用FLASH大小(KB)

#if (FLASH_SECTOR_SIZE == 1024)
#define FLASH_UPDATE_SIZE 1        //存储更新相关数据所用FLASH大小(KB)
#define FLASH_USERDATA_SIZE 1      //存储用户数据所用FLASH大小(KB)
#else
#define FLASH_UPDATE_SIZE 3        //存储更新相关数据所用FLASH大小(KB)
#define FLASH_USERDATA_SIZE 3      //存储用户数据所用FLASH大小(KB)
#endif

#define FLASH_WRITE_SIZE 4         //每次从缓存中提取多少字节数据写Flash(2,4,8)

//设置为1 即使用外部flash备份用户程序(在flash_extent里面配置flash通信)
//默认支持W25Q80/16/32/64/128
#define FLASH_EXTERN_BACKUP_ENABLE 0

//存储用户数据的FLASH起始地址 (FLASH最后FLASH_USERDATA_SIZE(KB))
#define FLASH_USERDATE_ADDR  (FLASH_MCU_BASE + 1024*(FLASH_MCU_SIZE - FLASH_USERDATA_SIZE))
//存储更新相关数据的FLASH起始地址
#define FLASH_UPDATE_ADDR    (FLASH_MCU_BASE + 1024*(FLASH_MCU_SIZE - FLASH_USERDATA_SIZE - FLASH_UPDATE_SIZE)) 
//用户程序运行地址
#define FLASH_APP1_ADDR  (FLASH_MCU_BASE+1024*FLASH_IAP_SIZE) //在BootLoader程序上面

#if (FLASH_EXTERN_BACKUP_ENABLE==1)//使用外部flash备份程序
	/*按照W25QXX*/
	#define FLASH_EXTERN_W25Qxx 16 //80/16/32/64/128
	
	#if FLASH_EXTERN_W25Qxx==80
//		#define FLASH_EXTERN_SIZE 1024 	 //所选MCU的FLASH容量大小(单位为KB),256个扇区,一个扇区4KB
	#elif FLASH_EXTERN_W25Qxx==16
//		#define FLASH_EXTERN_SIZE 2048 	 //所选MCU的FLASH容量大小(单位为KB),512个扇区,一个扇区4KB
	#elif FLASH_EXTERN_W25Qxx==32
//		#define FLASH_EXTERN_SIZE 4096 	 //所选MCU的FLASH容量大小(单位为KB),1024个扇区,一个扇区4KB
	#elif FLASH_EXTERN_W25Qxx==64
//		#define FLASH_EXTERN_SIZE 8192 	 //所选MCU的FLASH容量大小(单位为KB),2048个扇区,一个扇区4KB
	#elif FLASH_EXTERN_W25Qxx==128
//		#define FLASH_EXTERN_SIZE 16384 	 //所选MCU的FLASH容量大小(单位为KB),4096个扇区,一个扇区4KB
	#endif
	
	#define FLASH_EXTERN_BASE 0x00000000   //Flash基地址
	#define FLASH_EXTERN_SECTOR_SIZE	4096 //每个扇区Flash大小(单位为字节)
	
	#define FLASH_EXTERN_START_SECTOR 0   //备份程序的起始扇区
	#define FLASH_EXTERN_STOP_SECTOR 95    //备份程序的结束扇区(使用前6个块,6*16*4 = 384KB)
	
	//备份用户程序地址(起始地址)
	#define FLASH_APP2_ADDR  (FLASH_EXTERN_BASE+FLASH_EXTERN_START_SECTOR*FLASH_EXTERN_SECTOR_SIZE)
	//备份用户程序地址(结束地址)
	#define FLASH_APP2_ADDR_STOP  (FLASH_EXTERN_BASE+(FLASH_EXTERN_STOP_SECTOR+1)*FLASH_EXTERN_SECTOR_SIZE)
	//用户使用外部flash存储数据从这个地址开始存储(备份用户程序后面的扇区首地址)
	#define FLASH_EXTERN_USERDATE_ADDR  (FLASH_APP2_ADDR_STOP)
	
	//运行用户程序Flash大小(字节)
	#define FLASH_APP1_SIZE  (FLASH_UPDATE_ADDR-FLASH_APP1_ADDR)
	//备份用户程序Flash大小(字节)
	#define FLASH_APP2_SIZE  ((FLASH_EXTERN_STOP_SECTOR-FLASH_EXTERN_START_SECTOR+1)*FLASH_EXTERN_SECTOR_SIZE)
#else
	//备份用户程序地址(存储更新相关数据起始地址和运行用户程序起始地址之间的地址)
	#define FLASH_APP2_ADDR  (FLASH_APP1_ADDR+(FLASH_UPDATE_ADDR-FLASH_APP1_ADDR)/2)
	
	//运行用户程序Flash大小(字节)
	#define FLASH_APP1_SIZE  (FLASH_APP2_ADDR-FLASH_APP1_ADDR)
	//备份用户程序Flash大小(字节)
	#define FLASH_APP2_SIZE  (FLASH_UPDATE_ADDR-FLASH_APP2_ADDR)
#endif

/**********************
使用内部Flash备份程序分配
 _________________
|                 |
|  用户其它数据   | 
|_________________| _存储用户数据起始地址(FLASH_USERDATE_ADDR)
|                 |
|  更新相关数据   |
|_________________| _存储更新相关数据起始地址(FLASH_UPDATE_ADDR)
|                 | 
|                 |
|                 |
|  备份用户程序   |
|                 |
|                 |
|_________________| _备份用户程序起始地址(FLASH_APP2_ADDR)
|                 |
|                 |
|                 |
|  运行用户程序   |
|                 |
|                 |
|_________________| _运行用户程序起始地址(FLASH_APP1_ADDR)
|                 |
|                 |
|  运行BootLoader |
|                 |
|_________________| _Flash基地址(运行BootLoader)



使用外部Flash备份程序分配
 _________________
|                 |
|  用户其它数据   | 
|_________________| _存储用户数据起始地址(FLASH_USERDATE_ADDR)
|                 |
|  更新相关数据   |
|_________________| _存储更新相关数据起始地址(FLASH_UPDATE_ADDR)
|                 | 
|                 |
|                 |
|                 |
|                 |
|                 |
|                 |
|                 |
|                 |
|                 |
|  运行用户程序   |
|                 |
|                 |
|_________________| _运行用户程序起始地址(FLASH_APP1_ADDR)
|                 |
|                 |
|  运行BootLoader |
|                 |
|_________________| _Flash基地址(运行BootLoader)

********************/


#define iap_interface_updata_data_len 1024  //(设置为8的倍数,以适应所有写flash,最大1024就够了!)
#define iap_interface_updata_data_url_offset 20  //从数组的第20个数据开始到最后存储url

//更新相关数数据先写到这个数组里面,然后再一次性写到flash
//0-1记录硬件版本;2-3位记录更新标志;4-5位记录更新状态;  20位及其以后记录url
IAP_INTERFACE_Cx_ char iap_interface_updata_data[iap_interface_updata_data_len];


typedef union iap_interface_union //
{
	uint8_t  char2_temp[2];
	uint8_t  char4_temp[4];
	uint8_t  char8_temp[8];
	uint16_t uint16_t_value;
	uint32_t uint32_t_value;
	uint64_t uint64_t_value;
}iap_interface_union_typedef;
IAP_INTERFACE_Cx_ iap_interface_union_typedef iap_interface_union;

/*复位芯片(根据实际替代内部程序)*/
void iap_interface_reset_mcu(void);
//关闭BootLoader里面使用的中断(根据实际替代内部程序)
void iap_interface_close_all_interrupt(void);
//禁止中断
void iap_interface_disable_irq(void);
//允许中断
void iap_interface_enable_irq(void);
//恢复出厂设置
void iap_interface_reboot(void);
/*连接服务器函数(根据实际替代内部程序)*/
//连接成功返回1
char iap_interface_tcp_connect(void);
/*替换发送数据给服务器函数*/
void iap_interface_tcp_send(char *data,int len);
/*替换延时函数*/
void iap_interface_delay_ms(int value);
/*延时函数(根据实际替代内部程序)*/
void iap_interface_delay_us(int value);
//检测单片机运行区的程序是不是有效的程序(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_check_flash_data(uint32_t Address);
//检测单片机备份区的程序是不是有效的程序(根据实际替代内部程序)
//0: 成功  1:失败
char iap_interface_check_backup_flash_data(uint32_t Address);
/*从flash指定地址读取指定长度的半字数据(根据实际替代内部程序)*/
void iap_interface_flash_read_halfwords(uint32_t Address,uint16_t *data,int len);

//擦除运行区域的flash
//0: 成功  其它:失败
char iap_interface_erase_run_app(void);

//擦除备份区域的flash(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_erase_backup_app(void);

//把运行区的程序拷贝到备份区(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_run_to_backup(void);

//把备份区的程序拷贝到运行区(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_backup_to_run(void);

//把程序文件写入到flash(根据实际替代内部程序)
//addr:要写入的地址; data:要写入的数据
//0: 成功  其它:失败
char iap_interface_write_flash(uint32_t addr, char temp_data[FLASH_WRITE_SIZE]);


//擦除存储更新数据flash(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_erase_updata_data_flash(void);

//从flash中读取存储的更新相关的数据(根据实际替代内部程序)
void iap_interface_read_all_updata_data(void);

//把更新相关的所有数据写入到flash(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_write_all_updata_data(void);

//设置硬件版本
char iap_interface_set_hardware_version(uint16_t data);

//获取硬件版本
//返回值
uint16_t iap_interface_get_hardware_version(void);

//设置更新标志
//0: 成功  其它:失败
char iap_interface_set_update_flage(void);

//获取更新标志
//返回值
uint16_t iap_interface_get_update_flage(void);

//清除更新标志
//0: 成功  其它:失败
char iap_interface_clear_update_flage(void);

//设置更新状态
//0: 成功  其它:失败
char iap_interface_set_update_state(char data);

//获取更新状态
//返回值
char iap_interface_get_update_state(void);

//设置url
//0: 成功  其它:失败
char iap_interface_set_update_url(char *data,int len);

//获取url
void	iap_interface_get_update_url(char *data);

//清除url
//0: 成功  其它:失败
char iap_interface_clear_update_url(void);


/*程序跳转*/
typedef  void (*iapfun)(void);				    //定义一个函数指针类型		   
void iap_interface_load_app(uint32_t appxaddr);			//跳转到APP程序执行

#endif




