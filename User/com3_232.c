/**
  ******************************************************************************
  * @file    com3_232.c
  * @author  ChengLei Zhou  - 周成磊
  * @version V1.27
  * @date    2014-01-03
  * @brief   COM0 USE USART3
	* YLS
	******************************************************************************
	* @note	2014.12.3
	COM3: GPRS SIM7600CE 4G模块

	SIM7600CE说明:
	1. 利用连接4的本地端口号参数，可以打开回显，=11011时
		 利用连接4的域名，前面带@为电话号码，主动回发短信。 2014.12.7
		 3E是 >号

	2. 此串口1是MCU与 SIM7600CE 4G模块通讯口；

	3. 下面是服务器发过来的：（DTU发过去的相应的减去0x80，不全是这样！）
		 A. 心跳包注册功能码 0x81
		 B. 查询DTU参数 0x8B
		 C. 设置DTU参数 0x8D
		 D. 服务器分离终端，DTU重启指令 0x82

	4. B_RunGprsModule	运行GPRS模块标志；(没有与文本通讯，4秒后此值=1)；
		 B_ATComStatus 		AT命令状态；注意@11#进入；@22#退出；
		 B_PcSetGprs			PC设置GPRS标志；=1时，再收到数据(设置查询GPRS参数)则置=2，
						借用Sms_SetupInq()函数处理，处理完后让=1，然后再等待接收处理下一条信息。
						注意：@33#进入；@44#退出；
		 B_SmsRunStop		短信控制启停（设备停机）；	=1发送一次启停指令后，让B_SmsRunStop=2，
						没有收到应答延时继续发10次，收到应答则让=3，再=0，然后短信应答。
		 B_GprsDataReturn			GPRS 数据返回；有连接，收到串口2数据，就通过DTU发送出去
						远程上位机发送给DTU数据指令的返回，已经连接的每路连接都返回

	5.//2014.12.9 收到+CMTI(新短信提示)别立即去读短信，否则读完可能还会收到新短信
		提示（M35可能有BUG，还是同一条短信）
		+CMTI		新短信指示
		+CMGR		读来的短信内容（然后用SmsAndPC_SetupInq()函数处理）
		+CMGS:247  短信已经发送（用AT+CMGS="15610566803"）

	6. 短信控制功能：
		0x82: 重启  0x91：模块上电重启
		0x92: 设备启停(=0,启动; =1,停止) 0x95:看门狗复位重启

	7. 主动发送短信获得电话号码：//主动发送短信(收到连接4的域名带@电话号码后) 2014.12.7
		uchar B_ZhuDongSendSms;
		2014.12.7 利用连接4的域名，前面带@为电话号码，主动回发短信。
		如：@15610566803  GP311给：15610566803回发短信！	可以是其他11位的手机电话号码

	GP311说明: ( main.c )
	//--------------------------------
	1. 调试时把开门狗初始化注释掉，用到设备上时打开。
	2. B_RunGprsModule=0后，4秒没有访问，B_RunGprsModule=1，退出与文本相连，正常运行
	3.	0x00,0x00,0x25,0x80,0x03,0x00,0x00,0x03,						//0D串口工作方式
		//1.串口速率
		//2. 03 数据位为8位（00-5， 01-6， 02-7， 03-8）；（这里只能设置03）[00-5解释 00为设置值，5为在监控软件中的显示内容，其他以此类推]
		//3. 00 停止位为1位（00-1，04-2）；（这里可以设置00、04）
		//4. 00 校验位-无（00-无，08-奇，18-偶）；（这里可以设置00、08、18）
		//5. 03 流控-无流控（01-Xon/Xoff，02-硬流控，03-无流控，04-半双工485，05-全双工422）
	4. 短信设置和查询串口工作方式时：
		因为短信在手机中书写时，是ASCII码格式，所以（1）设置时是以在监控软件中的显示内容的
		ASCII码格式填写发送，GPRS模块收到后转成设置值保存起来。（2）读取时把GPRS中的设置值
		转成在监控软件中的显示内容的ASCII码格式填写发送。上面//4  //5 有汉字除外
		1.串口波特率 2.数据位 3.停止位 4.校验位 5.流控
		GPW+0D,19200,8,1,0,3;
	5. 2013.12.18 修改xmodem文件，
		(1)增加开始，结果，结束信息。
		-----Begin; -----Result info; -----End
		(2)增加串口率参数各项数值的容错，增加字符串参数(01.05.06.0x21.0x31.0x41.0x51)长度的限制
		(3)串口配置中i=FindFirstValue(ptr,0x2C ,0x00); 增加==255的限制，防止没有找到0x2C(,)
	6. GP311使用8M*4=32M频率，system_stm32f10x.c中SystemInit()函数在上电后，会在启动文件.s
		里先执行设定频率，此处没有合适频率，所以注释掉72M，在main.c中使用RCC_Configuration()
		重新设定频率。
		//#define SYSCLK_FREQ_72MHz  72000000		//2013.1.3注释掉！！！周成磊
	7. 主动发送短信不成功问题解决，因为：
		//2014.12.9 !!! 短信末尾加 ; 0x1A   0x0d
		Txd0Buffer[Txd0Max++]=';';
		Txd0Buffer[Txd0Max++]=0x1A;				//CTRL+Z结束(0x1A)
		Txd0Buffer[Txd0Max++]=0x0d;

		利用连接4的本地端口号参数，可以打开回显，=11011时
	  利用连接4的域名，前面带@为电话号码，主动回发短信。 2014.12.7
		如：@15610566803  GP311给：15610566803回发短信！	可以是其他11位的手机电话号码

	8.	2014.12.11 主动发短信时发现“参数设置成功”等发完短信才在“DSC_DEMO软件”显示，这里加个
			延时不用发完短信就可以显示，显得流畅！
		解决办法，加入：
		else if(S_DtuSetSend==4 && C_BetweenDtuSetSend>300)
	9. 上位机连续给DTU串口发数据，ORE错误解决方法见 stm32f10x_it.c
	10. 链接后用文本发送数据，有时候发送几次就不能发送了：
		原因：At_QISENDAccordingLength 发送后，容易收不到'>'应答符。
		解决办法：收不到'>'应答符后，300MS重新发 At_QISENDAccordingLength
		又加上：  收不到"OK"应答符后，1000MS重新发。经过仿真，进不了这段程序，看来OK都能收到。

	11. V129版本：150105-V4 V129 IAR6.4 MDK5.01 等待CallReady加大到 100 秒，解决15秒重启不上线问题OK
	12. V130版本：150117-V4 V130 IAR6.4 MDK5.01 CREG注册不成功加大到 95 秒，解决15秒重启不上线问题OK

	13. V131版本：
			DoWith.c
			A. 取消Gprs_ParBootLst参数的读，Gprs参数不再初始化。（防止GPRS参数ID因为初始化改变）//ZCL 2016.5.26
			B. Pw_OutPDecMax= 991;  //ZCL 2016.5.26 加上当成一个标志  可以观看是什么造成的初始化

			com0_232.c
			C. 修改：（1）心跳注册不成功，重启  2016.5.24 ZCL （无论几路都重启）也许会导致一路连接不上，频繁重启
				（2）建立连接不成功，重启。（以前是频繁连接）

			D. 关于重启的说明  2016.5.24 ZCL
					B_SmsOrFaultHardRestart=11;		//=11故障硬件重启; =1短信硬件重启
				（1）第2次接收到乱码（大量乱数据），就重启
				（2）第2次出现"CONNECT FAIL"后，就重启
				（3）接收到“ERROR”，重启
				（4）修改24，34，44，54分别代表1,2,3,4通道的连网方式（0-UDP，1-TCP），重启
				（5）有最大登录DSC次数(5次)故障标志（心跳注册不成功），重启！ 1路连接（以前仅1路时重启）
				//2016.7.5 取消（6）有最大登录DSC次数(5次)故障标志（心跳注册不成功），重启！ 2路以上连接
				（7）有10次故障标志（建立连接不成功），重新启动模块
				（8）短信修改24，34，44，54分别代表1,2,3,4通道的连网方式（0-UDP，1-TCP），重启

				（9）短信指令重启

			E.	2016.5.24 ZCL 备注说明下，以前就有此程序
				if(B_SmsOrFaultHardRestart==1 )
				{
					B_SmsOrFaultHardRestart=2;
					S_M35 = 0x01;						//模块关机，重启
				}
				else if(B_SmsOrFaultHardRestart==11 )
				{
					B_SmsOrFaultHardRestart=12;
					S_M35 = 0x01;						//模块关机，重启
				}
			F. 	DATA灯一直亮问题。??? 加入DATA灯如果一直亮20秒，看门狗重启。2016.5.27

	14. 备注：当用仿真器调试时，要把main()中B_LockCodeOK=1; 不进行密码校验的意思！
			GetLockCode(void)		// 2013.10.26 得到锁定密码；
			脱机编程器写程序时也会在指定位置写入一个密码；
			当程序检测到运算的密码跟指定位置的密码一样时，B_LockCodeOK=1；
			注：如果使用仿真器，没有让B_LockCodeOK=1，则密码不对，会导致串口数据发给模块，DATA灯就一直亮。！！！
			必要时可以把密码保护取消掉！

	15. 备注：修改程序后记得修改com0_232.c中  远程查询GPRS模块的版本号
			uc8 DtuProgVersion[7]="1.31.00";					//7 DTU程序版本:70
			uc8 DtuProgMakeDate[8]="20160527";				//8 DTU程序生成日期:71

	16. 备注：文本显示器在上电前插上，则可以通讯上。连接上以后，再插上文本显示器，则不再支持参数设定，
			那时候相当于文本显示器发出的指令，需要GPRS模块网络远传了。

	17. V131-2 ZCL 2016.7.1
			Com1_RcvProcess()中
			//2014.12.9 加上条件，数据转发时禁止接收。
			//2016.7.1 ZCL 因为这个条件的加入，必须后面在给每路链接发送完毕后，还得打开接收。！！！
			if(B_GprsDataReturn==0)
			{
				USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
				USART3->CR1 |=0x0004;  //.3位  运行接收
			}

		(1)	M35_Init()中：
			//2016.7.1 ZCL 打开接收，防止没有开接收中断，导致接收不到数据。
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
			USART3->CR1 |=0x0004;  //.3位  运行接收

		(2) //M35的操作循环
		switch(S_M35)
		{
			case 0x00:
			case 0x01:  //开机.关机.重启
									M35_Power_ONOFF();
									if(S_M35PowerOnOff==2 && STATUS_4G)
									{
										S_M35PowerOnOff=0;
										C_M35SendSum = 0;
										S_M35 = 0x03;

										// Enable USART1 Receive and Transmit interrupts
										USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //此时开接收，防止接收到乱码

										//2016.7.1 ZCL 打开接收，防止没有开接收中断，导致接收不到数据。
										USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
										USART3->CR1 |=0x0004;  //.3位  运行接收
									}
		18.		// ZCL 2016.7.5 (解决串口不停通讯问题，删掉跟文本通讯程序)				取消掉与文本通讯功能。否则导致GP311重启时，接收到SM510的主动发送，GP311回发，SM510收到，再次发送的死循环。
		注：在调试时可以打开与文本通讯功能，但在应用时必须注释掉此功能，否则出错！

		19. // ZCL 2016.7.5
		注: 在仿真调试时，要打开Boot_ParLst()中的读取GPRS参数功能，否则因为一下载程序就会把保存在FLASH区的GPRS 参数删掉，导致无法调试。
		   但在正常应用中，必须取消读取GPRS参数，防止参数未知的初始化。

		20. // ZCL 2017.5.25  -- 2017.5.31
		注: (1) case 6来测信号质量，原先case 6后移，信号质量必须>=8。用NET灯来指示信号质量，闪烁代表信号不行。
				用0x13:空闲下线时间间隔  来代表信号质量.
				(2)信号质量不好的时候，NET闪烁
				SM510：600秒主动发送； SM510：600秒给GPRS断电上电（信号不好时）

				(3)  下面的重启都会重新检测信号质量。
				（注：时刻检测CSQ信号质量重启没有加，编写会比较繁琐，引起故障）
				（注：定时重启没有加，下面的重启足够多，定时重启会把好的状态也破坏重启，不好）
					DoWith中  根据如下条件重启：
					//ZCL 2017.5.26 COM1禁止接收后，80S内没有恢复接收。重启
					//.3位  =0 禁止接收
					if( (USART3->CR1 && 0x0004)==0
						|| B_Uart0Cmd8B + B_Uart0Cmd8D + B_GprsDataReturn + B_ATComStatus
						+ B_Uart0Cmd89 + B_OtherCmd
						+ HeartSendFault + LinkFault
						+ S_SendSms+S_ReadNewSms+S_SmsFull>0)

					//ZCL 2017.5.30  com1接收不到数据，660S定时重启
					//ZCL 2017.5.30  com0接收不到网络数据，660S定时重启


		//--------ZCL 2019.4.9 --- 2019.4.15  DCM220_SRC_E GPRS调试  M35模块，改善模块掉线，不重启问题。
		1.
			//有连接标志的链路，转发数据
			//ZCL 2019.4.9 改成注册成功的返回
			//else if(CGD0_LinkConnectOK[SendDataReturnNo])
			else if(ZhuCeOkFLAG[SendDataReturnNo])

		2. //2019.4.11 ZCL 仿真发现GPRS偶尔灯闪，就是不上线问题？仿真因为：S_M35==5，收不到CallReady。解决办法：用看门狗超时重启好了！
			else if(S_M35==5)
			{
				if(C_M35SendSum>95 )
				{
					S_M35 = 0x01;						//模块关机，重启
					Delay_MS(3000);					//此时间内没有喂狗，设备复位！ 2019.4.11
				}
				//S_M35 = 0x01;			//ZCL123 测试 2019.4.10
			}
		3.  次数加多
				//2015.1.17  15次改成35次，65次(2019.4.4)，防止别的卡再出现类似问题。
				else if(C_M35SendSum >65 )
					S_M35 = 0x01;

		4. //ZCL 2019.4.9 IP和端口号正确就可以建立连接。 CGD0_LinkConnectOK
				//如：0, CONNECT OK。 服务器那边软件没运行，也可以建立连接。建立连接的标志: CGD0_LinkConnectOK
				//再用心跳包来判断是否真正 建立通讯（不只是建立连接：拼通） 建立通讯的标志: ZhuCeOkFLAG

		5.后备注，很重要!!! ZCL 2019.3.15
			#define TXD0_MAX		511					// 最大发送数量
			#define RCV0_MAX		1536 				// 接收缓冲区长度 //256*6			数量很大，接收GPRS网络数据用！
			#define TXD1_MAX		511					// 最大发送数量
			#define RCV1_MAX		2048	//2176 2019.4.5 	// 接收缓冲区长度 //256*8.5		数量很大，接收Xmodem用！

		6. 修改xmodem.c的内容（见xmodem.c中的文件头注释）

		//LORA主机才有此功能。
		7. //ZCL 2019.3.12 主机GPRS模式，处理MODBUS命令，然后自己返回数据
					//注：这里使用的是Txd2Buffer，上面填充的。代替Rcv3Buffer
					if(F_GprsMasterNotToCom || F_GprsMasterToCom)
					{
						if( Txd2Buffer[0]==2 )		// 从地址检测－接收到的上位机查询指令
						{
							。。。。。。
						}
						else
							i=3;
					}

			//ZCL 2019.3.12  主机GPRS模式下，解析出指令，准备返回！

		8. (3.4) //2019.4.11 ZCL
			问题：GPRS偶尔灯闪，就是不上线？仿真发现因为：有问题重启后到S_M35==5，收不到CallReady。再让S_M35=1重启不管用。
			解决办法：用看门狗超时重启好了！
			else if(S_M35==5)
			{
				if(C_M35SendSum>95 )
				{
					S_M35 = 0x01;						//模块关机，重启
					Delay_MS(3000);					//此时间内没有喂狗，设备复位！ 2019.4.11
				}
				//S_M35 = 0x01;			//ZCL123 测试 2019.4.10
			}


		//GP311
		8. ZCL 2019.4.19	重启序号，方便打印观察问题
			加入  B_RestartNo=1;			//ZCL 2019.4.19 重启序号，方便打印观察问题
					printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
			(1) B_RestartNo=1;
					[	//4. 服务器分离终端，DTU重启指令
						else if( *(ptr2+1)==0x82 )
						{
							S_M35 = 0x01;
							B_RestartNo=1;			//ZCL 2019.4.19 重启序号，方便打印观察问题
							printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
						}		]

			(2)	B_RestartNo=2;		//0x82：重启	  GPW+82,1;
					[	//1. 短信控制DTU重启成功，和短信返回
					if(B_SmsDtuRestart==1 )
					{
						B_SmsDtuRestart=2;
						S_M35 = 0x01;						//模块关机，重启
						B_RestartNo=2;			//ZCL 2019.4.19 重启序号，方便打印观察问题
						printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
					}		]

				实验数据：短信收到，串口显示，但不重启？因为接收的指令不是ASCII码：
					[2019-04-24_00:21:06]+CMTI: "SM",12
					[2019-04-24_00:21:08]1
					[2019-04-24_00:21:08]+CMGR: "REC UNREAD","+8615610566803","","2019/04/24 00:21:06+32"
					[2019-04-24_00:21:08]004700500057002B00380032002C0031003B0009 		//ZCL ???不是ASCII码(怎么不是：GPW+82,1;)   解决：重写GPW+82,1;指令，可能指令中有特殊字符！！！

					[2019-04-24_00:21:08]OK

			(3)	B_RestartNo=3;			//0x91：模块上电重启  GPW+91,1;
					[	//2. 短信 或者故障(2014.12.8) 控制DTU硬件重启成功，和短信返回
					if(B_SmsOrFaultHardRestart==1 )
					{
						B_SmsOrFaultHardRestart=2;
						S_M35 = 0x01;						//模块关机，重启
						B_RestartNo=3;			//ZCL 2019.4.19 重启序号，方便打印观察问题
						printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
					}	]

				实验数据：
					[2019-04-23_15:28:06]+CMGR: "REC UNREAD","+8615610566803","","2019/04/23 15:28:02+32"
					[2019-04-23_15:28:06]GPW+91,1;

					//------------这个也遇到了不对的情况，上面是对的！
					[2019-04-24_00:47:24]+CMTI: "SM",24
					[2019-04-24_00:47:26]1
					[2019-04-24_00:47:26]+CMGR: "REC UNREAD","+8615610566803","","2019/04/24 00:47:21+32"
					[2019-04-24_00:47:26]004700500057002B00390031002C0031003B0009			//ZCL ???不是ASCII码  解决：重写GPW+82,1;指令，可能指令中有特殊字符！！！

					[2019-04-24_00:47:26]OK


			(5)	B_RestartNo=5;
					[	//2.5 短信看门狗复位重启 2014.12.8
						if(B_SmsWatchdogRestart==1)
						{
							B_SmsWatchdogRestart=2;
							S_M35 = 0x01;						//模块关机，重启
							B_RestartNo=5;			//ZCL 2019.4.19 重启序号，方便打印观察问题
							printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
							//ZCL 2019.4.19 Delay_MS(3000);					//此时间内没有喂狗，设备复位！ 2013.7.3
						}	]

				实验数据：
					[2019-04-24_00:36:14]+CMTI: "SM",16
					[2019-04-24_00:36:16]1
					[2019-04-24_00:36:16]+CMGR: "REC UNREAD","+8615610566803","","2019/04/24 00:36:12+32"
					[2019-04-24_00:36:16]GPW+95,1;

					[2019-04-24_00:36:16]OK
					[2019-04-24_00:36:16]RestartNo=5
					[2019-04-24_00:36:19]滮
					[2019-04-24_00:36:20]Exit Xmodem , Enter normal run mode (2)

			(6)	B_RestartNo=6，7，8
					[	//2015.1.5 加入这个，因为发现3G卡等待CallReady信号需要更长的时间，16秒了。
						//2019.4.11 ZCL 仿真发现GPRS偶尔灯闪，就是不上线问题？仿真因为：S_M35==5，收不到CallReady。解决办法：用看门狗超时重启好了！
						else if(S_M35==5)
						{
							if(C_M35SendSum>95 )
							{
								S_M35 = 0x01;						//模块关机，重启
								B_RestartNo=6;			//ZCL 2019.4.19 重启序号，方便打印观察问题
								printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
								//ZCL 2019.4.19 Delay_MS(3000);					//此时间内没有喂狗，设备复位！ 2019.4.11
							}
							//S_M35 = 0x01;			//ZCL123 测试 2019.4.10
						}
						//2015.1.17 加入这个，因为发现+CREG: 0,2 17次才出现+CREG: 0,1（注册成功）
						else if(S_M35==7)		//ZCL 2017.5.25 6改成7,因为添加case 6为信号强度检测；
						{
							if(C_M35SendSum>95 )
							{
								S_M35 = 0x01;
								B_RestartNo=7;			//ZCL 2019.4.19 重启序号，方便打印观察问题
								printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
							}
						}
						//2015.1.17  15次改成35次，65次(2019.4.18)，防止别的卡再出现类似问题。
						else if(C_M35SendSum >65 )
						{
							S_M35 = 0x01;
							B_RestartNo=8;			//ZCL 2019.4.19 重启序号，方便打印观察问题
							printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
						}
					]

			(4) 原先：B_SmsOrFaultHardRestart=11时 B_RestartNo=4	 进行修改：
					因为B_SmsOrFaultHardRestart=11 原先有很多种情况。这里分开  ZCL 2019.4.23
					产生：B_SmsOrFaultHardRestart=11,12,13,14,15,16  最后赋给 B_RestartNo=11,12,13,14,15,16
				修改如下：
			(4.1)	B_SmsOrFaultHardRestart=11
						//第2次接收到乱码（大量乱数据），就重启
						else
						{
							B_RcvFail[ChannelNo]=0;
							B_SmsOrFaultHardRestart=11;		//=11故障硬件重启; =1短信硬件重启
						}
			(4.2)	B_SmsOrFaultHardRestart=12
						//第2次出现"CONNECT FAIL"后，就重启
						else if(B_ZhuCeOkNum==0)
						{
							B_SmsOrFaultHardRestart=12;		//=11故障硬件重启; =1短信硬件重启
						}
			(4.3)	B_SmsOrFaultHardRestart=13
						//2013.6.20 接收到“ERROR”，重启
						else if( strstr(Rcv0Buffer,"ERROR")!=NULL )
						{
							if(BakRcv0Count==9 && B_ZhuCeOkNum==0)
							{
								B_SmsOrFaultHardRestart=13;		//=11故障硬件重启; =1短信硬件重启
							}
						}
			(4.4)	B_SmsOrFaultHardRestart=14
							//连网 修改24，34，44，54分别代表1,2,3,4通道的连网方式（0-UDP，1-TCP），需要重启
							else if(CommandNo==0x24 || CommandNo==0x34 || CommandNo==0x44
							|| CommandNo==0x54)
							{
								//2013.6.21 原先有连接，硬件重启
								for(j=0;j<4;j++)		//2013.6.19
								{
									if(CGD0_LinkConnectOK[j] && CommandNo==0x24+0x10*j)		//(TCP,UDP协议修改)
									{
										B_SmsOrFaultHardRestart=14;		//=14故障硬件重启; =1短信硬件重启
									}
								}
							}
			(4.5)	B_SmsOrFaultHardRestart=15
						//5. 有10次故障标志（建立连接不成功），重新启动模块。
						//如果重连接时间间隔=0，则DTU几次注册失败后，立即进行软或硬启动
						//如果重连接时间间隔!=0，则不会重启，一直到重连接时间再重连。

						//if(j==0 && B_LinkFault[i]>=10)			//
						if(B_LinkFault[i]>=10)			//2016.5.24 修改，去掉j==0
						{
							B_SmsOrFaultHardRestart=15;		//=15故障硬件重启; =1短信硬件重启
						}
			(4.6)	B_SmsOrFaultHardRestart=16
						//短信 TCP,UDP协议修改，原先有连接，硬件重启 2013.6.21 24，34，44，54分别代表1,2,3,4通道的连网方式（0-UDP，1-TCP）
						for(j=0;j<4;j++)
						{
							if(CGD0_LinkConnectOK[j] && CommandNo==0x24+0x10*j)		//()
							{
								B_SmsOrFaultHardRestart=16;		//=16故障硬件重启; =1短信硬件重启
							}
						}

		9. //ZCL 2019.4.23  去掉case 0x05: 中的 B_WaitCallReady相关程序，发送 GPW+91,1;
				重启后，可以接收到GP311的回发短信：GPW+91,1, DtuHardRestart, Set OK! ;

		10.//ZCL 2019.4.23 刚进来时，打印一次，然后延时！ 集中在这里进行打印！非常重要，避免跟串口输出冲突
				//ZCL 2019.4.19 //=5，=6 为看门狗重启，等待打印完毕。
				if(B_RestartNo>0 && S_M35==0x01 && Txd1Max==0)
				{
					if( T_WatchdogRestartDelay!=SClk10Ms )
					{
						T_WatchdogRestartDelay=SClk10Ms;

						//ZCL 2019.4.23 刚进来时，打印一次，然后延时！ 集中在这里进行打印！非常重要，避免跟串口输出冲突
						if(C_WatchdogRestartDelay==0)
							printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题

						C_WatchdogRestartDelay++;

						if ( C_WatchdogRestartDelay>200 )
						{
							C_WatchdogRestartDelay=0;
							if(B_RestartNo==5 || B_RestartNo==6)
								Delay_MS(3000);					//此时间内没有喂狗，设备复位！ 2013.7.3
							//
							B_RestartNo=0;
						}
					}
				}

		11. [2019-04-24_01:18:52]ERROR
				[2019-04-24_01:18:52]RestartNo=13

		12. ZCL 2019.4.26
			修改for(,.) 中变量，限制大小，防止数组溢出

		13. ZCL 2019.4.26
			//0x95:GP311模块看门狗复位重启 2013.7.3  //GPW+95,1;
			//ZCL 2019.4.26 看门狗重启，模块不会返回短信，因为重启了，标志没有了！

		//DCM200_SCR_E(V2)
		14. ZCL 2019.4.26  SysTick_Handler()中加入喂狗限制。因为液晶屏在上电画面中有延时，显示完不再中断中喂狗
			if (S_Display==0) 				//ZCL 2019.4.26  =1，不显示动画页面了，不再喂狗
				IWDG_Feed();						// =1 去掉中断中的喂狗	否则DTU重启就不好用了 ZCL

		15. GPW+91,1; 指令  //DCM200_SCR_E(V2)  不好用！ 但GP311好用！

				(1) DCM200_SCR_E(V2)  不好用！
				ZCL 2019.4.26  问题：GPW+91,1;  模块上电重启，实现不了，打印出B_RestartNo=3; 出现2个OK就进行不下去，
				过会出现：B_RestartNo=6; 才能转换成 收不到CallReady，看门狗超时重启，为什么？

				//短信控制DTU硬件重启
				if(B_SmsOrFaultHardRestart==1 )	//0x91：模块上电重启		GPW+91,1;
				{
					B_SmsOrFaultHardRestart=2;
					S_M35 = 0x01;						//模块关机，重启
					B_RestartNo=3;

				实验数据：
				[2019-04-25_22:55:38]+CMTI: "SM",19
				[2019-04-25_22:55:40]1
				[2019-04-25_22:55:40]+CMGR: "REC UNREAD","+8615610566803","","2019/04/25 22:55:38+32"
				[2019-04-25_22:55:40]GPW+91,1;

				[2019-04-25_22:55:40]OK
				2019-04-25_22:55:40]RestartNo=3
				[2019-04-25_22:55:48]
				[2019-04-25_22:55:48]OK
				[2019-04-25_22:55:48]
				[2019-04-25_22:55:48]OK
				[2019-04-25_22:57:24]RestartNo=6

				(2)GP311好用！
				//GP311 正确  GPW+91,1;

				实验数据：
				[2019-04-26_23:34:22]+CMTI: "SM",45
				[2019-04-26_23:34:23]1
				[2019-04-26_23:34:23]+CMGR: "REC UNREAD","+8615610566803","","2019/04/26 23:34:23+32"
				[2019-04-26_23:34:23]GPW+91,1;

				[2019-04-26_23:34:23]OK
				[2019-04-26_23:34:23]RestartNo=3
				[2019-04-26_23:34:25]
				[2019-04-26_23:34:32]
				[2019-04-26_23:34:32]RDY
				[2019-04-26_23:34:32]
				[2019-04-26_23:34:32]+CFUN: 1
				[2019-04-26_23:34:32]
				[2019-04-26_23:34:32]OK

	*/

/* Includes ------------------------------------------------------------------*/
#include "com1_232.h"
#include "com3_232.h"
#include "Gprs_at.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include "string.h" //strstr函数：找出str2字符串在str1字符串中第一次出现的位置
#include <stdio.h>	//加上此句可以用printf
#include "sim7600ce.h"
#include <stdarg.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define DDPHEADTAILLEN 16 // DDP协议头和尾长度

