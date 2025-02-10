/**
 ******************************************************************************
 * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
 * @author  MCD Application Team
 * @version V3.5.0
 * @date    08-April-2011
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "com1_232.h"
#include "com2_232.h"
#include "GlobalConst.h"
#include "GlobalV_Extern.h" // 全局变量声明

#include "sx12xxEiger.h" //ZCL 2018.11.13
/** @addtogroup STM32F10x_StdPeriph_Template
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern u8 SoftClock[9]; // 辅助时钟

extern u8 Txd1Buffer[];
extern u8 Rcv1Buffer[];
extern u16 Cw_Txd1;
extern u16 Cw_Rcv1;
extern u16 C_NoRcv1;
extern u16 Cw_Txd1Max;

extern u8 Txd2Buffer[];
extern u8 Rcv2Buffer[];
extern u16 Cw_Txd2;
extern u16 Cw_Rcv2;
extern u16 C_NoRcv2;
extern u16 Cw_Txd2Max;

extern u8 Txd3Buffer[];
extern u8 Rcv3Buffer[];
extern u16 Cw_Txd3;
extern u16 Cw_Rcv3;
extern u16 C_NoRcv3;
extern u16 Cw_Txd3Max;

extern u8 Txd4Buffer[];
extern u8 Rcv4Buffer[];
extern u16 Cw_Txd4;
extern u16 Cw_Rcv4;
extern u16 C_NoRcv4;
extern u16 Cw_Txd4Max;

extern u16 C_PowenOnDelay;
extern volatile u32 sysTickValue;
extern u32 w32_SecCouter; // 2014.4.5

// ZCL 2018.12.11
extern uint C_M35Step;
extern uint C_M35PowerOnOff;
extern uint C_M35SendSum;
extern uint C_DtuInquireSend; // DTU查询指令计数器
extern uint C_DtuSetSend;     // DTU设置指令计数器
extern uint C_GprsDataSend;   // Gprs数据传输计数器
extern uint C_ReadNewSms;     // 读新的短信计数器
extern uint C_SendSms;        // 发送短信计数器，带b为字节型
extern uint C_SmsFull;        // 短信满计数器
// extern	uint	 C_ReLink;							//重连接计数器
extern uint C_DelayRunGprsModule; // 软串口透传计数器
extern uint C_ReceiveChar;
extern uint C_ReceiveChar_Com1;

extern uint C_HeartSendNow[];
extern uint C_HeartDelay[];
extern uint C_LinkDelay[];
extern uint C_LinkNow[];
extern uint C_LinkNow_DoName[];
extern uint C_SmsRunStop;

extern uint C_LedLight;              // LED闪烁延时计数器
extern uint C_BetweenDtuInquireSend; // 发送前延时3MS，收不到>标志延时300MS重发
extern uint C_BetweenDtuSetSend;     // 发送前延时3MS，收不到>标志延时300MS重发
extern uint C_BetweenSendDataReturn; // 发送前延时3MS，收不到>标志延时300MS重发
extern uint C_BetweenHeartSendNow;   // 发送前延时3MS，收不到>标志延时300MS重发
extern uint C_BetweenSendSms;        // 发送前延时3MS，收不到>标志延时300MS重发
extern uint C_NetLedLightCSQ;        // Net LED闪烁CSQ指示计数器 ZCL 2017.5.25

extern u8 S_Display;             // 控制显示内容		ZCL 2019.4.26
extern uint Cw_BakRcv3;          // 接收计数器//
extern u32 C_SendHeartbeatTime0; // 发送心跳时间延时次数
extern u32 C_SendHeartbeatTime;  // 发送心跳时间延时次数
/* Private function prototypes -----------------------------------------------*/
void IWDG_Feed(void); // 独立看门狗喂狗	 2017.6.19

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
  //  static uint16_t tim_cnt = 0;       // 定时器计数
  //  static uint8_t heart_beat_cnt = 0; // 心跳计数
  uchar i;

  SClk1Ms++; // 注意此句需要移到循环外，进行++。 2012.9.10
  sysTickValue++;
  sysTickMsValue++;
  // ZCL 2018.12.11

  C_ReceiveChar++;
  C_ReceiveChar_Com1++;
  C_LedLight++;
  C_BetweenDtuInquireSend++; // 发送前延时3MS，收不到>标志延时300MS重发
  C_BetweenDtuSetSend++;     // 发送前延时3MS，收不到>标志延时300MS重发
  C_BetweenSendDataReturn++; // 发送前延时3MS，收不到>标志延时300MS重发
  C_BetweenHeartSendNow++;   // 发送前延时3MS，收不到>标志延时300MS重发
  C_BetweenSendSms++;        // 发送前延时3MS，收不到>标志延时300MS重发
  C_NetLedLightCSQ++;        // Net LED闪烁CSQ指示计数器 ZCL 2017.5.25

  if (SClk1Ms >= 10) // 10Ms
  {
    SClk1Ms = 0;
    SClk10Ms++;

    if (S_Display == 0) // ZCL 2019.4.26  =1，不显示动画页面了，不再喂狗
      IWDG_Feed();      // =1 去掉中断中的喂狗	否则DTU重启就不好用了 ZCL

    C_M35Step++;
    C_M35PowerOnOff++;
    C_SmsRunStop++;
  }

  if (SClk10Ms >= 100) // 1S
  {
    SClk10Ms = 0;
    SClkSecond++;
    //
    w32_SecCouter++;
    // ZCL 2018.12.11
    C_M35SendSum++;     // 每进入一次计数器加1
    C_DtuInquireSend++; // DTU查询指令计数器
    C_DtuSetSend++;     // DTU设置指令计数器
    C_GprsDataSend++;   // Gprs数据传输计数器
    C_ReadNewSms++;     // 读新的短信计数器
    C_SendSms++;        // 发送短信
    C_SmsFull++;        // 短信满，删除短信
    // C_ReLink++;								//重连接计数器
    C_DelayRunGprsModule++;
    //
    for (i = 0; i < 4; i++)
    {
      C_HeartSendNow[i]++;
      C_HeartDelay[i]++;
      C_LinkDelay[i]++;
      C_LinkNow[i]++;
      C_LinkNow_DoName[i]++;
    }
  }

  if (C_PowenOnDelay < 10000) // MS  1200 ZCL 2019.4.25
    C_PowenOnDelay++;

  // ZCL 2018.11.13
  TickCounter++;

  //  /* 串口接收处理 */
  //  if (usart3_rev_flag) // 如果 usart3 接收数据标志为1
  //  {
  //    tim_cnt = 0;
  //    heart_beat_cnt = 0; // 当串口有数据过来时，心跳计数清零
  //    C_SendHeartbeatTime0 = 0;
  //    C_SendHeartbeatTime = 0;
  //    usart3_rev_cnt++;         // usart3 接收计数
  //    if (usart3_rev_cnt >= 20) // 当超过 3 ms 未接收到数据，则认为数据接收完成。
  //    {
  //      usart3_rev_finish = 1;
  //      usart3_rev_flag = 0;
  //      usart3_rev_cnt = 0;

  //      Rcv3Buffer[Cw_Rcv3] = 0; // 2013.5.9 周成磊 接收到的数据，末尾加0！
  //      //				printf("Receive Data：\r\n %s\r\n",Rcv3Buffer);		//YLS 2022.12.29 把收到的GPRS数据通过串口1发出去，显示出来
  //      Cw_BakRcv3 = Cw_Rcv3; // 把 Cw_Rcv3 保存
  //      Cw_Rcv3 = 0;          // 准备下次接收到缓存开始
  //    }
  //  }

  //  /* lte 心跳处理 */
  //  if (!usart3_rev_flag && lte_connect_flag) // 当串口没有接收到数据时
  //  {
  //    tim_cnt++;           // 定时器计数
  //    if (tim_cnt >= 1000) // 计时达到 1s
  //    {
  //      tim_cnt = 0;
  //      heart_beat_cnt++;         // 心跳计数
  //      if (heart_beat_cnt >= 30) // 计数达到 30s 则心跳标志置 1，可以根据实际需求修改该值。
  //      {
  //        heart_beat_cnt = 0;
  //        heart_beat_flag = 1;
  //      }
  //    }
  //  }
}

