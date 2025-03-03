#define IAP_INTERFACE_C_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash_extern.h"
#include "iap_interface.h"
#include "iap.h"

/*********包含头文件**************/
//#include "network_module.h"
//#include "usart.h"
//#include "delay.h"
//#include "iwdg.h"
/***********************/
extern void delay_ms(vu16 nCount);
extern void Delay_US(vu16 nCount);
iap_interface_union_typedef iap_interface_union;//共用体


/*复位芯片(根据实际替代内部程序)*/
void iap_interface_reset_mcu(void)
{
	__disable_fault_irq();   
	NVIC_SystemReset();//重启
}

/*延时函数(根据实际替代内部程序)*/
void iap_interface_delay_ms(int value)
{
	delay_ms(value);
}

/*延时函数(根据实际替代内部程序)*/
void iap_interface_delay_us(int value)
{
	Delay_US(value);
}

#if (IAPProgramSelect==IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)//BootLoader  or  BootLoader+User
/*喂狗(根据实际替代内部程序)*/
void iap_interface_wdg_feed(void)
{
	//擦除运行区; 擦除备份区; 备份程序; 耗时操作需要喂狗
	IWDG_Feed();//喂狗,根据自己的程序修改
	
	//重置整体运行超时
	IAPResetIAPMainTimeoutFunction();
}

//关闭BootLoader里面使用的中断(根据实际替代内部程序)
void iap_interface_close_all_interrupt(void)
{
	SysTick->CTRL=0;
	
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
	/*关闭串口中断*/
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART1, USART_IT_TC, DISABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
	
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);
	
  USART_Cmd(USART1, DISABLE);
	USART_Cmd(USART2, DISABLE);
}

//禁止中断
void iap_interface_disable_irq(void)
{
	__disable_irq();
}
//允许中断
void iap_interface_enable_irq(void)
{
	__enable_irq();
}


//恢复出厂设置
void iap_interface_reboot(void)
{
	//长按一个按键不松开,然后给设备重新上电
	if(0/*按键按下--用户自行实现*/)//检测到按键按下之后执行
	{
		/*控制指示灯闪耀--用户自行实现*/
		if(0/*检测到按键松开--用户自行实现*/)
		{
			iap_interface_set_update_flage();//设置更新标志
			//清空flash记录的更新地址,重新加载会使用默认地址
			iap_interface_clear_update_url();
			iap_interface_delay_ms(10);
			//重启
			iap_interface_reset_mcu();
		}
	}
}

/*连接服务器函数(根据实际替代内部程序)*/
//连接成功返回1
char iap_interface_tcp_connect(void)
{
	if(network_module_connect_tcp(IAPStructValue.IP, IAPStructValue.Port, IAPStructValue.SSLEN) == 1)
	{
		return 1;
	}
	return 0;
}


char tcp_send_buff[1024];
/*发送数据给服务器函数(根据实际替代内部程序)*/
void iap_interface_tcp_send(char *data,int len)
{
	int length=0;

	length = sprintf((char*)tcp_send_buff, "AT+CIPSEND=%d,%d\r\n", 0, len);
	
	usart2_send((char*)tcp_send_buff, length);//发送数据命令
	iap_interface_delay_ms(20);
	//发送数据给服务器
	usart2_send((char*)data, len);//发送数据给服务器
}


//检测单片机运行区的程序是不是有效的程序(根据实际替代内部程序)
//0: 成功  1:失败
char iap_interface_check_flash_data(uint32_t Address)
{
	if(((*(vu32*)(Address+4))&0xFF000000)==0x08000000)//Flash的高位地址是不是0x08						
	{
		if(((*(vu32*)Address)&0x2FFE0000)==0x20000000)//RAM的高位地址是不是0x20		
		{
			return 0;
		}
	}
	return 1;
}