// 1. 服务代码 中国移动的服务代码是*99***1#，中国联通的服务代码是#777。
// 2. APN 注：CDMA网络不是分组交换网络，因此，不需要填写该值。
uc8 Gprs_ParBootLst[288] = {
	'*', '9', '9', '*', '*', '*', '1', '#', 0, // 01(Hex,下面相同,代表指令代码序列) =服务代码8+1
	'C', 'M', 'N', 'E', 'T', 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					  // 05=  24+1 Access Point Name 接入点名称
	'1', '3', '9', '1', '2', '3', '4', '5', '6', '7', '8', 0, // 06:	11+1 DTU身份识别码
	0x01, 0x00, 0x01, 0x90, 0x01, 0x00, 0x00, 0x1E,			  // 07:	2 最大传输包长,08: 2 最后包空闲时间间隔,09:1 支持数据回执,0A: 1 自动应答,0B:2 重拨号时间间隔
	0x00, 0x00, 0x00, 0x25, 0x80, 0x03, 0x00, 0x00, 0x03,	  // 0C:1 控制台信息类型  0D串口工作方式:8 串口工作方式
	0x0A, 0x00, 0x00, 0x02, 0x00, 0x3C,						  // 0E:1 数据包分隔标识,0F:1 启用TDP协议,10:1 终端类型,  11:1 呼叫方式,  12:2 呼叫时间间隔
	0x00, 0x00, 0x07,										  // 13:2 空闲下线时间间隔, 14:1 空闲下线模式

	// 222.173.103.226	公司服务器	0xDE,0xAD,0x67,0xE2
	// 219.147.19.110	公司服务器	0xDB,0x93,0x13,0x6E		//ZCL 2019.3.13
	// 114.215.155.112 云服务器		0x72,0xD7,0x9B,0x70		//ZCL 2019.3.13

	//	0xDE,0xAD,0x67,0xE2,					0xDB,0x93,0x13,0x6E,					//20.30		IP  222.173.103.226
	//  0xDE,0xAD,0x67,0xE2,					0x00,0x00,0x00,0x00,					//20.30		IP  222.173.103.226
	47, 93, 6, 250, 39, 106, 64, 39,				// 20:4 第1路IP地址 30:4 第2路IP地址		IP  112.125.89.8	服务器IP地址
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 40:4 第3路IP地址 50:4 第4路IP地址							服务器IP地址

	0x13, 0x90, 0x27, 0x48, 0, 0, 0, 0,				// 22.32.42.52		5008=0x1390 10056=0x2748 0x1D,0x4C,   0x00,0x00,		服务器端口
	0x13, 0x89, 0x13, 0x8A, 0x13, 0x8B, 0x13, 0x8C, // 23.33.43.53		5001-5004	本地端口
	0x00, 0x00, 0x00, 0x00,							// 24.34.44.54		=0 UDP; =1 TCP
	0x01, 0x01, 0x01, 0x01,							// 25.35.45.55		=1 DDP协议; =0
	//	0x00,0x28,		 0x00,0x28,			0x00,0x28,		  0x00,0x28,		//26.36.46.56		心跳时间40S
	0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, // 26.36.46.56		心跳时间40S
	0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, // 27.37.47.57	网络检测时间间隔
	0x05, 0x05, 0x05, 0x05,							// 28.38.48.58	最大登陆DSC次数
	0x05, 0x05, 0x05, 0x05,							// 29.39.49.59	最大网络检测次数
	0x07, 0x07, 0x07, 0x07,							// 2B.3B.4B.5B	控制级别
	//	0x01,0x2C,		 0x01,0x2C,			0x01,0x2C,			0x01,0x2C,		//2C.3C.4C.5C	重连接时间间隔
	0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, // 2C.3C.4C.5C	重连接时间间隔
	// 60.61.62 通道工作模式 TCP保活时间间隔 DNS地址
	0x02, 0x00, 0x05, 0xCA, 0x60, 0x86, 0x85, // 202.96.134.133
	0x00, 0x00,								  // 63 PPP保活时间间隔

	/* 	'w' , 'n', 'c', 's', '.', 's', 'a', 'n', 'l', 'e', 					//21	域名
		'y' , '.', 'c', 'n',   0,   0,   0,   0,   0,   0,
		   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,	 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 21	域名
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 31	域名
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 41	域名
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 51	域名

};

// GPRS 指令序号
uc8 CmdNo[75] = {
	0x01,
	0x05,
	0x06,
	0x07,
	0x08,
	0x09,
	0x0A,
	0x0B,
	0x0C,
	0x0D,
	0x0E,
	0x0F,
	0x10,
	0x11,
	0x12,
	0x13,
	0x14,
	0x20,
	0x30,
	0x40,
	0x50,
	0x22,
	0x32,
	0x42,
	0x52,
	0x23,
	0x33,
	0x43,
	0x53,
	0x24,
	0x34,
	0x44,
	0x54,
	0x25,
	0x35,
	0x45,
	0x55,
	0x26,
	0x36,
	0x46,
	0x56,
	0x27,
	0x37,
	0x47,
	0x57,
	0x28,
	0x38,
	0x48,
	0x58,
	0x29,
	0x39,
	0x49,
	0x59,
	0x2B,
	0x3B,
	0x4B,
	0x5B,
	0x2C,
	0x3C,
	0x4C,
	0x5C,
	0x60,
	0x61,
	0x62,
	0x63,
	0x21,
	0x31,
	0x41,
	0x51,
	0x70,
	0x71,
	0x73,
	0x74,
	0x75,
	0x00,
}; // 补个0，为了判断结束

// GPRS 指令长度
uc8 CmdLen[75] = {
	8,
	24,
	11,
	2,
	2,
	1,
	1,
	2,
	1,
	8,
	1,
	1,
	1,
	1,
	2,
	2,
	1,
	4,
	4,
	4,
	4,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	1,
	2,
	4,
	2,
	31,
	31,
	31,
	31,
	7,
	8,
	19,
	6,
	5,
	0,
}; // 补个0，为了判断结束

// GPRS 指令保存位置 (与上面GprsCmdNo对应)
uc16 CmdPst[75] = {0, 9, 34, 46, 48, 50, 51, 52, 54,
				   55, 63, 64, 65, 66, 67, 69, 71,
				   72, 76, 80, 84, 88, 90, 92, 94,
				   96, 98, 100, 102, 104, 105, 106, 107,
				   108, 109, 110, 111, 112, 114, 116, 118,
				   120, 122, 124, 126, 128, 129, 130, 131,
				   132, 133, 134, 135, 136, 137, 138, 139,
				   140, 142, 144, 146, 148, 149, 151, 155,
				   157, 188, 219, 250, 0, 0, 0, 0,
				   0, 0};

// GPRS命令值两个字节的命令列表
uc8 GprsCmd2Byte[28] = {0x07, 0x08, 0x0B, 0x12, 0x13, 0x22, 0x32, 0x42, 0x52,
						0x23, 0x33, 0x43, 0x53, 0x26, 0x36, 0x46, 0x56,
						0x27, 0x37, 0x47, 0x57, 0x2C, 0x3C, 0x4C, 0x5C,
						0x61, 0x63, 0X00}; // 补个0，为了判断结束

// GPRS 发送注册心跳包
uc8 GPRS_ZhuCe[22] = {0x7B, 0x01, 0x00, 0x16, 0x31, 0x33, 0x39, 0x31, 0x32, 0x33, 0x34, 0x35,
					  0x36, 0x37, 0x38, 0x0A, 0x66, 0x9A, 0x00, 0x0F, 0xA2, 0x7B}; //,0x1A

// GPRS 查询返回的起始头和01-04项内容
uc8 GPRS_ServiceCode[31] = {
	0x7B,
	0x0B,
	0x01,
	0x6C,
	0x31,
	0x33,
	0x39,
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x36,
	0x37,
	0x38, // 长度和DTU身份识别码号 15个，还有一个0x7B
	0x01,
	0x0A,
	0x2A,
	0x39,
	0x39,
	0x2A,
	0x2A,
	0x2A,
	0x31,
	0x23, // 01代表服务代码
	0x02,
	0x02, // 02代表PPP用户名
	0x03,
	0x02, // 03代表PPP密码
	0x04,
	0x02,
}; // 04表示SIM PIN，未设置

// GPRS 设置参数返回
uc8 GPRS_SetParReturn[16] = {
	0x7B,
	0x0D,
	0x00,
	0x10,
	0x31,
	0x33,
	0x39,
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x36,
	0x37,
	0x38,
	0x7B,
};

// GPRS 发送数据帧头
uc8 GPRS_ZhenTou[16] = {0x7B, 0x09, 0x00, 0x10, 0x31, 0x33, 0x39, 0x31, 0x32, 0x33,
						0x34, 0x35, 0x36, 0x37, 0x38, 0x7B};

uc8 DtuProgVersion[7] = "5.30";		 // 7 DTU程序版本:70		修改完程序后，要改这2个地方 YLS 2023.12.06
uc8 DtuProgMakeDate[8] = "20250205"; // 8 DTU程序生成日期:71		修改完程序后，要改这2个地方 YLS 2023.12.06

uc8 DtuHardWare[14] = "V5.3-F103-4G  "; // 14+7 DTU硬件平台版本:73
uc8 DtuManufacture[6] = "SANLEY";		// 6 DTU生产商LOGO:74
uc8 DtuModel[6] = "ZGP331";				// 6 DTU型号描述:75

uc8 SetOK[9] = " Set OK! ";							 // 设置参数成功
uc8 SetFail[11] = " Set Fail! ";					 // 设置参数失败
uc8 ParameterError[18] = " Parameter Error! ";		 // 参数错误
uc8 CommandError[16] = " Command Error! ";			 // 命令错误
uc8 PasswordError[17] = " Password Error! ";		 // 密码错误
uc8 ReadOK[10] = " Read OK! ";						 // 读参数成功
uc8 Parameter[12] = " Parameter: ";					 // 参数
uc8 DtuRestartOK[12] = " DtuRestart,";				 // 短信DTU重启 OK
uc8 DtuHardRestartOK[16] = " DtuHardRestart,";		 // 短信DTU硬件重启 OK
uc8 SmsControlStopOK[16] = " SmsControlStop,";		 // 短信控制设备停止 OK
uc8 SmsControlRunOK[15] = " SmsControlRun,";		 // 短信控制设备运行 OK
uc8 SmsWatchdogRestart[20] = " SmsWatchdogRestart,"; // 短信控制看门狗复位 OK

uc8 BaudRateError[21] = " BaudRate Par Error! ";					// BaudRate参数错误
uc8 DataBitLengthError[26] = " DataBitLength Par Error! ";			// DataBitLength参数错误
uc8 StopBitLengthError[26] = " StopBitLength Par Error! ";			// StopBitLength参数错误
uc8 ParityBitLengthError[28] = " ParityBitLength Par Error! ";		// ParityBitLength参数错误
uc8 FlowControlError[24] = " FlowControl Par Error! ";				// FlowControl参数错误
uc8 SerialPortItemLackError[30] = " Serial Port Item Lack Error! "; // 设置串行口项缺少错误
uc8 StringlengthError[31] = " String Length Overlong Error! ";		// 字符串长度设置错误
uc8 ZhuDongSendSMS[21] = " This is a test SMS! ";					// 主动发送短信内容

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uchar Txd3Buffer[TXD3_MAX];		 // 发送缓冲区
char Rcv3Buffer[RCV3_MAX];		 // 接收缓冲区
char Rcv3_Back_Buffer[RCV3_MAX]; // 接收缓冲区
uchar Txd3TmpBuffer[TXD3_MAX];	 // ZCL 2019.3.14
uint Cw_Txd3TmpMax;				 // ZCL 2019.3.14

uint Cw_Rcv3;		 // 接收计数器//
uint Cw_Txd3;		 // 发送计数器//
uint Cw_BakRcv3;	 // 接收计数器//
uint Cw_Txd3Max;	 // 有多少个字符需要发送//
uint Lw_Txd3ChkSum;	 // 发送校验和，lo,hi 两位//
uint Lw_Com3RegAddr; // 串口3寄存器地址
//
uchar B_Com3Cmd03;
uchar B_Com3Cmd16;
uchar B_Com3Cmd01;
uchar B_Com3Cmd06;
uchar B_Com3Cmd73;
uchar T_NoRcv3; // 没有接收计数器
uint C_NoRcv3;

// GPRS参数
uint C_M35Step;			// M35模块步骤进入 2012.7.20
uchar ComeSmsPhone[11]; // 来短信的电话号码
uchar S_M35PowerOnOff;	// s步骤：M35上电断电
uchar S_M35;			// s步骤：M35流程

uchar AT_ComOk_Flag; // 收到OK字符的标志
uchar AT_CIPSEND_Flag;
uchar Net_Error_Flag;	   // 网络关闭标志
uchar AT_3EOk_Flag;		   // 0x3E='>'，回显标志（准备输入发送内容）
uchar AT_CallReadyOk_Flag; // 收到Call Ready标志
uint C_M35PowerOnOff;	   // M35上电断电计数器
uint C_M35SendSum;		   // M35流程每步的发送数量
uchar ChannelNo;		   // 应答通道,改成数值
uchar B_Com3Cmd8B;		   // 查询DTU参数命令 2012.8.17
uchar B_Com3Cmd8D;		   // 设置DTU参数命令 2012.8.24
uchar B_Com3Cmd89;		   // 服务器发送数据指令 2011.8.24
uchar B_OtherCmd;		   // 上位机通过DTU给串口2的其他指令（需发送到串口2）
uchar B_DoWithData;		   // 处理数据标志
uchar S_DtuInquireSend;	   // DTU查询发送步骤 2012.8.19
uchar S_DtuSetSend;		   // DTU设置步骤 		2012.8.24
uchar S_GprsDataSend;	   // Gprs数据发送 		2012.8.24

uint w_StringLength;	   // 字符串长度
uchar StringBuffer[12];	   // BCD码转成字符串缓存。主要对IP地址和长度进行转换
uchar AccessPointLength;   // 05指令		2012.8.20
uchar DtuNoLength;		   // 06指令
uchar DomainName0Length;   // 21(Hex)指令长度
uchar DomainName1Length;   // 31(Hex)指令长度
uchar DomainName2Length;   // 41(Hex)指令长度
uchar DomainName3Length;   // 51(Hex)指令长度
uint w_Txd3Number;		   // 串口3发送数量
uint C_HeartDelay[4];	   // 心跳包发送延时计数器
uchar B_HeartSendAsk[4];   // 发送心跳包请求标志
uchar B_HeartSendFault[4]; // 发送心跳包故障标志
uchar S_HeartSendNow[4];   // 发送心跳包 步骤
uint C_HeartSendNow[4];	   // 发送心跳包 计数器
uchar S_HeartSend;		   // s步骤：心跳包发送

uint C_LinkDelay[4];			 // 连接发送延时计数器  2012.10.4
uchar B_LinkAsk[4];				 // 发送连接请求标志
uchar B_LinkAsk_DomainName[4];	 // 发送连接请求标志
uchar B_LinkFault[4];			 // 发送连接故障标志
uchar S_LinkNow[4];				 // 发送连接 步骤
uchar S_LinkNow_DoName[4];		 // 发送连接 步骤
uint C_LinkNow[4];				 // 发送连接 计数器
uint C_LinkNow_DoName[4];		 // 发送连接 计数器
uchar S_SendLink;				 // 发送连接步骤
uchar S_SendLink_DoName;		 // 发送连接步骤
uchar B_FirstLink[4];			 // 第一次连接标志
uchar B_FirstLink_DomainName[4]; // 第一次连接标志

uchar SendHeartNo; // 发送心跳包序号 0-4路，循环检测条件
uchar ConnectNum;  // 连接数量
uchar ConnectNum_DomainName;
uchar CGD0_LinkConnectOK[6]; // 场景0连接标志
uchar F_IPOK[6];			 // 场景0连接标志
uchar CGD1_LinkConnectOK[6]; // 场景1连接标志
uchar ZhuCeOkFLAG[6];		 // 注册成功标志
uchar CGD0_ConnectNo;		 // 场景0连接序号
uchar DomainName_No;		 // 域名解析序号
uchar CGD1_ConnectNo;		 // 场景1连接序号
uchar SendDataReturnNo;		 // 发送数据返回序号(串口过来的数据发到每个连接)
uint C_DtuInquireSend;		 // DTU查询指令计数器
uint C_DtuSetSend;			 // DTU设置指令计数器
uint C_GprsDataSend;		 // Gprs数据传输计数器
uint w_NewSmsNo;			 // 新的短信序号
uint w_SmsMaxSum;			 // SIM卡可以装下的最大容量短信
uint w_SmsNowSum;			 // 当前SMS数量

uchar CommandNo;	   // 命令序号
uchar SendSmsBuf[150]; // 发送短信缓存！ 周成磊 2013.12.15 长度太小，溢出就死机了，尽量大点
uchar SendSmsLength;   // 发送短信长度

uchar S_SendSms;		// 发送短信步骤
uint C_SendSms;			// 发送短信计数器
uchar S_ReadNewSms;		// 读新短信步骤
uint C_ReadNewSms;		// 读新短信计数器
uchar S_SmsFull;		// 短信满步骤
uint C_SmsFull;			// 短信满计数器
uchar SmsFull;			// 短信满标志
uchar B_SmsSetup;		// 短信设定参数
uchar B_GprsDataReturn; // GPRS 数据返回。有连接，收到串口2数据，就通过DTU发送出去

uchar B_SmsDtuRestart;		   // 短信重启				这三个变量不能在M35_Init()中初始化
uchar B_SmsOrFaultHardRestart; // 短信或者故障硬件重启
uchar B_SmsWatchdogRestart;	   // 短信看门狗复位重启

uchar B_SmsRunStop;				 // 短信控制启停（设备停机）
uchar SmsControlStopValue;		 // 短信控制停止的值（设备停机） =1停止；=0启动
uint C_DelayRunGprsModule;		 // 延时运行GPRS模块标志
uchar B_CloseLink[4];			 // 关闭连接标志，然后延时，重连接时间到后，再重新连接
uchar *ptr, *ptr1, *ptr2, *ptr3; // 指针
uchar Csq[10];					 // 存放信号强度
uchar B_ATComStatus;			 // AT命令状态 2013.5.20
uchar B_ConnectFail[4];			 // 接收到CONNECT FAIL次数计数器
uchar B_NextS35Cmd;				 // 执行下一个S35命令
uchar B_ZhuCeOkNum;				 // 注册OK的数量
uchar B_RcvFail[4];				 // 接收到乱码错误
uint C_SmsRunStop;				 // 短信运行停止计数器
uchar BKCom1TxBuf[8];			 // 备份Com1发送缓存
uchar B_SendSmsRunStopCmdCount;	 // 发送短信运行停止命令次数
uchar B_ZhuDongSendSms;			 // 主动发送短信(收到连接4的域名带@电话号码后) 2014.12.7

uint C_BetweenDtuInquireSend; // 发送前延时3MS，收不到>标志延时300MS重发 2014.12.11
uint C_BetweenDtuSetSend;	  // 发送前延时3MS，收不到>标志延时300MS重发
uint C_BetweenSendDataReturn; // 发送前延时3MS，收不到>标志延时300MS重发
uint C_BetweenHeartSendNow;	  // 发送前延时3MS，收不到>标志延时300MS重发
uint C_BetweenSendSms;		  // 发送前延时3MS，收不到>标志延时300MS重发

uchar B_WaitCallReady;		  // ZCL 2018.12.12
uchar B_RestartNo;			  // ZCL 2019.4.19  重启序号，方便打印观察问题
uchar T_WatchdogRestartDelay; // ZCL 2019.4.19 看门狗重启延时
uint C_WatchdogRestartDelay;  // ZCL 2019.4.19 看门狗重启延时
u8 p1_TmpBuf[100] = {0};	  // 缓存2
u8 p_TmpBuf[512] = {0};		  // 缓存1
u8 *p_Txd1Buffer;
u8 domain_name[31] = {0};
/* Private variables extern --------------------------------------------------*/

extern uchar Txd2Buffer[];
extern uint Cw_Txd2;	// 发送计数器
extern uint Cw_Txd2Max; // 有多少个字符需要发送

extern uchar Rcv2Buffer[];
extern uint Cw_BakRcv2;		// 备份接收计数器
extern uchar B_ForceSavPar; // 标志
extern uchar B_PcSetGprs;	// 2013.6.6
extern uint Lw_GprsTmp;
extern uint C_PowenOnDelay;
extern uint C_Com3NoRcv; // COM0 没有接收网络数据计数器 ZCL 2017.5.30

extern u8 B_LoraSendWrite; // ZCL 2021.11.16 LORA 写命令的发送
extern u8 B_LoraSendWriteLength;
extern uint8_t LoRaTxBuf2[]; // ZCL 2021.11.18
extern u8 Txd1Buffer[];		 // 发送缓冲区
extern u16 Cw_Txd1;			 // 发送计数器//
extern u16 Cw_Txd1Max;		 // 有多少个字符需要发送//
extern u8 Txd2Buffer_TEMP[];
extern u8 Txd1Buffer_TEMP[];

extern void UART3_Output_Datas(uint8_t S_Out[], uint16_t Counter);
extern void Fill_data(void);
extern unsigned int atoi(unsigned char *s, unsigned char sz);
extern void delay_ms(vu16 nCount);
extern void LCD_DLY_ms(u32 Time);
extern void At_DomainName(u8 i);
// char *tcp_ip = "AT+CIPOPEN=0,\"TCP\",\"119.23.42.226\",8050\r\n";	         // 修改服务器IP和端口号
// char *tcp_ip = "AT+CIPOPEN=0,\"TCP\",\"112.125.89.8\",36987\r\n"; // 修改服务器IP和端口号

// uint8_t lte_power_on_flag = 0, lte_init_flag = 0, lte_net_flag = 0, lte_connect_flag = 0; // lte初始化完成标志，网络配置完成标志，连接成功标志
// uint8_t err_cnt = 0, err_num = 0;														  // ERROR 计数，错误值
// uint8_t heart_beat_flag = 0;															  // 心跳标志

/* Private function prototypes -----------------------------------------------*/
void GPIO_Com3_Configuration(void); // GPIO配置
void Com3_config(void);
uint CRC16(uchar *pCrcData, uchar CrcDataLen);
void Delay_MS(vu16 nCount);
void Enable_USART2_NVIC(void);
void Disable_USART2_NVIC(void);
void Com2_printf(char *fmt, ...);
void Com1_printf(char *fmt, ...);
void Com1_sendData(uint8_t txd_buffer[], uint16_t send_num);
void UART1_Output_Datas(uint8_t S_Out[], uint8_t Counter);
void UART2_Output_Datas(uint8_t S_Out[], uint8_t Counter);
void Send_DomainName(void);
void Fill_Dtu_Par(void);

// static uint16_t tim_cnt = 0;                         // 定时器计数
// static uint8_t heart_beat_cnt = 0;                   // 心跳计数
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : GPIO_Com3_Configuration
 * Description    : Configures the different GPIO ports.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Com3_Configuration(void) // 串口GPIO配置
{
	GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体

	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// ZCL 2018.11.5
}

/* USART3  configuration ------------------------------------------------------*/
/* USART  configured as follow:
		- BaudRate = 9600 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
				RTS/CTS协议(Request To Send/Clear To Send)即请求发送/允许发送协议
*/
void Com3_config(void) // 串口通讯配置
{
	USART_InitTypeDef USART_InitStructure;
	USART_DeInit(USART3);					// 复位 USART3
	USART_StructInit(&USART_InitStructure); // 把USART_InitStruct中的每一个参数按缺省值填入

//	Pw_GprsBaudRate3 = 57600;					 // 57600;19200;
	USART_InitStructure.USART_BaudRate = 115200; // 2.25M/BPS   RS232芯片:230.4kbps  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure 串口1-USART3 */
	USART_Init(USART3, &USART_InitStructure);
	/* Enable USART3 Receive and Transmit interrupts */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	/* Enable the USART3 */
	USART_Cmd(USART3, ENABLE);
}

/*******************************************************************************
 * Function Name  : Com3_Init
 * Description    : Initializes the peripherals used by the Usart driver.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Com3_Init(void) // 串口初始化
{
	GPIO_Com3_Configuration();
	Com3_config();
	//
	Cw_Txd3 = 0;
	Cw_Rcv3 = 0;
	//
	Delay_MS(35); // 2013.6.22 建议VBAT上电到管脚PWRKEY拉低之间的时间间隔为30MS
}

// 2007.10.29
// 接收处理程序 校验程序
void Com3_RcvProcess(void) // 接收处理程序 校验程序
{
	u8 i = 0; // 临时变量 k,s,
	u16 j, m;
	uchar s; // ttp:transparent protocol 透明协议
	uint k, len1, len2;
	u16 *p_wRead;
	u8 *p_bMove;
	u16 *p_wTarget;
	//	u8 ipbuf[15]; // IP缓存

	/* 	u8	 *p_bGen;
		u16	 *p_wTarget;			// 指向目标字符串　xdata zcl  2021.11.17	 */

	// 作为主机,指定接收时间到了,就可以处理接收到的字符串了
	//  在没收到串行字符的时间超过设定时，可以对接收缓存进行处理了
	//  **********************************rcv_counter<>0,收到字符才能处理
	// 2012.10.13 加入Cw_Txd2==0的限制
	if (Cw_Rcv3 != 0 && T_NoRcv3 != SClk1Ms) //&& Cw_Txd2 == 0
	{										 // 接收处理过程
		// Com3_CON=0;
		T_NoRcv3 = SClk1Ms; //
		C_NoRcv3++;
		if (C_NoRcv3 > NORCVMAXMS) // && Cw_Rcv3!=1 && Rcv3Buffer[0]!=0)
		{
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); // 禁止USART3串口中断,防止在处理期间更改
			//
			usart3_rev_finish = 1;
			Rcv3Buffer[Cw_Rcv3] = 0; // 2013.5.9 周成磊 接收到的数据，末尾加0！
			// if (GprsPar[ConsoleInfoTypeBase])
			// 	Com2_printf("Receive Data：\r\n %s\r\n", Rcv3Buffer);

			Cw_BakRcv3 = Cw_Rcv3; // 把 Cw_Rcv3 保存
			Cw_Rcv3 = 0;		  // 准备下次接收到缓存开始
			C_NoRcv3 = 0;		  // 清没有接收计数器
			//
			if (Cw_BakRcv3 <= RCV3_MAX) // 接收长度正确,继续处理.
			{							//;开始处理串口信息
										//;通过后,进行应答,回发数据 (本机是子机)
				if (S_M35 < 6)
				{
					if (strstr(Rcv3Buffer, "PB DONE") != NULL) //"Call Ready"
					{
						AT_CallReadyOk_Flag = TRUE;
					}
				}

				// 周成磊 2012.7.11 GPRS设置模式下返回数据的处理
				switch (S_M35)
				{
				case 0x01: // 必须加上，否则ES0=0不再进入中断
					break; // 周成磊 2012.9.12
				case 0x03: // 等待通信成功  41 54 0D 0D 0A 4F 4B 0D 0A(AT...OK..)
				case 0x04: // 设置AT命令回显
					goto ComOkCheck;

				case 0x05: // 等待Call Ready信号到达		2012.9.12
					/* 											if(B_WaitCallReady==0)			//ZCL 2019.4.23 删除  否则短信重启，不返回短信，并且重启2遍
																{
																	if(strstr(Rcv0Buffer,"OK")!=NULL )
																	{
																		AT_ComOk_Flag = TRUE;
																		B_WaitCallReady=1;
																	}
																} */

					if (strstr(Rcv3Buffer, "OK") != NULL) //"Call Ready"PB DONE
					{
						AT_CallReadyOk_Flag = TRUE;
						AT_ComOk_Flag = TRUE;
					}
					break;

					// 2017.5.25修改成检查信号强度，case 0x40 改成case 0x06，后面case 号顺便增加 (6-E改成7-F)
				case 0x06:												   // 0D,0A, +CSQ: 23,0(一般15以上就可以了)
					if ((ptr1 = (u8 *)strstr(Rcv3Buffer, "+CSQ")) != NULL) // ZCL 再加信号强度检测
					{
						ptr = (u8 *)strstr(Rcv3Buffer, ",");
						if (ptr != NULL)
						{
							i = 0;
							while (*ptr1 != 0x2c) //'\,'
							{
								Csq[i++] = *ptr1++;
							}
							if (*(ptr - 2) == 0x20) // 空格
							{
								w_GprsCSQ = *(ptr - 1) - 0x30;	  //'0'
								GprsPar[OfflineIntervalBase] = 0; // ZCL 2017.5.30 用空闲下线时间间隔来代表信号质量
								GprsPar[OfflineIntervalBase + 1] = w_GprsCSQ;
							}
							else
							{
								w_GprsCSQ = (*(ptr - 2) - 0x30) * 10 + *(ptr - 1) - 0x30;
								GprsPar[OfflineIntervalBase] = 0; // ZCL 2017.5.30 用空闲下线时间间隔来代表信号质量
								GprsPar[OfflineIntervalBase + 1] = w_GprsCSQ;
							}
						}
						else
							w_GprsCSQ = 0;

						if (w_GprsCSQ >= 8 && w_GprsCSQ != 99)
							AT_ComOk_Flag = TRUE;
					}

				case 0x07: // 等待注册网络
					ptr = (u8 *)strstr(Rcv3Buffer, "+CREG:");
					if (ptr != NULL)
					{
						if (*(ptr + 9) == '1' || *(ptr + 9) == '5')
							AT_ComOk_Flag = TRUE;
					}
					break;

				case 0x08: // 设置波特率

				// 短信初始化
				case 0x09: // 选择短消息存储器
				case 0x0A: // 配置短消息模式 		=0 PDU； =1 文本
				case 0x0B: // 选择 TE字符集
				case 0x0C: // 短信提示设置
				case 0x0D: // 设置文本格式参数
				case 0x0E: // 显示文本格式参数 =0显示部分参数 =1显示全部参数
					goto ComOkCheck;
				case 0x0F: // 确认短信息存储器,并查询容量
					// 0D 0A +CPMS: "SM",19,40,"SM",19,40,"SM",19,40
					// OK
					if (strstr(Rcv3Buffer, "+CPMS: ") != NULL)
					{
						AT_ComOk_Flag = TRUE;

						ptr = (u8 *)strstr(Rcv3Buffer, ","); // 查找第一个','
						if (ptr != NULL)
						{
							// 接收到的10进制 ASCII字符串：19 就是 0x31,0x39
							w_SmsNowSum = Str2Bcd(ptr + 1);
							ptr2 = (u8 *)strstr((char *)(ptr + 1), ","); // 查找第二个','
							if (ptr2 != NULL)
							{
								w_SmsMaxSum = Str2Bcd(ptr2 + 1);
							}
						}
						// w_SmsMaxSum>=20 起码SIM卡短信存储量超过10条
						if (w_SmsNowSum >= w_SmsMaxSum - 5 && w_SmsMaxSum >= 10)
							SmsFull = 1;
					}
					break;

				// GPRS
				case 0x10: // GPRS附着
				case 0x11: // 配置前置场景
				case 0x12: // 设置GPRS连接方式 0:CSD连接	1:GPRS连接
				case 0x13: // 设置服务器地址为IP方式
				case 0x14: // 查看数据显示格式 1：加上IP及端口号
				case 0x15: // 查看数据显示格式 1：加上IPD<Len>
				case 0x16: // 查看数据显示格式 1：加上协议类型
				case 0x17: // 0:单路连接   1:多路连接
				case 0x18: //=1使用透传模式 =0非透传
				case 0x19: // 收到串口数据后等待200ms发送，或串口数据达到512字节后，
				case 0x1A: // 设置AT命令回显
				ComOkCheck:
					if (strstr(Rcv3Buffer, "OK") != NULL)
					{
						AT_ComOk_Flag = TRUE;
					}
					break;

				case 0x1B:
					// 一.DTU接收到的数据处理	（用的次数更多一些，放到前面）
					// 		二.建立多路连接指令结果返回（UDP OR TCP）
					// 			UDP连接上发送心跳注册包：1先发指令.通道号.长度 At +
					// 	QISEND = x,
					// 	Y 2后发内容 三.短信接收 2012.9.19

					// 	一.DTU接收到的数据处理
					ptr = (uchar *)strstr(Rcv3Buffer, "RECV FROM:");

					if (ptr != NULL)
					{
						Com3_ReceiveData(); // YLS 2023.02.20
						// ChannelNo = *(ptr + 10) - 0x30; // 应答通道,改成数值

						// 接收到的带协议内容总长度  如：+RECEIVE: 0, 16
						// len1 = Str2Bcd(ptr + 13); // 字符串转10进制值
						// 2013.6.19 偶尔会在一个接收后，后面又跟一个大段数据接收乱码，检测到先关连接
						// 2013.6.19 再修改：两次后，直接重启

						len1 = ReceiveGPRSBufOneDateLen + ReceiveGPRSBufTwoDateLen + ReceiveGPRSBufThreeDateLen + ReceiveGPRSBufFourDateLen;
						if (len1 >= 512)
						{
							len1 = 500; // ZCL 2019.4.26 限制大小，防止数组溢出

							if (B_RcvFail[ChannelNo] < 1)
							{
								B_RcvFail[ChannelNo]++;
								B_CloseLink[ChannelNo] = 1;
							}
							// 第2次接收到乱码（大量乱数据），就重启
							else
							{
								B_RcvFail[ChannelNo] = 0;
								B_SmsOrFaultHardRestart = 11; //=11故障硬件重启; =1短信硬件重启
							}
						}
						else
							B_RcvFail[ChannelNo] = 0;

						ptr2 = (uchar *)strstr(Rcv3Buffer, "{"); //{的位置 =0x7B
						if (ptr2 != NULL)
						{
							// 1. DTU注册	 7B后面的字节是功能码，0x81心跳包注册功能码
							if (*(ptr2 + 1) == 0x81)
							{
								ZhuCeOkFLAG[*(ptr + 10) - 0x30] = 1; // ptr+10
							}
							// 2. 查询DTU参数
							else if (*(ptr2 + 1) == 0x8B)
								B_Com3Cmd8B = 1;
							// 3. 设置DTU参数
							else if (*(ptr2 + 1) == 0x8D)
								B_Com3Cmd8D = 1;
							// 4. 服务器分离终端，DTU重启指令
							else if (*(ptr2 + 1) == 0x82)
							{
								S_M35 = 0x01;
								B_RestartNo = 1; // ZCL 2019.4.19 重启序号，方便打印观察问题
												 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
							}

							// 2. 上位机通过DTU给串口2的指令(可能不需要返回数据)
							// IPD17UDP:7B 89 00 10 31 33 39 31 32 33 34 35 36 37 38 7B 31 32 33
							// IPD17TCP:7B 89 00 13 31 33 39 31 32 33 34 35 36 37 38 31 32 33 7B
							else if (*(ptr2 + 1) == 0x89) //&& *(ptr+i+19)==0x7B
							{
								B_Com3Cmd89 = 1;
							}
							else
								B_OtherCmd = 1; //=1，上位机通过DTU给串口2的其他指令，以后再加具体指令

							C_Com3NoRcv = 0; // ZCL 2017.5.30 COM0 没有接收网络数据计数器
						}

						// if(B_Com3Cmd89 || B_OtherCmd) 2014.12.5 移到后面
					}

					// 二.建立多路连接（UDP OR TCP）		0, CONNECT OK  'C'在Rcv3Buffer[5]
					// 有时候前面还会多一个OK(0D 0A O K 0D 0A), 'C'在Rcv3Buffer[11]
					else if ((ptr2 = (uchar *)strstr(Rcv3Buffer, "+CIPOPEN:")) != NULL) // YLS 2023.02.20
					{
						s = *(ptr2 + 10); // YLS 2023.02.20
						if (s >= 0x30)
						{
							// 这里原先使用 CGD0_ConnectNo，在测试状态会不对，因为是通过串口助手发
							// 指令，这里改成用接收的数据来判断连接号
							CGD0_LinkConnectOK[s - 0x30] = TRUE;
							// S_SendLink=0; 2013.6.19
						}
					}

					// UDP连接上发送心跳注册包，1先发指令.通道号.长度
					else if (strstr(Rcv3Buffer, ">") != NULL)
					{
						AT_3EOk_Flag = TRUE;
					}

					else if ((ptr2 = (uchar *)strstr(Rcv3Buffer, "+CIPSEND:")) != NULL) // YLS 2023.02.20
					{
						s = *(ptr2 + 10); // YLS 2023.02.20
						if (s >= 0x30)
						{
							// 这里原先使用 CGD0_ConnectNo，在测试状态会不对，因为是通过串口助手发
							// 指令，这里改成用接收的数据来判断连接号
							ZhuCeOkFLAG[s - 0x30] = TRUE;
							// S_SendLink=0; 2013.6.19
						}
						AT_CIPSEND_Flag = TRUE;
					}

					// 2013.6.19 没有连接成功“CONNECT FAIL”，关闭连接
					// else if ((ptr2 = (uchar *)strstr(Rcv3Buffer, "+CIPERROR")) != NULL)
					else if ((ptr2 = (uchar *)strstr(Rcv3Buffer, "+IPCLOSE:")) != NULL)
					{
						if ((ptr2 = (uchar *)strstr(Rcv3Buffer, ",")) != NULL)
						{
							s = *(ptr2 - 1) - 0x30;
							if (s <= 4)
							{
								CGD0_LinkConnectOK[s] = 0;
								ZhuCeOkFLAG[s] = 0;
								if (B_ConnectFail[s] < 3) // 1	ZCL 2019.4.24
								{
									B_ConnectFail[s]++;
									B_CloseLink[s] = 1;
								}
								// 第2次出现"CONNECT FAIL"后，就重启
								//								else if (B_ZhuCeOkNum == 0)
								//								{
								//									B_SmsOrFaultHardRestart = 12; //=12故障硬件重启; =1短信硬件重启 ZCL 2019.4.19
								//								}
							}
						}
					}
					else if (strstr(Rcv3Buffer, "+CDNSGIP: 1") != NULL)
					{
						DomainNameResolution();
					}
					else if (strstr(Rcv3Buffer, "+CIPERROR:") != NULL)
					{
						Net_Error_Flag = TRUE;
					}

					// UDP连接上发送心跳注册包，2后发内容 SEND OK		//+CIPSEND:
					else if (strstr(Rcv3Buffer, "OK") != NULL)
					{
						AT_ComOk_Flag = TRUE;
					}
					// 三.短信接收 2012.9.19
					//+CMTI: "SM",2  在位置2有新短信
					else if (strstr(Rcv3Buffer, "+CMTI") != NULL)
					{
						ptr = (uchar *)strstr(Rcv3Buffer, ",");
						if (ptr != NULL)
							w_NewSmsNo = Str2Bcd(ptr + 1);
						// 对总短信数量进行判断 2012.9.24
						if (w_NewSmsNo >= w_SmsMaxSum - 5)
							SmsFull = 1;
					}
					// 0D 0A +CMGR: "REC UNREAD","+8615610566803","","2012/09/18
					// 22:38:13+32" 0D 0A
					// 读来的短信内容
					else if (strstr(Rcv3Buffer, "+CMGR") != NULL)
					{
						S_ReadNewSms = 0;
						w_NewSmsNo = 0;
						// 查找来短信的号码 "+8615610566803"
						ptr2 = (uchar *)strstr(Rcv3Buffer, "+86");
						if (ptr2 != NULL)
						{
							for (s = 0; s < 11; s++) // 保存来短信的电话号码
							{
								ComeSmsPhone[s] = *(ptr2 + 3 + s);
							}
						}
						// 短信和PC设置和查询GPRS参数 !!!!
						SmsAndPC_SetupInq();
					}
					// 短信发送成功，返回+CMGS: 91  OK
					else if (strstr(Rcv3Buffer, "+CMGS") != NULL)
					{
						AT_ComOk_Flag = TRUE;
					}
					// 短信删除 2013.5.23
					else if (Rcv3Buffer[2] == 'O' && Rcv3Buffer[3] == 'K')
					{
						if (S_SmsFull == 2 && S_SendSms == 0) // AT+CMGD=1,4 刚刚发出短信全部删除指令
							AT_ComOk_Flag = TRUE;
					}
					// 2013.6.20 接收到“ERROR”，重启
					else if (strstr(Rcv3Buffer, "ERROR") != NULL) // ZCL 2019.4.24 注释来测试
					{
						if (B_ZhuCeOkNum == 0) // Cw_BakRcv3 == 9 &&
						{
							B_SmsOrFaultHardRestart = 13; //=13故障硬件重启; =1短信硬件重启 ZCL 2019.4.19
						}
					}

					// 可以加入其他内容
					else
						;

					break;
					//----------------------------

				case 0x30: // 暂存
				case 0x31: // 暂存
				case 0x32: // 暂存

					break;
				case 0x41: // 模块睡眠允许功能
					if (strstr(Rcv3Buffer, "OK") != NULL)
						AT_ComOk_Flag = TRUE;
					break;
				default:
					return;
				} // switch END

				// 周成磊 2012.7.7
				// 1.连接上，是B_Com3Cmd89.B_OtherCmd命令转发(DDP协议的需要解析,透明协议的直接传输)
				if (GprsPar[EnDDP0Base + ChannelNo] == 1 && (B_Com3Cmd89 || B_OtherCmd))
				{
					B_Com3Cmd89 = 0;
					B_OtherCmd = 0;
					B_DoWithData = 1;
				}
				else if (GprsPar[EnDDP0Base + ChannelNo] == 0 && ReceiveGPRSBuf[0] == 0x02)
				{
					B_DoWithData = 1;
				}

				if (B_DoWithData)
				{
					B_DoWithData = 0;

					// 接收到的数值长度，DDP协议下需要减16，正常情况下不用减
					len1 = ReceiveGPRSBufOneDateLen + ReceiveGPRSBufTwoDateLen + ReceiveGPRSBufThreeDateLen + ReceiveGPRSBufFourDateLen;
					if (GprsPar[EnDDP0Base + ChannelNo])
					{
						if (len1 >= 16) // ZCL 2019.4.16 加上条件
							len2 = len1 - 16;
						else
							len2 = 0;
					}
					else
						len2 = len1;

					// 2012.10.15 加入DDP协议(TCP.UDP数据格式不同)和透明协议(TCP.UDP一样)
					// 2012.10.10 加入TCP协议(=1)和UDP协议(=0)
					// UDP:7B 89 00 10 31 33 39 31 32 33 34 35 36 37 38 7B 31 32 33
					// UDP说明：7B(起始标志，固定值) 89(包类型，固定值) 00 10(包长度，2字节，固定值) 31 33 39 31 32 33 34 35 36 37 38(DTU身份识别码，11字节) 7B(结束标志，固定值) 31 32 33(用户数据，≤1024字节)
					// TCP:7B 89 00 13 31 33 39 31 32 33 34 35 36 37 38 31 32 33 7B
					// TCP说明:7B(起始标志，固定值) 89(包类型，固定值) 00 13(包长度，2字节，减0x10为实际值) 31 33 39 31 32 33 34 35 36 37 38(DTU身份识别码，11字节) 31 32 33(用户数据，≤1024字节) 7B(结束标志，固定值)
					for (k = 0; k < len2; k++) // 串口3(GPRS模块)接收到的发送到串口2
					{
						//=1 DDP协议 GprsPar[EnDDP0Base+ChannelNo]
						if (GprsPar[EnDDP0Base + ChannelNo])
						{
							//=0,UDP
							if (GprsPar[LinkTCPUDP0Base + ChannelNo] == 0)
								Txd2Buffer[k] = *(ptr2 + 16 + k);
							//=1,TCP
							else
								Txd2Buffer[k] = *(ptr2 + 15 + k);
						}
						//=0 透明协议 GprsPar[EnDDP0Base+ChannelNo]
						// IPD3UDP:***  (***是数据) 2014.11.27 补充说明
						else
						{
							// ptr3 = (u8 *)strstr(Rcv3Buffer, "P:"); // P:的位置
							// if (ptr3 != NULL)
							// {
							// 	Txd2Buffer[k] = *(ptr3 + 2 + k);
							// }
							Txd2Buffer[k] = ReceiveGPRSBuf[k]; // 直接存入Txd2Buffer
						}
					}
					memset(ReceiveGPRSBuf, 0, len1);

					// ZCL 2019.3.12 主机GPRS模式，处理MODBUS命令，然后自己返回数据
					// 注：这里使用的是Txd2Buffer，上面填充的。代替Rcv3Buffer
					if (Pw_EquipmentType == 0) // 双驱泵
					{
						if (F_GprsMasterNotToCom || F_GprsMasterToCom) // ZCL 2019.3.11 主机GPRS模式，设定输出信息到串口
						{
							if (Txd2Buffer[0] == Pw_LoRaEquipmentNo) // 从地址检测－接收到的上位机查询指令
							{
								j = CRC16(Txd2Buffer, len2 - 2); // CRC 校验
								k = j >> 8;
								s = j;
								if (k == Txd2Buffer[len2 - 2] && s == Txd2Buffer[len2 - 1])
								{							// CRC校验正确
									if (Txd2Buffer[1] == 3) // 03读取保持寄存器
									{
										B_Com3Cmd03 = 1;
										j = Txd2Buffer[2];
										Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];

										if (Lw_Com3RegAddr < 10000 || Lw_Com3RegAddr >= 60000)
										{
											Txd3TmpBuffer[0] = Pw_LoRaEquipmentNo; // 本机LoRa地址
											Txd3TmpBuffer[1] = Txd2Buffer[1];	   // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
											Txd3TmpBuffer[2] = Txd2Buffer[5] * 2;  // Rcv2Buffer[5]=字数 　

											if (Txd2Buffer[5] > 125)
												Txd2Buffer[5] = 125; // ZCL 2019.4.26 限制大小，防止数组溢出

											if (Lw_Com3RegAddr < 10000)
											{
												p_wRead = w_DNBParLst; // w_DNBParLst PAR区
												p_wRead += Lw_Com3RegAddr - DNB_ADDRESS;
											}
											else if (Lw_Com3RegAddr >= 60000)
											{
												p_wRead = w_ParLst; // w_ParLst区
												p_wRead += Lw_Com3RegAddr - 60000;
											}

											p_bMove = Txd3TmpBuffer;
											for (k = 0; k < Txd2Buffer[5]; k++) // 填充查询内容
											{
												// m = *(p_wRead + Lw_Com3RegAddr + k);// 2023.12.16 YLS
												m = *(p_wRead + k); // 2023.12.16 YLS
												*(p_bMove + 3 + k * 2) = m >> 8;
												*(p_bMove + 3 + k * 2 + 1) = m;
											}
											Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, Txd3TmpBuffer[2] + 3);
											Txd3TmpBuffer[Txd3TmpBuffer[2] + 3] = Lw_Txd3ChkSum >> 8; // /256
											Txd3TmpBuffer[Txd3TmpBuffer[2] + 4] = Lw_Txd3ChkSum;	  // 低位字节
											Cw_Txd3TmpMax = Txd3TmpBuffer[2] + 5;
											//
											B_Com3Cmd03 = 0;
											Cw_Txd3 = 0;

											// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
											B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络
										}
										else if (Lw_Com3RegAddr >= 10000 && Lw_Com3RegAddr < 60000) // 地址>10000，要通过LoRa转发，查询从机的参数
										{
											w_ZhuanFaAdd = Lw_Com3RegAddr;
											m = Txd2Buffer[4];
											m = (m << 8) + Txd2Buffer[5];
											w_ZhuanFaData = m;

											F_ModeParLora = 2; // 在LoRa主动发送中，=2，转发出去，查询某一台从机的参数
											B_Com3Cmd03 = 0;
										}
									}

									else if (Txd2Buffer[1] == 6) // 06预置单寄存器
									{
										B_Com3Cmd06 = 1;
										j = Txd2Buffer[2];
										Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];

										if (Lw_Com3RegAddr < 60000) // 如果地址在10000以下，就通过LoRa转发到下位机
										{
											w_ZhuanFaAdd = Lw_Com3RegAddr;
											m = Txd2Buffer[4];
											m = (m << 8) + Txd2Buffer[5];
											w_ZhuanFaData = m;

											if (Lw_Com3RegAddr < 10000) // 如果地址在10000以下，就通过LoRa转发到下位机
												F_ModeParLora = 1;		// 在LoRa主动发送中，=1，转发出去，写某一台从机的启停、设定压力、设定频率这3个参数
											else if (Lw_Com3RegAddr >= 10000 && Lw_Com3RegAddr < 60000)
												F_ModeParLora = 3; // 在LoRa主动发送中，=3，转发出去，写某一台从机的设定参数

											B_Com3Cmd06 = 0;
										}
										else if (Lw_Com3RegAddr >= 60000) // 如果地址在60000以上，就修改ZGP331本身的参数
										{
											// 这是预置本机的 设定参数；
											p_wTarget = w_ParLst; // 写STM32 PAR区
											p_wTarget += Lw_Com3RegAddr - 60000;
											m = Txd2Buffer[4];
											// w_ParLst[Lw_Com3RegAddr] = (m << 8) + Txd2Buffer[5];
											*p_wTarget = (m << 8) + Txd2Buffer[5]; // 修改参数

											// ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
											Txd3TmpBuffer[0] = Pw_LoRaEquipmentNo; // 设备从地址Pw_EquipmentNo
											Txd3TmpBuffer[1] = Txd2Buffer[1];	   // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
											Txd3TmpBuffer[2] = Txd2Buffer[2];	   // 　
											Txd3TmpBuffer[3] = Txd2Buffer[3];	   //
											Txd3TmpBuffer[4] = Txd2Buffer[4];	   //
											Txd3TmpBuffer[5] = Txd2Buffer[5];	   //

											Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, 6);
											Txd3TmpBuffer[6] = Lw_Txd3ChkSum >> 8; // /256
											Txd3TmpBuffer[7] = Lw_Txd3ChkSum;	   // 低位字节
											Cw_Txd3TmpMax = 8;
											//
											B_Com3Cmd06 = 0;
											Cw_Txd3 = 0;
											// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
											B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络
										}
									}
								}
							}
						}
					}
					else // 变频电机
					{
						if (F_GprsMasterNotToCom || F_GprsMasterToCom)
						{
							if (Txd2Buffer[0] == 2) // 从地址检测－接收到的上位机查询指令
							{
								j = CRC16(Txd2Buffer, len2 - 2); // CRC 校验
								k = j >> 8;
								s = j;
								if (k == Txd2Buffer[len2 - 2] && s == Txd2Buffer[len2 - 1])
								{							// CRC校验正确
									if (Txd2Buffer[1] == 3) // 03读取保持寄存器
									{
										B_Com3Cmd03 = 1;
										j = Txd2Buffer[2];
										Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];
									}
									else if (Txd2Buffer[1] == 1) // 01读取线圈状态
									{
										B_Com3Cmd01 = 1;
									}
									else if (Txd2Buffer[1] == 6) // 06预置单寄存器
									{
										// C_ForceSavPar=0;		// 强制保存参数计数器=0
										B_Com3Cmd06 = 1;
										j = Txd2Buffer[2];
										Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];

										// ZCL 2021.7.10  处理要修改的数据  ICCARD 取水机
										// ZCL 2021.11.17

										// 这是预置本机的 设定参数； 这里我们需要的是预置 LORA从机的参数，需要转发的！
										/* 									m=Txd2Buffer[4];
																			w_ParLst[Lw_Com3RegAddr]=(m<<8)+Txd2Buffer[5];	 */

										// ZCL 2021.11.17 转发指令
										B_LoraSendWrite = 1; // ZCL 2021.11.17

										//									w_ZhuanFaAdd = Lw_Com3RegAddr;
										m = Txd2Buffer[4];
										m = (m << 8) + Txd2Buffer[5];
										//				*(p_wTarget + Lw_Com2RegAddr) = m;
										//									w_ZhuanFaData = m;
										F_ModeParLora = 1;
									}

									// ZCL 2021.11.17
									else if (Txd2Buffer[1] == 16) // 16预置多寄存器
									{
										// C_ForceSavPar=0;		// 强制保存参数计数器=0
										B_Com3Cmd16 = 1;
										j = Txd2Buffer[2];
										Lw_Com3RegAddr = (j << 8) + Txd2Buffer[3];

										// 这是预置本机的 设定参数； 这里我们需要的是预置 LORA从机的参数，需要转发的！
										/* 									if ( Txd2Buffer[5] < 30 )			//ZCL 2021.11.17  限制下
																			{
																				p_bGen = Txd2Buffer;
																				p_wTarget = w_ParLst;
																				for ( k = 0; k < Txd2Buffer[5] ; k++ )		// Rcv0Buffer[5]=字数
																				{
																					m = *( p_bGen + 7 + k * 2 );
																					n = *( p_bGen + 7 + k * 2 + 1 );
																					*( p_wTarget + Lw_Com3RegAddr + k ) = ( m << 8 ) + n;
																				}
																			} */

										// ZCL 2021.11.17 转发指令
										B_LoraSendWrite = 1; // ZCL 2021.11.17
										B_LoraSendWriteLength = len2;

										for (j = 0; j < B_LoraSendWriteLength; j++) // Rcv0Buffer[5]=字数
										{
											LoRaTxBuf2[j] = Txd2Buffer[j];
										}
									}

									//								else
									//									i = 1;
								}
								//							else
								//								i = 2;
							}
							//						else
							//							i = 3;
						}

						// ZCL 2019.3.12  主机GPRS模式下，解析出指令，准备返回！
						if (B_Com3Cmd03)
						{
							Txd3TmpBuffer[0] = 2;				  // 设备从地址Pw_EquipmentNo
							Txd3TmpBuffer[1] = Txd2Buffer[1];	  // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
							Txd3TmpBuffer[2] = Txd2Buffer[5] * 2; // Rcv2Buffer[5]=字数 　

							// 转化地址 ZCL 2015.7.11

							if (Txd2Buffer[5] > 125)
								Txd2Buffer[5] = 125; // ZCL 2019.4.26 限制大小，防止数组溢出

							p_wRead = w_GprsParLst; // 读w_GprsParLst PAR区	2019.3.14
							p_bMove = Txd3TmpBuffer;
							for (k = 0; k < Txd2Buffer[5]; k++) // 填充查询内容
							{
								m = *(p_wRead + Lw_Com3RegAddr + k);
								*(p_bMove + 3 + k * 2) = m >> 8;
								*(p_bMove + 3 + k * 2 + 1) = m;
							}
							Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, Txd3TmpBuffer[2] + 3);
							Txd3TmpBuffer[Txd3TmpBuffer[2] + 3] = Lw_Txd3ChkSum >> 8; // /256
							Txd3TmpBuffer[Txd3TmpBuffer[2] + 4] = Lw_Txd3ChkSum;	  // 低位字节
							Cw_Txd3TmpMax = Txd3TmpBuffer[2] + 5;
							//
							//
							B_Com3Cmd03 = 0;
							Cw_Txd3 = 0;

							// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
							B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络

							// RS485_CON=1;
							// 2010.8.5 周成磊 TXE改成TC，一句改为两句
							// USART_SendData(USART2,Txd2Buffer[Cw_Txd2++]);
							// USART_ITConfig(USART2, USART_IT_TC, ENABLE);				// 开始发送.
						}

						// ZCL 2019.3.12 今后添加其他指令  2021.11.17
						else if (B_Com3Cmd06) // ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
						{
							B_Com3Cmd06 = 0;
							//...
							// ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
							Txd3TmpBuffer[0] = 2;			  // 设备从地址Pw_EquipmentNo
							Txd3TmpBuffer[1] = Txd2Buffer[1]; // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
							Txd3TmpBuffer[2] = Txd2Buffer[2]; // 　
							Txd3TmpBuffer[3] = Txd2Buffer[3]; //
							Txd3TmpBuffer[4] = Txd2Buffer[4]; //
							Txd3TmpBuffer[5] = Txd2Buffer[5]; //

							Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, 6);
							Txd3TmpBuffer[6] = Lw_Txd3ChkSum >> 8; // /256
							Txd3TmpBuffer[7] = Lw_Txd3ChkSum;	   // 低位字节
							Cw_Txd3TmpMax = 8;
							//
							//
							B_Com3Cmd06 = 0;
							Cw_Txd3 = 0;

							// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
							B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络

							// RS485_CON=1;
							// 2010.8.5 周成磊 TXE改成TC，一句改为两句
							// USART_SendData(USART2,Txd2Buffer[Cw_Txd2++]);
							// USART_ITConfig(USART2, USART_IT_TC, ENABLE);				// 开始发送.

							// if (B_Com2Cmd06) // 预置单个

							// else if (B_Com2Cmd16) // 预置多个
							// {
							// 	if (Rcv2Buffer[6] == 2)
							// 	{
							// 		m = Rcv2Buffer[2];
							// 		m = (m << 8) + Rcv2Buffer[3];
							// 		w_ZhuanFaAdd = m;

							// 		m = Rcv2Buffer[7];
							// 		m = (m << 8) + Rcv2Buffer[8];
							// 		w_ZhuanFaData = m;

							// 		F_ModeParLora = 1;
							// 	}
							// 	else
							// 		F_ModeParLora = 0;
							// }
						}

						// ZCL 2019.3.12 今后添加其他指令  2021.11.17
						else if (B_Com3Cmd16) // ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
						{
							B_Com3Cmd16 = 0;
							//...
							// ZCL 2021.7.10  06指令：收到的 和  返回的 是一样的。
							Txd3TmpBuffer[0] = 2;			  // 设备从地址Pw_EquipmentNo
							Txd3TmpBuffer[1] = Txd2Buffer[1]; // 功能码			ZCL 2019.3.12 这里比较特殊，用的Txd2Buffer
							Txd3TmpBuffer[2] = Txd2Buffer[2]; // 　
							Txd3TmpBuffer[3] = Txd2Buffer[3]; //
							Txd3TmpBuffer[4] = Txd2Buffer[4]; //
							Txd3TmpBuffer[5] = Txd2Buffer[5]; //

							Lw_Txd3ChkSum = CRC16(Txd3TmpBuffer, 6);
							Txd3TmpBuffer[6] = Lw_Txd3ChkSum >> 8; // /256
							Txd3TmpBuffer[7] = Lw_Txd3ChkSum;	   // 低位字节
							Cw_Txd3TmpMax = 8;
							//
							//
							B_Com3Cmd16 = 0;
							Cw_Txd3 = 0;

							// ZCL 2019.3.12 新添指令，比较重要！模仿透传中，串口收到数据，转发到GPRS网络
							B_GprsDataReturn = 1; // 模仿透传中，串口收到数据，转发到GPRS网络

							// RS485_CON=1;
							// 2010.8.5 周成磊 TXE改成TC，一句改为两句
							// USART_SendData(USART2,Txd2Buffer[Cw_Txd2++]);
							// USART_ITConfig(USART2, USART_IT_TC, ENABLE);				// 开始发送.
						}
					}

					// ZCL 2019.3.12 	对接收到的数据，进行串口输出
					if (F_GprsTransToCom) //|| F_GprsMasterToCom	YLS 2023.03.28
					{
						Cw_Txd2Max = len2;
						Cw_Txd2 = 0;

						// RS485_CON=1;		//2013.9.2
						//  2010.8.5 周成磊 TXE改成TC，一句改为两句
						// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
						USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
						USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
					}
				}

				// 2.1 (有连接，非B_Com3Cmd89.B_OtherCmd命令，串口1接收转发到串口2) 目前没有用
				// 2.2 无连接，一些AT指令返回结果，串口1接收转发到串口2 2013.5.20
				// ZCL 2019.4.23 必须等数据串口转发完，再进行printf信息输出
				else if (GprsPar[ConsoleInfoTypeBase]) // 控制台信息类型,=1输出调试信息
				{
					if (Cw_BakRcv3 >= 512)
						Cw_BakRcv3 = 500; // ZCL 2019.4.26 限制大小，防止数组溢出

					for (k = 0; k < Cw_BakRcv3; k++)
					{
						Txd2Buffer[k] = Rcv3Buffer[k]; // 串口3(GPRS模块)接收到的发送到串口1和串口2
					}

					Cw_Txd2Max = Cw_BakRcv3;
					Cw_Txd2 = 0;
					RS485_CON = 1; // 2013.9.2
					// 2010.8.5 周成磊 TXE改成TC，一句改为两句
					// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
					USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
					USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
				}
				else if (Pw_ConsoleInfo > 0) // 液晶屏参数：控制台信息类型,=1输出调试信息
				{
					if (Cw_BakRcv3 >= 512)
						Cw_BakRcv3 = 500; // ZCL 2019.4.26 限制大小，防止数组溢出

					for (k = 0; k < Cw_BakRcv3; k++)
					{
						Txd1Buffer[k] = Rcv3Buffer[k]; // 串口3(GPRS模块)接收到的发送到串口1和串口2
					}
					Cw_Txd1Max = Cw_BakRcv3;
					Cw_Txd1 = 0;
					USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 开始发送.
					USART_SendData(USART1, Txd1Buffer[Cw_Txd1++]);
				}
			}

			for (j = 0; j < Cw_BakRcv3; j++)
			{
				Rcv3_Back_Buffer[j] = Rcv3Buffer[j];
			}

			for (j = 0; j < Cw_BakRcv3; j++)
			{
				Rcv3Buffer[j] = 0;
			}

			// 开中断
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 2014.12.5 感觉在后面处理完，再开中断
		}
	}
}

