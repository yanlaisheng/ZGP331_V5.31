#define flash_extern_c_

#include "flash_extern.h"

#if 1 //是否打印日志
#define flash_extern_printf  printf
#else
#define u1_printf(...)
#endif


#if (FLASH_EXTERN_BACKUP_ENABLE==1)//使用外部flash备份程序


//初始化外部flash
uint16_t iap_interface_flash_extern_init(void)
{
	uint16_t value = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI时钟使能 	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORT时钟使能 
	//CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
	flash_extern_cs_set; //CS
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);
	
	iap_interface_delay_ms(10);
	flash_extern_cs_reset;
	iap_interface_flash_extern_send_byte(0x90);//发送读取ID命令
	iap_interface_flash_extern_send_byte(0x00);
	iap_interface_flash_extern_send_byte(0x00);
	iap_interface_flash_extern_send_byte(0x00);
	value|=iap_interface_flash_extern_send_byte(0xFF)<<8;  
	value|=iap_interface_flash_extern_send_byte(0xFF);	 
	flash_extern_cs_set;
	
	flash_extern_printf("flash_extern ID=%04x\r\n",value);
	
	if(value!=W25Q80_ID && value!=W25Q16_ID && value!=W25Q32_ID && value!=W25Q64_ID && value!=W25Q128_ID && value!=W25Q256_ID && value!=W25Q512_ID)
	{
		return 1;
	}
	
	return 0;
}

//SPI发送和接收数据
uint8_t iap_interface_flash_extern_send_byte(uint8_t data)
{		
	uint16_t retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>500)return 0;
	}			  
	SPI_I2S_SendData(SPI1, data); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>500)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据					    
}

#endif




