//检测单片机备份区的程序是不是有效的程序(根据实际替代内部程序)
//0: 成功  1:失败
char iap_interface_check_backup_flash_data(uint32_t Address)
{
	#if (FLASH_EXTERN_BACKUP_ENABLE==1)//使用外部flash备份程序
		uint32_t  read_start_addr;
		uint32_t data1,data2;
	
		read_start_addr = FLASH_APP2_ADDR;
		
		//WriteDisable
		flash_extern_cs_reset;//CS
		iap_interface_flash_extern_send_byte(W25Qxx_WriteDisable);
		flash_extern_cs_set;//CS
		//WriteEnable
		flash_extern_cs_reset;//CS
		iap_interface_flash_extern_send_byte(W25Qxx_WriteEnable);  
		flash_extern_cs_set;//CS
		//wait busy
		flash_extern_cs_reset;//CS
		iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg);//发送读取状态寄存器命令    
		while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
		flash_extern_cs_set;//CS
		
		/*读取外部flash数据*/
		flash_extern_cs_reset;//CS
		iap_interface_flash_extern_send_byte(W25Qxx_ReadData);         	//发送读取命令   
		iap_interface_flash_extern_send_byte((uint8_t)((read_start_addr)>>16));  	//发送24bit地址    
		iap_interface_flash_extern_send_byte((uint8_t)((read_start_addr)>>8));   
		iap_interface_flash_extern_send_byte((uint8_t)read_start_addr); 
		
		iap_interface_union.char4_temp[0] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		iap_interface_union.char4_temp[1] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		iap_interface_union.char4_temp[2] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		iap_interface_union.char4_temp[3] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		
		data1 = iap_interface_union.uint32_t_value;//flash数据的0-3字节
		
		iap_interface_union.char4_temp[0] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		iap_interface_union.char4_temp[1] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		iap_interface_union.char4_temp[2] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		iap_interface_union.char4_temp[3] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
		
		data2 = iap_interface_union.uint32_t_value;//flash数据的4-7字节
		
		flash_extern_cs_set;//CS
		
		//wait busy
		flash_extern_cs_reset;//CS
		iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
		while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
		flash_extern_cs_set;//CS
		
		iap_interface_printf("iap_interface_check_backup_flash_data data1:%08x\r\n",data1);
		iap_interface_printf("iap_interface_check_backup_flash_data data2:%08x\r\n",data2);
		
		/*检测*/
		if((data2&0xFF000000)==0x08000000)//Flash的高位地址是不是0x08						
		{
			if((data1&0x2FFE0000)==0x20000000)//RAM的高位地址是不是0x20		
			{
				return 0;
			}
		}
		return 1;
	#else
		return iap_interface_check_flash_data(Address);
	#endif
}



/*从单片机flash指定地址读取指定长度的半字数据(根据实际替代内部程序)*/
void iap_interface_flash_read_halfwords(uint32_t Address,uint16_t *data,int len)
{
	int i;
	uint16_t uint16_t_value;
	
	for(i=0;i<len;i++)
	{
		uint16_t_value = *(uint16_t*)Address;//读取
		data[i] = uint16_t_value;
		Address+=2;//偏移2个字节.	
	}
}


//擦除运行区域的flash(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_erase_run_app(void)
{
	uint32_t i=0;
	uint32_t secpos;	   //扇区地址
	uint32_t size;
	char FlashStatus = 0;
	
	size =  FLASH_APP1_SIZE;//flash空间大小
	size =  size/FLASH_SECTOR_SIZE;//有多少页
	
	secpos = FLASH_APP1_ADDR-FLASH_MCU_BASE;//实际地址
	secpos = secpos/FLASH_SECTOR_SIZE;//扇区地址
	
	{
		FLASH_Unlock();
		for(i=0;i<size;i++)
		{
			iap_interface_wdg_feed();//喂狗			
			iap_interface_delay_ms(5);
			FlashStatus = FLASH_ErasePage(secpos*FLASH_SECTOR_SIZE + FLASH_MCU_BASE);//擦除这个扇区
			if(FlashStatus != FLASH_COMPLETE) break;			
			secpos++;
		}
		FLASH_Lock();//上锁
	}
	
	if(FlashStatus != FLASH_COMPLETE) 
	{
		iap_interface_printf("iap_interface_erase_run_app ERR:1\r\n");
		return 1;
	}

	return 0;
}