/**
 * @brief  This function handles USART1 Handler.
 * @param  None
 * @retval None
 */
void USART1_IRQHandler(void) // 通讯口0使用USART1
{
  // ZCL 2018.11.13
  if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET) //
  {
    USART_ClearFlag(USART1, USART_FLAG_ORE); // 先读SR
    USART_ReceiveData(USART1);               // 后读DR 取出来扔掉
  }

  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    Rcv1Buffer[Cw_Rcv1++] = USART_ReceiveData(USART1);

    /* Clear the USART1 Receive interrupt */
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);

    if (Cw_Rcv1 >= RCV1_MAX + 2)
    {
      Cw_Rcv1--;
    }

    C_NoRcv1 = 0; // 没有接收计数器清零，周成磊 2008.12.4
  }

  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
    /* Clear the USART1 transmit interrupt */
    USART_ClearITPendingBit(USART1, USART_IT_TC); // 2010.7.4 周成磊 改成	USART_IT_TC

    if (Cw_Txd1 >= Cw_Txd1Max)
    {
      Cw_Txd1 = 0;    // 周成磊 2013.1.21
      Cw_Txd1Max = 0; // 周成磊 2013.1.21

      if (w_ZhouShanProtocol_bit9) // 串口1主动查询控制标志位
        B_Com1Con = 0;

      /* Disable the UART3 Transmit interrupt */
      USART_ITConfig(USART1, USART_IT_TC, DISABLE);
      // 发送完成，清除发送控制端，进入接收状态
      // RS485_CON=0;
      w_Com1TXCounter++; // ZCL 2019.10.20  发送完成，发送计数器+1
    }
    else
    {
      /* Write one byte to the transmit data register */
      USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
    }
  }
}

/**
 * @brief  This function handles USART2 Handler.
 * @param  None
 * @retval None
 */
