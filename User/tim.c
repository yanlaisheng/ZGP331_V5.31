///*******************************************************************************
//* @file    			tim.c
//* @function     定时器配置					  				
//* @brief 				用3.5.0版本库建的工程模板						  	
//* @attention		该例程仅作为参考，如需实际开发，请根据实际需求更改	
//* @version 			V1.0		
//* @company  		深圳市飞思创电子科技有限公司
//* @website  		https://freestrong.taobao.com
//* @Author       WisBarry
//* @date    			2018-11-10	
//********************************************************************************/

//#include "tim.h"
////#include "lte_uart2.h"
////#include "lte_at.h"

//uint8_t heart_beat_flag = 0;        // 心跳标志
//extern uint8_t usart3_rev_flag,usart3_rev_cnt,usart3_rev_finish,lte_connect_flag;

///**
//  * @brief  定时器2参数设置    
//  * @param  无
//  * @retval 无
//	* @note   1ms中断一次
//  */
//void tim2_parameter_init(void)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);              // 使能定时器2时钟
//	
//	/*定时器参数设置*/
//	TIM_TimeBaseStructure.TIM_Period = 999;					                // 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
//	TIM_TimeBaseStructure.TIM_Prescaler = 71;					              // 设置用来作为TIMx时钟频率除数的预分频值(72KHz的计数频率)
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		        // 设置时钟分割:不分割，选择默认TDTS = TIM_CKD_DIV1
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	      // TIM向上计数模式
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );           // 开启定时器2中断
//	TIM_Cmd(TIM2, ENABLE);  									            // 开启定时器2	
//}	

///**
//  * @brief  定时器2中断向量初始化配置   
//  * @param  无
//  * @retval 无
//  */
//void tim2_nvic_config(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;                        // 定义NVIC初始化结构体
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             // 设置中断优先级组为2，先占优先级和从优先级各两位(可设0～3)

//	/*定时器2中断向量配置*/
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				      // TIM2中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	// 设置抢占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			    // 设置响应优先级0级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				      // 使能IRQ通道
//	NVIC_Init(&NVIC_InitStructure); 	
//}

///**
//  * @brief  定时器2初始化  
//  * @param  无
//  * @retval 无
//  */
//void tim2_init(void)
//{
//  tim2_parameter_init();
//  tim2_nvic_config();
//}

///**
//  * @brief  定时器2中断处理 
//  * @param  无
//  * @retval 无
//  */
//void tim2_interrupt_process(void)
//{
//	static uint16_t tim_cnt = 0;                         // 定时器计数
//	static uint8_t heart_beat_cnt = 0;                   // 心跳计数
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)   // 检查TIM2更新中断发生与否
//	{	
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );      // 清除TIM2更新中断标志
//		/* 串口接收处理 */
//		if(usart3_rev_flag)                                // 如果 usart2 接收数据标志为1
//	  {
//			tim_cnt = 0;
//      heart_beat_cnt = 0;					                     // 当串口有数据过来时，心跳计数清零
//		  usart3_rev_cnt++;                                // usart2 接收计数	
//		  if(usart3_rev_cnt >= 3)                          // 当超过 3 ms 未接收到数据，则认为数据接收完成。
//		  {
//			  usart3_rev_finish = 1;
//				usart3_rev_flag = 0;
//				usart3_rev_cnt = 0;
//		  }
//	  }
//		
//		/* lte 心跳处理 */
//		if(!usart3_rev_flag && lte_connect_flag)         // 当串口没有接收到数据时
//		{
//			tim_cnt++;                                      // 定时器计数
//			if(tim_cnt >= 1000)                             // 计时达到 1s
//			{
//				tim_cnt = 0;
//			  heart_beat_cnt++;                             // 心跳计数
//			  if(heart_beat_cnt >= 30)                      // 计数达到 30s 则心跳标志置 1，可以根据实际需求修改该值。
//			  {
//				  heart_beat_cnt = 0;
//				  heart_beat_flag = 1;
//			  }
//		  }			
//		}
//	}
//}