//擦除备份区域的flash(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_erase_backup_app(void)
{
	#if (FLASH_EXTERN_BACKUP_ENABLE==1)//使用外部flash备份程序
		uint32_t i=0;
		uint32_t j=0;
		uint8_t ret=0;
		
		i=FLASH_APP2_ADDR;
		while(i<FLASH_APP2_ADDR_STOP)
		{
			iap_interface_wdg_feed();//喂狗
			
			//WriteDisable
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_WriteDisable);
			flash_extern_cs_set;//CS
			//WriteEnable
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_WriteEnable);  
			flash_extern_cs_set;//CS
			//wait busy
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
			while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
			flash_extern_cs_set;//CS
			
			
			/*先看下需不需要擦除*/
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadData);         	//发送读取命令   
			iap_interface_flash_extern_send_byte((uint8_t)((i)>>16));  	//发送24bit地址    
			iap_interface_flash_extern_send_byte((uint8_t)((i)>>8));   
			iap_interface_flash_extern_send_byte((uint8_t)i);   
			for(j=0;j<FLASH_EXTERN_SECTOR_SIZE;j++)
			{
				ret = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
				if(ret!=0xff){
					iap_interface_printf("iap_interface_erase_backup_app msg:!=0xff\r\n");
					break;
				}
			}
			flash_extern_cs_set;//CS
			
			if(ret!=0xff)//需要擦除
			{
				flash_extern_cs_reset;//CS
				iap_interface_flash_extern_send_byte(W25Qxx_SectorErase);      	//发送扇区擦除指令 
				iap_interface_flash_extern_send_byte((uint8_t)((i)>>16));  	//发送24bit地址    
				iap_interface_flash_extern_send_byte((uint8_t)((i)>>8));   
				iap_interface_flash_extern_send_byte((uint8_t)i);
				flash_extern_cs_set;//CS

				//wait busy
				flash_extern_cs_reset;//CS
				iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
				while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
				flash_extern_cs_set;//CS
			}
			
			/*是否正确擦除*/
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadData);         	//发送读取命令   
			iap_interface_flash_extern_send_byte((uint8_t)((i)>>16));  	//发送24bit地址    
			iap_interface_flash_extern_send_byte((uint8_t)((i)>>8));   
			iap_interface_flash_extern_send_byte((uint8_t)i);   
			for(j=0;j<FLASH_EXTERN_SECTOR_SIZE;j++)
			{
				ret = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
				if(ret!=0xff){
					break;
				}
			}
			flash_extern_cs_set;//CS
			
			//wait busy
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
			while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
			flash_extern_cs_set;//CS
			
			i = i + FLASH_EXTERN_SECTOR_SIZE;		

			if(ret!=0xff)
			{
				iap_interface_printf("iap_interface_erase_backup_app ERR:1\r\n");
				return 1;
			}	
		}
	#else
		uint32_t i=0;
		uint32_t secpos;	   //扇区地址
		uint32_t size;
		char FlashStatus = 0;
		
		size =  FLASH_APP2_SIZE;//flash空间大小(KB)
		size =  size/FLASH_SECTOR_SIZE;//有多少页
		
		secpos = FLASH_APP2_ADDR-FLASH_MCU_BASE;//实际地址
		secpos = secpos/FLASH_SECTOR_SIZE;//扇区地址
		
		{
			FLASH_Unlock();
			for(i=0;i<size;i++)
			{
				iap_interface_wdg_feed();//喂狗
				iap_interface_delay_ms(5);
				FlashStatus = FLASH_ErasePage(secpos*FLASH_SECTOR_SIZE + FLASH_MCU_BASE);//擦除这个扇区
				if(FlashStatus != FLASH_COMPLETE) break;
				secpos++;
			}
			FLASH_Lock();//上锁
		}
		
		if(FlashStatus != FLASH_COMPLETE) 
		{
			iap_interface_printf("iap_interface_erase_backup_app ERR:1\r\n");
			return 1;
		}
	#endif
	return 0;
}