void Com3_SlaveSend(void) // 串口3从机发送
{
	uint k, w;
	uchar j = 0, i, t, u, oneCmdLen, allCmdLen;
	u16 data_len;
	// uint	 *p_wRead;
	// uchar	 *p_bMove;
	// uchar	 *p_bGen;
	// uint	 *p_wTarget;			// 指向目标字符串　xdata zcl
	// uchar  *ptr;

	// ZCL 2018.12.10 删除 03 06 16指令
	if (S_HeartSend + S_SendLink + S_SendSms + S_ReadNewSms + S_SmsFull > 0)
	{
		;
	}

	// 1.查询DTU参数返回  2013.5.15修改  填充内容，计算长度
	else if (B_Com3Cmd8B && S_DtuSetSend + S_GprsDataSend == 0)
	{
		// 1.0发送AT指令，填充内容，计算长度
		if (S_DtuInquireSend == 0 && Cw_Txd3Max == 0)
		{
			S_DtuInquireSend = 1;
			C_DtuInquireSend = 0;
			C_BetweenDtuInquireSend = 0;
		}

		// 1.1发送AT指令，填充内容，计算长度
		else if (S_DtuInquireSend == 1 && Cw_Txd3Max == 0 && C_BetweenDtuInquireSend > 3)
		{
			S_DtuInquireSend = 2;
			C_BetweenDtuInquireSend = 0;

			// 填充要查询的参数，从31-353，共返回354个字节（地址从0开始，前面31个字节为固定的，DTU号另外填入）
			Fill_Dtu_Par();

			// f. AT发送指令		先发指令.通道号.长度
			Cw_Txd3Max = 0; // 2013.5.11 这一句非常重要，At_QISENDAccordingLength指令填充从0地址开始
			At_QISENDAccordingLength(ChannelNo, w_StringLength);
			// F_GPRSSendFinish = 0;
			// sim7600ceGPRSUpLoaded(Txd3Buffer, w_StringLength, 1);
		}
		// 2014.12.11 	因为遇到收不到AT_3EOk_Flag标志，所以加上延时后，重发数据
		else if (S_DtuInquireSend == 2 && C_BetweenDtuInquireSend > 300)
		{
			S_DtuInquireSend = 1; // ZCL1
			C_BetweenDtuInquireSend = 0;
		}
		// 1.2 发送前250个字节
		else if (S_DtuInquireSend == 2 && AT_3EOk_Flag) //
		{
			S_DtuInquireSend = 3;
			AT_3EOk_Flag = 0;
			// 7.重新填充前31个字节
			Cw_Txd3Max = 0;						// 2013.5.11 这一句非常重要，重新填充前31个字节
			Gprs_TX_Fill(GPRS_ServiceCode, 31); // 起始头 和 01-04
			// 8.修改长度
			Txd3Buffer[2] = (w_StringLength >> 8); // 修改长度高字节
			Txd3Buffer[3] = w_StringLength;		   // 修改长度低字节
			// 9.修改DTU身份识别码
			for (t = 0; t < 11; t++)
				Txd3Buffer[4 + t] = GprsPar[DtuNoBase + t];

			// 再次填充要查询的参数，从31-353，共返回354个字节（地址从0开始，前面31个字节为固定的，DTU号另外填入）
			Fill_Dtu_Par();

			// 数据发送
			Cw_Txd3 = 0;
			Cw_Txd3Max = 250;
			USART_SendData(USART3, Txd3Buffer[Cw_Txd3++]);
			USART_ITConfig(USART3, USART_IT_TC, ENABLE); // 改成USART_IT_TC,开始发送.
														 // UART3_Output_Datas(Txd3Buffer, Cw_Txd3Max);
		}
		// 1.3 发送后面的字节(w_StringLength-250)
		else if (S_DtuInquireSend == 3 && Cw_Txd3Max == 0)
		{
			S_DtuInquireSend = 4;
			// 数据发送
			Cw_Txd3 = 0;
			// Cw_Txd3Max=w_StringLength-250;

			// ZCL 2019.4.26 限制大小，防止数组溢出
			if (w_StringLength >= 250)
				Cw_Txd3Max = w_StringLength - 250;
			else
				Cw_Txd3Max = 0;
			if (Cw_Txd3Max >= 512)
				Cw_Txd3Max = 500;

			for (i = 0; i < Cw_Txd3Max; i++) // 移动内容
			{
				Txd3Buffer[i] = Txd3Buffer[250 + i];
			}
			USART_SendData(USART3, Txd3Buffer[Cw_Txd3++]);
			USART_ITConfig(USART3, USART_IT_TC, ENABLE); // 改成USART_IT_TC,开始发送.
			// UART3_Output_Datas(Txd3Buffer, Cw_Txd3Max);
			//
			C_BetweenDtuInquireSend = 0;
		}
		// 2014.12.11 	因为遇到收不到AT_ComOk_Flag标志，所以加上延时后，重发数据
		else if (S_DtuInquireSend == 4 && C_BetweenDtuInquireSend > 1000)
		{
			S_DtuInquireSend = 1; // ZCL2
			C_BetweenDtuInquireSend = 0;
		}
		// 1.4 发送完成，清标志
		else if (S_DtuInquireSend == 4) //&& AT_ComOk_Flag
		{
			AT_ComOk_Flag = 0;
			S_DtuInquireSend = 0;
			B_Com3Cmd8B = 0;
			// 清除不用发送心跳包标志
			C_HeartDelay[ChannelNo] = 0;
			B_HeartSendAsk[ChannelNo] = 0;
		}

		// 1.5 延时
		if (C_DtuInquireSend > 20)
		{
			S_DtuInquireSend = 0;
			B_Com3Cmd8B = 0;
		}
	}

	// 2.设置DTU参数和返回 2012.8.24
	else if (B_Com3Cmd8D && S_DtuInquireSend + S_GprsDataSend == 0)
	{
		// 设置DTU参数
		if (S_DtuSetSend == 0)
		{
			S_DtuSetSend = 1;
			C_DtuSetSend = 0;
			C_BetweenDtuSetSend = 0;

			// 0x7B(第一个数据) 如：7B 8D 00 13 31 33 39 31 32 33 34 35 36 37 38 14 03 06 7B
			//			ptr = (u8 *)strstr(Rcv3Buffer, "{"); //"{"，即0x7B

			ptr = (u8 *)strstr(Rcv3_Back_Buffer, "{"); //"{"，即0x7B
			if (ptr != NULL)
			{
				oneCmdLen = 0;
				// 设置时，不能超过255字节，这里只使用一个字节长度来运算的allCmdLen。
				allCmdLen = *(ptr + 3);			// DDP指令总字节数	如：UDP:7B 8D 00 13(13就是总字节数)
				ptr = ptr + DDPHEADTAILLEN - 1; // 指令序号 (要处理的那一组的第一个字节)

				// 需要处理的剩余字节数量（一次处理一个指令，长度和内容）
				while (allCmdLen - DDPHEADTAILLEN - oneCmdLen) // DDPHEADTAILLEN=16  :DDP协议头和尾长度
				{
					CommandNo = *ptr++;
					if (CommandNo != 0x7B) // 不是结束符
					{
						// 1.找到指令值在GPRS 参数保存序列的位置，并判断是否有效
						k = FindFirstValue(CmdNo, CommandNo, 0x00);
						if (k == 255)
							break;

						// 2.取指令长度 (要处理的那一组的第二个字节)
						t = *ptr++;

						if (t == 2) // 如果t=2，后面没有数据  如：2C 02 后面没有跟数据
						{
							GprsPar[CmdPst[k]] = 0; // 赋值为0
						}
						else if (t > 2) // ZCL 2019.4.26 + if(t>2)
						{
							// 3.取指令内容，保存 -；-2(如：14 03 06)
							for (j = 0; j < t - 2; j++)
							{
								u = *(ptr++);				// 内容
								GprsPar[CmdPst[k] + j] = u; // 进行赋值
							}
							if (!B_ZhuDongSendSms) // 2014.12.9 主动发送短信，不用保存
							{
								B_ForceSavPar = 1; // 要等待串口2发送完，给串口1发送完，才可以保存参数
								SavePar_Prompt();  // 保存参数 2012.10.11
							}

							// 4. 最后判断如果是字符串，需要在末尾加0  如：0x05：CMNET\0
							if (CommandNo == 0x01 || CommandNo == 0x05 || CommandNo == 0x06 || CommandNo == 0x21 || CommandNo == 0x31 || CommandNo == 0x41 || CommandNo == 0x51)
							{
								GprsPar[CmdPst[k] + j] = 0; // 2012.9.20 周成磊 字符串的末尾加0

								if (!B_ZhuDongSendSms) // 2014.12.9 主动发送短信，不用保存
								{
									B_ForceSavPar = 1; // 要等待串口2发送完，给串口1发送完，才可以保存参数
									SavePar_Prompt();  // 保存参数 2012.10.11
								}
								// 2013.6.14 DTU身份识别码。原先有连接，先关闭，2秒后重连
								if (CommandNo == 0x06)
								{
									for (j = 0; j < 4; j++)
									{
										if (CGD0_LinkConnectOK[j]) //(DTU身份识别码修改了)
											B_CloseLink[j] = 1;	   // 自动关闭各个连接，重启太慢！
																   // S_M35 = 0x01;							//重启
									}
								}
								// 2014.12.7 利用连接4的域名，前面带@为电话号码，主动回发短信。
								else if (CommandNo == 0x51)
								{
									if (GprsPar[DomainName3Base] == '@' && t - 2 == 12)
									{
										GprsPar[DomainName3Base] = 0; // 清@
										B_ZhuDongSendSms = 1;
									}
								}
							}
							// 20，30，40，50分别代表1,2,3,4通道DSC的IP地址
							else if (CommandNo == 0x20 || CommandNo == 0x30 || CommandNo == 0x40 || CommandNo == 0x50)
							{
								// 2013.6.11 原先有连接，先关闭，2秒后重连
								for (j = 0; j < 4; j++)
								{
									if (CGD0_LinkConnectOK[j] && CommandNo == 0x20 + 0x10 * j) //(IP地址修改了)
										B_CloseLink[j] = 1;
									//										S_M35 = 0x01;							//重启
								}
							}
							// 22，32，42，52分别代表1,2,3,4通道DSC的端口号
							else if (CommandNo == 0x22 || CommandNo == 0x32 || CommandNo == 0x42 || CommandNo == 0x52)
							{
								// 2013.6.11 原先有连接，先关闭，2秒后重连
								for (j = 0; j < 4; j++)
								{
									if (CGD0_LinkConnectOK[j] && CommandNo == 0x22 + 0x10 * j) //(端口修改了)
										B_CloseLink[j] = 1;
									//										S_M35 = 0x01;							//重启
								}
							}
							// 连网 修改24，34，44，54分别代表1,2,3,4通道的连网方式（0-UDP，1-TCP），需要重启
							else if (CommandNo == 0x24 || CommandNo == 0x34 || CommandNo == 0x44 || CommandNo == 0x54)
							{
								// 2013.6.21 原先有连接，硬件重启
								for (j = 0; j < 4; j++) // 2013.6.19
								{
									if (CGD0_LinkConnectOK[j] && CommandNo == 0x24 + 0x10 * j) //(TCP,UDP协议修改)
									{
										B_SmsOrFaultHardRestart = 14; //=14故障硬件重启; =1短信硬件重启 ZCL 2019.4.19
																	  // B_CloseLink[j] = 1;
									}
									//									S_M35 = 0x01;							//重启
								}
							}
							// 2013.12.13 周成磊 串口2的限制
							else if (CommandNo == 0x0D)
							{
								// 1.串口速率9600
								w = (uint)(GprsPar[BaudRateBase + 2]) << 8;
								w = w + GprsPar[BaudRateBase + 3];
								if (w != 2400 && w != 4800 && w != 9600 && w != 19200 && w != 38400 && w != 57600)
								{
									GprsPar[BaudRateBase + 2] = (9600 >> 8);
									GprsPar[BaudRateBase + 3] = (9600 & 0x00ff); // YLS 2023.03.12 这个地方原来是&&，应该是错了
								}
								// 2. 03 数据位为8位（00-5， 01-6， 02-7， 03-8）；（这里只能设置03）
								if (GprsPar[BaudRateBase + 4] != 0x03)
									GprsPar[BaudRateBase + 4] = 0x03;
								// 3. 00 停止位为1位（00-1，04-2）；（这里可以设置00、04）
								if (GprsPar[BaudRateBase + 5] != 0x00 && GprsPar[BaudRateBase + 5] != 0x04)
									GprsPar[BaudRateBase + 5] = 0x00;
								// 4. 00 校验位-无（00-无，08-奇，18-偶）；（这里可以设置00、08、18）
								if (GprsPar[BaudRateBase + 6] != 0x00 && GprsPar[BaudRateBase + 6] != 0x08 && GprsPar[BaudRateBase + 6] != 0x18)
									GprsPar[BaudRateBase + 6] = 0x00;
								// 5. 03 流控-无流控（01-Xon/Xoff，02-硬流控，03-无流控，04-半双工485，05-全双工422）
								// （这里只能设置02、03）
								if (GprsPar[BaudRateBase + 7] != 0x03 && GprsPar[BaudRateBase + 7] != 0x02)
									GprsPar[BaudRateBase + 7] = 0x03;
							}
						}
						// 5.返回重新执行，因为设置参数可以多条设置组合一块传过来。
						oneCmdLen += t; // 指向下一个处理位置
					}
				}
			}
		}

		// DTU返回
		else if (S_DtuSetSend == 1 && Cw_Txd3Max == 0 && C_BetweenDtuSetSend > 3)
		{
			S_DtuSetSend = 2;
			C_BetweenDtuSetSend = 0;
			// 1. AT发送指令		先发指令.通道号.长度
			// Cw_Txd3Max=0;		//判断条件中有Cw_Txd3Max==0，所以此句注释掉
			At_QISENDAccordingLength(ChannelNo, 16);
			// F_GPRSSendFinish = 0;
			// sim7600ceGPRSUpLoaded(Txd3Buffer, 16, 1);
		}
		// 2014.12.11 	因为遇到收不到AT_3EOk_Flag标志，所以加上延时后，重发数据
		else if (S_DtuSetSend == 2 && C_BetweenDtuSetSend > 300)
		{
			S_DtuSetSend = 1; // ZCL1
			C_BetweenDtuSetSend = 0;
		}

		else if (S_DtuSetSend == 2 && AT_3EOk_Flag) //
		{
			S_DtuSetSend = 3;
			AT_3EOk_Flag = 0;
			Cw_Txd3Max = 0; // 2014.12.8
			// 填充数据区
			Gprs_TX_Fill(GPRS_SetParReturn, 16); //	Cw_Txd3Max跟着变化
			// 修改DTU身份识别码
			for (t = 0; t < 11; t++)
				Txd3Buffer[4 + t] = GprsPar[DtuNoBase + t];

			// 数据发送
			Gprs_TX_Start();
			//
			C_BetweenDtuSetSend = 0;
		}
		// 2014.12.11 	因为遇到收不到AT_ComOk_Flag标志，所以加上延时后，重发数据
		else if (S_DtuSetSend == 3 && C_BetweenDtuSetSend > 1000)
		{
			S_DtuSetSend = 1; // ZCL2
			C_BetweenDtuSetSend = 0;
		}
		else if (S_DtuSetSend == 3 && AT_ComOk_Flag) //&& Rcv3Buffer[2]=='S'
		{
			S_DtuSetSend = 4;
			AT_ComOk_Flag = 0;
			C_BetweenDtuSetSend = 0;

			if (!B_ZhuDongSendSms) // 2014.12.9 主动发送短信，不用保存
			{
				B_ForceSavPar = 1; // 要等待串口2发送完，给串口1发送完，才可以保存参数
				SavePar_Prompt();  // 保存参数 2012.10.11
			}
			// 清除不用发送心跳包标志
			C_HeartDelay[ChannelNo] = 0;
			B_HeartSendAsk[ChannelNo] = 0;
		}

		// 2014.12.11 主动发短信时发现“参数设置成功”等发完短信才显示，这里加个延时不用发完短信！
		else if (S_DtuSetSend == 4 && C_BetweenDtuSetSend > 300) // MS
		{
			S_DtuSetSend = 0;
			B_Com3Cmd8D = 0;
			C_BetweenDtuSetSend = 0;
		}

		// 4. 延时
		if (C_DtuSetSend > 20)
		{
			S_DtuSetSend = 0;
			B_Com3Cmd8D = 0;
		}
	}

	// 3.远程上位机发送给DTU数据指令的返回，已经连接的每路连接都返回 2012.8.24
	else if (B_GprsDataReturn && S_DtuInquireSend + S_DtuSetSend == 0)
	{
		// 序号超过最大连接，延时5MS后，清除B_GprsDataReturn
		if (SendDataReturnNo > 3)
		{
			if (C_BetweenSendDataReturn > 5) // Ms
			{
				SendDataReturnNo = 0;
				B_GprsDataReturn = 0;
				C_GprsDataSend = 0;
				S_GprsDataSend = 0;
				/* Enable USART2 Receive interrupts */ // 2014.11.27 转发完，再开中断
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				USART2->CR1 |= 0x0004; //.3位  运行接收
			}
		}

		// 有连接标志的链路，转发数据
		// ZCL 2019.4.9 改成注册成功的返回
		else if (CGD0_LinkConnectOK[SendDataReturnNo])
		// else if (ZhuCeOkFLAG[SendDataReturnNo])
		{
			// 1. AT发送指令		先发指令.通道号.长度
			if (S_GprsDataSend == 0 && Cw_Txd3Max == 0)
			{
				S_GprsDataSend = 1;
				C_GprsDataSend = 0;
				C_BetweenSendDataReturn = 0;
			}

			else if (S_GprsDataSend == 1 && Cw_Txd3Max == 0 && C_BetweenSendDataReturn > 3)
			{
				S_GprsDataSend = 2;
				C_BetweenSendDataReturn = 0;
				// 2012.10.4 加入DDP协议和透明协议的选择
				// Cw_Txd3Max=0;		//判断条件中有Cw_Txd3Max==0，所以此句注释掉

				// ZCL 2019.3.14 Cw_BakRcv2 换成 Cw_Txd3TmpMax
				if (GprsPar[EnDDP0Base + SendDataReturnNo]) //=1，DDP协议；=0，透明协议
				{
					At_QISENDAccordingLength(SendDataReturnNo, 16 + Cw_Txd3TmpMax); // DDP协议	Cw_BakRcv2
																					// F_GPRSSendFinish = 0;
																					// sim7600ceGPRSUpLoaded(Txd3Buffer, 16 + Cw_Txd3TmpMax, 1);
				}
				else
				{
					At_QISENDAccordingLength(SendDataReturnNo, Cw_Txd3TmpMax); // 透明协议	Cw_BakRcv2
																			   // F_GPRSSendFinish = 0;
																			   // sim7600ceGPRSUpLoaded(Txd3Buffer, Cw_Txd3TmpMax, 1);
				}
			}

			// 2014.12.11 	因为遇到收不到AT_3EOk_Flag标志，所以加上延时后，重发数据
			else if (S_GprsDataSend == 2 && C_BetweenSendDataReturn > 300)
			{
				S_GprsDataSend = 1; // ZCL1
				C_BetweenSendDataReturn = 0;
			}

			// 2.数据发送
			else if (S_GprsDataSend == 2 && AT_3EOk_Flag) //
			{
				S_GprsDataSend = 3;
				AT_3EOk_Flag = 0;
				Cw_Txd3Max = 0; // 2014.12.8
				// 2012.10.4 加入DDP协议和透明协议的选择
				//=1，DDP协议
				if (GprsPar[EnDDP0Base + SendDataReturnNo]) //=1，DDP协议；=0，透明协议
				{
					Gprs_TX_Fill(GPRS_ZhenTou, 16); // DDP协议
					// 修改DTU身份识别码
					for (t = 0; t < 11; t++)
						Txd3Buffer[4 + t] = GprsPar[DtuNoBase + t];

					// 2012.10.10 加入TCP协议和UDP协议的区别
					// UDP: 7B 09 00 10 31 33 39 31 32 33 34 35 36 37 38 7B 61 62 63
					// TCP: 7B 09 00 13 31 33 39 31 32 33 34 35 36 37 38 61 62 63 7B

					// TCP
					if (GprsPar[LinkTCPUDP0Base + SendDataReturnNo]) //=1，TCP协议；=0，UDP协议
					{
						Cw_Txd3Max = 15; // 修改Cw_Txd3Max值从16改成15！ 2012.10.10

						// ZCL 2019.3.12 加限制，透传是原先程序；新主机GPRS模式是新程序，填充数据不一样。
						if (F_GprsTransToCom)
						{
							Gprs_TX_Fill(Rcv2Buffer, Cw_BakRcv2); // 填充数据
							Gprs_TX_Byte(0x7B);
							// 2012.10.10 修改发送长度（使用TCP协议，则长度为所有发送数据总长度！ 周成磊）
							//							Txd3Buffer[3] = 16 + Cw_BakRcv2;
							data_len = 16 + Cw_BakRcv2;
							Txd3Buffer[2] = data_len >> 8;
							Txd3Buffer[3] = data_len; // YLS 2023.10.13 解决一个BUG，原来只取了低字节，导致TCP模式下，不能查询(255-21)=234字节，也就是不能查询超过117个字
						}

						// ZCL 2019.3.12
						else if (F_GprsMasterNotToCom || F_GprsMasterToCom)
						{
							Gprs_TX_Fill(Txd3TmpBuffer, Cw_Txd3TmpMax); // 填充数据 ZCL 2019.3.12

							Gprs_TX_Byte(0x7B);
							// 2012.10.10 修改发送长度（使用TCP协议，则长度为所有发送数据总长度！ 周成磊）
							//							Txd3Buffer[3] = 16 + Cw_Txd3TmpMax;
							data_len = 16 + Cw_Txd3TmpMax;
							Txd3Buffer[2] = data_len >> 8;
							Txd3Buffer[3] = data_len; // YLS 2023.10.13 解决一个BUG，原来只取了低字节，导致TCP模式下，不能查询超过(255-21)=234字节，也就是不能查询超过117个字
						}
					}

					// UDP时
					else
					{
						// Gprs_TX_Fill(Rcv2Buffer,Cw_BakRcv2);		//填充数据
						// ZCL 2019.3.12 加限制，透传是原先程序；新主机GPRS模式是新程序，填充数据不一样。
						if (F_GprsTransToCom)
							Gprs_TX_Fill(Rcv2Buffer, Cw_BakRcv2); // 填充数据

						else if (F_GprsMasterNotToCom || F_GprsMasterToCom)
							Gprs_TX_Fill(Txd3TmpBuffer, Cw_Txd3TmpMax); // 填充数据 ZCL 2019.3.12

						// YLS 2024.03.21 对要发送到的数据，进行串口输出
						//						if (F_GprsTransToCom) //|| F_GprsMasterToCom	YLS 2023.03.28
						//						{
						//							if (Cw_Txd3Max >= 512)
						//								Cw_Txd3Max = 500; //

						//							for (k = 0; k < Cw_Txd3Max; k++)
						//							{
						//								Txd2Buffer[k] = Txd3Buffer[k]; // 串口3(GPRS模块)发送到串口2
						//							}
						//							Cw_Txd2Max = Cw_Txd3Max;
						//							Cw_Txd2 = 0;
						//							RS485_CON = 1; // 2013.9.2
						//							USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
						//							USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
						//							Delay_MS(1000);	//延时10ms
						//						}
					}
				}
				//=0，透明协议
				else // 透明协议不用加头，也不分TCP,UDP区别
				{
					// Gprs_TX_Fill(Rcv2Buffer,Cw_BakRcv2);		//不分TCP,UDP区别

					// ZCL 2019.3.12 加限制，透传是原先程序；新主机GPRS模式是新程序，填充数据不一样。
					if (F_GprsTransToCom)
						Gprs_TX_Fill(Rcv2Buffer, Cw_BakRcv2); // 填充数据

					else if (F_GprsMasterNotToCom || F_GprsMasterToCom)
						Gprs_TX_Fill(Txd3TmpBuffer, Cw_Txd3TmpMax); // 填充数据 ZCL 2019.3.12
				}

				// 2.数据发送
				Gprs_TX_Start();
				// if (GprsPar[ConsoleInfoTypeBase])
				// 	Com2_printf("SEND DATA:%s", Txd3Buffer);
				//
				C_BetweenSendDataReturn = 0;
			}
			// 2014.12.11 	因为遇到收不到AT_ComOk_Flag标志，所以加上延时后，重发数据
			else if (S_GprsDataSend == 3 && C_BetweenSendDataReturn > 1000)
			{
				S_GprsDataSend = 1; // ZCL2
				C_BetweenSendDataReturn = 0;
			}
			// 3. 收到正确OK标志，发送结束
			else if (S_GprsDataSend == 3 && AT_CIPSEND_Flag && C_BetweenSendDataReturn > 100) // 发送完必须加延时，很重要 YLS 2023.03.07
			{
				//				AT_ComOk_Flag = 0;
				AT_CIPSEND_Flag = 0;
				//
				S_GprsDataSend = 0;
				C_GprsDataSend = 0;
				// SendDataReturnNo++;				//指向下一个发送序号	 移到后面 2014.11.27
				// 清除不用发送心跳包标志
				C_HeartDelay[SendDataReturnNo] = 0;
				B_HeartSendAsk[SendDataReturnNo] = 0;
				// 移到这里 2014.11.27
				SendDataReturnNo++; // 指向下一个发送序号
				C_BetweenSendDataReturn = 0;
			}

			// 4. 没有正确发送，延时
			if (C_GprsDataSend > 1) // S	YLS 2023.03.23
			{
				S_GprsDataSend = 0;
				C_GprsDataSend = 0;
				SendDataReturnNo++; // 指向下一个发送序号
				C_BetweenSendDataReturn = 0;
			}
		}
		// 没有连接的链路，跳过，转向下一个连接
		else
		{
			S_GprsDataSend = 0;
			C_GprsDataSend = 0;
			SendDataReturnNo++; // 指向下一个发送序号
			C_BetweenSendDataReturn = 0;
		}
	}

	// else if  4.可以继续添加别的处理
}