void USART2_IRQHandler(void) // USART2
{
  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    Rcv2Buffer[Cw_Rcv2++] = USART_ReceiveData(USART2);

    /* Clear the USART2 Receive interrupt */
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);

    if (Cw_Rcv2 >= RCV2_MAX + 2)
    {
      Cw_Rcv2--;
    }

    C_NoRcv2 = 0; // 没有接收计数器清零，周成磊 2008.12.4
  }

  if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
  {
    /* Clear the USART2 transmit interrupt */
    USART_ClearITPendingBit(USART2, USART_IT_TC); // 2010.7.4 周成磊 改成	USART_IT_TC

    if (Cw_Txd2 >= Cw_Txd2Max)
    {
      Cw_Txd2 = 0;    // 周成磊 2013.1.21
      Cw_Txd2Max = 0; // 周成磊 2013.1.21

      /* Disable the UART3 Transmit interrupt */
      USART_ITConfig(USART2, USART_IT_TC, DISABLE);
      // 发送完成，清除发送控制端，进入接收状态
      // RS485_CON=0;
    }
    else
    {
      /* Write one byte to the transmit data register */
      USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
    }
  }
}

/**
 * @brief  This function handles USART3 Handler.
 * @param  None
 * @retval None
 */
void USART3_IRQHandler(void) // USART3
{
  if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET) //
  {
    USART_ClearFlag(USART3, USART_FLAG_ORE); // 先读SR
    USART_ReceiveData(USART3);               // 后读DR 取出来扔掉
  }

  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收非空成立=1
  {
    /* Read one byte from the receive data register */
    Rcv3Buffer[Cw_Rcv3++] = USART_ReceiveData(USART3);

    /* Clear the USART3 Receive interrupt */
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);

    if (Cw_Rcv3 >= RCV3_MAX)
    {
      Cw_Rcv3--;
    }

    usart3_rev_flag = 1; // 串口3 接收标志
    usart3_rev_cnt = 0;  // 串口3接收计数清零

    C_NoRcv3 = 0; // 没有接收计数器清零，周成磊 2008.12.4
  }

  if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  {
    /* Clear the USART3 transmit interrupt */     // USART_IT_TXE:发送数据寄存器空中断
    USART_ClearITPendingBit(USART3, USART_IT_TC); // 2010.7.4 周成磊 改成	USART_IT_TC

    if (Cw_Txd3 >= Cw_Txd3Max)
    {
      Cw_Txd3 = 0;    // 周成磊 2013.1.21
      Cw_Txd3Max = 0; // 周成磊 2013.1.21

      /* Disable the USART3 Transmit interrupt */
      USART_ITConfig(USART3, USART_IT_TC, DISABLE);
      // 发送完成，清除发送控制端，进入接收状态
      // GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    }
    else
    {
      /* Write one byte to the transmit data register */
      USART_SendData(USART3, Txd3Buffer[Cw_Txd3++]);
    }
  }
}

void UART4_IRQHandler(void) // UART4
{
  if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    Rcv4Buffer[Cw_Rcv4++] = USART_ReceiveData(UART4);
    F_Reset_GPS = 0; // 如果串口4收到GPS发来的数据，则清标志位，不重启
    C_Reset_GPS = 0; // 清重启计数器
    /* Clear the UART4 Receive interrupt */
    USART_ClearITPendingBit(UART4, USART_IT_RXNE);

    if (Cw_Rcv4 >= RCV4_MAX + 2)
    {
      Cw_Rcv4--;
    }

    C_NoRcv4 = 0; // 没有接收计数器清零，周成磊 2008.12.4
  }

  if (USART_GetITStatus(UART4, USART_IT_TC) != RESET)
  {
    /* Clear the UART4 transmit interrupt */
    USART_ClearITPendingBit(UART4, USART_IT_TC); // 2010.7.4 周成磊 改成	USART_IT_TC

    if (Cw_Txd4 >= Cw_Txd4Max)
    {
      Cw_Txd4 = 0;    // 周成磊 2013.1.21
      Cw_Txd4Max = 0; // 周成磊 2013.1.21

      /* Disable the UART3 Transmit interrupt */
      USART_ITConfig(UART4, USART_IT_TC, DISABLE);
      // 发送完成，清除发送控制端，进入接收状态
      // RS485_CON=0;
    }
    else
    {
      /* Write one byte to the transmit data register */
      USART_SendData(UART4, Txd4Buffer[Cw_Txd4++]);
    }
  }
}

// ZCL 2019.2.10
void EXTI2_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line2) != RESET) // 2019.2.10
  {
    LED0_TOGGLE;

    /* Clear the EXTI line 2 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line2);
  }
}

void EXTI9_5_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line6) != RESET) // 2019.2.10
  {
    LED1_TOGGLE;

    /* Clear the EXTI line 2 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line6);
  }

  if (EXTI_GetITStatus(EXTI_Line7) != RESET)
  {
    ;
  }
}

void TIM2_IRQHandler(void)
{
  //  tim2_interrupt_process();	  // 定时器中断处理
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