//把运行区的程序拷贝到备份区(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_run_to_backup(void)
{
	#if (FLASH_EXTERN_BACKUP_ENABLE==1)//使用外部flash备份程序
	
		int i,j;
		uint8_t ret=0;
		uint8_t value;
		uint32_t  data;
		uint32_t  size;
		uint32_t  read_start_addr;
		uint32_t  copy_start_addr;
		char copy_size = 4;//每次拷贝字节数
		//避免拷贝越界,运行区和备份区按照最小的拷贝
		size = FLASH_APP1_SIZE < FLASH_APP2_SIZE?FLASH_APP1_SIZE:FLASH_APP2_SIZE;
		size = size/copy_size;//每次拷贝4字节
		
		read_start_addr = FLASH_APP1_ADDR;//运行用户程序的首地址
		copy_start_addr = FLASH_APP2_ADDR;
		
		FLASH_Unlock();
		for(i=0;i<size;i++)
		{
			iap_interface_wdg_feed();//喂狗
			
			data = *(uint32_t*)read_start_addr;//读取flash数据
			
			//WriteDisable
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_WriteDisable);
			flash_extern_cs_set;//CS
			//WriteEnable
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_WriteEnable);  
			flash_extern_cs_set;//CS
			//wait busy
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
			while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
			flash_extern_cs_set;//CS
			
			//WriteData
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_PageProgram);      	//发送写页命令   
			iap_interface_flash_extern_send_byte((uint8_t)((copy_start_addr)>>16)); 	//发送24bit地址    
			iap_interface_flash_extern_send_byte((uint8_t)((copy_start_addr)>>8));   
			iap_interface_flash_extern_send_byte((uint8_t)copy_start_addr);
			iap_interface_union.uint32_t_value=data;
//			iap_interface_printf("value:%08x\r\n",iap_interface_union.uint32_t_value);
			iap_interface_flash_extern_send_byte(iap_interface_union.char4_temp[0]);
			iap_interface_flash_extern_send_byte(iap_interface_union.char4_temp[1]);
			iap_interface_flash_extern_send_byte(iap_interface_union.char4_temp[2]);
			iap_interface_flash_extern_send_byte(iap_interface_union.char4_temp[3]);
			flash_extern_cs_set;//CS
			
			//wait busy
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
			while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
			flash_extern_cs_set;//CS
			
			
			/*对比数据*/
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadData);         	//发送读取命令   
			iap_interface_flash_extern_send_byte((uint8_t)((copy_start_addr)>>16));  	//发送24bit地址    
			iap_interface_flash_extern_send_byte((uint8_t)((copy_start_addr)>>8));   
			iap_interface_flash_extern_send_byte((uint8_t)copy_start_addr);   
			for(j=0;j<copy_size;j++)
			{
				value = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
				if(value!=iap_interface_union.char4_temp[j]){
					ret=1;
					break;
				}
			}
			flash_extern_cs_set;//CS
			
//			iap_interface_printf("valueoo:%08x\r\n",iap_interface_union.uint32_t_value);
			
			//wait busy
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
			while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
			flash_extern_cs_set;//CS
			
			if(ret!=0) break;
			
			read_start_addr = read_start_addr+copy_size;
			copy_start_addr = copy_start_addr+copy_size;
		}
		FLASH_Lock();//上锁
		
		if(ret != 0){
			iap_interface_printf("iap_interface_run_to_backup ERR:%d\r\n",ret);
			return ret;
		} 
	#else
		int i;
		char return_data;
		uint32_t  data;
		uint32_t  size;
		uint32_t  read_start_addr;
		uint32_t  copy_start_addr;
		char FlashStatus = 0;
		char copy_size = 4;//每次拷贝字节数
		//避免拷贝越界,运行区和备份区按照最小的拷贝
		size = FLASH_APP1_SIZE < FLASH_APP2_SIZE?FLASH_APP1_SIZE:FLASH_APP2_SIZE;
		size = size/copy_size;//每次拷贝4字节
		
		read_start_addr = FLASH_APP1_ADDR;//运行用户程序的首地址
		copy_start_addr = FLASH_APP2_ADDR;//备份程序的首地址
		
		{
			FLASH_Unlock();
			
			for(i=0;i<size;i++)
			{
				iap_interface_wdg_feed();//喂狗
				return_data = 0;
				data = *(uint32_t*)read_start_addr;//读取flash数据
				FlashStatus = FLASH_ProgramWord(copy_start_addr,data);//写入数据
				if(FlashStatus == FLASH_COMPLETE)
				{
					if( (*(uint32_t*)copy_start_addr) != (*(uint32_t*)read_start_addr) )//数据不一致
					{
						return_data = 3;
						break;
					}
				}
				else//操作未完成,重新写一次
				{
					iap_interface_delay_ms(10);
					FlashStatus = FLASH_ProgramWord(copy_start_addr, data);//写入数据
					if(FlashStatus == FLASH_COMPLETE)
					{
						if( (*(uint32_t*)copy_start_addr) != (*(uint32_t*)read_start_addr) )//数据不一致
						{
							return_data = 4;
							break;
						}
					}
					else
					{
						return_data = 5;
						break;
					}
				}
				
				read_start_addr = read_start_addr+copy_size;
				copy_start_addr = copy_start_addr+copy_size;
			}
			
			FLASH_Lock();//上锁
			
			if(return_data != 0) return return_data;
		}
	#endif
	return 0;
}

