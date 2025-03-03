#ifndef flash_extern_ch_
#define flash_extern_ch_

#ifndef flash_extern_c_//如果没有定义
#define flash_extern_cx_ extern
#else
#define flash_extern_cx_
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "iap_interface.h"
#include <stm32f10x.h>

#if (FLASH_EXTERN_BACKUP_ENABLE==1)//使用外部flash备份程序

#define  flash_extern_cs_set (GPIO_SetBits(GPIOA, GPIO_Pin_4))
#define  flash_extern_cs_reset (GPIO_ResetBits(GPIOA, GPIO_Pin_4))


#define W25Q80_ID 	0XEF13
#define W25Q16_ID 	0XEF14
#define W25Q32_ID 	0XEF15
#define W25Q64_ID 	0XEF16
#define W25Q128_ID	0XEF17
#define W25Q256_ID	0XEF17
#define W25Q512_ID	0XEF17


#define W25Qxx_WriteEnable		0x06 
#define W25Qxx_WriteDisable		0x04 
#define W25Qxx_ReadStatusReg		0x05 
#define W25Qxx_WriteStatusReg		0x01 
#define W25Qxx_ReadData			  0x03 
#define W25Qxx_FastReadData		0x0B 
#define W25Qxx_FastReadDual		0x3B 
#define W25Qxx_PageProgram		0x02 
#define W25Qxx_BlockErase			0xD8 
#define W25Qxx_SectorErase		0x20 
#define W25Qxx_ChipErase			0xC7 
#define W25Qxx_PowerDown			0xB9 
#define W25Qxx_ReleasePowerDown	0xAB 
#define W25Qxx_DeviceID			0xAB 
#define W25Qxx_ManufactDeviceID	0x90 
#define W25Qxx_JedecDeviceID		0x9F 

//初始化外部flash
uint16_t iap_interface_flash_extern_init(void);
//SPI发送和接收数据
uint8_t iap_interface_flash_extern_send_byte(uint8_t data);
#endif


#endif