void Com3_MasterSend(void) // 串口1主机发送
{
	/* 	if(S_SendLink+S_DtuInquireSend+S_DtuSetSend+S_GprsDataSend
				+S_HeartSend+S_SendSms+S_ReadNewSms+S_SmsFull ==0 )
		{

		} */
}

/*2012.7.10  周成磊 GPRS编程 */
void Gprs_TX_Byte(u8 i) // 向发送缓存区写入一个字节
{
	Txd3Buffer[Cw_Txd3Max++] = i;
}

void Gprs_TX_String(u8 *String) // 向发送缓存区写入一个字符串
{
	u8 *Data = String;

	while (*Data != 0)
	{
		Txd3Buffer[Cw_Txd3Max++] = *Data;
		Data++;
	}
}

void Gprs_TX_Fill(uc8 *String, u8 Length) // 填充发送缓冲区
{
	uc8 *Data = String;

	while (Length--)
	{
		Txd3Buffer[Cw_Txd3Max++] = *Data;
		Data++;
	}
}

void Gprs_TX_Start(void) // 启动发送
{
	Cw_Txd3 = 0;
	// 2010.7.4 周成磊 TXE改成TC，一句改为两句
	// USART_ITConfig(USART3, USART_IT_TXE, ENABLE);				// 开始发送.
	USART_SendData(USART3, Txd3Buffer[Cw_Txd3++]);
	USART_ITConfig(USART3, USART_IT_TC, ENABLE); // 开始发送.

	// UART3_Output_Datas(Txd3Buffer, Cw_Txd3Max);
}

uchar Gprs_StrCmdLen(u8 *String) // 计算字符串长度
{
	u8 i = 0; //,j
	while (*String++ != 0)
	{
		i++;
		// j=*String;
	}
	return i;
}

/* 1个字转10进制字符串 */ // 返回值为字符个数
uchar Bcd2Str(uint Val)	  // 1个字转10进制字符串
{
	u8 i = 0;
	// 数值转成ASCII码

	if (Val >= 10000)
		goto Wanwei;
	else if (Val >= 1000)
		goto Qianwei;
	else if (Val >= 100)
		goto Baiwei;
	else if (Val >= 10)
		goto Shiwei;
	else
		goto Gewei;
Wanwei:
	StringBuffer[i++] = Val / 10000 + 0x30; // 万位
Qianwei:
	StringBuffer[i++] = (Val % 10000) / 1000 + 0x30; // 千位
Baiwei:
	StringBuffer[i++] = (Val % 1000) / 100 + 0x30; // 百位
Shiwei:
	StringBuffer[i++] = (Val % 100) / 10 + 0x30; // 十位
Gewei:
	StringBuffer[i++] = (Val % 10) + 0x30; // 各位
	return i;							   // ZCL 2019.4.26 返回值最大为5
}

/* 10进制字符串转1个字 */ // 返回值为转换值（字）
uint Str2Bcd(u8 *String)  // 10进制字符串转1个字
{
	uint sum = 0;
	u8 j, k = 0;
	u8 *Data = String;

	// 0x0D:回车键  0x2C:',' 0x2E:'.'  0x3B:';'
	while (*Data != 0x0D && *Data != 0x2C && *Data != 0x2E && *Data != 0x3B)
	{
		j = *Data;
		// 向左移位，乘以10操作；这里不用指数函数来实现（也不正确） 2012.9.21
		if (k == 0)
			sum = j - 0x30;
		else
			sum = sum * 10 + (j - 0x30);
		Data++;
		k++;
	}
	return sum;
}

/* 16进制字符串转1个字 */ // 返回值为转换值（字）
uint Str2Hex(u8 *String)  // 16进制字符串转1个字
{
	uint sum = 0;
	u8 j, k = 0;
	u8 *Data = String;

	// 0x0D:回车键  0x2C:',' 0x2E:'.'  0x3B:';'
	while (*Data != 0x0D && *Data != 0x2C && *Data != 0x2E && *Data != 0x3B && *Data != 0x29) // 2013.6.1 加 0x29:')'
	{
		j = *Data;
		//
		if (j >= 0x41) // A-F(A:0x41)
			j = j - 55;
		else // 0-9
			j = j - 0x30;

		// 向左移位，乘以16操作；这里不用指数函数，也不正确。 2012.9.21
		if (k == 0)
			sum = j;
		else
			sum = sum * 16 + j;
		Data++;
		k++;
	}
	return sum;
}