//把备份区的程序拷贝到运行区(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_backup_to_run(void)
{
	#if (FLASH_EXTERN_BACKUP_ENABLE==1)//使用外部flash备份程序
		int i,j;
		char ret=0;
		char FlashStatus = 0;
		uint32_t  size;
		uint32_t  read_start_addr;
		uint32_t  copy_start_addr;
		char copy_size = 4;//每次拷贝字节数
		//避免拷贝越界,运行区和备份区按照最小的拷贝
		size = FLASH_APP1_SIZE < FLASH_APP2_SIZE?FLASH_APP1_SIZE:FLASH_APP2_SIZE;
		size = size/copy_size;//每次拷贝4字节
		
		copy_start_addr = FLASH_APP1_ADDR;//运行用户程序的首地址
		read_start_addr = FLASH_APP2_ADDR;
		
		FLASH_Unlock();
		for(i=0;i<size;i++)
		{
			iap_interface_wdg_feed();//喂狗
			
			//WriteDisable
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_WriteDisable);
			flash_extern_cs_set;//CS
			//WriteEnable
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_WriteEnable);  
			flash_extern_cs_set;//CS
			//wait busy
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg);//发送读取状态寄存器命令    
			while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
			flash_extern_cs_set;//CS
			
			
			/*读取外部flash数据*/
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadData);         	//发送读取命令   
			iap_interface_flash_extern_send_byte((uint8_t)((read_start_addr)>>16));  	//发送24bit地址    
			iap_interface_flash_extern_send_byte((uint8_t)((read_start_addr)>>8));   
			iap_interface_flash_extern_send_byte((uint8_t)read_start_addr);   
			for(j=0;j<copy_size;j++)
			{
				iap_interface_union.char4_temp[j] = iap_interface_flash_extern_send_byte(0XFF);//循环读取数据
//				iap_interface_printf("value:%02x\r\n",iap_interface_union.char4_temp[j]);
			}
			flash_extern_cs_set;//CS
			
			//wait busy
			flash_extern_cs_reset;//CS
			iap_interface_flash_extern_send_byte(W25Qxx_ReadStatusReg); //发送读取状态寄存器命令    
			while((iap_interface_flash_extern_send_byte(0Xff)&0x01)==0x01);
			flash_extern_cs_set;//CS
			
			/*写入flash*/
			FlashStatus = FLASH_ProgramWord(copy_start_addr,iap_interface_union.uint32_t_value);//写入数据
			if(FlashStatus == FLASH_COMPLETE)
			{
				if( (*(uint32_t*)copy_start_addr) != iap_interface_union.uint32_t_value )//数据不一致
				{
					ret = 3;
					break;
				}
			}
			else
			{
				ret = 5;
				break;
			}
			
			read_start_addr = read_start_addr+copy_size;
			copy_start_addr = copy_start_addr+copy_size;
		}
		FLASH_Lock();//上锁

		if(ret != 0) return ret;
		
	#else
		int i;
		char return_data;
		uint32_t  data;
		uint32_t  size;
		uint32_t  read_start_addr;
		uint32_t  copy_start_addr;
		char FlashStatus = 0;
		char copy_size = 4;//每次拷贝字节数
		
		//避免拷贝越界,运行区和备份区按照最小的拷贝
		size = FLASH_APP1_SIZE < FLASH_APP2_SIZE?FLASH_APP1_SIZE:FLASH_APP2_SIZE;
		size = size/copy_size;//每次拷贝4字节
		read_start_addr = FLASH_APP2_ADDR;//备份程序的首地址
		copy_start_addr = FLASH_APP1_ADDR;//运行用户程序的首地址

		{
			FLASH_Unlock();//解锁
			
			for(i=0;i<size;i++)
			{
				iap_interface_wdg_feed();//喂狗
				return_data = 0;
				data = *(uint32_t*)read_start_addr;//读取flash数据
				FlashStatus = FLASH_ProgramWord(copy_start_addr,data);//写入数据
				if(FlashStatus == FLASH_COMPLETE)
				{
					if( (*(uint32_t*)copy_start_addr) != (*(uint32_t*)read_start_addr) )//数据不一致
					{
						return_data = 3;
						break;
					}
				}
				else//操作未完成,重新写一次
				{
					iap_interface_delay_ms(10);
					FlashStatus = FLASH_ProgramWord(copy_start_addr, data);//写入数据
					if(FlashStatus == FLASH_COMPLETE)
					{
						if( (*(uint32_t*)copy_start_addr) != (*(uint32_t*)read_start_addr) )//数据不一致
						{
							return_data = 4;
							break;
						}
					}
					else
					{
						return_data = 5;
						break;
					}
				}
				
				read_start_addr = read_start_addr+copy_size;
				copy_start_addr = copy_start_addr+copy_size;
			}
			
			FLASH_Lock();//上锁
			
			if(return_data != 0) return return_data;
		}
	#endif
	return 0;
}



