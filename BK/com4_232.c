/**
  ******************************************************************************
  * @file    com0_232.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.27
  * @date    2014-01-03
  * @brief   COM0 USE UART4
	******************************************************************************
	* @note	2014.12.3

		
	*/

/* Includes ------------------------------------------------------------------*/
#include "com4_232.h"
//#include "Gprs_at.h" ZCL123
#include "GlobalV_Extern.h"			// 全局变量声明
#include "GlobalConst.h"
#include "string.h"							//strstr函数：找出str2字符串在str1字符串中第一次出现的位置
#include <stdio.h>							//加上此句可以用printf

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uchar	Txd4Buffer[TXD4_MAX];		// 发送缓冲区
char	Rcv4Buffer[RCV4_MAX];		// 接收缓冲区
uint	Rcv4Counter; 						// 接收计数器//
uint	Txd4Counter;						// 发送计数器//
uint	BakRcv4Count; 					// 接收计数器//
uint  Txd4Max;								// 有多少个字符需要发送//
uint	w_Txd4ChkSum; 					// 发送校验和，lo,hi 两位//
uint	w_Uart4RegAddr;					// 串口1寄存器地址
//
uchar B_Uart4Cmd03;
uchar B_Uart4Cmd16;
uchar B_Uart4Cmd01;
uchar B_Uart4Cmd06;
uchar B_Uart4Cmd73;
uchar T_NoRcv4Count;					// 没有接收计数器
uint	C_NoRcv4Count;



/* Private variables extern --------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/
void GPIO_Com4_Configuration(void);							//GPIO配置
void Com4_config(void);
uint CRC16(uchar * pCrcData,uchar CrcDataLen);
void Delay_MS(vu16 nCount);
void Enable_Usart3_NVIC(void);
void Disable_Usart3_NVIC(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO_Com4_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Com4_Configuration(void)							//GPIO配置 
{
  GPIO_InitTypeDef GPIO_InitStructure;					//定义GPIO初始化结构体
  
	//串口说明：2个串口分别是Usart1.Usart3(USART口下标从1开始,跟STM32F103 ARM寄存器兼容)
  //Com4 实际为第一个串口
  /* Configure 232  */  		// RS232没有控制端
  /* Configure Com4(使用硬件:UART4) Rx (PC.11) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* Configure Com4(使用硬件:UART4) Tx (PC.10) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin_10);
}

/* UART4  configuration ------------------------------------------------------*/
/* USART  configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
				RTS/CTS协议(Request To Send/Clear To Send)即请求发送/允许发送协议				
*/
void Com4_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_DeInit(UART4); //复位 Usart1
	USART_StructInit(&USART_InitStructure);	//把USART_InitStruct中的每一个参数按缺省值填入	
	
	USART_InitStructure.USART_BaudRate = 9600;			//2.25M/BPS   RS232芯片:230.4kbps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure 串口1-UART4 */
	USART_Init(UART4, &USART_InitStructure);  
  /* Enable UART4 Receive and Transmit interrupts */
	//USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
  /* Enable the UART4 */
  USART_Cmd(UART4, ENABLE);	

}

/*******************************************************************************
* Function Name  : Com4_Init
* Description    : Initializes the peripherals used by the Usart driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Com4_Init(void)
{
  GPIO_Com4_Configuration();
  Com4_config();
	//
	Txd4Counter=0;
	Rcv4Counter=0;
	//
	//Delay_MS(35);			ZCL123			//2013.6.22 建议VBAT上电到管脚PWRKEY拉低之间的时间间隔为30MS	
}





/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