// 查找第一个值（从指定数组中）
uchar FindFirstValue(uc8 *Buffer, u8 FindValue, u8 EndValue) // 查找第一个值（从指定数组中）
{
	uchar i = 0; //,j
	uc8 *Data = Buffer;

	// j=*Data;
	while (*Data != FindValue) // 取的值不是要找的值
	{
		if (*Data != EndValue) // 取的值不是结束值
		{
			i++; // 计数器加1
			Data++;
			// j=*Data;
			if (i == 255) // 到达最大数量，强行退出
				break;
		}
		else // 取的值是结束值
		{
			i = 255; // 赋值i=255,为出错值
			break;	 // 强行退出
		}
	}
	return i;
}

// ZCL 2019.4.9 IP和端口号正确就可以建立连接。 CGD0_LinkConnectOK
// 如：0, CONNECT OK。 服务器那边软件没运行，也可以建立连接。建立连接的标志：CGD0_LinkConnectOK
// 再用心跳包来判断是否真正 建立通讯（不只是建立连接：拼通） 建立通讯的标志：ZhuCeOkFLAG
void Link_Function(void) // 建立多路 UDP,TCP连接
{
	uchar i, k;
	uint j;

	//---------------------------------------------A.B.C连接特殊的地方（与发心跳包比较）
	// A. 统计需要连接的数量
	ConnectNum = 0;
	for (i = 0; i < 4; i++)
	{
		if (GprsPar[Ip0Base + i * 4] + GprsPar[DomainName0Base + i * 31] != 0)
			ConnectNum++;
	}

	// B. 关闭连接 2012.9.16  2012.10.11加入标志限制  2012.10.12 新加SendSmsLength
	if (S_SendLink + S_DtuInquireSend + S_DtuSetSend + S_GprsDataSend + S_HeartSend + S_SendSms + S_ReadNewSms + S_SmsFull == 0)
	{
		for (j = 0; j < 4; j++)
		{
			if (B_CloseLink[j] == 1) // 2013.6.14 修改成 B_CloseLink[j]
			{
				At_QICLOSE(j);
				CGD0_LinkConnectOK[j] = 0;
				B_CloseLink[j] = 2;
				Delay_MS(80); // 2013.6.14 加入延时 1秒就可以
			}
		}
	}

	// C. 第一次连接，把IP地址不等于0或者域名不等于0的通道，置连接请求标志
	for (i = 0; i < 4; i++)
	{
		// 2012.10.13 加入域名设置成ASCII 0不连接，或者只有一位域名地址不连接
		// 2014.12.3 加入第4路首字母为@不连接，这是人为设置的短信主动发的电话号码！
		if (GprsPar[DomainName0Base + i * 31] == 0x30 || GprsPar[DomainName0Base + i * 31 + 1] == 0 || (i == 3 && GprsPar[DomainName0Base + i * 31] == '@')) // 2014.12.3 加入第4路首字母为@不连接
			k = 0;
		else
			k = GprsPar[DomainName0Base + i * 31]; // k为域名

		// IP地址不等于0或者域名不等于0，原先没有进行过连接，立即进行连接，否则下面延时进行连接
		if (GprsPar[Ip0Base + i * 4]) //|| k
		{
			if (B_FirstLink[i] == 0) //!!! 加快连接的过程 2014.12.3备注
			{
				B_LinkAsk[i] = 1;
				B_FirstLink[i] = 1;
			}
		}
		// 没有连接地址，清除第一次连接标志
		else
			B_FirstLink[i] = 0;
	}
	//---------------------------------------------上面A.B.C连接特殊的地方（与发心跳包比较）

	// 1. 连接进行延时判断
	for (i = 0; i < 4; i++)
	{
		if (B_LinkAsk[i] == 0)
		{
			// 已经连接上 或者 IP地址和域名地址为0了，都不需要连接了！ 2014.12.3
			if (CGD0_LinkConnectOK[i] || (GprsPar[Ip0Base + i * 4] + k) == 0)
				C_LinkDelay[i] = 0;
			// 时间到后，置连接请求标志B_LinkAsk[i]=1
			else
			{
				// 重连接时间间隔（300S），2个字节
				j = GprsPar[ReConnect0Base + i * 2];
				j = (j << 8) + GprsPar[ReConnect0Base + 1 + i * 2];
				// 2013.6.11 有关闭连接标志，必须延时2秒后，才可以再连接。（为了快速连接）
				if (B_CloseLink[i] == 2)
				{
					if (C_LinkDelay[i] > 3)
					{
						B_LinkAsk[i] = 1; // 时间到，置连接请求标志
						B_CloseLink[i] = 0;
					}
				}
				// 否则，只有到重连接时间间隔（300S）后才可以连接。
				else if (C_LinkDelay[i] > j)
				{
					B_LinkAsk[i] = 1; // 时间到，置连接请求标志
				}
			}
		}
		else
			C_LinkDelay[i] = 0;
	}

	// 2. 正在发送连接 （每路连接循环发送，有发送请求标志就发送连接！）
	if (CGD0_ConnectNo > 3)
		CGD0_ConnectNo = 0;
	i = CGD0_ConnectNo;

	if (B_LinkAsk[i] == 0)
	{
		CGD0_ConnectNo++; // 指向下一路连接的发送
	}
	// 有心跳包，先发送心跳包 ！！！
	else if (S_LinkNow[i] == 0 && B_HeartSendAsk[0] + B_HeartSendAsk[1] + B_HeartSendAsk[2] + B_HeartSendAsk[3] > 0)
		;
	//
	else
	{
		// 指令互锁限制
		// 可以改成==100就条件不成立，不发连接了
		if (S_HeartSend + S_DtuInquireSend + S_DtuSetSend + S_GprsDataSend + S_SendSms + S_ReadNewSms + S_SmsFull == 0) //=0  =100测试使用，暂不连接
		{
			if (S_LinkNow[i] == 0)
			{
				S_LinkNow[i] = 1;
				C_LinkNow[i] = 0;
				/* 				Disable_USART2_NVIC();
								printf("\r\n\r\nSend link %d CMD!", i);
								Enable_USART2_NVIC(); */
			}

			if (S_LinkNow[i] == 1) //&& C_LinkNow[i] > 2  作用不大，照样OK  1, CONNECT OK 一块出
			{
				S_LinkNow[i] = 2;
				Cw_Txd3Max = 0;	 // 不能删 2012.10.4
				At_QIMuxOPEN(i); // 发送连接指令
				C_LinkNow[i] = 0;
			}
			else if (S_LinkNow[i] == 2)
			{
				// 3. 有连接标志，清除其他标志
				if (CGD0_LinkConnectOK[i])
				{
					C_LinkDelay[i] = 0;
					C_LinkNow[i] = 0;
					S_LinkNow[i] = 0;
					B_LinkAsk[i] = 0;
					B_LinkFault[i] = 0;
					// 第一次连接上，置心跳包发送请求标志  !!!
					B_HeartSendAsk[i] = 1;
				}
			}
		}

		// 4. 延时，没有连接成功，清除
		if (S_LinkNow[i] > 0)
		{
			if (C_LinkNow[i] > 5)
			{
				C_LinkDelay[i] = 0; //=0清零移到前面，后面有个情况赋值 2013.6.28
				//
				if (B_LinkFault[i] < 3) // 前4次没有成功，立即重连。 2013.6.23
				{
					if (ConnectNum <= 1) // 只有1路连接设置可以直接再发连接请求 2013.6.28
						B_LinkAsk[i] = 1;
					else // 有2路及以上连接设置，需延时再发连接请求(给其他连接时间)
					{
						B_LinkAsk[i] = 0;
						//
						Lw_GprsTmp = GprsPar[ReConnect0Base + i * 2];
						Lw_GprsTmp = (Lw_GprsTmp << 8) + GprsPar[ReConnect0Base + 1 + i * 2];
						if (Lw_GprsTmp > 8)
							C_LinkDelay[i] = Lw_GprsTmp - 8;
						else
							C_LinkDelay[i] = Lw_GprsTmp;
					}
				}
				else // 如果故障大于3次，则清除B_LinkAsk[i]标志，重新计时
					B_LinkAsk[i] = 0;
				//
				C_LinkNow[i] = 0;
				S_LinkNow[i] = 0;
				B_LinkFault[i]++;
			}
		}

		// 5. 有10次故障标志（建立连接不成功），重新启动模块。
		// 如果重连接时间间隔=0，则DTU几次注册失败后，立即进行软或硬启动
		// 如果重连接时间间隔!=0，则不会重启，一直到重连接时间再重连。

		// if(j==0 && B_LinkFault[i]>=10)			//
		if (B_LinkFault[i] >= 10) // 2016.5.24 修改，去掉j==0
		{
			B_SmsOrFaultHardRestart = 15; //=15故障硬件重启; =1短信硬件重启 ZCL 2019.4.19
		}
	}

	// 合并一个状态
	S_SendLink = S_LinkNow[0] + S_LinkNow[1] + S_LinkNow[2] + S_LinkNow[3];
}

// 心跳包的延时发送  时间变量控制每一秒进入一次
// 1. UDP心跳注册包: 先发指令.通道号.长度		At+QISEND=X,Y
//		At_QISENDAccordingLength( i,22 );		//DTU 内容发送按照长度
void Gprs_HeartDelaySend(void) // Gprs心跳包的延时发送　2012.8.25
{
	uchar i, j;

	// 1. 进行判断 （延时后发送心跳包 和 发送心跳包超时延时）
	for (i = 0; i < 4; i++)
	{
		if (B_HeartSendAsk[i] == 0) // 2. 心跳包计时延时
		{
			// 没有连接标志，计数器清零
			if (CGD0_LinkConnectOK[i] == 0)
				C_HeartDelay[i] = 0;
			// 有连接标志，判断心跳时间是否到	2013.6.28
			else
			{
				Lw_GprsTmp = GprsPar[OnlineReportInterval0Base + i * 2]; // 心跳时间，2个字节  40S
				Lw_GprsTmp = (Lw_GprsTmp << 8) + GprsPar[OnlineReportInterval0Base + 1 + i * 2];
				if (C_HeartDelay[i] > Lw_GprsTmp)
				{
					B_HeartSendAsk[i] = 1; // 时间到，置发送请求标志
					C_HeartDelay[i] = 0;
				}
			}
		}
		else
			C_HeartDelay[i] = 0;
	}

	// 2. 正在发送心跳包 （每路连接循环发送，有发送请求标志就发送心跳包！）
	if (SendHeartNo > 3)
		SendHeartNo = 0;
	i = SendHeartNo;

	if (B_HeartSendAsk[i] == 0)
	{
		SendHeartNo++; // 指向下一路心跳包的发送
	}
	// 发送本路心跳包
	else
	{
		// UDP连接上发送心跳注册包，1先发指令.通道号.长度		At+QISEND=X,Y  2后发内容
		if (S_SendLink + S_DtuInquireSend + S_DtuSetSend + S_GprsDataSend + S_SendSms + S_ReadNewSms + S_SmsFull == 0)
		{
			if (S_HeartSendNow[i] == 0)
			{
				S_HeartSendNow[i] = 1;
				C_HeartSendNow[i] = 0;
				ZhuCeOkFLAG[i] = 0;
				C_BetweenHeartSendNow = 0; // 2014.12.11
										   /* 				Disable_USART2_NVIC();
														   Com2_printf("\r\n\r\nSend heart %d CMD!", i);
														   Enable_USART2_NVIC();	 */
			}

			// 1. UDP心跳注册包: 先发指令.通道号.长度		At+QISEND=X,Y
			if (S_HeartSendNow[i] == 1 && C_BetweenHeartSendNow > 3)
			{
				S_HeartSendNow[i] = 2;
				C_BetweenHeartSendNow = 0;
				Cw_Txd3Max = 0; // 不能删 2012.10.4

				At_QISENDAccordingLength(i, 22); // DTU 内容发送按照长度
			}
			// 1.5因为遇到收不到AT_3EOk_Flag标志，所以加上延时后，重发数据		2014.12.11
			else if (S_HeartSendNow[i] == 2 && C_BetweenHeartSendNow > 3000) // YLS 2023.03.11
			{
				S_HeartSendNow[i] = 1; // ZCL1 !!! 返回序号1，重发
				C_BetweenHeartSendNow = 0;
			}
			// 2. 后发内容
			else if (S_HeartSendNow[i] == 2 && AT_3EOk_Flag) //>
			{
				S_HeartSendNow[i] = 3;
				AT_3EOk_Flag = 0;
				Gprs_Send_ZhuCe();
				C_BetweenHeartSendNow = 0;
			}
			// 2.5 因为遇到收不到AT_ComOk_Flag标志，所以加上延时后，重发数据			2014.12.11
			else if (S_HeartSendNow[i] == 3 && C_BetweenHeartSendNow > 1000)
			{
				S_HeartSendNow[i] = 1; // ZCL2 !!! 返回序号1，重发
				C_BetweenHeartSendNow = 0;
			}
			// 3. 接收到OK标志
			else if (S_HeartSendNow[i] == 3 && AT_CIPSEND_Flag && C_BetweenHeartSendNow > 100) //&& AT_ComOk_Flag	YLS 2023.02.26
			{
				S_HeartSendNow[i] = 4;
				AT_CIPSEND_Flag = 0;
				// AT_ComOk_Flag = 0;
			}
			else if (S_HeartSendNow[i] == 4)
			{
				// 3. 有注册标志，清除其他标志和注册标志本身 (接收服务器返回信息)
				if (ZhuCeOkFLAG[i])
				{
					C_HeartDelay[i] = 0; // 2013.6.27
					C_HeartSendNow[i] = 0;
					S_HeartSendNow[i] = 0;
					//
					B_HeartSendAsk[i] = 0;
					B_HeartSendFault[i] = 0;
					B_ConnectFail[i] = 0; // 连接故障清零
					SendHeartNo++;		  // 2013.6.27 指向下一个"发送心跳序号"
				}
			}
		}

		// 4. 延时，没有注册成功，清除
		if (S_HeartSendNow[i] > 0)
		{
			if (C_HeartSendNow[i] > 1) // 2013.5.7 5秒内没有返回确认包，再重发
			{
				C_HeartDelay[i] = 0; //=0清零移到前面，后面有个情况赋值 2013.6.28
				//
				if (B_HeartSendFault[i] < 2) // 前3次没有成功，3秒延时重发心跳包。 2013.6.23
				{
					if (ConnectNum <= 1)	   // 只有1路连接设置可以直接发心跳包 2013.6.28
						B_HeartSendAsk[i] = 1; // 2013.6.28		置1
					else					   // 有2路及以上连接设置，需延时再发心跳包(给其他连接时间)
					{
						B_HeartSendAsk[i] = 0; // 2013.6.28		清0
						//
						Lw_GprsTmp = GprsPar[OnlineReportInterval0Base + i * 2];
						Lw_GprsTmp = (Lw_GprsTmp << 8) + GprsPar[OnlineReportInterval0Base + 1 + i * 2];
						if (Lw_GprsTmp > 3)
							C_HeartDelay[i] = Lw_GprsTmp - 3; // 这样设置后，3秒后即可重新发送心跳包
						else
							C_HeartDelay[i] = Lw_GprsTmp;
					}
				}
				else					   // 如果故障大于3次，则清除B_HeartSendAsk[i]标志，重新计时
					B_HeartSendAsk[i] = 0; // 2013.6.28		清0

				C_HeartSendNow[i] = 0;
				S_HeartSendNow[i] = 0;
				B_HeartSendFault[i]++;
				ZhuCeOkFLAG[i] = 0;
				SendHeartNo++; // 2013.6.27
			}
		}

		// 5. 有最大登录DSC次数(5次)故障标志（心跳注册不成功），关闭通道 2013.5.7
		j = GprsPar[MaxLoginDSCNum0Base + i];
		if (B_HeartSendFault[i] >= j)
		{
			/* 			if(B_ZhuCeOkNum==0)				//没有1路连接，模块重启
						{
							B_SmsOrFaultHardRestart=17;		//=17故障硬件重启; =1短信硬件重启	 ZCL 2019.4.19
						}
						else */
			// ZCL 2017.5.27
			//			At_QICLOSE(i); // 有其他连接，只关闭此通道 2013.5.7 YLS 2023.03.11
			//
			CGD0_LinkConnectOK[i] = 0;

			// ZCL 2017.5.26 注释掉
			// B_HeartSendFault[i]=0;		//2013.5.23		ZCL 2019.4.17
		}
	}

	// 合并一个状态
	S_HeartSend = S_HeartSendNow[0] + S_HeartSendNow[1] + S_HeartSendNow[2] + S_HeartSendNow[3];
	B_ZhuCeOkNum = ZhuCeOkFLAG[0] + ZhuCeOkFLAG[1] + ZhuCeOkFLAG[2] + ZhuCeOkFLAG[3];
}

// 心跳包（注册包）的发送
void Gprs_Send_ZhuCe(void) // 心跳包（注册包）的发送			//DTU 内容发送
{
	uchar i;

	for (i = 0; i < 22; i++)
		Txd3Buffer[i] = GPRS_ZhuCe[i];
	// 修改DTU身份识别码
	for (i = 0; i < 11; i++)
		Txd3Buffer[4 + i] = GprsPar[DtuNoBase + i];
	Cw_Txd3Max = 22;
	Txd3Buffer[Cw_Txd3Max++] = 0x0d;
	Gprs_TX_Start();
}

/* void SMS_SendGSM(void)			//发送短信
{
  GSM_CipSend_Flag = FALSE;

  Gprs_TX_String("AT+CMGS=\0");
  Gprs_TX_Byte('"');
  Gprs_TX_String((u8 *)Device_Info.Phone_Num);
  Gprs_TX_Byte('"');
  Gprs_TX_Byte(0x0d);

  while(!GSM_CipSend_Flag);
  Gprs_TX_String((u8 *)Device_Info.SMS_Buf);
  Gprs_TX_Byte(0x1a);
  SendSMS_Flag = FALSE;
} */