//把程序文件写入到flash(根据实际替代内部程序)
//addr:要写入的地址; data:要写入的数据
//0: 成功  其它:失败
char iap_interface_write_flash(uint32_t addr, char temp_data[FLASH_WRITE_SIZE])
{
	char FlashStatus = 0;
	
	#if (FLASH_WRITE_SIZE == 2)//2字节
		iap_interface_union.char2_temp[0] = temp_data[0];
		iap_interface_union.char2_temp[1] = temp_data[1];
	#elif (FLASH_WRITE_SIZE == 4)//4字节
		iap_interface_union.char4_temp[0] = temp_data[0];
		iap_interface_union.char4_temp[1] = temp_data[1];
		iap_interface_union.char4_temp[2] = temp_data[2];
		iap_interface_union.char4_temp[3] = temp_data[3];	
	#elif (FLASH_WRITE_SIZE == 8)//8字节
		iap_interface_union.char8_temp[0] = temp_data[0];
		iap_interface_union.char8_temp[1] = temp_data[1];
		iap_interface_union.char8_temp[2] = temp_data[2];
		iap_interface_union.char8_temp[3] = temp_data[3];
		iap_interface_union.char8_temp[4] = temp_data[4];
		iap_interface_union.char8_temp[5] = temp_data[5];
		iap_interface_union.char8_temp[6] = temp_data[6];
		iap_interface_union.char8_temp[7] = temp_data[7];
	#endif
	
	
	FLASH_Unlock();//解锁
	iap_interface_delay_us(50);
	#if (FLASH_WRITE_SIZE == 2)//2字节
	
		FlashStatus = FLASH_ProgramHalfWord(addr,iap_interface_union.uint16_t_value);//写入数据
	#elif (FLASH_WRITE_SIZE == 4)//4字节
	
		FlashStatus = FLASH_ProgramWord(addr,iap_interface_union.uint32_t_value);//写入数据
	#endif
	iap_interface_delay_us(50);
	FLASH_Lock();//上锁
	if(FlashStatus != FLASH_COMPLETE)
	{
		iap_interface_printf("iap_interface_write_flash ERR:2\r\n");
		return 2;
	}
	return 0;
}

#endif


/***********************************************************************************************************************************/
char iap_interface_updata_data[iap_interface_updata_data_len];


