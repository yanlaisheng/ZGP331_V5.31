/**
  ******************************************************************************
  * @file    com1_485.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.27
  * @date    2014-01-03
  * @brief   COM1 USE USART3
	******************************************************************************
	*/
	
/* Includes ------------------------------------------------------------------*/
#include "com1_485.h"
#include "GlobalV_Extern.h"			// 全局变量声明
#include "GlobalConst.h"
#include "CRCdata.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8  Txd1Buffer[TXD1_MAX];	// 发送缓冲区
u8  Rcv1Buffer[RCV1_MAX];	// 接收缓冲区
u16   Rcv1Counter; 					// 接收计数器//
u16	 Txd1Counter;						// 发送计数器//
u16   BakRcv1Count; 					// 接收计数器//
u16   Txd1Max;								// 有多少个字符需要发送//
u16   w_Txd1ChkSum; 					// 发送校验和，lo,hi 两位//
u16	 w_Com1RegAddr;					// 串口2寄存器地址
u8  T_NoRcv1Count;					// 没有接收计数器
u16	 C_NoRcv1Count;
u8	 B_PcSetGprs;						// PC设置GPRS标志 2013.6.5
//
u8  B_Com1Cmd03;
u8  B_Com1Cmd16;
u8  B_Com1Cmd01;
u8  B_Com1Cmd06;

/* Private variables extern --------------------------------------------------*/
extern	u16	C_ForceSavPar;		// 强制保存参数计数器
extern	u8 B_ForceSavPar;					//强制保存参数标志
extern	u8 F_ModTime;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Com1_Configuration(void);							//GPIO配置
void Com1_config(void);
u16 CRC16(u8 * pCrcData,u8 CrcDataLen);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO_Com1_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Com1_Configuration(void)							//GPIO配置 
{
  GPIO_InitTypeDef GPIO_InitStructure;					//定义GPIO初始化结构体
	
  /* Configure 232  */  		// RS232没有控制端
  /* Configure Com0(使用硬件:USART1) Tx (PA.9) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure Com0(使用硬件:USART1) Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  /* Configure 485 控制端 PC.15 */  
	/* PC13，PC14和PC15引脚通过电源开关进行供电，因此这三个引脚作为输出引脚时有以下限制：
	作为输出脚时只能工作在2MHz模式下,最大驱动负载为30pF, 同一时间，三个引脚中只有一个引脚
	能作为输出引脚。 */ 	
/*   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
  // Com1控制端
  RS485_CON=0;   	 */
}