// 短信设置和查询 + PC串口来设置GPRS参数
//+CMGR: "REC UNREAD","+8615610566803","","2012/09/18 22:38:13+32"
// 读来的短信内容    如：GPR+62;		如：GPW+26,42;
// GPRS模块短信返回：如：GPR+62, Parameter: 202.96.134.133, Read OK! ;
// GPRS模块短信返回：如：GPW+26,42, Set OK! ;
// Com2_RcvProcess()、main()中调用SmsAndPC_SetupInq()
void SmsAndPC_SetupInq(void) // 短信和PC设置和查询GPRS参数
{
	uchar i, j, k, r, s, err = 0, value;
	uchar r2; // ZCL 2019.4.26 增加个r2来保存r，用于r==25情况
	uint ip[4], w1, n = 0;
	// uchar *ptr;

	// 一、设置指令W 或者 w
	if (B_PcSetGprs == 2)							   // 2013.6.5 加入这三句
		ptr = (u8 *)strstr((char *)Rcv2Buffer, "GPW"); // 通过PC串口调试软件可以设置GPRS参数
	else											   // 2013.6.5 加入这三句
		ptr = (u8 *)strstr(Rcv3Buffer, "GPW");
	Rcv3Buffer[0] = 0x31; // 让Rcv3Buffer[0]不为'G'，相当于清零

	if (ptr != NULL && SendSmsLength == 0)
	{
		// 1. 合成命令，找命令，查找相应字符位置以利于下一步使用
		// 把收到的字符串 如：GPW+26,42;		0x32和0x36合成16进制指令值0x26
		CommandNo = Str2Hex(ptr + 4);
		if (CommandNo <= 0x63)
			n = FindFirstValue(CmdNo, CommandNo, 0x00); // n:找到指令值在GPRS 参数保存序列的位置
		i = FindFirstValue(ptr, 0x2C, 0x00);			// 0x2C=,		//查找第一个','
		r = FindFirstValue(ptr, 0x3B, 0x00);			// 0x3B=;		//查找结束符';'
		r2 = r;											// ZCL 2019.4.26 增加个r2来保存r，用于r==25情况

		// 2.错误指令的处理; ZCL 添加 2012.9.23
		// n==255 没有找到此命令；CommandNo>0xA0 此命令超过存在的命令
		if (n == 255 || i == 255 || r2 == 255 || CommandNo > 0xA0) // ZCL 2019.4.26 增加个r2来保存r，用于r==25情况
		{
			// 写的错误号从1开始 （可以1-9）
			// 置错误标志 err=1
			err = 1;
			for (s = 0; s < 16; s++) // 填充命令错误的短信内容，准备发送短信
				SendSmsBuf[SendSmsLength++] = CommandError[s];
		}
		else
		{
			if (r >= 150)
				r = 149; // ZCL 2019.4.26 控制数组长度，uchar SendSmsBuf[150];

			// 3.保存指令 如：GPW+26,42（没有分号）
			for (s = 0; s < r; s++)
				SendSmsBuf[s] = *(ptr + s);
			// 补上','号，SendSmsLength=r+1
			SendSmsBuf[r] = ',';
			SendSmsLength = r + 1;
		}

		// 周成磊 2013.2.2 	有错误跳过
		ptr = ptr + i + 1; // 指令内容的地址
		value = *ptr;	   // 指令内容的值
		if (err)
			;
		// 4. 加入短信控制功能 2012.10.12
		// 0x82：重启  0x91：模块上电重启
		// 0x92:设备启停(=0,启动; =1,停止) 0x95:看门狗复位重启

		// 0x82：重启
		else if (CommandNo == 0x82)
		{
			// GPW+82,1;		注：收到的1为ASCII码0x31
			if (value == 0x31)
				B_SmsDtuRestart = 1; // 置DTU重启标志
		}
		// 0x91：模块上电重启		//GPW+91,1;
		else if (CommandNo == 0x91)
		{
			// GPW+91,1;		注：收到的1为ASCII码0x31
			if (value == 0x31)
				B_SmsOrFaultHardRestart = 1; // 置模块上电重启标志
											 // S_M35 = 0x01;		//ZCL 2019.4.26 测试，取消上面，加上这条，则可以重启！ 为什么？
		}
		// 0x92:设备启停(=0,启动; =1,停止)  //GPW+92,1;	GPW+92,0;
		else if (CommandNo == 0x92)
		{
			// GPW+92,1;	GPW+92,0;		注：收到的1为ASCII码0x31，收到的0为ASCII码0x30
			if (value == 0x31 || value == 0x30)
			{
				// SmsControlStopValue 此值在这里可能为1，可能为0
				SmsControlStopValue = value - 0x30;
				B_SmsRunStop = 1;
			}
		}
		// 0x95:GP311模块看门狗复位重启 2013.7.3  //GPW+95,1;
		// ZCL 2019.4.26 看门狗重启，模块不会返回短信，因为重启了，标志没有了
		else if (CommandNo == 0x95)
		{
			if (value == 0x31)
				B_SmsWatchdogRestart = 1;
		}

		// ZCL 2019.4.26 测试，随便定义个0x89，可以重启，为什么0x91 不行？
		else if (CommandNo == 0x89)
		{
			if (value == 0x31)
			{
				S_M35 = 0x01;
			}
		}

		// 5. IP地址: 发送过来如222.173.103.226 因为有'.'需要去掉
		// 0x20,0x30,0x40,0x50 是IP地址, 0x62是DNS地址
		else if (CommandNo == 0x20 || CommandNo == 0x30 || CommandNo == 0x40 || CommandNo == 0x50 || CommandNo == 0x62)
		{
			// 把收到的字符串 如：0x32,0x32,0x32=222，合成10进制值222(在内存中保存为0xDE)
			for (s = 0; s < 4; s++)
			{
				ip[s] = Str2Bcd(ptr);	  // 字符串转10进制值
				if (s == 0 && ip[s] == 0) // 解决IP设置为空时显示出错问题 2013.6.4
				{
					ip[1] = 0;
					ip[2] = 0;
					ip[3] = 0;
					s = 3;
				}
				if (s == 3)
				{
					// 2013.6.11 原先有连接，先关闭，2秒后重连
					for (j = 0; j < 4; j++)
					{
						if (CGD0_LinkConnectOK[j] && CommandNo == 0x20 + 0x10 * j) //(IP地址修改了)
							B_CloseLink[j] = 1;
					}
					// 2013.6.11 上面新加
					break; // 最后面没有'.'了
				}
				// 查找第一二三个'.' ，当s==3时，上面就break退出循环了，不会执行下面语句。
				i = FindFirstValue(ptr, 0x2E, 0x00); // 0x2E=.
				ptr = ptr + i + 1;
			}

			// 如果正确则进行保存；如果错误则返回错误信息
			for (s = 0; s < 4; s++)
			{
				if (ip[s] <= 255)
				{
					// n: 指令值在GPRS 参数保存序列的位置
					// 正确保存，把IP地址写到GprsPar参数表
					GprsPar[CmdPst[n] + s] = ip[s];
				}
				else
				{
					// 错误处理 标记：err=2
					err = 2;
					for (s = 0; s < 18; s++) // 填充参数错误的短信内容，准备发送短信
					{
						SendSmsBuf[SendSmsLength++] = ParameterError[s];
					}
					// 2013.1.24 周成磊 加入退出循环
					break;
				}
			}
		}

		// 6. 字符串的设置（不是数值） 如：0x05：CMNET
		// 服务代码:01		接入点名称:05		DTU身份识别码:06		域名:21.31.41.51
		else if (CommandNo == 0x01 || CommandNo == 0x05 || CommandNo == 0x06 || CommandNo == 0x21 || CommandNo == 0x31 || CommandNo == 0x41 || CommandNo == 0x51)
		{
			// r(;位置)  i(,位置)
			if (CommandNo == 0x01 && r - i - 1 > 8)
				err = 3;
			if (CommandNo == 0x05 && r - i - 1 > 24)
				err = 3;
			if (CommandNo == 0x06 && r - i - 1 > 11)
				err = 3;
			if (CommandNo == 0x21 && r - i - 1 > 30)
				err = 3;
			if (CommandNo == 0x31 && r - i - 1 > 30)
				err = 3;
			if (CommandNo == 0x41 && r - i - 1 > 30)
				err = 3;
			if (CommandNo == 0x51 && r - i - 1 > 30)
				err = 3;

			if (r < i + 1) // ZCL 2019.4.26 限制大小，防止溢出
				err = 3;

			if (err == 3) // 错误处理
			{
				// 填充参数错误的短信内容，准备发送短信
				for (s = 0; s < 31; s++)
				{
					SendSmsBuf[SendSmsLength++] = StringlengthError[s];
				}
			}
			else // 没有错误
			{
				for (s = 0; s < (r - i - 1); s++)
				{
					GprsPar[CmdPst[n] + s] = *(ptr + s);
				}
				GprsPar[CmdPst[n] + s] = 0; // 2012.9.20 周成磊 字符串的末尾加0

				// 2013.6.14 DTU身份识别码。原先有连接，先关闭，2秒后重连
				if (CommandNo == 0x06)
				{
					for (j = 0; j < 4; j++)
					{
						if (CGD0_LinkConnectOK[j]) //(DTU身份识别码修改了)
							B_CloseLink[j] = 1;	   // 自动关闭各个连接，重启太慢！
												   // S_M35 = 0x01;						//重启  2014.12.4注释掉
					}
				}
			}
		}

		// 8. 串口的设置，0x0D指令：两个字  0x00,0x00,0x25,0x80 = 9600
		//  目前程序只支持最后一个字的波特率设置，只支持到0-65535
		else if (CommandNo == 0x0D)
		{
			// 8.1 波特率
			// 把收到的字符串 如：0x39,0x36,0x30,0x30=9600，合成10进制值(在内存中保存为0x25,0x80)
			// 过来的值不需要换算,合并起来就行
			w1 = Str2Bcd(ptr); // 字符串转10进制值
			if (w1 == 1200 || w1 == 2400 || w1 == 4800 || w1 == 9600 || w1 == 19200 || w1 == 38400 || w1 == 57600)
			{
				GprsPar[CmdPst[n] + 2] = w1 >> 8; // 高字节
				GprsPar[CmdPst[n] + 3] = w1;	  // 低字节
												  // 在Com1_BaudRateSet()中修改Pw_BaudRate1
			}
			else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
			{
				;
			}
			else
			{
				// 错误参数的处理
				err = 4;
				// 填充参数错误的短信内容，准备发送短信
				for (s = 0; s < 21; s++)
				{
					SendSmsBuf[SendSmsLength++] = BaudRateError[s];
				}
			}

			// 8.2 数据位 数据位为8位（00-5，01-6，02-7，03-8）	过来的值要5,6,7,8要换算成 0,1,2,3
			// 实际显示值是8（这里只能设置03）
			// 查找下一个',' [0x2C:',']
			i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
			if (i == 255)
			{
				w1 = 255;
				goto SMS_SetBaudItemLack_END; // 2013.12.18
			}
			else
			{
				ptr = ptr + i + 1;
				w1 = Str2Bcd(ptr); // 字符串转10进制值
			}

			if (w1 == 8)
			{
				GprsPar[CmdPst[n] + 4] = w1 - 5;
			}
			else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
			{
				;
			}
			else
			{
				// 错误参数的处理
				err = 5;
				for (s = 0; s < 26; s++)
				{
					SendSmsBuf[SendSmsLength++] = DataBitLengthError[s];
				}
			}

			// 8.3 停止位 停止位为1位（00-1，04-2）	过来的值要1,2要换算成 0,0x04
			// 实际显示值是1、2（这里可以设置00、04）
			// 查找下一个',' [0x2C:',']
			i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
			if (i == 255)
			{
				w1 = 255;
				goto SMS_SetBaudItemLack_END; // 2013.12.18
			}
			else
			{
				ptr = ptr + i + 1;
				w1 = Str2Bcd(ptr); // 字符串转10进制值
			}
			if (w1 == 1 || w1 == 2)
			{
				GprsPar[CmdPst[n] + 5] = (w1 - 1) * 4;
			}
			else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
			{
				;
			}
			else
			{
				// 错误参数的处理
				err = 6;
				for (s = 0; s < 26; s++)
				{
					SendSmsBuf[SendSmsLength++] = StopBitLengthError[s];
				}
			}

			// 8.4 奇偶校验 校验位-无（00-无0，08-奇1，18-偶2）	过来的值要0,1,2要换算成 0，0x08，0x18
			// 实际显示值是0、1、2（这里可以设置0x00、0x08、0x18）
			// 查找下一个',' [0x2C:',']
			i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
			if (i == 255)
			{
				w1 = 255;
				goto SMS_SetBaudItemLack_END; // 2013.12.18
			}
			else
			{
				ptr = ptr + i + 1;
				if (*ptr != 0x2C)	   // 下一个位置不是“,”号，进行取值，否者直接赋w1=100，代表有问题
					w1 = Str2Bcd(ptr); // 字符串转10进制值
				else
					w1 = 100; // 上面都是用0代表没有改变，这里0是有用的参数值，所以换成100了。
			}

			if (w1 <= 2)
			{
				if (w1 == 0)
					GprsPar[CmdPst[n] + 6] = 0;
				else if (w1 == 1)
					GprsPar[CmdPst[n] + 6] = 0x08;
				else if (w1 == 2)
					GprsPar[CmdPst[n] + 6] = 0x18;
			}
			else if (w1 == 100) // 周成磊 2013.12.19 如果=100，维持原值
			{
				;
			}
			else
			{
				// 错误参数的处理
				err = 7;
				for (s = 0; s < 28; s++)
				{
					SendSmsBuf[SendSmsLength++] = ParityBitLengthError[s];
				}
			}

			// 8.5 流控-无流控（01-Xon/Xoff1，02-硬流控2，03-无流控3，04-半双工485 4，05-全双工422 5）
			// 实际显示值是2、3（这里可以设置0x02、0x03）
			// 过来的值不需要换算。			//（这里只能设置02、03）
			// 查找下一个',' [0x2C:',']
			i = FindFirstValue(ptr, 0x2C, 0x00); // 0x2C=,
			if (i == 255)
			{
				w1 = 255;
				goto SMS_SetBaudItemLack_END; // 2013.12.18
			}
			else
			{
				ptr = ptr + i + 1;
				w1 = Str2Bcd(ptr); // 字符串转10进制值
			}
			if (w1 == 2 || w1 == 3)
			{
				GprsPar[CmdPst[n] + 7] = w1;
			}
			else if (w1 == 0) // 周成磊 2013.12.18 如果值为空，维持原值
			{
				;
			}
			else
			{
				// 错误参数的处理
				err = 8;
				for (s = 0; s < 24; s++)
				{
					SendSmsBuf[SendSmsLength++] = FlowControlError[s];
				}
			}

		SMS_SetBaudItemLack_END: // 设置波特率中设置项缺少错误。GPW+0D,9600,8,1,2,3;
			if (w1 == 255 && err == 0)
			{
				// 错误参数的处理
				err = 9;
				for (s = 0; s < 30; s++)
				{
					SendSmsBuf[SendSmsLength++] = SerialPortItemLackError[s];
				}
			}
		}

		// 9. 双字节参数和单字节参数：数值的设置 0-65535范围内
		else
		{
			// 查找指令是不是双字节参数
			s = FindFirstValue(GprsCmd2Byte, CommandNo, 0x00);
			// 参数的值
			w1 = Str2Bcd(ptr); // 字符串转10进制值
			// 双字节参数
			if (s != 255) //
			{
				GprsPar[CmdPst[n] + 0] = w1 >> 8; // 保存设置值
				GprsPar[CmdPst[n] + 1] = w1;	  // 保存设置值
				// 端口修改了，原先有连接，先关闭，2秒后重连；2013.6.11 22，32，42，52分别代表1,2,3,4通道DSC的端口号
				for (j = 0; j < 4; j++)
				{
					if (CGD0_LinkConnectOK[j] && CommandNo == 0x22 + 0x10 * j) //()
						B_CloseLink[j] = 1;
				}
				// 2013.6.11 上面新加
			}
			// 单字节参数
			else
			{
				GprsPar[CmdPst[n] + 0] = w1; // 保存设置值
				// 短信 TCP,UDP协议修改，原先有连接，硬件重启 2013.6.21 24，34，44，54分别代表1,2,3,4通道的连网方式（0-UDP，1-TCP）
				for (j = 0; j < 4; j++)
				{
					if (CGD0_LinkConnectOK[j] && CommandNo == 0x24 + 0x10 * j) //()
					{
						B_SmsOrFaultHardRestart = 16; //=16故障硬件重启; =1短信硬件重启 ZCL 2019.4.19
					}
				}
				// 2013.6.19 上面新加
			}
		}

		// 10. 短信返回
		// 有错误跳过
		if (err)
			;
		// 正确的话，填充返回短信内容，根据情况保存参数
		else
		{
			if (B_SmsDtuRestart) // 短信DTU重启
			{
				for (s = 0; s < 12; s++) // 填充返回短信内容
					SendSmsBuf[SendSmsLength++] = DtuRestartOK[s];
			}
			else if (B_SmsOrFaultHardRestart == 1 || B_SmsOrFaultHardRestart == 11 || B_SmsOrFaultHardRestart == 12 // ZCL 2019.4.23
					 || B_SmsOrFaultHardRestart == 13 || B_SmsOrFaultHardRestart == 14								// ZCL 2019.4.23
					 || B_SmsOrFaultHardRestart == 15 || B_SmsOrFaultHardRestart == 16								// ZCL 2019.4.23
					 )																								// 短信或者故障硬件重启
			{
				for (s = 0; s < 16; s++) // 填充返回短信内容
					SendSmsBuf[SendSmsLength++] = DtuHardRestartOK[s];
			}
			else if (B_SmsWatchdogRestart == 1) // 短信看门狗硬件重启  2014.12.8 //0x95:GP311模块看门狗复位重启 2013.7.3  //GPW+95,1;
			{
				for (s = 0; s < 20; s++) // 填充返回短信内容
					SendSmsBuf[SendSmsLength++] = SmsWatchdogRestart[s];
			}

			else if (B_SmsRunStop) // 短信控制SM510设备启停
			{
				if (SmsControlStopValue) //=1,停机
				{
					for (s = 0; s < 16; s++) // 填充返回短信内容
						SendSmsBuf[SendSmsLength++] = SmsControlStopOK[s];
				}
				else //=0,启动
				{
					for (s = 0; s < 15; s++) // 填充返回短信内容
						SendSmsBuf[SendSmsLength++] = SmsControlRunOK[s];
				}
			}

			// 填充设置OK的短信内容。Sms_Function()中发送短信。
			for (s = 0; s < 9; s++)
				SendSmsBuf[SendSmsLength++] = SetOK[s];

			// 短信控制功能 不用保存
			if (B_SmsDtuRestart + B_SmsOrFaultHardRestart + B_SmsRunStop + B_SmsWatchdogRestart > 0) // 属于控制功能，不保存
				B_SmsSetup = 0;																		 // 短信设定参数标志（=1，后面对短信设定的参数进行保存）
			else
				B_SmsSetup = 1; // 短信设定参数标志（=1，后面对短信设定的参数进行保存）
		}
		// 短信末尾加 ; 0x1A   0x0d
		SendSmsBuf[SendSmsLength++] = ';';
		SendSmsBuf[SendSmsLength++] = 0x1A; // CTRL+Z结束(0x1A)
		SendSmsBuf[SendSmsLength++] = 0x0d;
	}

	// 二、查询指令 2012.9.22
	if (B_PcSetGprs == 2)							   // 2013.6.5 加入这三句
		ptr = (u8 *)strstr((char *)Rcv2Buffer, "GPR"); // 通过PC串口调试软件可以设置GPRS参数
	else											   // 2013.6.5 加入这三句
		ptr = (u8 *)strstr(Rcv3Buffer, "GPR");
	Rcv3Buffer[0] = 0x31; // 让Rcv3Buffer[0]不为'G'，相当于清零

	if (ptr != NULL && SendSmsLength == 0)
	{
		// 1. 合成命令，找命令，查找相应字符位置以利于下一步使用
		// 把收到的字符串 如：GPR+62;  0x36和0x32合成16进制指令值0x62
		CommandNo = Str2Hex(ptr + 4);
		// n:找到指令值在GPRS 参数保存序列的位置
		n = FindFirstValue(CmdNo, CommandNo, 0x00);
		// 查找结束符';' 从position
		r = FindFirstValue(ptr, 0x3B, 0x00); // 0x3B=;
		r2 = r;								 // ZCL 2019.4.26 增加个r2来保存r，用于r==25情况

		// 2. 错误指令的处理;
		// 周成磊 2013.2.2 添加r2==255的判断，否则短信写指令没有';'结束符的话出错
		if (n == 255 || r2 == 255 || CommandNo > 0xA0) // ZCL 2019.4.26 增加个r2来保存r，用于r==25情况
		{
			// 读的错误号从11开始
			err = 11;
			for (s = 0; s < 16; s++)
			{
				SendSmsBuf[SendSmsLength++] = CommandError[s];
			}
		}
		else
		{
			if (r >= 150)
				r = 149; // ZCL 2019.4.26 控制数组长度，uchar SendSmsBuf[150];

			// 3. 保存指令 如：GPR+43（没有分号）
			for (s = 0; s < r; s++)
			{
				SendSmsBuf[s] = *(ptr + s);
			}
			SendSmsBuf[r] = ',';
			SendSmsLength = r + 1;

			// 短信返回，加入 "Parameter:"提示 			2012.10.12
			for (s = 0; s < 12; s++)
			{
				SendSmsBuf[SendSmsLength++] = Parameter[s];
			}
		}

		// 周成磊 2013.2.2 	有错误跳过
		if (err)
			;
		// 短信读指令正确
		// 5. IP地址: 发送过来为222.173.103.226 因为有'.'需要去掉
		// 0x20,0x30,0x40,0x50 是IP地址, 0x62是DNS地址
		else if (CommandNo == 0x20 || CommandNo == 0x30 || CommandNo == 0x40 || CommandNo == 0x50 || CommandNo == 0x62)
		{
			// 填充IP值
			for (s = 0; s < 4; s++)
			{
				// 1个字转10进制字符串 如:0xDE 转成 222
				j = Bcd2Str(GprsPar[CmdPst[n] + s]);
				// 填充内容到短信返回缓存中
				for (k = 0; k < j; k++) // ZCL 2019.4.26 返回值j最大为5
				{
					SendSmsBuf[SendSmsLength++] = StringBuffer[k];
				}
				// 在前3个数值后加.  如：222.173.103.226
				if (s < 3)
					SendSmsBuf[SendSmsLength++] = '.';
			}
		}

		// 6. 字符串的查询，不是数值 如：0x05：CMNET
		// 服务代码:01		接入点名称:05		DTU身份识别码:06		域名:21.31.41.51
		else if (CommandNo == 0x01 || CommandNo == 0x05 || CommandNo == 0x06 || CommandNo == 0x21 || CommandNo == 0x31 || CommandNo == 0x41 || CommandNo == 0x51)
		{
			s = 0;
			// 填充内容到短信返回缓存中
			while (GprsPar[CmdPst[n] + s])
				SendSmsBuf[SendSmsLength++] = GprsPar[CmdPst[n] + s++];
		}

		// 7. 版本号等的查询，不是数值 如：DTU程序版本:70				1.00.00
		else if (CommandNo == 0x70)
		{
			s = 0;
			while (DtuProgVersion[s])
				SendSmsBuf[SendSmsLength++] = DtuProgVersion[s++];
		}
		else if (CommandNo == 0x71)
		{
			s = 0;
			while (DtuProgMakeDate[s])
				SendSmsBuf[SendSmsLength++] = DtuProgMakeDate[s++];
		}
		else if (CommandNo == 0x73)
		{
			s = 0;
			while (DtuHardWare[s])
				SendSmsBuf[SendSmsLength++] = DtuHardWare[s++];
		}
		else if (CommandNo == 0x74)
		{
			s = 0;
			while (DtuManufacture[s])
				SendSmsBuf[SendSmsLength++] = DtuManufacture[s++];
		}
		else if (CommandNo == 0x75)
		{
			s = 0;
			while (DtuModel[s])
				SendSmsBuf[SendSmsLength++] = DtuModel[s++];
		}

		// 8. 串口的查询，0x0D指令：两个字  0x00,0x00,0x25,0x80 = 9600
		// 目前程序只支持最后一个字的波特率设置，只支持到0-65535
		else if (CommandNo == 0x0D)
		{
			// 8.1 波特率 0x00,0x00,0x25,0x80 中后一个字
			w1 = (GprsPar[CmdPst[n] + 2] << 8) + GprsPar[CmdPst[n] + 3];

			j = Bcd2Str(w1);		// 转成字符串
			for (k = 0; k < j; k++) // ZCL 2019.4.26 返回值j最大为5
			{
				SendSmsBuf[SendSmsLength++] = StringBuffer[k];
			}
			SendSmsBuf[SendSmsLength++] = ',';
			// 8.2 数据位 为8位（00-5， 01-6， 02-7， 03-8）	保存的值为0,1,2,3，要换算成 5,6,7,8，并转成字符
			w1 = GprsPar[CmdPst[n] + 4];
			if (w1 <= 3)
				SendSmsBuf[SendSmsLength++] = w1 + 5 + 0x30; // 换算并转成字符
			else
				SendSmsBuf[SendSmsLength++] = '!';
			SendSmsBuf[SendSmsLength++] = ',';
			// 8.3 停止位 停止位为1位（00-1，04-2）	保存的值0,4要换算成 1,2，并转成字符
			w1 = GprsPar[CmdPst[n] + 5];
			if (w1 == 0 || w1 == 4)
				SendSmsBuf[SendSmsLength++] = w1 / 4 + 1 + 0x30; // 换算并转成字符
			else
				SendSmsBuf[SendSmsLength++] = '!';
			SendSmsBuf[SendSmsLength++] = ',';
			// 8.4 奇偶校验 校验位-无（00-无0，08-奇1，18-偶2）	保存的值0x00，0x08，0x18要换算成 0，1，2，并转成字符
			w1 = GprsPar[CmdPst[n] + 6];
			if (w1 == 0)
				SendSmsBuf[SendSmsLength++] = 0x30; // 换算并转成字符
			else if (w1 == 0x08)
				SendSmsBuf[SendSmsLength++] = 0x31; // 换算并转成字符
			else if (w1 == 0x18)
				SendSmsBuf[SendSmsLength++] = 0x32; // 换算并转成字符
			else
				SendSmsBuf[SendSmsLength++] = '!';
			SendSmsBuf[SendSmsLength++] = ',';
			// 8.5 流控-无流控（01-Xon/Xoff1，02-硬流控2，03-无流控3，04-半双工485 4，05-全双工422 5），并转成字符
			w1 = GprsPar[CmdPst[n] + 7];
			if (w1 >= 1 && w1 <= 5)
				SendSmsBuf[SendSmsLength++] = w1 + 0x30; // 换算并转成字符
			else
				SendSmsBuf[SendSmsLength++] = '!';
		}

		// 9. 双字节参数和单字节参数：数值的查询 0-65535范围内
		else
		{
			// 查找指令是不是双字节参数
			s = FindFirstValue(GprsCmd2Byte, CommandNo, 0x00);
			if (s != 255) // 双字节参数		//s>0 && 		2013.6.4这个不应该要
			{
				w1 = (GprsPar[CmdPst[n]] << 8) + GprsPar[CmdPst[n] + 1];

				j = Bcd2Str(w1);
				for (k = 0; k < j; k++) // ZCL 2019.4.26 返回值j最大为5
				{
					SendSmsBuf[SendSmsLength++] = StringBuffer[k];
				}
			}
			else // 单字节参数
			{
				w1 = GprsPar[CmdPst[n]];

				j = Bcd2Str(w1);
				for (k = 0; k < j; k++) // ZCL 2019.4.26 返回值j最大为5
				{
					SendSmsBuf[SendSmsLength++] = StringBuffer[k];
				}
			}
		}

		// 10. 短信返回
		// 有错误跳过
		if (err)
			;
		// 设置参数成功
		else
		{
			SendSmsBuf[SendSmsLength++] = ',';
			for (s = 0; s < 10; s++)
			{
				SendSmsBuf[SendSmsLength++] = ReadOK[s];
			}
		}
		// 短信末尾加 ; 0x1A   0x0d
		SendSmsBuf[SendSmsLength++] = ';';
		SendSmsBuf[SendSmsLength++] = 0x1A; // CTRL+Z结束(0x1A)
		SendSmsBuf[SendSmsLength++] = 0x0d;
	}

	// 11.把PC设置GPRS PAR情况返回 2013.6.6
	if (B_PcSetGprs == 2 && SendSmsLength > 0)
	{
		B_PcSetGprs = 1;					//=1，表示在PC设置GPRS PAR状态，但还没有收到数据时；=2收到数据
		Cw_Txd2Max = 0;						// 2014.12.3 为了保险
		for (s = 0; s < SendSmsLength; s++) // ZCL 2019.4.26 TXD2_MAX为512，长度够用
			Txd2Buffer[Cw_Txd2Max++] = SendSmsBuf[s];
		SendSmsLength = 0;
		Cw_Txd2 = 0;
		RS485_CON = 1; // 2013.9.2
		// 2010.8.5 周成磊 TXE改成TC，一句改为两句
		// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
		USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
		USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
	}
}

// 短信功能函数		M35_LOOP()中调用
void Sms_Function(void) // 短信指令：重启，硬件重启，看门狗复位重启，设备启停，短信返回-进行发送短信，读短信，短信满-删除短信
{
	uchar s;
	uint i;
	// char ComeSmsPhone[11]="15610566803";				//来短信的电话号码

	// 1. 短信控制DTU重启成功，和短信返回
	// 2. 短信控制DTU硬件重启成功，和短信返回
	// 2.5 短信看门狗复位重启 2014.12.8
	// 3. 短信控制设备启停（通过串口2向SM510发送启停指令）；并短信返回执行结果
	//--------------------------------------------------------------
	// 4. 短信查询、设置、控制指令的 短信返回！ 2012.9.23  如：AT+CMGS="13001697270"
	// 5. 发送指令，去读短信（接收到新短信提示）
	// 6. 短信满，删除全部短信  2012.9.24
	//--------------------------------------------------------------
	// 7. 如果到时间没有收到启停返回，再次发送启停指令

	// ZCL 2017.5.26  1.  2.  2.5 从下面移出
	// 2012.10.12 加入短信控制功能  短信重启，短信紧急重启，短信控制设备启停
	// 1. 短信控制DTU重启成功，和短信返回
	if (B_SmsDtuRestart == 1) // 0x82：重启
	{
		B_SmsDtuRestart = 2;
		S_M35 = 0x01;	 // 模块关机，重启
		B_RestartNo = 2; // ZCL 2019.4.19 重启序号，方便打印观察问题
						 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
	}
	// 重启完毕后，等S_M35==0x1B，发送DtuRestartOK短信信息
	else if (B_SmsDtuRestart == 2 && S_M35 == 0x1B) //&& C_M35SendSum>5
	{
		B_SmsDtuRestart = 0;
	}

	// 2. 短信 或者故障(2014.12.8) 控制DTU硬件重启成功，和短信返回
	// 短信控制DTU硬件重启
	if (B_SmsOrFaultHardRestart == 1) // 0x91：模块上电重启		GPW+91,1;
	{
		B_SmsOrFaultHardRestart = 2;
		S_M35 = 0x01;	 // 模块关机，重启
		B_RestartNo = 3; // ZCL 2019.4.19 重启序号，方便打印观察问题
						 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
	}
	// 注意这里B_DtuHardRestart=3，不是2。等S_M35==0x1B，发送DtuHardRestartOK短信信息
	else if (B_SmsOrFaultHardRestart == 3 && S_M35 == 0x1B)
	{
		B_SmsOrFaultHardRestart = 0;
	}

	// ZCL 2019.4.26	 故障控制DTU硬件重启
	if (B_SmsOrFaultHardRestart >= 11 && B_SmsOrFaultHardRestart <= 16) // ZCL 2019.4.23
	{
		S_M35 = 0x01; // 模块关机，重启
		// B_RestartNo=4;			//ZCL 2019.4.19 重启序号，方便打印观察问题

		B_RestartNo = B_SmsOrFaultHardRestart; // ZCL 2019.4.23
		B_SmsOrFaultHardRestart = 22;		   // 12	ZCL 2019.4.23

		// printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
	}
	else if (B_SmsOrFaultHardRestart == 23 && S_M35 == 0x1B) // 13	ZCL 2019.4.23
	{
		B_SmsOrFaultHardRestart = 0;
	}

	// 2.5 短信看门狗复位重启 2014.12.8
	if (B_SmsWatchdogRestart == 1) // 0x95:GP311模块看门狗复位重启 2013.7.3  //GPW+95,1;
	{
		B_SmsWatchdogRestart = 2;
		S_M35 = 0x01;	 // 模块关机，重启
		B_RestartNo = 5; // ZCL 2019.4.19 重启序号，方便打印观察问题
						 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
						 // ZCL 2019.4.19 Delay_MS(3000);					//此时间内没有喂狗，设备复位！ 2013.7.3
	}
	// 重启完毕后，等S_M35==0x1B，发送短信信息
	else if (B_SmsWatchdogRestart == 2 && S_M35 == 0x1B)
	{
		B_SmsWatchdogRestart = 0;
	}

	// 限制：没有发送心跳包，没有连接查询和设置、透传指令
	if (S_HeartSend + S_SendLink + S_DtuInquireSend + S_DtuSetSend + S_GprsDataSend == 0)
	{
		// ZCL 2017.5.26  1.  2.  2.5 移出到上面

		// 3. 短信控制设备启停（通过串口2向SM510发送启停指令）；并短信返回执行结果
		// 停止设备: 02 06 01 3D 00 01			说明：0x013D: 是SM510中的317地址
		// 启动设备: 02 06 01 3D 00 00
		if (B_SmsRunStop == 1) //=1收到短信控制设备启停指令，利用串口2给SM510发启停指令
		{
			B_SmsRunStop = 2; // 发送一次启停指令后，让B_SmsRunStop=2，没有收到延时继续发，收到=3
			C_SmsRunStop = 0;
			//
			Txd2Buffer[0] = 0x02;				 // 设备从地址
			Txd2Buffer[1] = 0x06;				 // 功能码
			Txd2Buffer[2] = 0x01;				 // 寄存器地址
			Txd2Buffer[3] = 0x3D;				 // 寄存器地址
			Txd2Buffer[4] = 0x00;				 // 预置数据高位
			Txd2Buffer[5] = SmsControlStopValue; // 预置数据低位
			i = CRC16(Txd2Buffer, 6);
			Txd2Buffer[6] = i >> 8; // /256
			Txd2Buffer[7] = i;		// 低位字节
			// 2013.6.28 防止短信启停指令通过串口2发送给SM510时丢失，如果没有返回，重试10次
			for (i = 0; i < 8; i++)
			{
				BKCom1TxBuf[i] = Txd2Buffer[i];
			}
			B_SendSmsRunStopCmdCount = 0; // 发送短信运行停止命令计数器
			C_SmsRunStop = 0;			  // 短信运行停止计数器清零
			//
			Cw_Txd2Max = 8;
			Cw_Txd2 = 0;
			RS485_CON = 1; // 2013.9.2
			// 2010.8.5 周成磊 TXE改成TC，一句改为两句
			// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
			USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
			USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
		}
		// 2014.12.5移到这里。 如果到时间没有收到启停返回，再次发送启停指令  2013.6.28
		else if (B_SmsRunStop == 2 && C_SmsRunStop > 250) // C_SmsRunStop:10Ms
		{
			if (S_SendSms + S_ReadNewSms + S_SmsFull == 0) //+SendSmsLength
			{
				B_SendSmsRunStopCmdCount++;
				if (B_SendSmsRunStopCmdCount > 10) // 超过10次，清除 2013.6.30
				{
					B_SendSmsRunStopCmdCount = 0;
					B_SmsRunStop = 0;
					SendSmsLength = 0;
				}
				else
				{
					for (i = 0; i < 8; i++)
					{
						Txd2Buffer[i] = BKCom1TxBuf[i];
					}
					Cw_Txd2Max = 8;
					Cw_Txd2 = 0;
					RS485_CON = 1; // 2013.9.2
					// 2010.8.5 周成磊 TXE改成TC，一句改为两句
					// USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				// 开始发送.
					USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 开始发送.
					USART_SendData(USART2, Txd2Buffer[Cw_Txd2++]);
				}
			}
			C_SmsRunStop = 0;
		}
		// 通过串口2向SM510发送启停指令，再接收到SM510发送回来的应答信号后，B_SmsControlStop=3
		// 然后，短信返回执行结果。发送SmsControlStopOK短信信息
		// 串口2中判断条件 if(Rcv2Buffer[2]==0x01 && Rcv2Buffer[3]==0x3D )
		else if (B_SmsRunStop == 3)
		{
			B_SmsRunStop = 0;
		}

		//--------------------------------------------------------------
		// 4. 短信 查询、设置参数、控制指令的短信返回，进行发送短信
		// 条件：B_SmsDtuRestart+B_SmsOrFaultHardRestart+B_SmsRunStop==0 上面控制都已经执行完
		// 2014.12.7 加入B_ZhuDongSendSms(连接4的域名，前面带@为电话号码，主动回发短信。如：@15610566803)
		// ZCL 2019.4.26 说明：SendSmsLength>0 说明短信内容已经填充完毕，就等各标志清零，就可以发送短信了！
		//		但是不是看门狗重启，否则SendSmsLength就被清零了。
		if ((SendSmsLength > 0 || B_ZhuDongSendSms) && S_ReadNewSms + S_SmsFull == 0 && B_SmsDtuRestart + B_SmsOrFaultHardRestart + B_SmsRunStop + B_SmsWatchdogRestart == 0 && B_Com3Cmd8D == 0 && C_PowenOnDelay > 4500) // 2014.12.8 加入时间的限制
		{
			// 发送 AT+CMGS="13001697270"
			if (S_SendSms == 0)
			{
				S_SendSms = 1;
				C_SendSms = 0;
				C_BetweenSendSms = 0;
			}
			if (S_SendSms == 1 && C_BetweenSendSms > 3)
			{
				S_SendSms = 2;
				Cw_Txd3Max = 0;
				//
				Gprs_TX_String("AT+CMGS=");
				Gprs_TX_Byte('"');
				// 2014.12.7 加入主动发送短信，来获得电话号码
				if (B_ZhuDongSendSms)
				{
					for (s = 0; s < 11; s++)
					{
						Txd3Buffer[Cw_Txd3Max++] = GprsPar[DomainName3Base + 1 + s];
					}
				}
				// 常规部分
				else
				{
					for (s = 0; s < 11; s++)
					{
						Txd3Buffer[Cw_Txd3Max++] = ComeSmsPhone[s];
					}
				}
				Gprs_TX_Byte('"');
				Gprs_TX_Byte(0x0d);
				Gprs_TX_Start();
				//
				C_BetweenSendSms = 0;
			}
			// 2014.12.11 	因为遇到收不到AT_3EOk_Flag标志，所以加上延时后，重发数据
			else if (S_SendSms == 2 && C_BetweenSendSms > 500)
			{
				S_SendSms = 1; // ZCL1
				C_BetweenSendSms = 0;
			}
			else if (S_SendSms == 2 && AT_3EOk_Flag)
			{
				// 收到'>'后，发送内容 CTRL+Z结束(0x1A)
				S_SendSms = 3;
				AT_3EOk_Flag = 0;
				Cw_Txd3Max = 0;
				// 2014.12.7 加入主动发送短信，来获得电话号码
				if (B_ZhuDongSendSms)
				{
					for (s = 0; s < 21; s++)
					{
						Txd3Buffer[Cw_Txd3Max++] = ZhuDongSendSMS[s];
					}
					// 2014.12.9 !!! 短信末尾加 ; 0x1A   0x0d
					Txd3Buffer[Cw_Txd3Max++] = ';';
					Txd3Buffer[Cw_Txd3Max++] = 0x1A; // CTRL+Z结束(0x1A)
					Txd3Buffer[Cw_Txd3Max++] = 0x0d;
				}
				// 常规部分
				else
				{
					for (s = 0; s < SendSmsLength; s++) // ZCL 2019.4.26 TXD3_MAX为512，长度够用
					{
						Txd3Buffer[Cw_Txd3Max++] = SendSmsBuf[s];
					}
					// 2014.12.9 !!! 短信末尾加 ; 0x1A  0x0d  (正常的设置和查询短信已经在填充时加上)
				}
				Gprs_TX_Start();
				//
				C_BetweenSendSms = 0;
			}
			// 2014.12.11 	因为遇到收不到AT_ComOk_Flag标志，所以加上延时后，重发数据
			// 去掉，否则在第一次时容易收到两条短信。
			// 原因：发短信时，收到OK会非常慢，大约4-5秒。干脆取消了！
			/* 			else if(S_SendSms==3 && C_BetweenSendSms>1000)
						{
							S_SendSms=1;		//ZCL2
							C_BetweenSendSms = 0;
						}		 */
			else if (S_SendSms == 3 && AT_ComOk_Flag)
			{
				AT_ComOk_Flag = 0;
				SendSmsLength = 0;
				B_ZhuDongSendSms = 0;
				C_SendSms = 0;
				S_SendSms = 0;
				//
				if (B_SmsSetup)
				{
					B_ForceSavPar = 1; // 对短信设定的参数，进行保存
					SavePar_Prompt();  // 保存参数 2012.10.11
					//
					B_SmsSetup = 0; // 短信设定参数 2013.1.24
				}
			}

			// 发送不成功 =8的时候可以返回S_SendSms=1状态继续发送
			/* 			if(C_SendSms==8 )
						{
							AT_ComOk_Flag=0;
							//SendSmsLength=0;
							S_SendSms=1;
						}	 */
			// 发送还不成功 =15的时候，丢弃退出
			if (C_SendSms > 10)
			{
				AT_ComOk_Flag = 0;
				SendSmsLength = 0;
				B_ZhuDongSendSms = 0;
				C_SendSms = 0;
				S_SendSms = 0;
			}
		}

		// 5. 发送指令，去读短信（接收到新短信提示）
		// w_NewSmsNo S_ReadNewSms在接收到短信后，Com3_RcvProcess()中清零。
		else if (w_NewSmsNo > 0 && S_SendSms + S_SmsFull == 0) // 间隔1S才会进来一次
		{
			if (S_ReadNewSms == 0)
			{
				S_ReadNewSms = 1;
				C_ReadNewSms = 0;
			}

			// 2014.12.9 收到+CMTI(新短信提示)别立即去读短信，否则读完可能还会收到新短信
			//					提示（M35可能有BUG，还是同一条短信）
			// 加入2秒的延时！ && C_ReadNewSms==2
			if (S_ReadNewSms == 1 && C_ReadNewSms == 2) //!
			{
				S_ReadNewSms = 2;
				At_CMGR(w_NewSmsNo); // 读短信指令
			}
			else if (S_ReadNewSms == 2)
			{
				// 读不成功 =3,=6的时候可以返回 S_ReadNewSms=1状态继续读
				if (C_ReadNewSms == 4 || C_ReadNewSms == 8)
				{
					S_ReadNewSms = 1;
				}
				// 读还不成功 =9的时候，丢弃退出
				else if (C_ReadNewSms > 10)
				{
					w_NewSmsNo = 0;
					S_ReadNewSms = 0;
					C_ReadNewSms = 0;
				}
			}
		}

		// 6. 短信满，删除全部短信  2012.9.24
		else if (SmsFull && S_SendSms + S_ReadNewSms == 0)
		{
			if (S_SmsFull == 0)
			{
				S_SmsFull = 1;
				C_SmsFull = 0;
			}

			if (S_SmsFull == 1)
			{
				S_SmsFull = 2;
				At_CMGD('4');
			}
			else if (S_SmsFull == 2)
			{
				if (AT_ComOk_Flag)
				{
					AT_ComOk_Flag = 0;
					SmsFull = 0;
					S_SmsFull = 0;
					C_SmsFull = 0;
				}
			}
			// 删除不成功 =7的时候可以返回 S_SmsFull=1状态继续删除
			if (C_SmsFull == 5)
			{
				AT_ComOk_Flag = 0;
				S_SmsFull = 1;
			}
			// 删除还不成功 =12的时候，丢弃退出
			else if (C_SmsFull > 10)
			{
				AT_ComOk_Flag = 0;
				SmsFull = 0;
				S_SmsFull = 0;
				C_SmsFull = 0;
			}
		}
	}
}