//擦除存储更新数据flash(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_erase_updata_data_flash(void)
{
	int i;
	uint32_t secpos;
	uint32_t  size;
	uint8_t FlashStatus = 0;
	iap_interface_delay_ms(10);
	size = FLASH_UPDATE_SIZE;
	size = size*1024;
	size = size/FLASH_SECTOR_SIZE;
	
	secpos = FLASH_UPDATE_ADDR-FLASH_MCU_BASE;//实际地址
	secpos = secpos/FLASH_SECTOR_SIZE;//扇区地址
	
	
	FLASH_Unlock();//解锁
	for(i=0;i<size;i++)
	{
		FlashStatus = FLASH_ErasePage(secpos*FLASH_SECTOR_SIZE+FLASH_MCU_BASE);//擦除这个扇区
		if(FlashStatus != FLASH_COMPLETE) break;
		secpos++;
	}
	FLASH_Lock();//上锁
	if(FlashStatus != FLASH_COMPLETE)
	{
		iap_interface_printf("iap_interface_erase_updata_data_flash ERR:1\r\n");
		return 1;
	}

	return 0;
}


//从flash中读取存储的更新相关的数据(根据实际替代内部程序)
void iap_interface_read_all_updata_data(void)
{
	int i;
	uint32_t uint32_t_data;
	uint32_t addr = FLASH_UPDATE_ADDR;
	iap_interface_delay_ms(10);
	for(i=0;i<iap_interface_updata_data_len;i+=4)
	{
		uint32_t_data = *(uint32_t*)addr;//读取flash数据
		iap_interface_union.uint32_t_value=uint32_t_data;
		memcpy(&iap_interface_updata_data[i],iap_interface_union.char4_temp,4);
		addr+=4;
	}
}


//把更新相关的所有数据写入到flash(根据实际替代内部程序)
//0: 成功  其它:失败
char iap_interface_write_all_updata_data(void)
{
	int i;
	uint32_t addr = FLASH_UPDATE_ADDR;
	uint8_t FlashStatus = 0;
	iap_interface_delay_ms(10);
	
	FLASH_Unlock();//解锁
	for(i=0;i<iap_interface_updata_data_len;i+=4)
	{
		iap_interface_union.char4_temp[0] = iap_interface_updata_data[i+0];
		iap_interface_union.char4_temp[1] = iap_interface_updata_data[i+1];
		iap_interface_union.char4_temp[2] = iap_interface_updata_data[i+2];
		iap_interface_union.char4_temp[3] = iap_interface_updata_data[i+3];
		
		//写入数据
		FlashStatus = FLASH_ProgramWord(addr, iap_interface_union.uint32_t_value);
		
		if(FlashStatus != FLASH_COMPLETE)
		{
			break;
		}
		addr+=4;
	}
	FLASH_Lock();//上锁

	if(FlashStatus != FLASH_COMPLETE)
	{
		iap_interface_printf("iap_interface_write_all_updata_data ERR:2\r\n");
		return 2;
	}
	return 0;
}