/* USART1  configuration ------------------------------------------------------*/
/* USART  configured as follow:
        - BaudRate = 19200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
				RTS/CTS协议(Request To Send/Clear To Send)即请求发送/允许发送协议				
*/
void Com1_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	Pw_BaudRate1=9600;
	USART_InitStructure.USART_BaudRate = Pw_BaudRate1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure Com1-USART1 */
	USART_Init(USART1, &USART_InitStructure);  
  /* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);			//发送中断
	
  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);		

}

/*******************************************************************************
* Function Name  : Com1_Init
* Description    : Initializes the peripherals used by the Usart driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Com1_Init(void)
{
  GPIO_Com1_Configuration();
  Com1_config();
	//
	Txd1Counter=0;
	Rcv1Counter=0;
	BakRcv1Count=0;
	RS485_CON=0;
}

//接收处理程序 校验程序
void Com1_RcvProcess(void)
{
	u8 k,s,i=0;       						// 临时变量
  u16 j;
	//作为主机,指定接收时间到了,就可以处理接收到的字符串了	
	// 在没收到串行字符的时间超过设定时，可以对接收缓存进行处理了
	// **********************************rcv_counter<>0,收到字符才能处理
	if ( Rcv1Counter>0 &&  T_NoRcv1Count!=SClk1Ms )
	{								// 接收处理过程
		RS485_CON=0;
		T_NoRcv1Count=SClk1Ms;				// 
		C_NoRcv1Count++;
		if ( C_NoRcv1Count>NORCVMAXMS )				// 	
		{
			//	
			USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);		// 禁止UART1串口中断,防止在处理期间更改					
			BakRcv1Count=Rcv1Counter;		// 把 Rcv1Counter 保存
			C_NoRcv1Count=0;				// 清没有接收计数器
			//
			if(BakRcv1Count<=RCV1_MAX)		// 接收长度正确,继续处理.
			{
				if( Rcv1Buffer[0]==2 )		// 从地址检测－接收到的上位机查询指令
				{	
					j=CRC16( Rcv1Buffer, BakRcv1Count-2);		// CRC 校验
					k=j>>8;
					s=j;
					if ( k==Rcv1Buffer[BakRcv1Count-2] 
						&& s==Rcv1Buffer[BakRcv1Count-1] )
					{											// CRC校验正确
						if ( Rcv1Buffer[1]==3 )		// 03读取保持寄存器
						{
							B_Com1Cmd03=1;
							j=Rcv1Buffer[2];
							w_Com1RegAddr=(j<<8)+Rcv1Buffer[3];
						}
						else if ( Rcv1Buffer[1]==16 )	// 16预置多寄存器
						{
							C_ForceSavPar=0;		// 强制保存参数计数器=0							
							B_Com1Cmd16=1;
							j=Rcv1Buffer[2];
							w_Com1RegAddr=(j<<8)+Rcv1Buffer[3];
						}
						else if ( Rcv1Buffer[1]==1 )		// 01读取线圈状态
						{
							B_Com1Cmd01=1;
						}
						else if ( Rcv1Buffer[1]==6 )		// 06预置单寄存器
						{
							C_ForceSavPar=0;		// 强制保存参数计数器=0							
							B_Com1Cmd06=1;
							j=Rcv1Buffer[2];
							w_Com1RegAddr=(j<<8)+Rcv1Buffer[3];
						}
						else
							i=1;
					}
					else
						i=2;
				}
				else
					i=3;

			}
			else
				i=4;
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
			Rcv1Counter=0;					// 准备下次接收到缓存开始
		}
	}
	if(i>0)
	{
		for(j=0;j<20;j++)
		{
			Rcv1Buffer[j]=0;
		}
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		
	}
}


void Com1_SlaveSend(void)			// 串口2从机发送  
{
	u16	m,n; 
	u8	j=0,k;
	u16	 *p_wRead;
	u8	 *p_bMove;
	u8	 *p_bGen;
	u16	 *p_wTarget;			// 指向目标字符串　xdata zcl

	//
	if ( B_Com1Cmd03 )		// 读取保持寄存器
	{
		Txd1Buffer[0]=2;				// 设备从地址Pw_EquipmentNo
		Txd1Buffer[1]=Rcv1Buffer[1];	// 功能码
		Txd1Buffer[2]=Rcv1Buffer[5]*2;	// Rcv1Buffer[5]=字数 　
		//
		p_wRead=w_ParLst;			// PAR区
		p_bMove=Txd1Buffer;
		for ( k=0;k<Rcv1Buffer[5] ;k++ )	// 填充查询内容
		{
			m=*(p_wRead+w_Com1RegAddr+k);	
			*(p_bMove+3+k*2)=m>>8;
			*(p_bMove+3+k*2+1)=m;
		}
		w_Txd1ChkSum=CRC16( Txd1Buffer, Txd1Buffer[2]+3 );
		Txd1Buffer[Txd1Buffer[2]+3]=w_Txd1ChkSum>>8;			// /256
		Txd1Buffer[Txd1Buffer[2]+4]=w_Txd1ChkSum;				// 低位字节
		Txd1Max=Txd1Buffer[2]+5;
		//
		//
		B_Com1Cmd03=0;
		Txd1Counter=0;
		RS485_CON=1;
		// 2010.8.5 周成磊 TXE改成TC，一句改为两句		
		//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);				// 开始发送.
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);				// 开始发送.			
		USART_SendData(USART1,Txd1Buffer[Txd1Counter++]); 				
	}
	//
	else if ( B_Com1Cmd16 || B_Com1Cmd06 )					// 16预置多寄存器
	{
		// 填充 可以直接修改的参数
		if ( w_Com1RegAddr==4 )  // 
			j=1;

		// 需要口令才可以修改的参数(需要先把Pw_ModPar 修改成规定值，才能修改的参数)
		else //if ( Pw_ModPar==10000 )	
			j=1;

		// 修改参数单元
		if ( j )					
		{
			if ( w_Com1RegAddr>=10 && w_Com1RegAddr<=16 )			// 修改时间
			{
				w_ModTimeNo=w_Com1RegAddr-10;
				F_ModTime=1;
			}				
			if ( B_Com1Cmd06 )		// 预置单个
			{
				m=Rcv1Buffer[4];
				w_ParLst[w_Com1RegAddr]=(m<<8)+Rcv1Buffer[5];
			}
			else if ( B_Com1Cmd16 )	// 预置多个
			{
				if ( Rcv1Buffer[5]<90 )
				{
					p_bGen=Rcv1Buffer;
					p_wTarget=w_ParLst;
					for ( k=0;k<Rcv1Buffer[5] ;k++ )		// Rcv1Buffer[5]=字数
					{	
						m = *(p_bGen+7+k*2);
						n = *(p_bGen+7+k*2+1);
						*(p_wTarget+w_Com1RegAddr+k)= (m<<8) + n;	
					}	
				}
			}
		}


		// -------------------------
		// 返回数据
		Txd1Buffer[0]=2;				// 设备从地址
		Txd1Buffer[1]=Rcv1Buffer[1];	// 功能码
		Txd1Buffer[2]=Rcv1Buffer[2];	// 开始地址高位字节
		Txd1Buffer[3]=Rcv1Buffer[3];	// 开始地址低位字节
		Txd1Buffer[4]=Rcv1Buffer[4];	// 寄存器数量高位
		Txd1Buffer[5]=Rcv1Buffer[5];	// 寄存器数量低位	
		if ( j==0 )							// 如果不能被正常预置，返回FFFF zcl
		{
			Txd1Buffer[4]=0xff;				// 寄存器数量高位、预置数据
			Txd1Buffer[5]=0xff;				// 寄存器数量低位、预置数据
		}
		w_Txd1ChkSum=CRC16( Txd1Buffer, 6);
		Txd1Buffer[6]=w_Txd1ChkSum>>8;		// /256
		Txd1Buffer[7]=w_Txd1ChkSum;				// 低位字节
		Txd1Max=8;
		B_Com1Cmd16=0;
		B_Com1Cmd06=0;
		Txd1Counter=0;
		RS485_CON=1;		
		// 2010.8.5 周成磊 TXE改成TC，一句改为两句		
		//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);				// 开始发送.
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);				// 开始发送.
		USART_SendData(USART1,Txd1Buffer[Txd1Counter++]); 		

	}// 06、16预置寄存器 结束
}

// 作为主模块时程序
void Com1_MasterSend(void)		// 串口2主发送程序
{

}

u16 CRC16(u8 * pCrcData,u8 CrcDataLen)
{
	u8 CRC16Hi=0xff;                   /* 高CRC字节初始化 */
  u8 CRC16Lo=0xff;                   /* 低CRC字节初始化*/
	u8 Index=0;
	u16  w_CRC16=0;
	while(CrcDataLen--)
	{
		Index = CRC16Hi ^* pCrcData++;
		CRC16Hi = CRC16Lo ^ CRC_H[Index];
		CRC16Lo = CRC_L[Index];
	}
	w_CRC16 = (CRC16Hi << 8) | CRC16Lo; 
	return (w_CRC16);
}

/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
	RS485_CON=1;
  USART_SendData(USART1, (u8) ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
  }
	RS485_CON=0;
  return ch;
}


/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