/*	GPRS操作函数	 M35初始化、上电、断电 */
void M35_Init(void) // GPRS操作函数	 M35初始化、上电、断电
{
	uchar i;
	// PWR_KEY=0;							//模块开机或者关机脚 =0经过三极管出来高，模块不开机
	//	DTR=0;									//DTR管脚拉低20ms后,串口被激活

	// S_M35PowerOnOff=0;
	S_M35 = 0x01; // M35的状态 2012.7.20
	AT_ComOk_Flag = 0;
	AT_3EOk_Flag = 0;
	AT_CIPSEND_Flag = 0;
	C_M35PowerOnOff = 0;
	C_M35SendSum = 0;
	C_DtuInquireSend = 0; // DTU查询指令计数器
	C_DtuSetSend = 0;	  // DTU设置指令计数器
	C_GprsDataSend = 0;	  // Gprs数据传输计数器
	C_ReadNewSms = 0;	  // 读新的短信计数器
	C_SendSms = 0;		  // 发送短信
	C_SmsFull = 0;		  // 短信满，删除短信

	S_SendSms = 0;	  // 发送短信步骤
	S_ReadNewSms = 0; // 读新短信步骤
	S_SmsFull = 0;	  // 短信满步骤
	S_SendLink = 0;	  // 发送连接步骤
	S_HeartSend = 0;
	S_DtuInquireSend = 0;
	S_DtuSetSend = 0;
	S_GprsDataSend = 0;

	CGD0_ConnectNo = 0;
	CGD1_ConnectNo = 0;
	SendDataReturnNo = 0; // 发送数据返回序号(串口过来的数据发到每个连接)
	w_NewSmsNo = 0;
	SmsFull = 0; // 短信满标志
	SendHeartNo = 0;
	ChannelNo = 0;

	B_Com3Cmd8B = 0;  // 查询DTU参数命令 2012.8.17
	B_Com3Cmd8D = 0;  // 设置DTU参数命令 2012.8.24
	B_Com3Cmd89 = 0;  // 服务器发送数据指令 2011.8.24
	B_DoWithData = 0; // 处理数据

	B_ATComStatus = 0;
	B_SmsSetup = 0;		  // 短信设定参数
	B_GprsDataReturn = 0; // GPRS 数据返回
	B_NextS35Cmd = 0;	  // 执行下一个S35命令
	B_ZhuCeOkNum = 0;	  // 注册OK的数量
	B_RunGprsModule = 0;  // 上电没有与文本通讯，4秒后此值=1
	B_ZhuDongSendSms = 0; // 主动发送短信(收到连接4的域名带@电话号码后) 2014.12.7

	for (i = 0; i < 4; i++)
	{
		C_HeartDelay[i] = 0;
		B_HeartSendAsk[i] = 0;
		B_HeartSendFault[i] = 0;
		S_HeartSendNow[i] = 0;
		C_HeartSendNow[i] = 0;

		C_LinkDelay[i] = 0;
		B_LinkAsk[i] = 0;
		B_LinkFault[i] = 0;
		S_LinkNow[i] = 0;
		C_LinkNow[i] = 0;
		B_FirstLink[i] = 0;
		B_ConnectFail[i] = 0;
		B_RcvFail[i] = 0;

		B_LinkAsk_DomainName[i] = 0;
		S_LinkNow_DoName[i] = 0;
		C_LinkNow_DoName[i] = 0;
		B_FirstLink_DomainName[i] = 0;
	}

	for (i = 0; i < 6; i++)
	{
		CGD0_LinkConnectOK[i] = 0;
		CGD1_LinkConnectOK[i] = 0;
		ZhuCeOkFLAG[i] = 0;
	}

	B_WaitCallReady = 0;
	B_RestartNo = 0; // ZCL 2019.4.19 重启序号，方便打印观察问题

	// 2016.7.1 ZCL 打开接收，防止没有开接收中断，导致接收不到数据。
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART2->CR1 |= 0x0004; //.3位  运行接收
}

/*	PWRKEY：（IN）拉低一段规定时间来开机或者关机；经过三极管后反着极性用。
		RESET_4G：（IN）拉低该脚200ms以上能重启模块；经过三极管后反着极性用。
		STATUS_4G：（OUT）指示模块的运行状态。高电平表示模块开机，低电平表示模块关机。
				1.VBAT上电后，PWRKEY 管脚可以启动模块，并且是低电平开机，在STATUS_4G 管脚输
		出高电平之后，开机成功，PWRKEY 管脚可以释放。可以通过检测STATUS_4G管脚的电平来
		判别模块是否开机。推荐使用开集驱动电路来控制PWRKEY 管脚。
				2.在拉低管脚PWRKEY之前，保证VBAT电压稳定。建议VBAT上电到管脚PWRKEY拉低
		之间的时间间隔为30毫秒。
				3.注意：可以通过检测 STATUS_4G 电平来判别模块是否开机。STATUS_4G 管脚输出高电平后，
		PWRKEY 可以释放。如果不使用 STATUS_4G 管脚，则拉低  PWRKEY 至少26秒来开启模块。
				4.正常关机之后，PWRKEY拉低一段时间可以重启模块。在检测到STATUS_4G管脚为低电平
		之后，等待至少500ms才能重启模块。
*/
void M35_Power_ONOFF(void) // M35 电源上电断电
{
	// 2012.10.12 加入紧急重启
	if (B_SmsOrFaultHardRestart == 2 || B_SmsOrFaultHardRestart == 22) // ZCL 2019.4.23
	{
		if (S_M35PowerOnOff == 0)
		{
			RESET_4G = 1; // =1经过三极管出来低(>100ms并且<500ms，推荐200ms)，模块重启
			S_M35PowerOnOff = 1;
			C_M35PowerOnOff = 0; // YLS 2023.12.06 原来少了这一句，导致复位延时时间太短，几乎没有，导致复位不成功
		}
		else if (S_M35PowerOnOff == 1 && C_M35PowerOnOff > 20) //(>200ms) 发出低电平复位信号，至少200ms，否则复位不成功
		{
			RESET_4G = 0; // =0 取消重启信号
			S_M35PowerOnOff = 2;
			C_M35PowerOnOff = 0;
		}
		else if (S_M35PowerOnOff == 2 && C_M35PowerOnOff > 210) // 紧急关机后至少等待2s，再重启
		{
			S_M35PowerOnOff = 0;
			C_M35PowerOnOff = 0;
			if (B_SmsOrFaultHardRestart == 2)
				B_SmsOrFaultHardRestart = 3; // 此标志置成3，后面按照正常重启
			else if (B_SmsOrFaultHardRestart == 22)
				B_SmsOrFaultHardRestart = 23; // 13	ZCL 2019.4.23
		}
	}

	// 正常开机，关机(重启)
	else
	{
		// 1. A上电开机; B关机，准备重启
		if (S_M35PowerOnOff == 0)
		{
			// A上电开机
			if (STATUS_4G == 0) // 判断STATUS_4G==0原先没有开机，进行上电开机
			{
				PWR_KEY = 1; // 此时为开机	//模块开机或者关机脚 =1经过三极管出来低，模块开机（0.5s以上的低电平脉冲，开机）
				S_M35PowerOnOff = 1;
				C_M35PowerOnOff = 0;
				M35_Init();
			}
			// B关机，准备重启
			else // 判断STATUS_4G==1原先已经开机，进行关机 重启
			{
				PWR_KEY = 1;		  // 此时为关机，经过三极管出来低（2.5s到5s的低电平脉冲，关机）
				S_M35PowerOnOff = 11; // 模块关机，重启
				C_M35PowerOnOff = 0;  // 如果STATUS_4G为高电平，即开机状态，则PWR_KEY发送一个2.5s-5s的低电平脉冲，延时26s即可关机
			}
		}
		// 2. 等待STATUS_4G==1，开机成功
		// （不用STATUS_4G时 t > 2000ms，使用STATUS_4G只需要延时>(1000MS-800MS)即可）
		else if (S_M35PowerOnOff == 1 && STATUS_4G)
		{
			/*2013.10.17 下面这句PWR_KEY=0注释掉，因为STATUS_4G为高电平后，PWR_KEY还需要延时一点点时间。
把PWR_KEY=0加在ComOK_OverTime:里，检测到AT_CallReadyOk_Flag后  */
			// PWR_KEY=0;							//=0经过三极管出来高，正常运行退出开机时序
			//			S_M35PowerOnOff = 2; // 开机成功，让S_M35PowerOnOff=2，后面M35_LOOP()中判断用
			//			C_M35PowerOnOff = 0;
			if (C_M35PowerOnOff > 30)
			{
				S_M35PowerOnOff = 2;
				C_M35PowerOnOff = 0;
				PWR_KEY = 0; //=0经过三极管出来高，正常运行退出开机时序
			}
		}

		// 关机延时，再返回开机（先关机再重启）
		// 11.关机延时
		else if (S_M35PowerOnOff == 11) // 此时STATUS_4G=1
		{
			if (C_M35PowerOnOff > 300) // 拉低PWRKEY(5S>Pulldown>2.5s)一段时间，模块关机
			{
				S_M35PowerOnOff = 12;
				C_M35PowerOnOff = 0;
				PWR_KEY = 0; //=0经过三极管出来高，正常运行退出关机时序	(SIM7600_硬件设计手册_V1.12 P30)
			}
		}
		// 12. 判断是否已经关机
		else if (S_M35PowerOnOff == 12)
		{
			// 在检测到STATUS_4G管脚为低电平之后，等待至少500ms才能重启模块。(M35_硬件设计手册_V1.2 P31)
			if (!STATUS_4G) // 当检测到STATUS_4G为低时，关机完毕；
			{				// 等待至少1000ms才能重启模块 2013.6.27！
				S_M35PowerOnOff = 13;
				C_M35PowerOnOff = 0;
			}
			else if (C_M35PowerOnOff > 6000) // 防止长时间STATUS_4G=1
			{
				S_M35PowerOnOff = 0; // 关机时间典型值为26s，如果大于60s，还没有关机完毕，回到0状态重新开机
				C_M35PowerOnOff = 0;
			}
		}
		// 13.	关机完毕，延时1000MS再返回（进行重新开机）
		else if (S_M35PowerOnOff == 13)
		{
			if (C_M35PowerOnOff > 100) // 延时>1s
			{
				S_M35PowerOnOff = 0; // 回到0状态重新开机
				C_M35PowerOnOff = 0; // 如果STATUS_4G为低电平，即关机状态，则PWR_KEY发送一个0.5s的低电平脉冲，延时12s-16s即可开机
			}
		}
	}
}

/*	GPRS操作函数	 M35循环状态 */
void M35_LOOP(void) // GPRS操作函数	 M35循环状态
{
	// uchar i;
	uint w;

	// C_M35Step在定时中断中加1(10MS计数器),mian()中C_M35Step>=100执行M35_LOOP()
	C_M35Step = 0;
	B_NextS35Cmd = 0; // 执行下一个S35命令

	// 周成磊 2012.10.15 出现模块运行一段时间后STATUS_4G=0，关机现象！OK解决：电源能力不足导致
	if (STATUS_4G == 0 && S_M35 == 0x1B)
	{
		// S_M35 = 0x01;			//调试时用，这样代表故障了，重新运行。
		// B_ForceSavPar=1;							//保存参数
		// Pw_GprsFaultRestart = Pw_GprsFaultRestart + 1; // 故障重启次数加1，观看用
	}

	// 为了文本显示器跟GP311相连，通讯用。(上电没有与文本通讯，4秒后此值=1)
	// 当C_DelayRunGprsModule>4，说明文本没有数据访问，退出与文本相连，正常运行GPRS
	if (B_RunGprsModule == 0 && C_DelayRunGprsModule > 4)
	{
		B_RunGprsModule = 1; //=1，才可以正常运行GPRS流程
							 // S_M35 = 0x01;
	}

	// ZCL 2019.4.19 //=5，=6 为看门狗重启，等待打印完毕。
	// ZCL 2019.4.23 刚进来时，打印一次，然后延时！ 集中在这里进行打印！非常重要，避免跟串口输出冲突
	if (B_RestartNo > 0 && S_M35 == 0x01 && Cw_Txd2Max == 0) // Txd1Max ZCL 2019.4.25
	{
		if (T_WatchdogRestartDelay != SClk10Ms)
		{
			T_WatchdogRestartDelay = SClk10Ms;

			// ZCL 2019.4.23 刚进来时，打印一次，然后延时！ 集中在这里进行打印！非常重要，避免跟串口输出冲突
			if (C_WatchdogRestartDelay == 0)
				printf("RestartNo=%d\r\n", B_RestartNo); // ZCL 2019.4.19 重启序号，方便打印观察问题

			// printf("B_SmsOrFaultHardRestart=%d\r\n", B_SmsOrFaultHardRestart);
			C_WatchdogRestartDelay++;

			if (C_WatchdogRestartDelay > 200)
			{
				C_WatchdogRestartDelay = 0;
				//=5 0x95 短信看门狗重启； =6 收不到CallReady重启
				if (B_RestartNo == 5 || B_RestartNo == 6)
					Delay_MS(3000); // 此时间内没有喂狗，设备复位！ 2013.7.3
				//
				B_RestartNo = 0;
			}
		}
	}

	// B_RunGprsModule=0 B_ATComStatus=1 B_PcSetGprs=1 跳过M35循环操作的情况 ！
	// B_RunGprsModule=0 上电4秒后，判断是否与文本通讯
	// B_ATComStatus=1		AT命令状态
	if (!B_RunGprsModule || B_ATComStatus || B_PcSetGprs || B_RestartNo > 0) //=5，=6 为看门狗重启，等待打印完毕。ZCL 2019.4.19
		;
	else
		// M35的操作循环
		switch (S_M35)
		{
		case 0x00:
		case 0x01: // 开机.关机.重启
			M35_Power_ONOFF();
			if (S_M35PowerOnOff == 2 && STATUS_4G)
			// if (STATUS_4G)
			{
				S_M35PowerOnOff = 0;
				C_M35SendSum = 0;
				S_M35 = 0x03;

				/* Enable USART3 Receive and Transmit interrupts */
				USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 此时开接收，防止接收到乱码

				// 2016.7.1 ZCL 打开接收，防止没有开接收中断，导致接收不到数据。
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				USART2->CR1 |= 0x0004; //.3位  运行接收
			}
			w_GprsCSQ = 0;
			break;
		case 0x03:									// 等待通信成功
			if (!AT_ComOk_Flag && C_M35SendSum > 2) // 上电成功后延时发送指令
				At_AT();
			goto ComOK_OverTime;
		case 0x04: // 设置AT命令回显
			if (!AT_ComOk_Flag)
			{
				// 利用连接4的本地端口号参数，可以打开回显，=11011时；
				// 2013.9.26 注意正常运行时关闭回显
				w = GprsPar[LoclaPort3Base]; // ！周成磊 2013.9.26
				w = (w << 8) + GprsPar[LoclaPort3Base + 1];
				if (w == 11011)
					At_ATE('1'); // 0关闭回显	1打开回显(默认)
				else
					At_ATE('0'); // 0关闭回显	1打开回显(默认)
			}

			goto ComOK_OverTime;
		case 0x05: // 等待Call Ready信号到达		2012.9.12
			/* 								if(B_WaitCallReady==0)			//ZCL 2019.4.23 删除  否则短信重启，不返回短信，并且重启2遍
											{
												if(!AT_ComOk_Flag && C_M35SendSum >1)	//
													At_QIURC();
											} */
			if (!AT_ComOk_Flag && C_M35SendSum > 3) // 上电成功后延时发送指令
				At_AT();
			goto ComOK_OverTime;

		case 0x06: // 检查信号强度  2017.5.25添加，后面case 号顺便增加 (6-E改成7-F)
				   //+CSQ: 23,0  (一般15以上就还可以了)
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CSQ();
			goto ComOK_OverTime;

		case 0x07:									 // 等待注册网络		查询GSM注册状态  返回1,5 已注册
			if (!AT_ComOk_Flag && C_M35SendSum >= 1) //
				At_CREG();
			goto ComOK_OverTime;
		case 0x08: // 设置波特率
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			// 	At_IPR(); // 19200
			goto ComOK_OverTime;

		// 短信初始化
		case 0x09:									 // 选择短消息存储器
			if (!AT_ComOk_Flag && C_M35SendSum >= 1) //
				At_CPMS();							 // AT+CPMS="SM","SM","SM"  返回:+CPMS: 8,40,8,40,8,40
			goto ComOK_OverTime;
		case 0x0A: // 配置短消息模式 =0 PDU； =1 文本
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CMGF('1'); //=1 文本
			goto ComOK_OverTime;
		case 0x0B: // 选择 TE字符集  默认用GSM字符集，以后加上UCS2
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CSCS(); // 默认用GSM字符集
			goto ComOK_OverTime;
		case 0x0C: // 短信提示设置 AT+CNMI=2,2,0,0,0
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CNMI();
			goto ComOK_OverTime;
		case 0x0D: // 设置文本格式下发送参数  AT+CSMP=17,169,0,241
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CSMP();
			goto ComOK_OverTime;
		case 0x0E: // 显示文本格式参数 =0显示部分参数 =1显示全部参数
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CSDH(); //=0
			goto ComOK_OverTime;
		case 0x0F: // 确认短信息存储器,并查询容量
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CPMSInq(); // AT+CPMS?
			goto ComOK_OverTime;

			// GPRS
		case 0x10: // GPRS附着
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_CGATT();
			goto ComOK_OverTime;
		case 0x11: // 配置前置场景
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			//                if(!AT_ComOk_Flag)
			//									At_QIFGCNT('0');
			goto ComOK_OverTime;
		case 0x12: // 设置GPRS连接方式 0:CSD连接 1: GPRS连接
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			//                if(!AT_ComOk_Flag)
			//									At_QICSGP();			//1:GPRS连接
			goto ComOK_OverTime;
		case 0x13: // 设置服务器地址为IP方式		=0 IP访问; =1 域名访问
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			//                if(!AT_ComOk_Flag)
			//									At_QIDNSIP('1');		//=1 可以同时满足IP或者域名访问 2012.10.4 周成磊
			goto ComOK_OverTime;
		case 0x14: // 查看数据显示格式 1: 加上IP及端口号
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			//                if(!AT_ComOk_Flag)
			//									At_QISHOWRA('1');
			goto ComOK_OverTime;
		case 0x15: // 查看数据显示格式 1: 加上IPD<Len>
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			//                if(!AT_ComOk_Flag)
			//									At_QIHEAD('1');
			goto ComOK_OverTime;
		case 0x16: // 查看数据显示格式 1: 加上协议类型
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			//                if(!AT_ComOk_Flag)
			//									At_QISHOWPT('1');
			goto ComOK_OverTime;
		case 0x17: // 0:单路连接   1: 多路连接
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_AT();
			//                if(!AT_ComOk_Flag)
			//									At_QIMUX('1');
			goto ComOK_OverTime;
		case 0x18: //=0非透传  =1使用透传模式
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_QIMODE('0'); //=0非透传  =1透传模式
			goto ComOK_OverTime;

		case 0x19: // 收到串口数据后等待200ms发送，或串口数据达到512字节后，
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_NETOPEN();
			//                if(!AT_ComOk_Flag)
			//									At_QITCFG();
			goto ComOK_OverTime;
		case 0x1A: // 关闭AT命令回显
			if (!AT_ComOk_Flag && C_M35SendSum >= 1)
				At_ATE('0'); // 0关闭回显
			goto ComOK_OverTime;

		// 在正常运行中	S_M35一直为0x1B，这样可以时刻满足条件，再去发送心跳包，以及建立连接
		case 0x1B:				 // 这三条语句全部注释掉，可以进行M35串口发指令测试 2012.10.4 周成磊
			if (B_RunGprsModule) //=1，才可以正常运行GPRS流程
			{
				Send_DomainName();
				// 连接功能 2012.10.4  测试时，把里面的0改成100
				Link_Function();

				// 心跳延时+发送心跳注册包，1先发指令.通道号.长度 At+QISEND=X,Y；2后发内容
				Gprs_HeartDelaySend(); // 心跳包的延时发送函数，每一秒进入一次
				// 短信功能 2012.9.19
				Sms_Function();
			}
			break;

		case 0x30: // 关闭当前TCP连接
			if (!AT_ComOk_Flag)
				At_QICLOSE(0);
			goto ComOK_OverTime;
		case 0x31: // 关闭当前的GPRS场景。
			if (!AT_ComOk_Flag)
				At_QIDEACT();
			goto ComOK_OverTime;

		case 0x41: // 模块睡眠允许功能
			if (!AT_ComOk_Flag)
				At_QSCLK('1');
			// goto  ComOK_OverTime;

		ComOK_OverTime:
			if (AT_ComOk_Flag)
			{
				/* 									if(S_M35==0x03)				S_M35 = 0x05;
													else  */
				if (S_M35 == 0x0F)
					S_M35 = 0x10; // ZCL 2017.5.25 0x0E改成0x0F
				else if (S_M35 == 0x41)
					S_M35 = 0x00;
				else
				{
					S_M35++;
					C_PowenOnDelay = 0;
				}

				AT_ComOk_Flag = FALSE;
				B_NextS35Cmd = 1; // 置1：执行下一个S35命令
				C_M35SendSum = 0; // 清空发送延时计数器
								  // break;
			}
			//			else if (AT_CallReadyOk_Flag && S_M35 == 5)
			else if ((AT_ComOk_Flag || AT_CallReadyOk_Flag) && S_M35 == 5)
			{
				S_M35 = 0x06;
				AT_ComOk_Flag = 0;
				AT_CallReadyOk_Flag = 0;

				/*2013.10.17 下面这句PWR_KEY=0 加在此处，因为STATUS_4G为高电平后，
					PWR_KEY还需要延时一点点时间。
					把PWR_KEY=0加在ComOK_OverTime:里，检测到AT_CallReadyOk_Flag后  */
				PWR_KEY = 0; //=0经过三极管出来高，正常运行退出开机时序

				// 2015.1.5 补上这一句
				C_M35SendSum = 0; // 清空发送延时计数器
			}

			// 2015.1.5 加入这个，因为发现3G卡等待CallReady信号需要更长的时间，16秒了。
			// 2019.4.11 ZCL 仿真发现GPRS偶尔灯闪，就是不上线问题？仿真因为：S_M35==5，收不到CallReady。解决办法：用看门狗超时重启好了！
			else if (S_M35 == 5)
			{
				if (C_M35SendSum > 95)
				{
					S_M35 = 0x01;	 // 模块关机，重启
					B_RestartNo = 6; // ZCL 2019.4.19 重启序号，方便打印观察问题
									 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
									 // ZCL 2019.4.19 Delay_MS(3000);					//看门狗重启，此时间内没有喂狗，设备复位！ 2019.4.11
				}
				// S_M35 = 0x01;			//ZCL123 测试 2019.4.10
			}

			// 2015.1.17 加入这个，因为发现+CREG: 0,2 17次才出现+CREG: 0,1（注册成功）
			else if (S_M35 == 7) // ZCL 2017.5.25 6改成7,因为添加case 6为信号强度检测；
			{
				if (C_M35SendSum > 95)
				{
					S_M35 = 0x01;
					B_RestartNo = 7; // ZCL 2019.4.19 重启序号，方便打印观察问题
									 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
				}
			}

			// 2015.1.17  15次改成35次，65次(2019.4.4)，防止别的卡再出现类似问题。
			else if (C_M35SendSum > 65)
			{
				S_M35 = 0x01;
				B_RestartNo = 8; // ZCL 2019.4.19 重启序号，方便打印观察问题
								 // printf("RestartNo=%d\r\n", B_RestartNo);			//ZCL 2019.4.19 重启序号，方便打印观察问题
			}
			break;

		case 0xFF:				// 暂存 一般不太需要
			At_ThreePlusSign(); // 退出透传，进入命令模式 +++
			At_ATO0();			// 退出命令，进入数据透传模式
			At_ATAndF();		// 设置所有TA参数为出厂设置
			At_ATAndW();		// 保存用户配置参数
			At_ATV1();			// 设置TA响应返回内容格式。 =0数字码；=1详细码(默认)
			// At_QPOWD();						//! =0立即关机；=1正常关机
			// At_IPR();							//设置波特率，若=0，为自适应波特率
			At_CFUN();	 // 设置模块所有功能开启。	第二个参数=0，不复位立即有效；
			At_CPIN();	 // 确保PIN码已解。ZCL:一般不需要
			At_QINDI();	 // 查看数据接收方式1: 需要命令提取数据
			At_QISACK(); //! 检测数据是否发送成功  +QISACK: 4,4,0 说明发送长度和已确认
			At_QISTAT(); // 查询网络状态
			At_ATI();	 // 显示产品信息
			At_ATQ0();	 // 设置结果码显示模式  0=向TE发送结果码(默认)；1=不发送
			break;

		default:
			return;
		}
}