//设置硬件版本
//0: 成功  其它:失败
char iap_interface_set_hardware_version(uint16_t data)
{
	//获取所有数据
	iap_interface_read_all_updata_data();
	/*设置新的数据*/
	iap_interface_union.uint16_t_value = data;
	iap_interface_updata_data[0] = iap_interface_union.char2_temp[0];
	iap_interface_updata_data[1] = iap_interface_union.char2_temp[1];
	
	if(iap_interface_erase_updata_data_flash() == 0)
	{
		if(iap_interface_write_all_updata_data()==0)
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
}


//获取硬件版本
//返回值
uint16_t iap_interface_get_hardware_version(void)
{	
	//获取所有数据
	iap_interface_read_all_updata_data();
	//组合数据
	iap_interface_union.char2_temp[0]=iap_interface_updata_data[0];
	iap_interface_union.char2_temp[1]=iap_interface_updata_data[1];
	
	return iap_interface_union.uint16_t_value;
}



//设置更新标志
//0: 成功  其它:失败
char iap_interface_set_update_flage(void)
{
	//获取所有数据
	iap_interface_read_all_updata_data();
	/*设置新的数据*/
	iap_interface_union.uint16_t_value = 1;
	iap_interface_updata_data[2] = iap_interface_union.char2_temp[0];
	iap_interface_updata_data[3] = iap_interface_union.char2_temp[1];
	
	if(iap_interface_erase_updata_data_flash() == 0)//擦除
	{
		if(iap_interface_write_all_updata_data()==0)//更新
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
}


//获取更新标志
//返回值
uint16_t iap_interface_get_update_flage(void)
{
	//获取所有数据
	iap_interface_read_all_updata_data();	
	//组合数据
	iap_interface_union.char2_temp[0]=iap_interface_updata_data[2];
	iap_interface_union.char2_temp[1]=iap_interface_updata_data[3];
	
	return iap_interface_union.uint16_t_value;
}


//清除更新标志
//0: 成功  其它:失败
char iap_interface_clear_update_flage(void)
{
	//获取所有数据
	iap_interface_read_all_updata_data();
	/*设置新的数据*/
	iap_interface_union.uint16_t_value = 0;
	iap_interface_updata_data[2] = iap_interface_union.char2_temp[0];
	iap_interface_updata_data[3] = iap_interface_union.char2_temp[1];
	
	if(iap_interface_erase_updata_data_flash() == 0)//擦除
	{
		if(iap_interface_write_all_updata_data()==0)//更新
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
}



//设置更新状态
//0: 成功  其它:失败
char iap_interface_set_update_state(char data)
{	
	//获取所有数据
	iap_interface_read_all_updata_data();
	/*设置新的数据*/
	iap_interface_union.uint16_t_value = data;
	iap_interface_updata_data[4] = iap_interface_union.char2_temp[0];
	iap_interface_updata_data[5] = iap_interface_union.char2_temp[1];
	
	if(iap_interface_erase_updata_data_flash() == 0)//擦除
	{
		if(iap_interface_write_all_updata_data()==0)//更新
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
}


//获取更新状态
//返回值
char iap_interface_get_update_state(void)
{
	//获取所有数据
	iap_interface_read_all_updata_data();
	//组合数据
	iap_interface_union.char2_temp[0]=iap_interface_updata_data[4];
	iap_interface_union.char2_temp[1]=iap_interface_updata_data[5];
	
	return iap_interface_union.uint16_t_value;
}



//设置url
//0: 成功  其它:失败
char iap_interface_set_update_url(char *data,int len)
{
	//获取所有数据
	iap_interface_read_all_updata_data();
	/*设置新的数据*/
	memset(&iap_interface_updata_data[iap_interface_updata_data_url_offset],0,iap_interface_updata_data_len-iap_interface_updata_data_url_offset);
	memcpy(&iap_interface_updata_data[iap_interface_updata_data_url_offset],data,len);
	
	if(iap_interface_erase_updata_data_flash() == 0)//擦除
	{
		if(iap_interface_write_all_updata_data()==0)//更新
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
}


//获取url
void	iap_interface_get_update_url(char *data)
{
	//获取所有数据
	iap_interface_read_all_updata_data();
	//组合数据
	memcpy(data,&iap_interface_updata_data[iap_interface_updata_data_url_offset],iap_interface_updata_data_len-iap_interface_updata_data_url_offset);
}


//清除url
//0: 成功  其它:失败
char iap_interface_clear_update_url(void)
{
	//获取所有数据
	iap_interface_read_all_updata_data();
	/*设置新的数据*/
	memset(&iap_interface_updata_data[iap_interface_updata_data_url_offset],0,iap_interface_updata_data_len-iap_interface_updata_data_url_offset);
	
	if(iap_interface_erase_updata_data_flash() == 0)//擦除
	{
		if(iap_interface_write_all_updata_data()==0)//更新
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
}



/***************************************************************************************************************************/
iapfun jump2app;
/*程序跳转*/
//addr:栈顶地址
__asm void MSR_MSP(uint32_t addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_interface_load_app(uint32_t appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//拷贝APP程序的复位中断函数地址
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针,对APP程序的堆栈进行重构,就是说重新分配RAM
		jump2app();									//执行APP的复位中断函数,最终便会跳转到APP的main函数
	}
}		