void Com3_ReceiveData(void)
{
	u8 RecNumBit, IP1NumBit, IP2NumBit, IP3NumBit, IP4NumBit, PortNumBit; // 接收到的数据长度是几位的
	u16 j = 0, n, m, i;
	u8 *p2, *p3;
	// u8 *p1;
	u8 ipbuf[15];			   // IP缓存
	u8 portBuf[5] = {0};	   // 远端端口缓存
	u8 p_RECV_IPBuf[40] = {0}; // 接收缓存

	Fill_data();

	//	p2 = (u8 *)strstr((const char *)Rcv3Buffer, "RECV FROM:"); // 接收到TCP/UDP数据
	p2 = (u8 *)strstr((const char *)Rcv3Buffer, "RECV FROM:"); // 接收到TCP/UDP数据
	if (p2)													   // 接收到TCP/UDP数据
	{
		COM2_DATA = 1; // 接收到数据开打指示灯

		// 第1路
		if ((Pw_Link1IP1 + Pw_Link1IP2 + Pw_Link1IP3 + Pw_Link1IP4) != 0) // 第一路的IP地址
		{
			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link1IP1, Pw_Link1IP2, Pw_Link1IP3, Pw_Link1IP4); // 存放IP
			sprintf((char *)portBuf, "%d", Pw_Link1Port);											   // 第一路链接远端端口字转换为ascii码
			sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
			if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第1路发来的数据
			{
				p3 = (u8 *)strstr((const char *)p2, "+IPD");
				//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第1路数据长度字符 类似这样的指针指向的字符串向数组传值，不知道能传几个数，很容易出现溢出，
				for (i = 0; i < 15; i++)
				{
					p1_TmpBuf[i] = *(p3 + 4);
					p3++;
				}
				for (i = 0; i <= 5; i++) // 把接收到的第1路数据长度字符转化为16进制数
				{
					if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
					{
						ReceiveGPRSBufOneDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

						ReceiveGPRSBufTwoDateLen = 0;
						ReceiveGPRSBufThreeDateLen = 0;
						ReceiveGPRSBufFourDateLen = 0;
						break; //
					}
				}
				for (i = 0; i <= 5; i++)
					p1_TmpBuf[i] = 0; // 把接收的第1路字符缓存长度数清0，为下一次计算
				//					for (i = 0; i <RCV3_MAX; i++)
				//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
				memset(ReceiveGPRSBuf, 0, RCV3_MAX);
				m = ReceiveGPRSBufOneDateLen;
				for (RecNumBit = 0; m != 0; RecNumBit++)
					m /= 10;
				m = Pw_Link1IP1;
				for (IP1NumBit = 0; m != 0; IP1NumBit++)
					m /= 10;
				m = Pw_Link1IP2;
				for (IP2NumBit = 0; m != 0; IP2NumBit++)
					m /= 10;
				m = Pw_Link1IP3;
				for (IP3NumBit = 0; m != 0; IP3NumBit++)
					m /= 10;
				m = Pw_Link1IP4;
				for (IP4NumBit = 0; m != 0; IP4NumBit++)
					m /= 10;
				m = Pw_Link1Port;
				for (PortNumBit = 0; m != 0; PortNumBit++)
					m /= 10;
				n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
				for (i = 24 + n; i <= (ReceiveGPRSBufOneDateLen + 24 + n); i++)
				{
					ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第1路数据给ReceiveGPRSBuf
													   //					ReceiveGPRSBuf[j] = Rcv3_Back_Buffer[i]; // 接收到的第1路数据给ReceiveGPRSBuf

					j++;
				}
				if (j >= ReceiveGPRSBufOneDateLen)
					j = 0;
				ChannelNo = 0; // 通道号，第几路通道收到数据
				F_AcklinkNum = 1;
			}
		}

		// 第2路
		if ((Pw_Link2IP1 + Pw_Link2IP2 + Pw_Link2IP3 + Pw_Link2IP4) != 0)
		{
			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link2IP1, Pw_Link2IP2, Pw_Link2IP3, Pw_Link2IP4); // 存放IP
			sprintf((char *)portBuf, "%d", Pw_Link2Port);											   // 第一路链接远端端口字转换为ascii码
			sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
			if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第2路发来的数据
			{
				p3 = (u8 *)strstr((const char *)p2, "+IPD");
				//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第2路数据长度字符
				for (i = 0; i < 15; i++)
				{
					p1_TmpBuf[i] = *(p3 + 4);
					p3++;
				}
				for (i = 0; i <= 5; i++) // 把接收到的第2路数据长度字符转化为16进制数
				{
					if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
					{
						ReceiveGPRSBufTwoDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

						ReceiveGPRSBufOneDateLen = 0;
						ReceiveGPRSBufThreeDateLen = 0;
						ReceiveGPRSBufFourDateLen = 0;
						break; //
					}
				}
				for (i = 0; i <= 5; i++)
					p1_TmpBuf[i] = 0; // 把接收的第2路字符缓存长度数清0，为下一次计算
				//					for (i = 0; i <RCV3_MAX; i++)
				//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
				memset(ReceiveGPRSBuf, 0, RCV3_MAX);
				m = ReceiveGPRSBufTwoDateLen;
				for (RecNumBit = 0; m != 0; RecNumBit++)
					m /= 10;
				m = Pw_Link2IP1;
				for (IP1NumBit = 0; m != 0; IP1NumBit++)
					m /= 10;
				m = Pw_Link2IP2;
				for (IP2NumBit = 0; m != 0; IP2NumBit++)
					m /= 10;
				m = Pw_Link2IP3;
				for (IP3NumBit = 0; m != 0; IP3NumBit++)
					m /= 10;
				m = Pw_Link2IP4;
				for (IP4NumBit = 0; m != 0; IP4NumBit++)
					m /= 10;
				m = Pw_Link2Port;
				for (PortNumBit = 0; m != 0; PortNumBit++)
					m /= 10;
				n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
				for (i = 24 + n; i <= (ReceiveGPRSBufTwoDateLen + 24 + n); i++)
				{
					ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第2路数据给ReceiveGPRSBuf
													   //					ReceiveGPRSBuf[j] = Rcv3_Back_Buffer[i]; // 接收到的第1路数据给ReceiveGPRSBuf
					j++;
				}
				if (j >= ReceiveGPRSBufTwoDateLen)
					j = 0;
				ChannelNo = 1; // 通道号，第几路通道收到数据
				F_AcklinkNum = 2;
			}
		}

		// 第3路
		if ((Pw_Link3IP1 + Pw_Link3IP2 + Pw_Link3IP3 + Pw_Link3IP4) != 0)
		{
			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link3IP1, Pw_Link3IP2, Pw_Link3IP3, Pw_Link3IP4); // 存放IP
			sprintf((char *)portBuf, "%d", Pw_Link3Port);											   // 第一路链接远端端口字转换为ascii码
			sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
			if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第3路发来的数据
			{
				p3 = (u8 *)strstr((const char *)p2, "+IPD");
				//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第3路数据长度字符
				for (i = 0; i < 15; i++)
				{
					p1_TmpBuf[i] = *(p3 + 4);
					p3++;
				}
				for (i = 0; i <= 5; i++) // 把接收到的第3路数据长度字符转化为16进制数
				{
					if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
					{
						ReceiveGPRSBufThreeDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

						ReceiveGPRSBufOneDateLen = 0;
						ReceiveGPRSBufTwoDateLen = 0;
						ReceiveGPRSBufFourDateLen = 0;
						break; //
					}
				}
				for (i = 0; i <= 5; i++)
					p1_TmpBuf[i] = 0; // 把接收的第3路字符缓存长度数清0，为下一次计算
				//					for (i = 0; i <RCV3_MAX; i++)
				//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
				memset(ReceiveGPRSBuf, 0, RCV3_MAX);
				m = ReceiveGPRSBufThreeDateLen;
				for (RecNumBit = 0; m != 0; RecNumBit++)
					m /= 10;
				m = Pw_Link3IP1;
				for (IP1NumBit = 0; m != 0; IP1NumBit++)
					m /= 10;
				m = Pw_Link3IP2;
				for (IP2NumBit = 0; m != 0; IP2NumBit++)
					m /= 10;
				m = Pw_Link3IP3;
				for (IP3NumBit = 0; m != 0; IP3NumBit++)
					m /= 10;
				m = Pw_Link3IP4;
				for (IP4NumBit = 0; m != 0; IP4NumBit++)
					m /= 10;
				m = Pw_Link3Port;
				for (PortNumBit = 0; m != 0; PortNumBit++)
					m /= 10;
				n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
				for (i = 24 + n; i <= (ReceiveGPRSBufThreeDateLen + 24 + n); i++)
				{
					ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第3路数据给ReceiveGPRSBuf
													   //					ReceiveGPRSBuf[j] = Rcv3_Back_Buffer[i]; // 接收到的第1路数据给ReceiveGPRSBuf
					j++;
				}
				if (j >= ReceiveGPRSBufThreeDateLen)
					j = 0;
				ChannelNo = 2; // 通道号，第几路通道收到数据
				F_AcklinkNum = 3;
			}
		}

		// 第4路
		if ((Pw_Link4IP1 + Pw_Link4IP2 + Pw_Link4IP3 + Pw_Link4IP4) != 0)
		{
			sprintf((char *)ipbuf, "%d.%d.%d.%d", Pw_Link4IP1, Pw_Link4IP2, Pw_Link4IP3, Pw_Link4IP4); // 存放IP
			sprintf((char *)portBuf, "%d", Pw_Link4Port);											   // 第一路链接远端端口字转换为ascii码
			sprintf((char *)p_RECV_IPBuf, "%s:%s", ipbuf, portBuf);									   // 把网络信息给p_RECV_IPBuf
			if (strstr((const char *)p2, (const char *)p_RECV_IPBuf))								   // 收到的第4路发来的数据
			{
				p3 = (u8 *)strstr((const char *)p2, "+IPD");
				//					sprintf((char *)p1_TmpBuf, "%s", p3 + 4); //接收到的第4路数据长度字符
				for (i = 0; i < 15; i++)
				{
					p1_TmpBuf[i] = *(p3 + 4);
					p3++;
				}
				for (i = 0; i <= 5; i++) // 把接收到的第4路数据长度字符转化为16进制数
				{
					if (p1_TmpBuf[i] == 0x0D) // 说明数据长度字符串内容没有了
					{
						ReceiveGPRSBufFourDateLen = atoi(&p1_TmpBuf[0], i); // 转换为16进制

						ReceiveGPRSBufOneDateLen = 0;
						ReceiveGPRSBufTwoDateLen = 0;
						ReceiveGPRSBufThreeDateLen = 0;
						break; //
					}
				}
				for (i = 0; i <= 5; i++)
					p1_TmpBuf[i] = 0; // 把接收的第4路字符缓存长度数清0，为下一次计算
				//					for (i = 0; i <RCV3_MAX; i++)
				//						ReceiveGPRSBuf[i] = 0; //先清空ReceiveGPRSBuf，然后再把接收到的数据放到里面
				memset(ReceiveGPRSBuf, 0, RCV3_MAX);
				m = ReceiveGPRSBufFourDateLen;
				for (RecNumBit = 0; m != 0; RecNumBit++)
					m /= 10;
				m = Pw_Link4IP1;
				for (IP1NumBit = 0; m != 0; IP1NumBit++)
					m /= 10;
				m = Pw_Link4IP2;
				for (IP2NumBit = 0; m != 0; IP2NumBit++)
					m /= 10;
				m = Pw_Link4IP3;
				for (IP3NumBit = 0; m != 0; IP3NumBit++)
					m /= 10;
				m = Pw_Link4IP4;
				for (IP4NumBit = 0; m != 0; IP4NumBit++)
					m /= 10;
				m = Pw_Link4Port;
				for (PortNumBit = 0; m != 0; PortNumBit++)
					m /= 10;
				n = RecNumBit + IP1NumBit + IP2NumBit + IP3NumBit + IP4NumBit + PortNumBit;
				for (i = 24 + n; i <= (ReceiveGPRSBufFourDateLen + 24 + n); i++)
				{
					ReceiveGPRSBuf[j] = Rcv3Buffer[i]; // 接收到的第4路数据给ReceiveGPRSBuf
													   //					ReceiveGPRSBuf[j] = Rcv3_Back_Buffer[i]; // 接收到的第1路数据给ReceiveGPRSBuf
					j++;
				}
				if (j >= ReceiveGPRSBufFourDateLen)
					j = 0;
				ChannelNo = 3; // 通道号，第几路通道收到数据
				F_AcklinkNum = 4;
			}
		}

		// delay_ms(50);
		// sim7600ce_DoWithData(); // 处理接收到的数据
	}
}

// 发送域名解析命令
void Send_DomainName(void)
{
	uchar i;

	//---------------------------------------------A.B.C连接特殊的地方（与发心跳包比较）
	// A. 统计需要连接的数量
	ConnectNum_DomainName = 0;
	for (i = 0; i < 4; i++)
	{
		if (GprsPar[Ip0Base + i * 4] == 0 && GprsPar[DomainName0Base + i * 31] != 0)
		{
			F_IPOK[i] = 1;
			ConnectNum_DomainName++;
		}
		else
		{
			F_IPOK[i] = 0;
		}
	}

	// C. 第一次连接，把IP地址不等于0或者域名不等于0的通道，置连接请求标志
	for (i = 0; i < 4; i++)
	{
		// IP地址不等于0或者域名不等于0，原先没有进行过连接，立即进行连接，否则下面延时进行连接
		if (GprsPar[Ip0Base + i * 4] == 0 && GprsPar[DomainName0Base + i * 31] != 0)
		{
			if (B_FirstLink_DomainName[i] == 0) //!!! 加快连接的过程 2014.12.3备注
			{
				B_LinkAsk_DomainName[i] = 1;
				B_FirstLink_DomainName[i] = 1;
			}
		}
		// 没有连接地址，清除第一次连接标志
		else
			B_FirstLink_DomainName[i] = 0;
	}
	//---------------------------------------------上面A.B.C连接特殊的地方（与发心跳包比较）

	// 1. 连接进行延时判断
	// for (i = 0; i < 4; i++)
	// {
	// 	if (B_LinkAsk_DomainName[i] == 0)
	// 	{
	// 		// 已经连接上 或者 IP地址和域名地址为0了，都不需要连接了！ 2014.12.3
	// 		if (CGD0_LinkConnectOK[i] || (GprsPar[Ip0Base + i * 4] + k) == 0)
	// 			C_LinkDelay[i] = 0;
	// 		// 时间到后，置连接请求标志B_LinkAsk_DomainName[i]=1
	// 		else
	// 		{
	// 			// 重连接时间间隔（300S），2个字节
	// 			j = GprsPar[ReConnect0Base + i * 2];
	// 			j = (j << 8) + GprsPar[ReConnect0Base + 1 + i * 2];
	// 			// 2013.6.11 有关闭连接标志，必须延时2秒后，才可以再连接。（为了快速连接）
	// 			if (B_CloseLink[i] == 2)
	// 			{
	// 				if (C_LinkDelay[i] > 3)
	// 				{
	// 					B_LinkAsk_DomainName[i] = 1; // 时间到，置连接请求标志
	// 					B_CloseLink[i] = 0;
	// 				}
	// 			}
	// 			// 否则，只有到重连接时间间隔（300S）后才可以连接。
	// 			else if (C_LinkDelay[i] > j)
	// 			{
	// 				B_LinkAsk_DomainName[i] = 1; // 时间到，置连接请求标志
	// 			}
	// 		}
	// 	}
	// 	else
	// 		C_LinkDelay[i] = 0;
	// }

	// 2. 正在发送连接 （每路连接循环发送，有发送请求标志就发送连接！）
	if (DomainName_No > 3)
		DomainName_No = 0;
	i = DomainName_No;

	if (B_LinkAsk_DomainName[i] == 0)
	{
		DomainName_No++; // 指向下一路连接的发送
	}
	// 有心跳包，先发送心跳包 ！！！
	else if (S_LinkNow_DoName[i] == 0 && B_HeartSendAsk[0] + B_HeartSendAsk[1] + B_HeartSendAsk[2] + B_HeartSendAsk[3] > 0)
		;
	//
	else
	{
		// 指令互锁限制
		// 可以改成==100就条件不成立，不发连接了
		if (S_HeartSend + S_DtuInquireSend + S_DtuSetSend + S_GprsDataSend + S_SendSms + S_ReadNewSms + S_SmsFull == 0) //=0  =100测试使用，暂不连接
		{
			if (S_LinkNow_DoName[i] == 0)
			{
				S_LinkNow_DoName[i] = 1;
				C_LinkNow_DoName[i] = 0;
			}

			if (S_LinkNow_DoName[i] == 1 && C_LinkNow_DoName[i] > 2) //  作用不大，照样OK  1, CONNECT OK 一块出
			{
				S_LinkNow_DoName[i] = 2;
				Cw_Txd3Max = 0;	  // 不能删 2012.10.4
				At_DomainName(i); // 发送域名查询指令
				C_LinkNow_DoName[i] = 0;
			}
			else if (S_LinkNow_DoName[i] == 2)
			{
				// 3. 有连接标志，清除其他标志
				//				if (F_IPOK[i])
				{
					// C_LinkDelay[i] = 0;
					C_LinkNow_DoName[i] = 0;
					S_LinkNow_DoName[i] = 0;
					B_LinkAsk_DomainName[i] = 0;
					// B_FirstLink_DomainName[i] = 0;
				}
			}
		}

		// 4. 延时，没有连接成功，清除
		// if (S_LinkNow_DoName[i] > 0)
		// {
		// 	if (C_LinkNow_DoName[i] > 5)
		// 	{
		// 		C_LinkDelay[i] = 0; //=0清零移到前面，后面有个情况赋值 2013.6.28
		// 		//
		// 		if (B_LinkFault[i] < 3) // 前4次没有成功，立即重连。 2013.6.23
		// 		{
		// 			if (ConnectNum <= 1) // 只有1路连接设置可以直接再发连接请求 2013.6.28
		// 				B_LinkAsk_DomainName[i] = 1;
		// 			else // 有2路及以上连接设置，需延时再发连接请求(给其他连接时间)
		// 			{
		// 				B_LinkAsk_DomainName[i] = 0;
		// 				//
		// 				Lw_GprsTmp = GprsPar[ReConnect0Base + i * 2];
		// 				Lw_GprsTmp = (Lw_GprsTmp << 8) + GprsPar[ReConnect0Base + 1 + i * 2];
		// 				if (Lw_GprsTmp > 8)
		// 					C_LinkDelay[i] = Lw_GprsTmp - 8;
		// 				else
		// 					C_LinkDelay[i] = Lw_GprsTmp;
		// 			}
		// 		}
		// 		else // 如果故障大于3次，则清除B_LinkAsk_DomainName[i]标志，重新计时
		// 			B_LinkAsk_DomainName[i] = 0;
		// 		//
		// 		C_LinkNow_DoName[i] = 0;
		// 		S_LinkNow_DoName[i] = 0;
		// 		B_LinkFault[i]++;
		// 	}
		// }
	}

	// 合并一个状态
	// S_SendLink = S_LinkNow_DoName[0] + S_LinkNow_DoName[1] + S_LinkNow_DoName[2] + S_LinkNow_DoName[3];
}

// 域名解析
void DomainNameResolution(void)
{
	u8 *p2, *p3;
	u8 i = 0, k = 0;

	// 第1路域名
	for (i = 0; i < 31; i++)
	{
		domain_name[i] = GprsPar[DomainName0Base + i];
	}
	p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name); // 接收解析的IP信息
	if (p2)																	// 解析正确
	{
		p3 = (u8 *)strstr((const char *)p2, ","); //
		sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
		for (i = 0; i <= 3; i++)				  // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link1IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
				GprsPar[Ip0Base] = Pw_Link1IP1;		 // YLS 2022.12.27
				k = i + 1;							 // IP下一位的值
				break;								 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link1IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip0Base + 1] = Pw_Link1IP2;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link1IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip0Base + 2] = Pw_Link1IP3;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x22)
			{
				Pw_Link1IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip0Base + 3] = Pw_Link1IP4;		 // YLS 2022.12.27
				k = 0;									 //
				break;									 //
			}
		}
	}

	// 第2路域名
	for (i = 0; i < 31; i++)
	{
		domain_name[i] = GprsPar[DomainName1Base + i];
	}
	p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name); // 接收解析的IP信息
	if (p2)																	// 解析正确
	{
		p3 = (u8 *)strstr((const char *)p2, ","); //
		sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
		for (i = 0; i <= 3; i++)				  // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link2IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
				GprsPar[Ip1Base] = Pw_Link2IP1;		 // YLS 2022.12.27
				k = i + 1;							 // IP下一位的值
				break;								 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link2IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip1Base + 1] = Pw_Link2IP2;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link2IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip1Base + 2] = Pw_Link2IP3;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x22)
			{
				Pw_Link2IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip1Base + 3] = Pw_Link2IP4;		 // YLS 2022.12.27
				k = 0;									 //
				break;									 //
			}
		}
	}

	// 第3路域名
	for (i = 0; i < 31; i++)
	{
		domain_name[i] = GprsPar[DomainName2Base + i];
	}
	p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name); // 接收解析的IP信息
	if (p2)																	// 解析正确
	{
		p3 = (u8 *)strstr((const char *)p2, ","); //
		sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
		for (i = 0; i <= 3; i++)				  // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link3IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
				GprsPar[Ip2Base] = Pw_Link3IP1;		 // YLS 2022.12.27
				k = i + 1;							 // IP下一位的值
				break;								 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link3IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip2Base + 1] = Pw_Link3IP2;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link3IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip2Base + 2] = Pw_Link3IP3;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x22)
			{
				Pw_Link3IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip2Base + 3] = Pw_Link3IP4;		 // YLS 2022.12.27
				k = 0;									 //
				break;									 //
			}
		}
	}

	// 第4路域名
	for (i = 0; i < 31; i++)
	{
		domain_name[i] = GprsPar[DomainName3Base + i];
	}
	p2 = (u8 *)strstr((const char *)Rcv3Buffer, (const char *)domain_name); // 接收解析的IP信息
	if (p2)																	// 解析正确
	{
		p3 = (u8 *)strstr((const char *)p2, ","); //
		sprintf((char *)p_TmpBuf, "%s", p3 + 2);  // 把解析的IP信息给p_TmpBuf
		for (i = 0; i <= 3; i++)				  // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link4IP1 = atoi(&p_TmpBuf[0], i); // 转换为16进制
				GprsPar[Ip3Base] = Pw_Link4IP1;		 // YLS 2022.12.27
				k = i + 1;							 // IP下一位的值
				break;								 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link4IP2 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip3Base + 1] = Pw_Link4IP2;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x2E)
			{
				Pw_Link4IP3 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip3Base + 2] = Pw_Link4IP3;		 // YLS 2022.12.27
				k = i + 1;								 // IP下一位的值
				break;									 //
			}
		}
		for (i = k; i <= 3 + k; i++) // 转化为16进制数
		{
			if (p_TmpBuf[i] == 0x22)
			{
				Pw_Link4IP4 = atoi(&p_TmpBuf[k], i - k); // 转换为16进制
				GprsPar[Ip3Base + 3] = Pw_Link4IP4;		 // YLS 2022.12.27
				k = 0;									 //
				break;									 //
			}
		}
	}
}

void Fill_data(void)
{
	// 服务器(DSC)的IP地址
	Pw_Link1IP1 = GprsPar[Ip0Base];
	Pw_Link1IP2 = GprsPar[Ip0Base + 1];
	Pw_Link1IP3 = GprsPar[Ip0Base + 2];
	Pw_Link1IP4 = GprsPar[Ip0Base + 3];

	Pw_Link2IP1 = GprsPar[Ip1Base];
	Pw_Link2IP2 = GprsPar[Ip1Base + 1];
	Pw_Link2IP3 = GprsPar[Ip1Base + 2];
	Pw_Link2IP4 = GprsPar[Ip1Base + 3];

	Pw_Link3IP1 = GprsPar[Ip2Base];
	Pw_Link3IP2 = GprsPar[Ip2Base + 1];
	Pw_Link3IP3 = GprsPar[Ip2Base + 2];
	Pw_Link3IP4 = GprsPar[Ip2Base + 3];

	Pw_Link4IP1 = GprsPar[Ip3Base];
	Pw_Link4IP2 = GprsPar[Ip3Base + 1];
	Pw_Link4IP3 = GprsPar[Ip3Base + 2];
	Pw_Link4IP4 = GprsPar[Ip3Base + 3];

	// 服务器(DSC)的端口
	Pw_Link1Port = (GprsPar[Port0Base] << 8) + GprsPar[Port0Base + 1];
	Pw_Link2Port = (GprsPar[Port1Base] << 8) + GprsPar[Port1Base + 1];
	Pw_Link3Port = (GprsPar[Port2Base] << 8) + GprsPar[Port2Base + 1];
	Pw_Link4Port = (GprsPar[Port3Base] << 8) + GprsPar[Port3Base + 1];

	// 在线报告时间间隔(心跳)(*10毫秒)
	Pw_link1OnlineReportTime = (GprsPar[OnlineReportInterval0Base] << 8) + GprsPar[OnlineReportInterval0Base + 1];
	Pw_link2OnlineReportTime = (GprsPar[OnlineReportInterval1Base] << 8) + GprsPar[OnlineReportInterval1Base + 1];
	Pw_link3OnlineReportTime = (GprsPar[OnlineReportInterval2Base] << 8) + GprsPar[OnlineReportInterval2Base + 1];
	Pw_link4OnlineReportTime = (GprsPar[OnlineReportInterval3Base] << 8) + GprsPar[OnlineReportInterval3Base + 1];

	// 网络检测检测间隔时间（秒）
	Pw_Link1NetWorkCheckTime = (GprsPar[NetCheckInterval0Base] << 8) + GprsPar[NetCheckInterval0Base + 1];
	Pw_Link2NetWorkCheckTime = (GprsPar[NetCheckInterval1Base] << 8) + GprsPar[NetCheckInterval1Base + 1];
	Pw_Link3NetWorkCheckTime = (GprsPar[NetCheckInterval2Base] << 8) + GprsPar[NetCheckInterval2Base + 1];
	Pw_Link4NetWorkCheckTime = (GprsPar[NetCheckInterval3Base] << 8) + GprsPar[NetCheckInterval3Base + 1];

	// 本地端口
	Pw_Link1localPort = (GprsPar[LoclaPort0Base] << 8) + GprsPar[LoclaPort0Base + 1];
	Pw_Link2localPort = (GprsPar[LoclaPort1Base] << 8) + GprsPar[LoclaPort1Base + 1];
	Pw_Link3localPort = (GprsPar[LoclaPort2Base] << 8) + GprsPar[LoclaPort2Base + 1];
	Pw_Link4localPort = (GprsPar[LoclaPort3Base] << 8) + GprsPar[LoclaPort3Base + 1];

	// TCP=1.UDP=0
	Pw_Link1TCP_UDPType = GprsPar[LinkTCPUDP0Base];
	Pw_Link2TCP_UDPType = GprsPar[LinkTCPUDP1Base];
	Pw_Link3TCP_UDPType = GprsPar[LinkTCPUDP2Base];
	Pw_Link4TCP_UDPType = GprsPar[LinkTCPUDP3Base];

	// 重拨号时间间隔
	Pw_link1ReconnectTimeBetwen = (GprsPar[RedialIntervalBase] << 8) + GprsPar[RedialIntervalBase + 1];

	// 支持数据回执
	Pw_SupportDataReceipt = GprsPar[DataReturnBase];
	// 最后包空闲时间间隔
	Pw_LastPacketIdleTime = GprsPar[LastPacketIdleIntervalBase];
}

void Com2_printf(char *fmt, ...)
{
	uint16_t txd_max;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)Txd2Buffer_TEMP, fmt, ap); // vsprintf   sprintf
	va_end(ap);

	// 堵塞方式发送
	txd_max = strlen((const char *)Txd2Buffer_TEMP);
	UART2_Output_Datas(Txd2Buffer_TEMP, txd_max);
}

void Com1_printf(char *fmt, ...)
{
	uint16_t txd_max;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)Txd1Buffer_TEMP, fmt, ap); // vsprintf   sprintf
	va_end(ap);
	// 堵塞方式发送
	txd_max = strlen((const char *)Txd1Buffer_TEMP);
	UART1_Output_Datas(Txd1Buffer_TEMP, txd_max);
}

void Com1_sendData(uint8_t txd_buffer[], uint16_t send_num)
{
	uint16_t txd_max;

	if (send_num == 0)
	{
		txd_max = strlen((const char *)txd_buffer);
	}
	else
	{
		txd_max = send_num;
	}
	UART1_Output_Datas(txd_buffer, txd_max);
}

void UART1_Output_Datas(uint8_t S_Out[], uint8_t Counter)
{
	uint8_t i;
	USART_ITConfig(USART1, USART_IT_TC, DISABLE); // 禁止发送中断
	for (i = 0; i < Counter; i++)
	{
		USART_SendData(USART1, S_Out[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
			; // 等待发送结束
	}
	USART_ITConfig(USART1, USART_IT_TC, ENABLE); // 允许发送中断
}

void UART2_Output_Datas(uint8_t S_Out[], uint8_t Counter)
{
	uint8_t i;

	USART_ITConfig(USART2, USART_IT_TC, DISABLE); // 禁止发送中断
	for (i = 0; i < Counter; i++)
	{
		USART_SendData(USART2, S_Out[i]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET)
			; // 等待发送结束
	}
	USART_ITConfig(USART2, USART_IT_TC, ENABLE); // 允许发送中断
}

// 填充要查询返回的DTU参数
void Fill_Dtu_Par(void)
{
	uchar i, len;

	// a.长度计算
	AccessPointLength = Gprs_StrCmdLen(&GprsPar[AccessPointBase]); // 05
	DtuNoLength = Gprs_StrCmdLen(&GprsPar[DtuNoBase]);			   // 06
	DomainName0Length = Gprs_StrCmdLen(&GprsPar[DomainName0Base]); // 21
	DomainName1Length = Gprs_StrCmdLen(&GprsPar[DomainName1Base]); // 31
	DomainName2Length = Gprs_StrCmdLen(&GprsPar[DomainName2Base]); // 41
	DomainName3Length = Gprs_StrCmdLen(&GprsPar[DomainName3Base]); // 51

	// b.头长度，在第二步骤再填充，因为At_QISENDAccordingLength指令会覆盖开始部分。
	Cw_Txd3Max = 31; // 发送长度高位字节.低位字节 跳过
					 // 起始头31个字节

	// c.填充保存的指令
	for (i = 1; i < 69; i++) // 69个  0x70之前的指令
	{
		len = CmdLen[i];

		if (CmdNo[i] == 0x05)
			len = AccessPointLength;
		else if (CmdNo[i] == 0x06)
			len = DtuNoLength;
		else if (CmdNo[i] == 0x21)
			len = DomainName0Length;
		else if (CmdNo[i] == 0x31)
			len = DomainName1Length;
		else if (CmdNo[i] == 0x41)
			len = DomainName2Length;
		else if (CmdNo[i] == 0x51)
			len = DomainName3Length;
		Txd3Buffer[Cw_Txd3Max++] = CmdNo[i];	// 指令
		Txd3Buffer[Cw_Txd3Max++] = len + 2;		// 长度
		Gprs_TX_Fill(&GprsPar[CmdPst[i]], len); // 内容
	}
	// d.填充不保存的指令
	Txd3Buffer[Cw_Txd3Max++] = 0x70;  // 70: DTU程序版本
	Txd3Buffer[Cw_Txd3Max++] = 9;	  // 长度
	Gprs_TX_Fill(DtuProgVersion, 7);  // 内容
	Txd3Buffer[Cw_Txd3Max++] = 0x71;  // 71: DTU程序生成日期
	Txd3Buffer[Cw_Txd3Max++] = 10;	  // 长度
	Gprs_TX_Fill(DtuProgMakeDate, 8); // 内容
	Txd3Buffer[Cw_Txd3Max++] = 0x73;  // 73: DTU硬件平台版本
	Txd3Buffer[Cw_Txd3Max++] = 24;	  // 长度
	Gprs_TX_Fill(DtuHardWare, 14);	  // 内容
	Gprs_TX_Fill(Csq, 8);			  // 内容
	Txd3Buffer[Cw_Txd3Max++] = 0x74;  // 74: DTU生产商LOGO
	Txd3Buffer[Cw_Txd3Max++] = 8;	  // 长度
	Gprs_TX_Fill(DtuManufacture, 6);  // 内容
	Txd3Buffer[Cw_Txd3Max++] = 0x75;  // 75: DTU型号描述
	Txd3Buffer[Cw_Txd3Max++] = 8;	  // 长度
	Gprs_TX_Fill(DtuModel, 6);		  // 内容
	// e.末尾
	Txd3Buffer[Cw_Txd3Max++] = 0x7B; // 结束标志
	w_StringLength = Cw_Txd3Max;
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
