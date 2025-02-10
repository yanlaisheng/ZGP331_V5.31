/**
 ******************************************************************************
 * @file    Key_Menu.c
 * @author  ChengLei Zhou  - 周成磊
 * @version V1.28
 * @date    2014-01-03
 * @brief   Code  for UG-2864KSWLG01 WiseChip 焊接接口，IC:SSD1306
 * 				 Hardware	: STM32F103(101) C8T6,CBT6
 * 				 Platform	: IAR6.40,Keil-MDK5.01,LIB:V3.5.0(11/03/2011),JLINK V6-V7
 ******************************************************************************
 * @attention	2014.1.3
 ******************************************************************************/

/* 定义后电流1位小数，2018.1.26 ZCL 增加条件编译
	 注释后电流跟以前的一样，2位小数。  */
// #define CURRENT_OneXiaoShu	//电流1位小数

/*
	// 取消 #define CURRENT_OneXiaoShu	//电流1位小数
	用参数 SCR液晶屏本身参数代替: Pw_ScrCurrentBits 		ZCL 2018.9.13
	*/

/* @note	2014.4.8
  周成磊 2011.5.9
  1. 液晶RAM数据LSB在上，MSB在下
  2. 进行智晶OLED液晶1.3寸(UG-2864KSWLG01  WiseChip 焊接接口)显示操作的底层函数

  3. ZCL 2019.4.16 备注：DCM200_SCR_E中KEY3、KEY4、KEY5 被占用，GetKey()使用时要注释掉！

  */

/* Includes ------------------------------------------------------------------*/
#include "Key_Menu.h"
#include "GlobalV_Extern.h" // 全局变量声明
#include "GlobalConst.h"
#include "LCD.h"
#include <stdlib.h>

#include "com1_232.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
u8 Key_Pressed;		 // 按键按下
u8 Key_Data;		 // 按键值，代表那个按键
u8 Sa_OldKeyData;	 // 保存旧键值（防抖处理用）
u16 Lw_PageNo;		 // 显示的画面序号
u16 Lw_SavePageNo;	 // 保存画面序号
u8 F_KeyExec;		 // 按键执行标志，防止重复执行
u8 S_ModParStatus;	 // 修改参数时=1，修改画面序号时=0
u8 ModParNo;		 // 修改参数序号（在一个画面里，顺序修改可设定的参数）
u8 F_ModPar1;		 // 修改参数标志1	 用来指定本页面那个参数要修改
u8 F_ModPar2;		 // 修改参数标志2	 用来指定本页面那个参数要修改
u8 F_ModPar3;		 // 修改参数标志3	 用来指定本页面那个参数要修改
u8 F_ModPar4;		 // 修改参数标志4	 用来指定本页面那个参数要修改
u8 F_ModPar5;		 // 修改参数标志5	 用来指定本页面那个参数要修改
u8 F_ModPar6;		 // 修改参数标志6	 用来指定本页面那个参数要修改
u8 Sa_ModParKeyData; // 保存的修改参数键值
u8 ModParBitPos;	 // 修改参数位-位置

u8 T_LcdDisplay; // 用于控制LCD延时显示，避免显示出现条纹
u16 C_LcdDisplay;
u8 T_KeyPressOverTime; // 用于按键超时，从新采集，增加按键速度
u16 C_KeyPressOverTime;
u8 B_RightKeyCount;		// 右键计数器，用于快键键
u8 B_LeftKeyCount;		// 左键计数器，用于快键键
u8 B_PressedStopRunKey; // 按下STOP或者RUN键 2016.12.6		2018.5.19

u8 T_TimeUpdatePage; // 用于定时更新页面，出现乱码及时更新 ZCL 2015.9.17
u16 C_TimeUpdatePage;

u8 B_TimeReadBPDJPar; // 定时读BPDJ参数 ZCL 2019.2.18
u8 T_TimeReadBPDJPar; // 定时读BPDJ参数 ZCL 2019.2.18
u16 C_TimeReadBPDJPar;

u16 Lw_KEYStableCounter; // ZCL 2018.4.21 用L_		按键稳定计数器
u16 Lw_TmpModParValue;	 // ZCL 2018.4.21 用L_		临时修改参数值
u16 Lw_OldModParValue;	 // ZCL 2018.5.16 用L_		旧的修改参数值（保存对比用）
u8 S_OldModParStatus;	 // ZCL 2018.5.16					旧的修改参数状态（保存对比用）
u8 S_DisplayPar;		 // ZCL 2018.5.17	显示参数 S参数。 =0延时中，=1可以显示

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private variables extern --------------------------------------------------*/
extern uc8 BMP[];
extern volatile u32 sysTickValue;
extern u8 F_ModTime;

extern u8 S_Com1SendNoNum; // Com1发送次数
extern u8 S_Com1Send;	   // Com1发送步骤
extern u8 F_Com1Send;	   // Com1发送标志
extern u8 T_Com1Send;
extern u16 C_Com1Send;
extern u8 T_Com1TimeSend;
extern u16 C_Com1TimeSend;
extern u8 S_RdWrNo; // 读写序号

extern u16 C_LCDBackLight; // ZCL 2017.9.19 背光控制

extern u16 C_Com1SendDelay; // ZCL 2018.5.14

extern u16 Cw_Rcv1;	 // 接收计数器 ZCL 2018.5.15
extern u16 C_NoRcv1; // 没有接收计数器

extern u8 B_ModYW310SZM220; // ZCL 2019.10.19

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//********************下面为显示画面****************************
// 1.Demo 片头
// 2.主菜单画面

void Menu_Demo(void) // 画面 Demo 林戈智能
{
}

void Menu_Welcome(void) // 画面 欢迎
{
	LCD12864_ClrText(); // 清屏
	LCD12864_String(0x88, "Booting...");
	LCD_DLY_ms(1500); // 延时1秒

	LCD12864_ClrText(); // 清屏
	LCD12864_String(0x90, "青岛三利智能动力");
	LCD_DLY_ms(5); // 延时5MS;
	LCD12864_String(0x8A, "欢迎您！");
	LCD_DLY_ms(3000); // 延时3000MS;
}

void Menu_Dynamic_TXT(void) // 画面 动态文本
{
}

void Menu_Feature(void) // 画面 产品特点
{
}

void Menu_LaserTxt(void) // 画面 激光文本(扫描书写效果)
{
}

void EnterMenu_InitPar(void) // 进入菜单初始化参数  ZCL 2018.5.15
{
	C_TimeUpdatePage = 0; // 用于定时更新页面，出现乱码及时更新 ZCL 2015.9.17
	S_Com1Send = 0;		  // 读或者写参数，重新开始
	C_Com1Send = 0;
	C_Com1SendDelay = 0; // 读或者写参数的延时，重新开始
	S_ModParStatus = 0;	 // 修改参数时=1，修改画面序号时=0 。 这里应该没用，冗余赋值0

	// 这2个清零很管用，防止切换了页面，刚读参数，就收到上页读参数的返回值，而误赋值给本页的读参数
	// ZCL 2018.5.15
	Cw_Rcv1 = 0;  // 接收计数器赋值0
	C_NoRcv1 = 0; // 不接收计数器

	S_RdWrNo = 0;	  // 切换了页面，从第一个参数开始读写
	S_DisplayPar = 0; // ZCL 2018.5.17 这个可以不要，冗余设计
	C_LcdDisplay = 0; // ZCL 2018.5.17

	B_TimeReadBPDJPar = 0; // ZCL 2019.3.8
}

void Menu_StmReadPar1(void) // 画面 读参数1
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水压力");
		LCD12864_String(0x87, "Mp");
		// 第二行
		LCD12864_String(0x90, "出水压力");
		LCD12864_String(0x97, "Mp");
		// 第三行
		LCD12864_String(0x88, "设定压力");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "变频频率");
		LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */

	ModParNum(0); // 指定:修改参数的数量
	// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x03, &w_InPDec, 1, 0);
	DispMod_Par(0x94, 0x04, 0x03, &w_OutPDec, 1, 0);
	DispMod_Par(0x8C, 0x04, 0x03, &w_PIDCalcP, 1, 0);
	DispMod_Par(0x9C, 0x04, 0x01, &w_dspNowHz, 3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数	ZCL 2019.3.19 调整结构，加入 if(B_ModPar==0)
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_InPDec, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_OutPDec, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_PIDCalcP, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspNowHz, 3), 1);
	}

	// ZCL 2015.8.31 OK 测试在后半字显示'A'
	/* 	LCD12864_WriteCommand(0x84);
		i=LCD12864_ReadData();
		LCD12864_WriteData(i);
		LCD12864_WriteData('A'); */
}

void Menu_StmReadPar2(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "母线电压");
		LCD12864_String(0x87, " V");

		// 第二行
		LCD12864_String(0x90, "电机电压");
		LCD12864_String(0x97, " V");

		// 第三行
		LCD12864_String(0x88, "电机电流");
		LCD12864_String(0x8F, " A");

		// 第四行
		LCD12864_String(0x98, "模块温度");
		LCD12864_String(0x9F, "度");
	}

	ModParNum(0); // 指定:修改参数的数量
	// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspBusBarVoltage, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspOutVoltage, 3, 0);
	// DispMod_Par(0x8C, 0x04,0x02, &w_dspAPhaseCurrent,3, 0);

	if (Pw_ScrCurrentBits == 1) // ZCL 2018.8.29  电流1位小数
	{
		DispMod_Par(0x8C, 0x04, 0x01, &w_dspAPhaseCurrent, 3, 0); // ZCL 2018.5.19
	}
	else if (Pw_ScrCurrentBits == 2) // ZCL 2018.8.29  电流2位小数
	{
		DispMod_Par(0x8C, 0x05, 0x02, &w_dspAPhaseCurrent, 3, 0); // ZCL 2018.5.19  2019.6.7  0x04
	}

	/* #ifdef CURRENT_OneXiaoShu		//ZCL 2018.5.19  电流1位小数
	#else
	#endif	 */
	// ZCL 2018.8.29 用Pw_ScrCurrentBits 代替

	DispMod_Par(0x9C, 0x04, 0x01, &w_dspNowTemperature, 3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspBusBarVoltage, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspOutVoltage, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspAPhaseCurrent, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspNowTemperature, 3), 1);
	}
}

void Menu_StmReadPar3(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "供水状态");
		LCD12864_String(0x87, "种");
		// 第二行
		LCD12864_String(0x90, "--");

		// 第三行
		LCD12864_String(0x88, "停机原因");
		LCD12864_String(0x8F, "种");
		// 第四行
		LCD12864_String(0x98, "--");

		Lw_SaveEquipOperateStatus = 1000;
		Lw_SaveEquipAlarmStatus = 1000;
	}

	ModParNum(0);
	DispMod_Par(0x84, 0x05, 0x00, &w_EquipOperateNum, 1, 0);
	// DispMod_Par(0x94, 0x04,0x02, &w_SoftVer,1, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_EquipStopNum, 1, 0);
	// DispMod_Par(0x9C, 0x04,0x00, &w_Writetime,1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (Lw_SaveEquipOperateStatus != w_EquipOperateStatus)
	{
		Lw_SaveEquipOperateStatus = w_EquipOperateStatus;
		// 运行状态的提示  ZCL 2015.9.7
		switch (w_EquipOperateStatus)
		{
		case 0:
			LCD12864_String(0x91, "正常运行      ");
			break; //
		case 1:
			LCD12864_String(0x91, "停机状态      ");
			break; //
		case 2:
			LCD12864_String(0x91, "巡检状态      ");
			break; //
		case 3:
			LCD12864_String(0x91, "通讯失败      ");
			break; //
		case 4:
			LCD12864_String(0x91, "小流量处理    ");
			break; //
		case 5:
			LCD12864_String(0x91, "小流量破坏    ");
			break; //
		case 6:
			LCD12864_String(0x91, "小流量间隔    ");
			break; //
		// case  7: LCD12864_String(0x91, "              ");	break;		//
		// case  8: LCD12864_String(0x91, "              ");	break; 		//
		// case  9: LCD12864_String(0x91, "              ");	break;		//
		// case 10: LCD12864_String(0x91, "              ");	break;		//
		// case 11: LCD12864_String(0x91, "              ");	break;		//
		// case 12: LCD12864_String(0x91, "              ");	break;		//
		case 13:
			LCD12864_String(0x91, "变频器复位失败");
			break; //
		case 14:
			LCD12864_String(0x91, "补偿器预警    ");
			break; //
		// case 15: LCD12864_String(0x91, "              ");	break;		//
		case 16:
			LCD12864_String(0x91, "定时交换      ");
			break; //
		case 17:
			LCD12864_String(0x91, "电动阀故障    ");
			break; //	ZCL 2018.8.22
		case 18:
			LCD12864_String(0x91, "自来水无水    ");
			break; //	ZCL 2018.8.22
		case 19:
			LCD12864_String(0x91, "水箱无水      ");
			break; //	ZCL 2018.8.22
		// case 20: LCD12864_String(0x91, "              ");	break;		//
		// case 21: LCD12864_String(0x91, "              ");	break; 		//
		// case 22: LCD12864_String(0x91, "              ");	break;		//
		case 23:
			LCD12864_String(0x91, "软件超压保护！");
			break; //
		case 24:
			LCD12864_String(0x91, "定频状态      ");
			break; //	ZCL 2019.9.10
		case 25:
			LCD12864_String(0x91, "供水控制模式  ");
			break; //	ZCL 2019.9.11
		case 26:
			LCD12864_String(0x91, "变频控制模式  ");
			break; //	ZCL 2019.9.11
		case 27:
			LCD12864_String(0x91, "检查变频报警  ");
			break; //	ZCL 2022.10.3
		// case 25: LCD12864_String(0x91, "              ");	break;		//
		// case 26: LCD12864_String(0x91, "              ");	break; 		//
		// case 27: LCD12864_String(0x91, "              ");	break;		//
		// case 28: LCD12864_String(0x91, "              ");	break;		//
		// case 29: LCD12864_String(0x91, "              ");	break;		//
		// case 30: LCD12864_String(0x91, "              ");	break; 		//
		// case 31: LCD12864_String(0x91, "              ");	break; 		//
		default:
			return;
		}
	}

	if (Lw_SaveEquipAlarmStatus != w_EquipAlarmStatus)
	{
		Lw_SaveEquipAlarmStatus = w_EquipAlarmStatus;
		// 停机原因的状态的提示  ZCL 2015.9.7
		switch (w_EquipAlarmStatus)
		{
		case 0:
			LCD12864_String(0x99, "无－设备正常  ");
			break; //
		case 1:
			LCD12864_String(0x99, "无水停机      ");
			break; //
		case 2:
			LCD12864_String(0x99, "主令停止状态  ");
			break; //
		case 3:
			LCD12864_String(0x99, "手动状态      ");
			break; //
		case 4:
			LCD12864_String(0x99, "变频器报警    ");
			break; //
		case 5:
			LCD12864_String(0x99, "电源故障      ");
			break; //
		case 6:
			LCD12864_String(0x99, "液位超高      ");
			break; //
		case 7:
			LCD12864_String(0x99, "管网超压      ");
			break; //
		case 8:
			LCD12864_String(0x99, "所有泵故障    ");
			break; //
		case 9:
			LCD12864_String(0x99, "小流量保压    ");
			break; //
		case 10:
			LCD12864_String(0x99, "压力传感器坏  ");
			break; //
		case 11:
			LCD12864_String(0x99, "进压高节能停机");
			break; //
		case 12:
			LCD12864_String(0x99, "定时段停机    ");
			break; //
		case 13:
			LCD12864_String(0x99, "特殊定时停机  ");
			break; //
		case 14:
			LCD12864_String(0x99, "远程遥控停机  ");
			break; //
		case 15:
			LCD12864_String(0x99, "进水压高保护！");
			break; //
		case 16:
			LCD12864_String(0x99, "满负荷保护！  ");
			break; //
		case 17:
			LCD12864_String(0x99, "电池掉电！    ");
			break; //
		case 18:
			LCD12864_String(0x99, " 1从机通讯失败");
			break; //
		case 19:
			LCD12864_String(0x99, " 2从机通讯失败");
			break; //
		case 20:
			LCD12864_String(0x99, "变频器通讯失败");
			break; //
		case 21:
			LCD12864_String(0x99, "无主机命令停机");
			break; //
		case 22:
			LCD12864_String(0x99, "按键手动停机  ");
			break; //
		case 23:
			LCD12864_String(0x99, "按键停止停机  ");
			break; //
		case 24:
			LCD12864_String(0x99, "补偿器预警停机");
			break; //
		case 25:
			LCD12864_String(0x99, "进水压力高停机");
			break; // ZCL 2018.7.17 不是保护停机
		case 26:
			LCD12864_String(0x99, "              ");
			break; //
		case 27:
			LCD12864_String(0x99, "              ");
			break; //
		case 28:
			LCD12864_String(0x99, "              ");
			break; //
		case 29:
			LCD12864_String(0x99, "              ");
			break; //
		case 30:
			LCD12864_String(0x99, "              ");
			break; //
		case 31:
			LCD12864_String(0x99, "              ");
			break; //
		default:
			return;
		}
	}

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		/* 	文本的版本不用通讯过来，DCM200_SCR自己的参数 */
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_EquipOperateNum, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_EquipOperateStatus, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_EquipStopNum, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_EquipAlarmStatus, 1), 1);
	}
}

void Menu_StmReadPar4(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "变频故障");
		LCD12864_String(0x87, "种");
		// 第二行
		LCD12864_String(0x90, "--");

		// 第三行
		LCD12864_String(0x88, "故障码1 ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "故障码2 ");
		LCD12864_String(0x9F, "  ");

		Lw_SaveDspFaultStatus = 1000;
	}

	ModParNum(0);
	DispMod_Par(0x84, 0x05, 0x00, &w_DspAlarmNum, 1, 0);
	// DispMod_Par(0x94, 0x04,0x02, &w_SoftVer,1, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspSysErrorCode1, 3, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_dspSysErrorCode2, 3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (Lw_SaveDspFaultStatus != w_DspAlarmStatus)
	{
		Lw_SaveDspFaultStatus = w_DspAlarmStatus;
		// 停机原因的状态的提示  ZCL 2015.9.7
		switch (w_DspAlarmStatus)
		{
		case 0:
			LCD12864_String(0x91, "无故障- 正常  ");
			break; //
		case 1:
			LCD12864_String(0x91, "过流保护      ");
			break; //
		case 2:
			LCD12864_String(0x91, "过压保护      ");
			break; //
		case 3:
			LCD12864_String(0x91, "欠压保护      ");
			break; //
		case 4:
			LCD12864_String(0x91, "缺相保护      ");
			break; //
		case 5:
			LCD12864_String(0x91, "IPM 故障      ");
			break; //
		case 6:
			LCD12864_String(0x91, "电流互感器故障");
			break; //
		case 7:
			LCD12864_String(0x91, "IPM 过热      ");
			break; //
		case 8:
			LCD12864_String(0x91, "电机过热      ");
			break; //
		case 9:
			LCD12864_String(0x91, "风扇故障      ");
			break; //
		case 10:
			LCD12864_String(0x91, "EEPROM故障    ");
			break; //
		case 11:
			LCD12864_String(0x91, "DSP 处理器故障");
			break; //
		case 12:
			LCD12864_String(0x91, "STM 处理器故障");
			break; //
		case 13:
			LCD12864_String(0x91, "接地保护      ");
			break; //
		case 14:
			LCD12864_String(0x91, "加速中欠电压  ");
			break; //
		case 15:
			LCD12864_String(0x91, "减速中过电压  ");
			break; //

		default:
			return;
		}
	}

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_DspAlarmNum, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_DspAlarmStatus, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspSysErrorCode1, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspSysErrorCode2, 3), 1);
	}
}

void Menu_StmReadPar5(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "  水泵累计运行  ");
		// LCD12864_String(0x87, "  ");

		// 第二行
		LCD12864_String(0x90, "小时");
		LCD12864_String(0x95, "    时");

		// 第三行
		LCD12864_String(0x88, "  秒");
		LCD12864_String(0x8D, "    秒");

		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	DispMod_Par(0x92, 0x05, 0x00, &w_Pump1RunHour, 1, 0);
	DispMod_Par(0x8A, 0x05, 0x00, &w_Pump1RunSecond, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_Pump1RunHour, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_Pump1RunSecond, 1), 1);
	}
}

void Menu_StmReadPar6(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "供水版本");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "Arm-Year");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "Arm-Date");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// DispMod_Par(0x84, 0x04,0x02, &Lw_DCM200SCR_VERSION,1, 0);
	DispMod_Par(0x84, 0x04, 0x02, &w_SoftVer, 1, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_WriteDate, 1, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_Writetime, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		/* 	文本的版本不用通讯过来，DCM200_SCR自己的参数 */
		// if(S_RdWrNo==1)
		// Read_Stm32Data( Address(&w_DCM200SCR_VERSION,1), 1 );

		if (S_RdWrNo == 1) // ZCL 2018.9.13 上面if注释了，这里用了else if 错误。
			Read_Stm32Data(Address(&w_SoftVer, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_WriteDate, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_Writetime, 1), 1);
	}
}

void Menu_StmReadPar7(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "变频版本");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "文本版本");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "编译日期");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "编译日期");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x02, &w_dspSoftVersion, 3, 0);
	DispMod_Par(0x94, 0x04, 0x02, &w_ScrVERSION, 5, 0); // ZCL 2019.4.3
	DispMod_Par(0x8C, 0x05, 0x00, &w_ScrWriteYear, 1, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_ScrWriteDate, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspSoftVersion, 3), 1);
		//		if(S_RdWrNo==2)
		//			Read_Stm32Data( Address(&w_FmramErrH,1), 1 );
		//		else if(S_RdWrNo==3)
		//			Read_Stm32Data( Address(&w_FmramErrL,1), 1 );
	}
}

// ZCL 2018.4.19 删除
void Menu_StmReadPar8_NoUse(void) // 画面 读参数
{
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_StmReadPar9(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "读Dsp OK");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "写Dsp OK");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "读Dsp Er"); // ZCL 2015.9.9
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "写Dsp Er");
		LCD12864_String(0x9F, "  ");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	DispMod_Par(0x84, 0x05, 0x00, &w_ReadDspOKCount, 1, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_WriteDspOKCount, 1, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_ReadDspErrCount, 1, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_WriteDspErrCount, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_ReadDspOKCount, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_WriteDspOKCount, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_ReadDspErrCount, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_WriteDspErrCount, 1), 1);
	}
}

void Menu_StmReadPar10(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		LCD12864_String(0x80, "读Dsp 序号");
		// LCD12864_ClrText();          //清屏
		// 第一行
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "写Dsp 序号");
		// LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "RstDsp_C  "); // ZCL 2015.9.9
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "RstDspDlyC");
		// LCD12864_String(0x9F, "  ");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	DispMod_Par(0x85, 0x05, 0x00, &w_RdDspNo, 1, 0);
	DispMod_Par(0x95, 0x05, 0x00, &w_WrDspNo, 1, 0);
	DispMod_Par(0x8D, 0x05, 0x00, &w_ResetedDspCount, 1, 0);
	DispMod_Par(0x9D, 0x05, 0x00, &w_ResetDspDelayCount, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_RdDspNo, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_WrDspNo, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_ResetedDspCount, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_ResetDspDelayCount, 1), 1);
	}
}

void Menu_StmReadPar11(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "1:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 1次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_EquipAlarmLast1,1, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_EquipAlarm1YM, 1, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_EquipAlarm1DH, 1, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_EquipAlarm1MS, 1, 0);
	//
	FaultDisplay(w_EquipAlarmLast1, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;					   // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_EquipAlarmLast1, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_EquipAlarm1YM, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_EquipAlarm1DH, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_EquipAlarm1MS, 1), 1);
	}
}

void Menu_StmReadPar12(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "2:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 2次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_EquipAlarmLast2,1, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_EquipAlarm2YM, 1, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_EquipAlarm2DH, 1, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_EquipAlarm2MS, 1, 0);
	//
	FaultDisplay(w_EquipAlarmLast2, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;					   // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_EquipAlarmLast2, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_EquipAlarm2YM, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_EquipAlarm2DH, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_EquipAlarm2MS, 1), 1);
	}
}

void Menu_StmReadPar13(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "3:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 3次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_EquipAlarmLast3,1, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_EquipAlarm3YM, 1, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_EquipAlarm3DH, 1, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_EquipAlarm3MS, 1, 0);
	//
	FaultDisplay(w_EquipAlarmLast3, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;					   // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_EquipAlarmLast3, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_EquipAlarm3YM, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_EquipAlarm3DH, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_EquipAlarm3MS, 1), 1);
	}
}

void Menu_StmReadPar14(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "4:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 4次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_EquipAlarmLast4,1, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_EquipAlarm4YM, 1, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_EquipAlarm4DH, 1, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_EquipAlarm4MS, 1, 0);
	//
	FaultDisplay(w_EquipAlarmLast4, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;					   // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_EquipAlarmLast4, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_EquipAlarm4YM, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_EquipAlarm4DH, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_EquipAlarm4MS, 1), 1);
	}
}

void Menu_StmReadPar15(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "5:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 5次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_EquipAlarmLast5,1, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_EquipAlarm5YM, 1, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_EquipAlarm5DH, 1, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_EquipAlarm5MS, 1, 0);
	//
	FaultDisplay(w_EquipAlarmLast5, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;					   // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_EquipAlarmLast5, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_EquipAlarm5YM, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_EquipAlarm5DH, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_EquipAlarm5MS, 1), 1);
	}
}

void Menu_StmReadPar16(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "6:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 6次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_EquipAlarmLast6,1, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_EquipAlarm6YM, 1, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_EquipAlarm6DH, 1, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_EquipAlarm6MS, 1, 0);
	//
	FaultDisplay(w_EquipAlarmLast6, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;					   // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_EquipAlarmLast6, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_EquipAlarm6YM, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_EquipAlarm6DH, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_EquipAlarm6MS, 1), 1);
	}
}

void Menu_StmReadPar17(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "选择故障记录:   ");
		// 第二行
		LCD12864_String(0x90, "n:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x98, "（总  上    次）");
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x98, "（总");
		LCD12864_String(0x9B, "上");
		LCD12864_String(0x9E, "次）");

		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 nb_modpar)	 */
	ModParNum(1);
	F_ModPar1 = 0;
	// DispMod_Par(0x84, 0x05,0x00, &w_SelEquipAlarm,1, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_SelEquipAlarm1YM, 1, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_SelEquipAlarm1DH, 1, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_SelEquipAlarm1MS, 1, 0);
	//
	DispMod_Par(0x9A, 0x02, 0x00, &w_FaultRecNum, 1, 0);
	F_ModPar1 = 1;
	DispMod_Par(0x9D, 0x02, 0x00, &w_SelFaultNo, 1, F_ModPar1);
	//
	FaultDisplay(w_SelEquipAlarm, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_SelFaultNo, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 6)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_SelEquipAlarm, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_SelEquipAlarm1YM, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_SelEquipAlarm1DH, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_SelEquipAlarm1MS, 1), 1);
		else if (S_RdWrNo == 5)
			Read_Stm32Data(Address(&w_SelFaultNo, 1), 1);
		else if (S_RdWrNo == 6)
			Read_Stm32Data(Address(&w_FaultRecNum, 1), 1);
	}
}

void Menu_StmReadPar18(void) // 画面 读参数
{
	// #define TIMEPAGE					7 				//
	// 注意时间页的修改 TIMEPAGE的修改  ZCL 2015.9.7
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "现在时间:       ");
		// LCD12864_String(0x87, "  ");
		// 第二行
		// LCD12864_String(0x90, "      年    月  ");
		LCD12864_String(0x90, "  ");
		LCD12864_String(0x93, "年"); // ZCL 2017.6.17
		LCD12864_String(0x96, "月  ");
		// LCD12864_String(0x97, "  ");
		// 第三行
		// LCD12864_String(0x88, "      日    时  ");
		LCD12864_String(0x88, "  ");
		LCD12864_String(0x8B, "日"); // ZCL 2017.6.17
		LCD12864_String(0x8E, "时  ");

		// LCD12864_String(0x8F, "  ");
		// 第四行
		// LCD12864_String(0x98, "      分    秒  ");
		LCD12864_String(0x98, "  ");
		LCD12864_String(0x9B, "分"); // ZCL 2017.6.17
		LCD12864_String(0x9E, "秒  ");
		// LCD12864_String(0x8F, "  ");
	}

	ModParNum(6);
	// 显示和修改参数
	DispMod_Par(0x91, 0x04, 0x00, &w_SetYear, 1, F_ModPar1);
	DispMod_Par(0x95, 0x02, 0x00, &w_SetMonth, 1, F_ModPar2);
	DispMod_Par(0x8A, 0x02, 0x00, &w_SetDay, 1, F_ModPar3);
	DispMod_Par(0x8D, 0x02, 0x00, &w_SetHour, 1, F_ModPar4);
	DispMod_Par(0x9A, 0x02, 0x00, &w_SetMinute, 1, F_ModPar5);
	DispMod_Par(0x9D, 0x02, 0x00, &w_SetSecond, 1, F_ModPar6);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_SetYear, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_SetMonth, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&w_SetDay, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_SetHour, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 5)
			Write_Stm32Data(Address(&w_SetMinute, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 6)
			Write_Stm32Data(Address(&w_SetSecond, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 6)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_SetYear, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_SetMonth, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_SetDay, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_SetHour, 1), 1);
		else if (S_RdWrNo == 5)
			Read_Stm32Data(Address(&w_SetMinute, 1), 1);
		else if (S_RdWrNo == 6)
			Read_Stm32Data(Address(&w_SetSecond, 1), 1);
	}
}

// ZCL 2017.9.12 加入AI1-AI4读值，方便调试。
void Menu_StmReadPar19(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "AI1 读值");
		LCD12864_String(0x87, "  ");

		// 第二行
		LCD12864_String(0x90, "AI2 读值");
		LCD12864_String(0x97, "  ");

		// 第三行
		LCD12864_String(0x88, "AI3 读值");
		LCD12864_String(0x8F, "  ");
		// LCD12864_String(0x88, "                ");

		// 第四行
		LCD12864_String(0x98, "AI4 读值");
		LCD12864_String(0x9F, "  ");
		// LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	DispMod_Par(0x84, 0x05, 0x00, &w_AI1, 1, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_AI2, 1, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_AI3, 1, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_AI4, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_AI1, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_AI2, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_AI3, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_AI4, 1), 1);
	}
}

// ZCL 2019.8.7 开关量输入状态DI，开关量输出状态DO  提示
void Menu_StmReadPar20(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "DI  状态");
		LCD12864_String(0x87, "种");
		// 第二行
		LCD12864_String(0x90, "--");

		// 第三行
		LCD12864_String(0x88, "DO  状态");
		LCD12864_String(0x8F, "种");
		// 第四行
		LCD12864_String(0x98, "--");

		Lw_SaveKglInStatus = 1000;
		Lw_SaveKglOutStatus = 1000;
	}

	ModParNum(0);
	DispMod_Par(0x84, 0x05, 0x00, &w_KglInNum, 1, 0);
	// DispMod_Par(0x94, 0x04,0x02, &w_SoftVer,1, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_KglOutNum, 1, 0);
	// DispMod_Par(0x9C, 0x04,0x00, &w_Writetime,1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (Lw_SaveKglInStatus != w_KglInStatus)
	{
		Lw_SaveKglInStatus = w_KglInStatus;
		// DI状态的提示  ZCL 2019.8.7
		switch (w_KglInStatus)
		{
		case 0:
			LCD12864_String(0x91, "无DI接通      ");
			break; //
		case 1:
			LCD12864_String(0x91, "DI1 接通      ");
			break; //
		case 2:
			LCD12864_String(0x91, "DI2 接通      ");
			break; //
		case 3:
			LCD12864_String(0x91, "DI3 接通      ");
			break; //
		case 4:
			LCD12864_String(0x91, "DI4 接通      ");
			break; //

		default:
			return;
		}
	}

	if (Lw_SaveKglOutStatus != w_KglOutStatus)
	{
		Lw_SaveKglOutStatus = w_KglOutStatus;
		// DO状态的提示  ZCL 2019.8.7
		switch (w_KglOutStatus)
		{
		case 0:
			LCD12864_String(0x99, "无Q 接通      ");
			break; //
		case 1:
			LCD12864_String(0x99, "Q1接通        ");
			break; //
		case 2:
			LCD12864_String(0x99, "Q2接通        ");
			break; //

		default:
			return;
		}
	}

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		/* 	文本的版本不用通讯过来，DCM200_SCR自己的参数 */
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_KglInNum, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_KglOutNum, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_KglInStatus, 1), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_KglOutStatus, 1), 1);
	}
}

// ZCL 2018.8.21 电动阀状态
void Menu_StmReadPar21(void) // 画面 读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "电动阀状态");
		LCD12864_String(0x87, "      ");

		// 第二行
		LCD12864_String(0x90, "=0打开; =1关闭; ");
		// 第三行
		LCD12864_String(0x88, "=2故障; =3打开中");
		// 第四行
		LCD12864_String(0x98, "=4关闭中;       ");
	}

	ModParNum(0);
	DispMod_Par(0x85, 0x03, 0x00, &w_DDFStatus, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 1)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_DDFStatus, 1), 1);
	}
}

void Menu_StmSetPar1(void) // 画面 设定参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "1.供水设定参数  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	// ZCL 2017.3.21 必须加上下面一段，否则在没有参数的画面按OK键，会出现进入参数修改状态，出不来。
	ModParNum(0);
	// 显示和修改参数
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_StmSetPar2(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "修改参数");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "恢复参数");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "本机地址");
		LCD12864_String(0x8F, "号");
		// 第四行
		LCD12864_String(0x98, "(DCM100)        ");
	}

	ModParNum(3);
	DispMod_Par(0x84, 0x05, 0x00, &w_ModPar, 1, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &w_ParInitial, 1, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_EquipmentNo, 0, F_ModPar3);
	// DispMod_Par(0x9C, 0x03,0x01, &w_Pump1Current,1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_ModPar, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_ParInitial, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_EquipmentNo, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_NoWaterStopP,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_ModPar, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_ParInitial, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_EquipmentNo, 0), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_NoWaterStopP,0), 1 );
	}
}

void Menu_StmSetPar3(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水量程");
		LCD12864_String(0x87, "Mp");
		// 第二行
		LCD12864_String(0x90, "出水量程");
		LCD12864_String(0x97, "Mp");
		// 第三行
		LCD12864_String(0x88, "从机压力");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "从机启动YS");
		LCD12864_String(0x9F, " S");
	}

	ModParNum(4); // ZCL 2019.8.7 3
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x03, &Pw_InPDecMax, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x03, &Pw_OutPDecMax, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x02, &Pw_SetP, 0, F_ModPar3);			  // ZCL 2018.5.17 压力小数位3不管用
	DispMod_Par(0x9D, 0x03, 0x00, &Pw_SlaveStartDelay, 0, F_ModPar4); // ZCL 2019.8.7
	S_DisplayPar = 0;												  // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_InPDecMax, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_OutPDecMax, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_SetP, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_SlaveStartDelay, 0), Lw_SaveSetValue); // ZCL 2019.8.7
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4) // ZCL 2019.8.7  3
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_InPDecMax, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_OutPDecMax, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_SetP, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_SlaveStartDelay, 0), 1); // ZCL 2019.8.7
	}
}

void Menu_StmSetPar4(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "主机定时En"); // ZCL 5个字名称
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "快速设时间");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "                ");
		// 第四行
		LCD12864_String(0x98, "（主机参数！！）");
	}

	ModParNum(2);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &Pw_EnableTime, 0, F_ModPar1);
	DispMod_Par(0x95, 0x03, 0x00, &w_AutoAssignTime, 1, F_ModPar2);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_EnableTime, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_AutoAssignTime, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_EnableTime, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_AutoAssignTime, 1), 1);
	}
}

void Menu_StmSetPar5(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "  主机时间压力  ");
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "主启1Tim");
		LCD12864_String(0x97, "HM");
		// 第三行
		LCD12864_String(0x88, "主停1Tim");
		LCD12864_String(0x8F, "HM");
		// 第四行
		LCD12864_String(0x98, "主1 压力");
		LCD12864_String(0x9F, "Mp");
	}

	ModParNum(3);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x03,0x00, &Pw_EnableTime,0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_Time1Run, 0, F_ModPar1);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_Time1Stop, 0, F_ModPar2);
	DispMod_Par(0x9C, 0x04, 0x02, &Pw_Time1SetP, 0, F_ModPar3); // ZCL 2018.5.17 压力小数位3不管用
	S_DisplayPar = 0;											// 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		// if(S_RdWrNo==1)
		// Write_Stm32Data( Address(&Pw_EnableTime,0), Lw_SaveSetValue );
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_Time1Run, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_Time1Stop, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_Time1SetP, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		//  if(S_RdWrNo==1)
		//  Read_Stm32Data( Address(&Pw_EnableTime,0), 1 );
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_Time1Run, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_Time1Stop, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_Time1SetP, 0), 1);
	}
}

void Menu_StmSetPar6(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "主启2Tim");
		LCD12864_String(0x87, "HM");
		// 第二行
		LCD12864_String(0x90, "主停2Tim");
		LCD12864_String(0x97, "HM");
		// 第三行
		LCD12864_String(0x88, "主2 压力");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（主机时间段GN）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_Time2Run, 0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_Time2Stop, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x02, &Pw_Time2SetP, 0, F_ModPar3); // ZCL 2018.5.17 压力小数位3不管用
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_Time2Run, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_Time2Stop, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_Time2SetP, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_Time2Run, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_Time2Stop, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_Time2SetP, 0), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar7(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "主启3Tim");
		LCD12864_String(0x87, "HM");
		// 第二行
		LCD12864_String(0x90, "主停3Tim");
		LCD12864_String(0x97, "HM");
		// 第三行
		LCD12864_String(0x88, "主3 压力");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（主机时间段GN）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_Time3Run, 0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_Time3Stop, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x02, &Pw_Time3SetP, 0, F_ModPar3); // ZCL 2018.5.17 压力小数位3不管用
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_Time3Run, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_Time3Stop, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_Time3SetP, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_Time3Run, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_Time3Stop, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_Time3SetP, 0), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar8(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "主启4Tim");
		LCD12864_String(0x87, "HM");
		// 第二行
		LCD12864_String(0x90, "主停4Tim");
		LCD12864_String(0x97, "HM");
		// 第三行
		LCD12864_String(0x88, "主4 压力");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（主机时间段GN）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_Time4Run, 0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_Time4Stop, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x02, &Pw_Time4SetP, 0, F_ModPar3); // ZCL 2018.5.17 压力小数位3不管用
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_Time4Run, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_Time4Stop, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_Time4SetP, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_Time4Run, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_Time4Stop, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_Time4SetP, 0), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar9(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏

		// 第一行
		LCD12864_String(0x80, "有水开压");
		LCD12864_String(0x87, "Mp");
		// 第二行
		LCD12864_String(0x90, "无水停压");
		LCD12864_String(0x97, "Mp");
		// 第三行
		LCD12864_String(0x88, "有水延时");
		LCD12864_String(0x8F, " S");
		// 第四行
		LCD12864_String(0x98, "无水延时");
		LCD12864_String(0x9F, " S");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x02, &Pw_HaveWaterRunP, 0, F_ModPar1); // ZCL 2018.5.17 压力小数位3不管用
	DispMod_Par(0x94, 0x04, 0x02, &Pw_NoWaterStopP, 0, F_ModPar2);	// ZCL 2018.5.17 压力小数位3不管用
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_HaveWaterRunDelay, 0, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_NoWaterStopDelay, 0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_HaveWaterRunP, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_NoWaterStopP, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_HaveWaterRunDelay, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_NoWaterStopDelay, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_HaveWaterRunP, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_NoWaterStopP, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_HaveWaterRunDelay, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_NoWaterStopDelay, 0), 1);
	}
}

void Menu_StmSetPar10(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "PIDS比例");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "PIDS采样");
		LCD12864_String(0x97, "秒");
		// 第三行
		LCD12864_String(0x88, "PIDS积分");
		LCD12864_String(0x8F, "分");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x03, &Pw_PID_Kc, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x03, &Pw_PID_Ts, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x03, &Pw_PID_Ti, 0, F_ModPar3);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_PID_Kc, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_PID_Ts, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_PID_Ti, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_PID_Kc, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_PID_Ts, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_PID_Ti, 0), 1);
	}
}

void Menu_StmSetPar11(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "PIDX比例");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "PIDX采样");
		LCD12864_String(0x97, "秒");
		// 第三行
		LCD12864_String(0x88, "PIDX积分");
		LCD12864_String(0x8F, "分");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(3); // ZCL 2019.8.7 3
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x03, &Pw_DownPID_Kc, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x03, &Pw_DownPID_Ts, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x03, &Pw_DownPID_Ti, 0, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_EquipType,0, F_ModPar4);			//ZCL 2019.8.7
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_DownPID_Kc, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_DownPID_Ts, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_DownPID_Ti, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_EquipType,0), Lw_SaveSetValue );			//ZCL 2019.8.7
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3) // ZCL 2019.8.7 3
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_DownPID_Kc, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_DownPID_Ts, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_DownPID_Ti, 0), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_EquipType,0), 1 );				//ZCL 2019.8.7
	}
}

void Menu_StmSetPar12(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水类型");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "出水类型");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "进水初值");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "出水初值");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_InPType, 0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_OutPType, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_InPSensorZero, 0, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_OutPSensorZero, 0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_InPType, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_OutPType, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_InPSensorZero, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_OutPSensorZero, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_InPType, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_OutPType, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_InPSensorZero, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_OutPSensorZero, 0), 1);
	}
}

void Menu_StmSetPar13(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "软件超压En");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "超压偏差");
		LCD12864_String(0x97, "Mp");
		// 第三行
		LCD12864_String(0x88, "超压延时");
		LCD12864_String(0x8F, "MS");
		// 第四行
		LCD12864_String(0x98, "（软件超压功能）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x85, 0x05, 0x00, &Pw_SoftOverPEn, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x03, &Pw_OverPThanSetPEk, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_SoftOverPDelay, 0, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_SoftOverPEn, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_OverPThanSetPEk, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_SoftOverPDelay, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_SoftOverPEn, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_OverPThanSetPEk, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_SoftOverPDelay, 0), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar14(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水压高节能En");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "停机延时");
		LCD12864_String(0x97, " S");
		// 第三行
		LCD12864_String(0x88, "开机延时");
		LCD12864_String(0x8F, " S");
		// 第四行
		LCD12864_String(0x98, "（进水高节能GN）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x87, 0x01, 0x00, &Pw_InPBigSetEn, 0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_InPBigSetDelay, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_InPNormalDelay, 0, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_InPBigSetEn, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_InPBigSetDelay, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_InPNormalDelay, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_InPBigSetEn, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_InPBigSetDelay, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_InPNormalDelay, 0), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar15(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水压高保护En");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "保护延时");
		LCD12864_String(0x97, " S");
		// 第三行
		//  LCD12864_String(0x88, "开机延时");
		//  LCD12864_String(0x8F, " S");
		LCD12864_String(0x88, "                ");
		// 第四行
		LCD12864_String(0x98, "（进水高保护GN）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(2);
	// 显示和修改参数
	DispMod_Par(0x87, 0x01, 0x00, &Pw_InPBigOutEn, 0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_InPBigOutDelay, 0, F_ModPar2);
	// DispMod_Par(0x8C, 0x04,0x00, &Pw_InPNormalDelay,0, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_InPBigOutEn, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_InPBigOutDelay, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==3)
		// Write_Stm32Data( Address(&Pw_InPNormalDelay,0), Lw_SaveSetValue );
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_InPBigOutEn, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_InPBigOutDelay, 0), 1);
		// else if(S_RdWrNo==3)
		// Read_Stm32Data( Address(&Pw_InPNormalDelay,0), 1 );
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar16(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "表坏延时");
		LCD12864_String(0x87, "秒"); // ZCL 2015.9.23
		// 第二行
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "                ");
		// 第四行
		LCD12864_String(0x98, "（压力传感坏GN）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(1);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_PSensorProtectDelay, 0, F_ModPar1);
	// DispMod_Par(0x94, 0x04,0x00, &Pw_InPBigOutDelay,0, F_ModPar2);
	// DispMod_Par(0x8C, 0x04,0x00, &Pw_InPNormalDelay,0, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_PSensorProtectDelay, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==2)
		// Write_Stm32Data( Address(&Pw_InPBigOutDelay,0), Lw_SaveSetValue );
		// else if(S_RdWrNo==3)
		// Write_Stm32Data( Address(&Pw_InPNormalDelay,0), Lw_SaveSetValue );
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 1)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_PSensorProtectDelay, 0), 1);
		// else if(S_RdWrNo==2)
		// Read_Stm32Data( Address(&Pw_InPBigOutDelay,0), 1 );
		// else if(S_RdWrNo==3)
		// Read_Stm32Data( Address(&Pw_InPNormalDelay,0), 1 );
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar17(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "软停使能");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "每秒减频");
		LCD12864_String(0x97, "Hz");
		// 第三行
		//  LCD12864_String(0x88, "开机延时");
		//  LCD12864_String(0x8F, " S");

		LCD12864_String(0x88, "                ");
		// 第四行
		LCD12864_String(0x98, "（设备软停功能）");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(2);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_PumpSoftStopEn, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x01, &Pw_SoftStopDownDec, 0, F_ModPar2);
	// DispMod_Par(0x8C, 0x04,0x00, &Pw_InPNormalDelay,0, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_OutPSensorZero,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_PumpSoftStopEn, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_SoftStopDownDec, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==3)
		// Write_Stm32Data( Address(&Pw_InPNormalDelay,0), Lw_SaveSetValue );
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_OutPSensorZero,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_PumpSoftStopEn, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_SoftStopDownDec, 0), 1);
		// else if(S_RdWrNo==3)
		// Read_Stm32Data( Address(&Pw_InPNormalDelay,0), 1 );
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_OutPSensorZero,0), 1 );
	}
}

void Menu_StmSetPar18(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水限频En");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "进设定压");
		LCD12864_String(0x97, "Mp");
		// 第三行
		LCD12864_String(0x88, "每秒降频");
		LCD12864_String(0x8F, "Hz");
		// 第四行
		LCD12864_String(0x98, "最低频率");
		LCD12864_String(0x9F, "Hz");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &Pw_InP_CTLEn, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x03, &Pw_SetInP, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x01, &Pw_VfFreqDownSircle, 0, F_ModPar3);
	DispMod_Par(0x9C, 0x04, 0x01, &Pw_VfFreqDownMin, 0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_InP_CTLEn, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_SetInP, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_VfFreqDownSircle, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_VfFreqDownMin, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_InP_CTLEn, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_SetInP, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_VfFreqDownSircle, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_VfFreqDownMin, 0), 1);
	}
}

void Menu_StmSetPar19(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "小LL使能");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "小LL频率");
		LCD12864_String(0x97, "Hz");
		// 第三行
		LCD12864_String(0x88, "恒压偏差");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "恒压延时");
		LCD12864_String(0x9F, " S");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_EnableSmall, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x01, &Pw_InSmallFreqMinValue, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x03, &Pw_SmallStablePEk, 0, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_SmallJudgeDelay, 0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_EnableSmall, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_InSmallFreqMinValue, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_SmallStablePEk, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_SmallJudgeDelay, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_EnableSmall, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_InSmallFreqMinValue, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_SmallStablePEk, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_SmallJudgeDelay, 0), 1);
	}
}

void Menu_StmSetPar20(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "小LL压力");
		LCD12864_String(0x87, "Mp");
		// 第二行
		LCD12864_String(0x90, "小LL间隔");
		LCD12864_String(0x97, " S");
		// 第三行
		LCD12864_String(0x88, "小LL破坏");
		LCD12864_String(0x8F, " S");
		// 第四行
		LCD12864_String(0x98, "小LL恒压_r");
		LCD12864_String(0x9F, " S");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x02, &Pw_SmallHoldP, 0, F_ModPar1); // ZCL 2018.5.17 压力小数位3不管用
	DispMod_Par(0x94, 0x05, 0x00, &Pw_BetweenSmallSec, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_SmallBadClrSec, 0, F_ModPar3);
	DispMod_Par(0x9D, 0x03, 0x00, &w_SmallStableRunSec, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_SmallHoldP, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_BetweenSmallSec, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_SmallBadClrSec, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_SmallStableRunSec, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_SmallHoldP, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_BetweenSmallSec, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_SmallBadClrSec, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_SmallStableRunSec, 1), 1);
	}
}

void Menu_StmSetPar21(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "小LL上升");
		LCD12864_String(0x87, "秒");
		// 第二行
		LCD12864_String(0x90, "不恒压-1En");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "退出偏差");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "每秒增频");
		LCD12864_String(0x9F, "Hz");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_SmallRiseSec, 0, F_ModPar1);
	DispMod_Par(0x95, 0x03, 0x00, &Pw_SmallNoStableSubOneEn, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x03, &Pw_ExitSmallEk, 0, F_ModPar3);
	DispMod_Par(0x9C, 0x04, 0x01, &Pw_SmallVfUpValue, 0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_SmallRiseSec, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_SmallNoStableSubOneEn, 0), Lw_SaveSetValue);
		if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_ExitSmallEk, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_SmallVfUpValue, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_SmallRiseSec, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_SmallNoStableSubOneEn, 0), 1);
		if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_ExitSmallEk, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_SmallVfUpValue, 0), 1);
	}
}

void Menu_StmSetPar22(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "Max AI值");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "MaxPID值");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "Max Vf值");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "最小频率");
		LCD12864_String(0x9F, "Hz");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_MaxAIHex, 0, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_MaxPIDHex, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_MaxFreqHex, 0, F_ModPar3);
	DispMod_Par(0x9C, 0x04, 0x01, &Pw_MinRunFreqDec, 0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_MaxAIHex, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_MaxPIDHex, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_MaxFreqHex, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_MinRunFreqDec, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_MaxAIHex, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_MaxPIDHex, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_MaxFreqHex, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_MinRunFreqDec, 0), 1);
	}
}

void Menu_StmSetPar23(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "恒压长延时");
		LCD12864_String(0x87, " S");
		// 第二行
		LCD12864_String(0x90, "加压短延时");
		LCD12864_String(0x97, " S");
		// 第三行
		LCD12864_String(0x88, "变压系数");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "进水偏差");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &Pw_HengYaLongDelay, 0, F_ModPar1);
	DispMod_Par(0x95, 0x03, 0x00, &Pw_JiaYaShortDelay, 0, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_InPChangeOutPXiShu, 0, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_InPChangeEk, 0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_HengYaLongDelay, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_JiaYaShortDelay, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_InPChangeOutPXiShu, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_InPChangeEk, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_HengYaLongDelay, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_JiaYaShortDelay, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_InPChangeOutPXiShu, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_InPChangeEk, 0), 1);
	}
}

void Menu_StmSetPar24(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "定频模式");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "定频频率");
		LCD12864_String(0x97, "Hz");
		// 第三行
		LCD12864_String(0x88, "                ");
		// 第四行
		LCD12864_String(0x98, "（此模式手动）");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(2);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_ZDRunMode, 0, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x01, &Pw_ZDRunHz, 0, F_ModPar2);
	// DispMod_Par(0x8C, 0x04,0x00, &Pw_MaxFreqHex,0, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ZDRunMode, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_ZDRunHz, 0), Lw_SaveSetValue);
		// else if(S_RdWrNo==3)
		// Write_Stm32Data( Address(&Pw_MaxFreqHex,0), Lw_SaveSetValue );
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_ScrFastKeyBetweenMS,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ZDRunMode, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_ZDRunHz, 0), 1);
		// else if(S_RdWrNo==3)
		// Read_Stm32Data( Address(&Pw_MaxFreqHex,0), 1 );
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_ScrFastKeyBetweenMS,0), 1 );
	}
}

void Menu_StmSetPar25(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水偏差倍数");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "变频复位数"); // ZCL 2018.3.8
		LCD12864_String(0x97, "次");
		// 第三行
		LCD12864_String(0x88, "变频复位秒"); // ZCL 2018.3.8
		LCD12864_String(0x8F, "秒");

		// 第四行
		LCD12864_String(0x98, "已报警次数"); // ZCL 2018.3.8
		LCD12864_String(0x9F, "次");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x86, 0x02, 0x00, &Pw_InPPidEkXiShu, 0, F_ModPar1); // 0x84
	DispMod_Par(0x95, 0x03, 0x00, &Pw_VvvfResetMax, 0, F_ModPar2);	// 0x94
	DispMod_Par(0x8D, 0x03, 0x00, &Pw_VvvfResetSec, 0, F_ModPar3);	// 0x9C
	DispMod_Par(0x9D, 0x03, 0x00, &w_VvvfAlmNum, 1, 0);				// 0x8C

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_InPPidEkXiShu, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_VvvfResetMax, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_VvvfResetSec, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_VvvfAlmNum, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_InPPidEkXiShu, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_VvvfResetMax, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_VvvfResetSec, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_VvvfAlmNum, 1), 1);
	}
}

void Menu_StmSetPar26(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "选择参数项");
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "    参数值");
		// LCD12864_String(0x97, "Hz");
		// 第三行
		LCD12864_String(0x88, "按键间隔");
		LCD12864_String(0x8F, "MS");
		// 第四行
		// LCD12864_String(0x98, "                ");
		LCD12864_String(0x98, "变频减次秒"); // ZCL 2018.6.13
											 // LCD12864_String(0x9F, " S");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x85, 0x05, 0x00, &w_SelPar, 1, F_ModPar1);
	DispMod_Par(0x95, 0x05, 0x00, &w_SelParValue, 1, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_ScrFastKeyBetweenMS, 0, F_ModPar3);
	DispMod_Par(0x9D, 0x05, 0x00, &Pw_VfAlarmSubOneSecs, 0, F_ModPar4);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_SelPar, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_SelParValue, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_ScrFastKeyBetweenMS, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_VfAlarmSubOneSecs, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_SelPar, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_SelParValue, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_ScrFastKeyBetweenMS, 0), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_VfAlarmSubOneSecs, 0), 1);
	}
}

void Menu_StmSetPar27(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "水源切换使能:   "); // ZCL 2018.8.21 一个画面2个参数
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x91, "(0-1) ");
		// 第三行
		LCD12864_String(0x88, "DDF 手动使能:   ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x99, "(0-1) ");
	}

	ModParNum(2);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_SelWaterToWater, 0, F_ModPar1);
	// DispMod_Par(0x8C, 0x05,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar2);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_DDFManuAutoCtlEn, 0, F_ModPar2);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_SelWaterToWater, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_DDFManuAutoCtlEn, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_SelWaterToWater, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_DDFManuAutoCtlEn, 0), 1);
	}
}

void Menu_StmSetPar28(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "开电动阀时间:   "); // ZCL 2018.8.21 一个画面2个参数
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "秒");
		// 第三行
		LCD12864_String(0x88, "关电动阀时间:   ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "秒");
	}

	ModParNum(2);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x93, 0x05, 0x00, &Pw_DDFOpenDelay, 0, F_ModPar1);
	// DispMod_Par(0x8C, 0x05,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar2);
	DispMod_Par(0x9B, 0x05, 0x00, &Pw_DDFCloseDelay, 0, F_ModPar2);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_DDFOpenDelay, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_DDFCloseDelay, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_DDFOpenDelay, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_DDFCloseDelay, 0), 1);
	}
}

void Menu_StmSetPar29(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "自来水切换时间:   "); // ZCL 2018.8.21 一个画面2个参数
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "分");
		// 第三行
		LCD12864_String(0x88, "自来水切换走时:   ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "分");
	}

	ModParNum(1);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x93, 0x05, 0x00, &Pw_ChangeWaterSourceHour, 0, F_ModPar1);
	// DispMod_Par(0x8C, 0x05,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar3);
	DispMod_Par(0x9B, 0x05, 0x00, &w_ChangeWaterRunningHour, 1, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ChangeWaterSourceHour, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_ChangeWaterRunningHour, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ChangeWaterSourceHour, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_ChangeWaterRunningHour, 1), 1);
	}
}

void Menu_StmSetPar30(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "水箱切换时间:     "); // ZCL 2018.8.21 一个画面2个参数
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "分");
		// 第三行
		LCD12864_String(0x88, "水箱切换走时:     ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "分");
	}

	ModParNum(1);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x93, 0x05, 0x00, &Pw_ChangeWaterSourceHour2, 0, F_ModPar1);
	// DispMod_Par(0x8C, 0x05,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar3);
	DispMod_Par(0x9B, 0x05, 0x00, &w_ChangeWaterRunningHour2, 1, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ChangeWaterSourceHour2, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_ChangeWaterRunningHour2, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ChangeWaterSourceHour2, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_ChangeWaterRunningHour2, 1), 1);
	}
}

void Menu_StmSetPar31(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "定点关阀时间1:  "); // ZCL 2018.8.21 一个画面2个参数
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "HM");
		// 第三行
		LCD12864_String(0x88, "定点开阀时间1:   ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "HM");
	}

	ModParNum(2);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x93, 0x05, 0x00, &Pw_Time1HM, 0, F_ModPar1);
	// DispMod_Par(0x8C, 0x05,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar3);
	DispMod_Par(0x9B, 0x05, 0x00, &Pw_Time3HM, 0, F_ModPar2);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_Time1HM, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_Time3HM, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_Time1HM, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_Time3HM, 0), 1);
	}
}

void Menu_StmSetPar32(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "定点关阀时间2:  "); // ZCL 2018.8.21 一个画面2个参数
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "HM");
		// 第三行
		LCD12864_String(0x88, "定点开阀时间2:   ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "HM");
	}

	ModParNum(2);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x93, 0x05, 0x00, &Pw_Time2HM, 0, F_ModPar1);
	// DispMod_Par(0x8C, 0x05,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar3);
	DispMod_Par(0x9B, 0x05, 0x00, &Pw_Time4HM, 0, F_ModPar2);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_Time2HM, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_Time4HM, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_Time2HM, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_Time4HM, 0), 1);
	}
}

void Menu_StmSetPar33(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "定点关阀时间3:  "); // ZCL 2018.8.21 一个画面2个参数
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "HM");
		// 第三行
		LCD12864_String(0x88, "定点开阀时间3:   ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "HM");
	}

	ModParNum(2);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x93, 0x05, 0x00, &Pw_Time5HM, 0, F_ModPar1);
	// DispMod_Par(0x8C, 0x05,0x00, &Pw_ScrFastKeyBetweenMS,0, F_ModPar3);
	DispMod_Par(0x9B, 0x05, 0x00, &Pw_Time6HM, 0, F_ModPar2);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_Time5HM, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_Time6HM, 0), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_Time5HM, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_Time6HM, 0), 1);
	}
}

void Menu_StmSetPar34(void) // 画面 设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "电动阀手动控制: "); // ZCL 2018.8.21 一个画面2个参数  2019.8.7！
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "清除变频检查:   "); // ZCL 2019.8.10
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(2);
	// 显示和修改参数
	// DispMod_Par(0x85, 0x05,0x00, &w_SelPar,1, F_ModPar1);
	DispMod_Par(0x93, 0x05, 0x00, &w_TouchDDFOnOFF, 1, F_ModPar1);
	DispMod_Par(0x8E, 0x02, 0x00, &w_DelVvvfAlarmed, 1, F_ModPar2);
	// DispMod_Par(0x9B, 0x05,0x00, &Pw_Time6HM,0, F_ModPar2);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_TouchDDFOnOFF, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 2) // ZCL 2022.10.3
			Write_Stm32Data(Address(&w_DelVvvfAlarmed, 1), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2) // ZCL 2022.10.3
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_TouchDDFOnOFF, 1), 1);
		else if (S_RdWrNo == 2) // ZCL 2022.10.3
			Read_Stm32Data(Address(&w_DelVvvfAlarmed, 1), 1);
	}
}

void Menu_StmSetPar35(void) // 画面 设定参数 2019.8.30
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "TS定时口令");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "TS定时M-D ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "TS状态:         ");
		// LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(2);
	// 显示和修改参数
	DispMod_Par(0x85, 0x05, 0x00, &Pw_TimePwdStopPwd, 0, F_ModPar1);
	DispMod_Par(0x95, 0x05, 0x00, &Pw_TimePwdStopMD, 0, F_ModPar2);
	// DispMod_Par(0x8C, 0x05,0x00, &w_TimePwdStopST,1, 0);
	// DispMod_Par(0x9C, 0x04,0x01, &Pw_MinRunFreqDec,0, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (w_TimePwdStopST == 0)
		LCD12864_String(0x8C, "未启动！");
	else if (w_TimePwdStopST == 1)
		LCD12864_String(0x8C, "已启动！");
	else if (w_TimePwdStopST == 2)
		LCD12864_String(0x8C, "停机！  ");

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_TimePwdStopPwd, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_TimePwdStopMD, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&w_TimePwdStopST, 1), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_MinRunFreqDec,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_TimePwdStopPwd, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_TimePwdStopMD, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_TimePwdStopST, 1), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_MinRunFreqDec,0), 1 );
	}
}

void Menu_StmSetPar36(void) // 画面 设定参数 2019.8.30
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "设备类型");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "供水变频模式  ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "(0供水,1变频)   ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "补偿器停机EN    ");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(3);
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x00, &Pw_EquipType, 0, F_ModPar1); // ZCL 2019.8.7;
	DispMod_Par(0x96, 0x03, 0x00, &Pw_ISwitch1, 0, F_ModPar2);
	// DispMod_Par(0x8C, 0x05,0x00, &w_TimePwdStopST,1, 0);
	DispMod_Par(0x9E, 0x03, 0x00, &Pw_BCQErrStopEN, 0, F_ModPar3); // ZCL 2019.9.7
	S_DisplayPar = 0;											   // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_EquipType, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_ISwitch1, 0), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_BCQErrStopEN, 0), Lw_SaveSetValue); // ZCL 2019.9.7
																			// else if(S_RdWrNo==4)
																			// Write_Stm32Data( Address(&Pw_MinRunFreqDec,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_EquipType, 0), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_ISwitch1, 0), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_BCQErrStopEN, 0), 1); // ZCL 2019.9.7
															 // else if(S_RdWrNo==4)
															 // Read_Stm32Data(Address(&Pw_MinRunFreqDec,0), 1 );
	}
}

void Menu_DspReadPar0(void) // 画面 DSP读参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "2.变频观看参数  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	// ZCL 2017.3.21 必须加上下面一段，否则在没有参数的画面按OK键，会出现进入参数修改状态，出不来。
	ModParNum(0);
	// 显示和修改参数
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_DspReadPar1(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "当前频率");
		LCD12864_String(0x87, "Hz");

		// 第二行
		LCD12864_String(0x90, "电机电压");
		LCD12864_String(0x97, " V");

		// 第三行
		LCD12864_String(0x88, "电机电流");
		LCD12864_String(0x8F, " A");

		// 第四行
		LCD12864_String(0x98, "模块温度");
		LCD12864_String(0x9F, "度");
	}

	ModParNum(0);
	// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

	DispMod_Par(0x84, 0x04, 0x01, &w_dspNowHz, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspOutVoltage, 3, 0);

	if (Pw_ScrCurrentBits == 1) // ZCL 2018.8.29  电流1位小数
	{
		DispMod_Par(0x8C, 0x04, 0x01, &w_dspAPhaseCurrent, 3, 0); // ZCL 2018.5.19
	}
	else if (Pw_ScrCurrentBits == 2) // ZCL 2018.8.29  电流2位小数
	{
		DispMod_Par(0x8C, 0x05, 0x02, &w_dspAPhaseCurrent, 3, 0); // ZCL 2018.5.19  2019.6.7  0x04
	}

	/* #ifdef CURRENT_OneXiaoShu		//ZCL 2018.1.26  电流1位小数
	#else
	#endif	 */
	// ZCL 2018.8.29 用Pw_ScrCurrentBits 代替

	DispMod_Par(0x9C, 0x04, 0x01, &w_dspNowTemperature, 3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// ZCL 2015.9.1 把数据通讯过来
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspNowHz, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspOutVoltage, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspAPhaseCurrent, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspNowTemperature, 3), 1);

		// w_dspAPhaseCurrent =19856;	//ZCL123 2017.7.24 测试
	}
}

void Menu_DspReadPar2(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "输入电压");
		LCD12864_String(0x87, " V");
		// 第二行
		LCD12864_String(0x90, "当前转速");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "目标频率"); // w_dspTargetHz
		LCD12864_String(0x8F, "Hz");

		// 第四行
		LCD12864_String(0x98, "临时频率");
		LCD12864_String(0x9F, "Hz");
		// LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspBusBarVoltage, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspNowRotateSpeed, 3, 0);
	DispMod_Par(0x8C, 0x04, 0x01, &w_dspTargetHz, 3, 0);
	DispMod_Par(0x9C, 0x04, 0x01, &w_dspTmpTargetHz, 3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspBusBarVoltage, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspNowRotateSpeed, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspTargetHz, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspTmpTargetHz, 3), 1);
	}
}

void Menu_DspReadPar3(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "A~电流值");
		LCD12864_String(0x87, " A");
		// 第二行
		LCD12864_String(0x90, "B~电流值");
		LCD12864_String(0x97, " A");
		// 第三行
		LCD12864_String(0x88, "C~电流值");
		LCD12864_String(0x8F, " A");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	/* 			w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位
		w_dspBPhaseCurrent = w_dspBPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位
		w_dspCPhaseCurrent = w_dspCPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位	 */
	// 显示和修改参数

	// DispMod_Par(0x84, 0x04,0x02, &w_dspAPhaseCurrent,3, 0);		//ZCL 2018.1.26	//ZCL 2016.10.6 曾经改成1位小数，改回2位小数

	/* #ifdef CURRENT_OneXiaoShu
	#else
	#endif */
	// ZCL 2018.8.29 用Pw_ScrCurrentBits 代替

	if (Pw_ScrCurrentBits == 1) // ZCL 2018.8.29  电流1位小数
	{
		DispMod_Par(0x84, 0x04, 0x01, &w_dspAPhaseCurrent, 3, 0); // ZCL 2018.1.26
		DispMod_Par(0x94, 0x04, 0x01, &w_dspBPhaseCurrent, 3, 0);
		DispMod_Par(0x8C, 0x04, 0x01, &w_dspCPhaseCurrent, 3, 0);
	}
	else if (Pw_ScrCurrentBits == 2) // ZCL 2018.8.29  电流2位小数
	{
		DispMod_Par(0x84, 0x05, 0x02, &w_dspAPhaseCurrent, 3, 0); // ZCL 2018.5.19  2019.6.7  0x04
		DispMod_Par(0x94, 0x05, 0x02, &w_dspBPhaseCurrent, 3, 0); // ZCL 2018.5.19  2019.6.7  0x04
		DispMod_Par(0x8C, 0x05, 0x02, &w_dspCPhaseCurrent, 3, 0); // ZCL 2018.5.19  2019.6.7  0x04
	}

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspAPhaseCurrent, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspBPhaseCurrent, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspCPhaseCurrent, 3), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_dspTorquePromote,3), 1 );
	}
}

void Menu_DspReadPar4(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "A~电流采");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "B~电流采");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "C~电流采");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspAPhaseCurrentADCValue, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspBPhaseCurrentADCValue, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspCPhaseCurrentADCValue, 3, 0);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_dspTorquePromote,3, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspAPhaseCurrentADCValue, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspBPhaseCurrentADCValue, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspCPhaseCurrentADCValue, 3), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_dspTorquePromote,3), 1 );
	}
}

void Menu_DspReadPar5(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "A~ DLH零");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "B~ DLH零");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "C~ DLH零");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspAPhaseDLHZero, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspBPhaseDLHZero, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspCPhaseDLHZero, 3, 0);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_dspTorquePromote,3, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspAPhaseDLHZero, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspBPhaseDLHZero, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspCPhaseDLHZero, 3), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_dspTorquePromote,3), 1 );
	}
}

// ZCL 2019.9.7 Menu_DspReadPar6()移植从 V139-3
void Menu_DspReadPar6(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "变频停原因");
		LCD12864_String(0x87, "种");
		// 第二行
		LCD12864_String(0x90, "--");
		// 第三行
		LCD12864_String(0x88, "故障码1 ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "--");

		Lw_SaveDspStopStatus = 1000;
		Lw_SaveDspFaultStatus = 1000;
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &w_DspStopNum, 1, 0); // 停机数量 w_dspStopNum	ZCL 2018.8.25
	// DispMod_Par(0x94, 0x05,0x00, &w_dspCounter3,3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspSysErrorCode1, 3, 0); // 故障码
	// DispMod_Par(0x9C, 0x05,0x00, &w_dspSysErrorCode1,3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 1. 变频停原因
	if (Lw_SaveDspStopStatus != w_DspStopStatus)
	{
		Lw_SaveDspStopStatus = w_DspStopStatus;
		// 停机原因的状态的提示  ZCL 2015.9.7
		switch (w_DspStopStatus)
		{
		case 0:
			LCD12864_String(0x91, "无停机- 正常  ");
			break; //
		case 1:
			LCD12864_String(0x91, "按键停机      ");
			break; //
		case 2:
			if (Pw_ScrMenuMode == 0)
			{
				LCD12864_String(0x91, "ARM 控制停机    ");
				break;
			} // ZCL 2019.9.7 显示：ARM 控制停机，具体原因从ARM去找
			else if (Pw_ScrMenuMode == 1 || Pw_ScrMenuMode == 2) // ZCL 2019.9.10 =1也显示：DI口停机
			{
				LCD12864_String(0x91, "DI口停机      ");
				break;
			} // ZCL 2019.7.20 后备注：变频停原因。这个停机来源于ARM口线送给DSP

		case 3:
			LCD12864_String(0x91, "变频故障停机  ");
			break; //

		default:
			return;
		}
	}

	// 2. 故障码 具体内容
	if (Lw_SaveDspFaultStatus != w_DspAlarmStatus) // w_dspFaultStatus ZCL 2018.8.25
	{
		Lw_SaveDspFaultStatus = w_DspAlarmStatus; // w_dspFaultStatus ZCL 2018.8.25
		// 停机原因的状态的提示  ZCL 2015.9.7
		switch (w_DspAlarmStatus) // w_dspFaultStatus ZCL 2018.8.25
		{
		case 0:
			LCD12864_String(0x99, "无故障- 正常  ");
			break; //
		case 1:
			LCD12864_String(0x99, "过流保护      ");
			break; //
		case 2:
			LCD12864_String(0x99, "过压保护      ");
			break; //
		case 3:
			LCD12864_String(0x99, "欠压保护      ");
			break; //
		case 4:
			LCD12864_String(0x99, "缺相保护      ");
			break; //
		case 5:
			LCD12864_String(0x99, "IPM 故障      ");
			break; //
		case 6:
			LCD12864_String(0x99, "电流互感器故障");
			break; //
		case 7:
			LCD12864_String(0x99, "IPM 过热      ");
			break; //
		case 8:
			LCD12864_String(0x99, "电机过热      ");
			break; //
		case 9:
			LCD12864_String(0x99, "风扇故障      ");
			break; //
		case 10:
			LCD12864_String(0x99, "EEPROM故障    ");
			break; //
		case 11:
			LCD12864_String(0x99, "DSP 处理器故障");
			break; //
		case 12:
			LCD12864_String(0x99, "STM 处理器故障");
			break; //
		case 13:
			LCD12864_String(0x99, "接地保护      ");
			break; //
		case 14:
			LCD12864_String(0x99, "加速中欠电压  ");
			break; //
		case 15:
			LCD12864_String(0x99, "减速中过电压");
			break; //

		default:
			return;
		}
	}

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_DspStopNum, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_DspStopStatus, 1), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&w_dspSysErrorCode1, 3), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_DspAlarmStatus, 1), Lw_SaveSetValue); // w_dspFaultStatus ZCL 2018.8.25
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_DspStopNum, 1), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_DspStopStatus, 1), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspSysErrorCode1, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_DspAlarmStatus, 1), 1); // w_dspFaultStatus ZCL 2018.8.25
	}
}

void Menu_DspReadPar7(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "通讯ER_C");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "上电读_C");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "通讯OK_C");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "通讯计数");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2018.5.17
	ModParNum(0); // 指定:修改参数的数量
	// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspCommErrCounter, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspCounter2, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspCounter4, 3, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_dspCounter, 3, 0);
	S_DisplayPar = 0;

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspCommErrCounter, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspCounter2, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspCounter4, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspCounter, 3), 1);
	}
}

void Menu_DspReadPar8(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "PDPINT_C        ");
		// 第二行
		LCD12864_String(0x90, "秒计数器");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "                ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspCounter5, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspCounter3, 3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspCounter5, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspCounter3, 3), 1);
	}
}

void Menu_DspReadPar29(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "AI1 输入");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "AI2 输入");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "PDPINT_C        ");
		// 第四行
		LCD12864_String(0x98, "秒计数器");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspAI1ADCValue, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspAI2ADCValue, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspCounter5, 3, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_dspCounter3, 3, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspAI1ADCValue, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspAI2ADCValue, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspCounter5, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspCounter3, 3), 1);
	}
}

// ZCL 2019.3.15 以下DspReadPar画面，用于DCM220
void Menu_DspReadPar30(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "观察值 1");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "观察值 2");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "观察值 3");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspTemp1, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspTemp2, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspTemp3, 3, 0);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_dspTorquePromote,3, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspTemp1, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspTemp2, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspTemp3, 3), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_dspTorquePromote,3), 1 );
	}
}

void Menu_DspReadPar31(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "观察值 4");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "观察值 5");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "观察值 6");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &w_dspTemp4, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspTemp5, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspTemp6, 3, 0);
	// DispMod_Par(0x9C, 0x04,0x00, &Pw_dspTorquePromote,3, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspTemp4, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspTemp5, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspTemp6, 3), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_dspTorquePromote,3), 1 );
	}
}

void Menu_DspReadPar32(void) // 画面 读参数  			//ZCL 2016.10.6 增加此画面
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "变频版本");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "Dsp-Year");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "Dsp-Date");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "文本版本");
		LCD12864_String(0x9F, "  "); // ZCL 2018.9.21
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x02, &w_dspSoftVersion, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspWriteYear, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspWriteDate, 3, 0);
	DispMod_Par(0x9C, 0x04, 0x02, &w_ScrVERSION, 5, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// ZCL 2015.9.1 把数据通讯过来
	if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		/* 	文本的版本不用通讯过来，DCM200_SCR自己的参数 */

		if (S_RdWrNo == 1) // ZCL 2018.9.13 上面if注释了，这里用了else if 错误。
			Read_Stm32Data(Address(&w_dspSoftVersion, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspWriteYear, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspWriteDate, 3), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data( Address(&w_ScrVERSION,4), 1 );
	}
}

// ZCL 2017.3.9
void Menu_DspReadPar33(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "Comp1Ds ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "Comp1Qs ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "FrqComp ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "FrqLimit");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);
	DispMod_Par(0x84, 0x05, 0x00, &w_dspdeadComp1Ds, 3, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspdeadComp1Qs, 3, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dspFrqComp, 3, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_dspFrqCompAfterLimt, 3, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspdeadComp1Ds, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspdeadComp1Qs, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspFrqComp, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspFrqCompAfterLimt, 3), 1);
	}
}

void Menu_DspReadPar34(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "1:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 1次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 b_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_dspEquipAlarmLast1,3, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_dspEquipAlarm1YM, 3, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_dspEquipAlarm1DH, 3, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_dspEquipAlarm1MS, 3, 0);
	//
	DspFaultDisplay(w_dspEquipAlarmLast1, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;							 // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspEquipAlarmLast1, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspEquipAlarm1YM, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspEquipAlarm1DH, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspEquipAlarm1MS, 3), 1);
	}
}

void Menu_DspReadPar35(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "2:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 2次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 b_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_dspEquipAlarmLast1,3, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_dspEquipAlarm2YM, 3, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_dspEquipAlarm2DH, 3, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_dspEquipAlarm2MS, 3, 0);
	//
	DspFaultDisplay(w_dspEquipAlarmLast2, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;							 // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspEquipAlarmLast2, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspEquipAlarm2YM, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspEquipAlarm2DH, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspEquipAlarm2MS, 3), 1);
	}
}

void Menu_DspReadPar36(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "3:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 3次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 b_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_dspEquipAlarmLast3,3, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_dspEquipAlarm3YM, 3, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_dspEquipAlarm3DH, 3, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_dspEquipAlarm3MS, 3, 0);
	//
	DspFaultDisplay(w_dspEquipAlarmLast3, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;							 // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspEquipAlarmLast3, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspEquipAlarm3YM, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspEquipAlarm3DH, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspEquipAlarm3MS, 3), 1);
	}
}

void Menu_DspReadPar37(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "4:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 4次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 b_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_dspEquipAlarmLast4,3, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_dspEquipAlarm4YM, 3, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_dspEquipAlarm4DH, 3, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_dspEquipAlarm4MS, 3, 0);
	//
	DspFaultDisplay(w_dspEquipAlarmLast4, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;							 // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspEquipAlarmLast4, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspEquipAlarm4YM, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspEquipAlarm4DH, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspEquipAlarm4MS, 3), 1);
	}
}

void Menu_DspReadPar38(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "5:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 5次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 b_modpar)	 */
	ModParNum(0);
	// DispMod_Par(0x84, 0x05,0x00, &w_dspEquipAlarmLast5,3, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_dspEquipAlarm5YM, 3, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_dspEquipAlarm5DH, 3, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_dspEquipAlarm5MS, 3, 0);
	//
	DspFaultDisplay(w_dspEquipAlarmLast5, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;							 // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspEquipAlarmLast5, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspEquipAlarm5YM, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspEquipAlarm5DH, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspEquipAlarm5MS, 3), 1);
	}
}

void Menu_DspReadPar39(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "最近六次故障记录");
		// 第二行
		LCD12864_String(0x90, "6:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "（上 6次）      ");
		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 b_modpar)	 */
	ModParNum(0);

	// DispMod_Par(0x84, 0x05,0x00, &w_dspEquipAlarmLast6,3, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_dspEquipAlarm6YM, 3, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_dspEquipAlarm6DH, 3, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_dspEquipAlarm6MS, 3, 0);

	//
	DspFaultDisplay(w_dspEquipAlarmLast6, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;							 // 显示参数结束，清零S_DisplayPar，再次延时

	if (B_ModPar == 0) // 读参数
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspEquipAlarmLast6, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspEquipAlarm6YM, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspEquipAlarm6DH, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspEquipAlarm6MS, 3), 1);
	}
}

void Menu_DspReadPar40(void) // 画面 读参数
{
	// u8 i;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "选择故障记录:   ");
		// 第二行
		LCD12864_String(0x90, "n:");
		// LCD12864_String(0x97, " L");
		// 第三行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x88, "    --    --    ");		//ZCL 2015.9.10
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x8A, "--"); // ZCL 2015.9.10
		LCD12864_String(0x8D, "--"); // ZCL 2015.9.10

		// LCD12864_String(0x8F, "Mp");
		// 第四行
		// ZCL 2017.6.17 这样写导致定时刷新页面时闪烁
		// LCD12864_String(0x98, "（总  上    次）");
		// ZCL 2017.6.17 改成如下：
		LCD12864_String(0x98, "（总");
		LCD12864_String(0x9B, "上");
		LCD12864_String(0x9E, "次）");

		// LCD12864_String(0x9F, "Hz");
	}

	/*  DispMod_Par(u8 x_pos,u8 length, u8 XiaoShuBits,  u16 *Address, u8 ParType,
	u8 b_modpar)	 */
	ModParNum(1);
	F_ModPar1 = 0;
	// DispMod_Par(0x84, 0x05,0x00, &w_dspSelEquipAlarm,3, 0);
	DispMod_Par(0x88, 0x04, 0x00, &w_dspSelEquipAlarm1YM, 3, 0);
	DispMod_Par(0x8B, 0x04, 0x00, &w_dspSelEquipAlarm1DH, 3, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_dspSelEquipAlarm1MS, 3, 0);
	//
	DispMod_Par(0x9A, 0x02, 0x00, &w_dspFaultRecNum, 3, 0);
	F_ModPar1 = 1;
	DispMod_Par(0x9D, 0x02, 0x00, &w_dspSelFaultNo, 3, F_ModPar1);
	//
	DspFaultDisplay(w_dspSelEquipAlarm, 0x91); // ZCL 2018.5.19 加入S_DisplayPar对显示限制，防止频繁刷新
	S_DisplayPar = 0;						   // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_dspSelFaultNo, 3), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 6)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspSelEquipAlarm, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspSelEquipAlarm1YM, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_dspSelEquipAlarm1DH, 3), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspSelEquipAlarm1MS, 3), 1);
		else if (S_RdWrNo == 5)
			Read_Stm32Data(Address(&w_dspSelFaultNo, 3), 1);
		else if (S_RdWrNo == 6)
			Read_Stm32Data(Address(&w_dspFaultRecNum, 3), 1);
	}
}

/* void Menu_DspReadPar52(void)				//画面 DSP读参数
{
	if(Lw_SavePageNo!=Lw_PageNo)
	{
		Lw_SavePageNo=Lw_PageNo;		C_TimeUpdatePage=0; 	S_Com1Send=0;
		//LCD12864_ClrText();          //清屏
		//第一行
		LCD12864_String(0x80, "A~校零值");
		LCD12864_String(0x87, "  ");
		//第二行
		LCD12864_String(0x90, "B~校零值");
		LCD12864_String(0x97, "  ");
		//第三行
		LCD12864_String(0x88, "C~校零值");
		LCD12864_String(0x8F, "  ");
		//第四行
		LCD12864_String(0x98, "                ");
		S_RdWrNo=0;
	}

	if(T_LcdDisplay!=SClk10Ms)
	{
		if(T_LcdDisplay!=SClk10Ms)
		C_LcdDisplay++;
		if(C_LcdDisplay>5)
		{
			C_LcdDisplay=0;
			ModParNum(0);
			//显示和修改参数
			DispMod_Par(0x84, 0x05,0x00, &w_dspAPhaseCalibrationZero,3, 0);
			DispMod_Par(0x94, 0x05,0x00, &w_dspBPhaseCalibrationZero,3, 0);
			DispMod_Par(0x8C, 0x05,0x00, &w_dspCPhaseCalibrationZero,3, 0);
			// DispMod_Par(0x9C, 0x04,0x00, &Pw_dspTorquePromote,2, F_ModPar4);
		}
	}

	//写参数 2015.9.1
	if(B_ModPar==1)
	{
		B_ModPar=2;
		S_Com1Send=0;
		C_Com1Send=0;
		S_Com1SendNoNum=0;
	}
	if(B_ModPar==2)
	{
		if(S_RdWrNo==1)
			Write_Stm32Data( Address(&w_dspAPhaseCalibrationZero,3), Lw_SaveSetValue );
		else if(S_RdWrNo==2)
			Write_Stm32Data( Address(&w_dspBPhaseCalibrationZero,3), Lw_SaveSetValue );
		else if(S_RdWrNo==3)
			Write_Stm32Data( Address(&w_dspCPhaseCalibrationZero,3), Lw_SaveSetValue );
		// else if(S_RdWrNo==4)
			// Write_Stm32Data( Address(&Pw_dspTorquePromote,3), Lw_SaveSetValue );
	}

	//读参数
	else if(B_ModPar==0)		//ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		//ZCL 2015.9.1 把数据通讯过来
		if(S_RdWrNo>3)
			S_RdWrNo=0;
		if(S_RdWrNo==0)
		{
			S_RdWrNo=1;
			S_Com1Send=0;
			C_Com1Send=0;
			S_Com1SendNoNum=0;
		}
		//按顺序读取数据
		if(S_RdWrNo==1)
			Read_Stm32Data( Address(&w_dspAPhaseCalibrationZero,3), 1 );
		else if(S_RdWrNo==2)
			Read_Stm32Data( Address(&w_dspBPhaseCalibrationZero,3), 1 );
		else if(S_RdWrNo==3)
			Read_Stm32Data( Address(&w_dspCPhaseCalibrationZero,3), 1 );
		// else if(S_RdWrNo==4)
			// Read_Stm32Data(Address(&Pw_dspTorquePromote,3), 1 );

	}
}

void Menu_DspReadPar53(void)				//画面 DSP读参数
{
	if(Lw_SavePageNo!=Lw_PageNo)
	{
		Lw_SavePageNo=Lw_PageNo;		C_TimeUpdatePage=0; 	S_Com1Send=0;
		//LCD12864_ClrText();          //清屏
		//第一行
		LCD12864_String(0x80, "A~校增益");
		LCD12864_String(0x87, "  ");
		//第二行
		LCD12864_String(0x90, "B~校增益");
		LCD12864_String(0x97, "  ");
		//第三行
		LCD12864_String(0x88, "C~校增益");
		LCD12864_String(0x8F, "  ");
		//第四行
		LCD12864_String(0x98, "                ");
		S_RdWrNo=0;
	}

	if(T_LcdDisplay!=SClk10Ms)
	{
		if(T_LcdDisplay!=SClk10Ms)
		C_LcdDisplay++;
		if(C_LcdDisplay>5)
		{
			C_LcdDisplay=0;
			ModParNum(0);
			//显示和修改参数
			DispMod_Par(0x84, 0x05,0x00, &w_dspAPhaseCalibrationGain,3, 0);
			DispMod_Par(0x94, 0x05,0x00, &w_dspBPhaseCalibrationGain,3, 0);
			DispMod_Par(0x8C, 0x05,0x00, &w_dspCPhaseCalibrationGain,3, 0);
			// DispMod_Par(0x9C, 0x04,0x00, &Pw_dspTorquePromote,2, F_ModPar4);
		}
	}

	//写参数 2015.9.1
	if(B_ModPar==1)
	{
		B_ModPar=2;
		S_Com1Send=0;
		C_Com1Send=0;
		S_Com1SendNoNum=0;
	}
	if(B_ModPar==2)
	{
		if(S_RdWrNo==1)
			Write_Stm32Data( Address(&w_dspAPhaseCalibrationGain,3), Lw_SaveSetValue );
		else if(S_RdWrNo==2)
			Write_Stm32Data( Address(&w_dspBPhaseCalibrationGain,3), Lw_SaveSetValue );
		else if(S_RdWrNo==3)
			Write_Stm32Data( Address(&w_dspCPhaseCalibrationGain,3), Lw_SaveSetValue );
		// else if(S_RdWrNo==4)
			// Write_Stm32Data( Address(&Pw_dspTorquePromote,3), Lw_SaveSetValue );
	}

	//读参数
	else if(B_ModPar==0)		//ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		//ZCL 2015.9.1 把数据通讯过来
		if(S_RdWrNo>3)
			S_RdWrNo=0;
		if(S_RdWrNo==0)
		{
			S_RdWrNo=1;
			S_Com1Send=0;
			C_Com1Send=0;
			S_Com1SendNoNum=0;
		}
		//按顺序读取数据
		if(S_RdWrNo==1)
			Read_Stm32Data( Address(&w_dspAPhaseCalibrationGain,3), 1 );
		else if(S_RdWrNo==2)
			Read_Stm32Data( Address(&w_dspBPhaseCalibrationGain,3), 1 );
		else if(S_RdWrNo==3)
			Read_Stm32Data( Address(&w_dspCPhaseCalibrationGain,3), 1 );
		// else if(S_RdWrNo==4)
			// Read_Stm32Data(Address(&Pw_dspTorquePromote,3), 1 );

	}
}
 */

void Menu_DspSetPar1(void) // 画面 DSP设定参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		if (Pw_ScrMenuMode == 0) // ZCL 2018.8.28 =0，控制器模式
			LCD12864_String(0x88, "3.变频设定参数  ");
		else if (Pw_ScrMenuMode == 1 || Pw_ScrMenuMode == 2) // ZCL 2018.8.28 =1，变频器模式
			LCD12864_String(0x88, "  变频设定参数  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	// ZCL 2017.3.21 必须加上下面一段，否则在没有参数的画面按OK键，会出现进入参数修改状态，出不来。
	ModParNum(0);
	// 显示和修改参数
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_DspSetPar2(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "加速时间");
		LCD12864_String(0x87, " S");
		// 第二行
		LCD12864_String(0x90, "减速时间");
		LCD12864_String(0x97, " S");
		// 第三行
		LCD12864_String(0x88, "电机转向");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "转矩提升");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2018.5.17
	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_dspAddSpeedTime, 2, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_dspSubSpeedTime, 2, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_dspMotorDirection, 2, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_dspTorquePromote, 2, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspAddSpeedTime, 2), Lw_SaveSetValue); // ZCL 2017.6.19 Pw_dspAddSpeedTime,0修正，应该为Pw_dspAddSpeedTime,2
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspSubSpeedTime, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspMotorDirection, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_dspTorquePromote, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspAddSpeedTime, 2), 1); // ZCL 2017.6.19 Pw_dspAddSpeedTime,0修正，应该为Pw_dspAddSpeedTime,2
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspSubSpeedTime, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspMotorDirection, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_dspTorquePromote, 2), 1);
	}
}

void Menu_DspSetPar3(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "VF曲线  ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "启动频率");
		LCD12864_String(0x97, "Hz");
		// 第三行
		LCD12864_String(0x88, "上限频率");
		LCD12864_String(0x8F, "Hz");
		// 第四行
		LCD12864_String(0x98, "下限频率");
		LCD12864_String(0x9F, "Hz");
	}

	// ZCL 2018.5.17
	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_dspVFCurve, 2, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x01, &Pw_dspStartHz, 2, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x01, &Pw_dspUpperLimitHz, 2, F_ModPar3);
	DispMod_Par(0x9C, 0x04, 0x01, &Pw_dspLowerLimitHz, 2, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspVFCurve, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspStartHz, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspUpperLimitHz, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_dspLowerLimitHz, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspVFCurve, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspStartHz, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspUpperLimitHz, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_dspLowerLimitHz, 2), 1);
	}
}

void Menu_DspSetPar4(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "载波频率");
		if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1) // ZCL 2019.3.18
			// 2017.6.17 改成1个字，防止闪烁(k前的数字闪烁) ZCL 2019.3.15
			LCD12864_String(0x86, " kHz"); // ZCL 2019.4.15 注意这里用了2个字
		else if (Pw_ScrMenuMode == 2)
			LCD12864_String(0x87, "kH"); // ZCL 2019.4.15
		// 第二行
		LCD12864_String(0x90, "死区补偿");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "过流电流");
		LCD12864_String(0x8F, " A");
		// 第四行
		LCD12864_String(0x98, "过压电压");
		LCD12864_String(0x9F, " V");
	}

	ModParNum(4);
	// 显示和修改参数
	if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1)					   // ZCL 2019.3.18
		DispMod_Par(0x84, 0x03, 0x00, &Pw_dspCarrierHz, 2, F_ModPar1); // ZCL 2015.9.29 注意后面单位用了2个字
	else if (Pw_ScrMenuMode == 2)									   // ZCL 2019.3.18  =11 DCM220 变频器模式
		DispMod_Par(0x84, 0x04, 0x01, &Pw_dspCarrierHz, 2, F_ModPar1); // ZCL 2017.5.4 改成1位小数 DCM220

	DispMod_Par(0x94, 0x04, 0x02, &Pw_dspDeadSupplement, 2, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_dspOCPCurrent, 2, F_ModPar3); // ZCL 2018.9.13 注：没有小数点
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_dspOVPVoltage, 2, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspCarrierHz, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspDeadSupplement, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspOCPCurrent, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_dspOVPVoltage, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspCarrierHz, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspDeadSupplement, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspOCPCurrent, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_dspOVPVoltage, 2), 1);
	}
}

void Menu_DspSetPar5(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "欠压电压");
		LCD12864_String(0x87, " V");
		// 第二行
		LCD12864_String(0x90, "过热温度");
		LCD12864_String(0x97, "度");
		// 第三行
		LCD12864_String(0x88, "风扇转温"); // ZCL 2019.3.16 霍尔类型
		LCD12864_String(0x8F, "度");
		// 第四行
		LCD12864_String(0x98, "风扇运行");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_dspUVPVoltage, 2, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x01, &Pw_dspOHPTemperature, 2, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x01, &Pw_dspFanRunTemperature, 2, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_dspFanRunMode, 2, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspUVPVoltage, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspOHPTemperature, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspFanRunTemperature, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_dspFanRunMode, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspUVPVoltage, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspOHPTemperature, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspFanRunTemperature, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_dspFanRunMode, 2), 1);
	}
}

void Menu_DspSetPar6(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "按键停模式");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "DI停机模式");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "霍尔类型");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "故障测试  "); // ZCL 2019.3.23 多一个字，后面从95，9D开始写（正常94，9D）
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &Pw_dspKeyStopMode, 2, F_ModPar1);
	DispMod_Par(0x95, 0x03, 0x00, &Pw_dspDIStopMode, 2, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_dspHuoErType, 2, F_ModPar3);
	DispMod_Par(0x9D, 0x03, 0x00, &w_dspManMadeFault, 3, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspKeyStopMode, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspDIStopMode, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspHuoErType, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_dspManMadeFault, 3), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0)
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspKeyStopMode, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspDIStopMode, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspHuoErType, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_dspManMadeFault, 3), 1);
	}
}

void Menu_DspSetPar7(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "AO1 输出源");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "AO2 输出源");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "DO1 输出源");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "DO2 输出源");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &Pw_dspAO1OutSource, 2, F_ModPar1);
	DispMod_Par(0x95, 0x03, 0x00, &Pw_dspAO2OutSource, 2, F_ModPar2);
	DispMod_Par(0x8D, 0x03, 0x00, &Pw_dspDO1OutSource, 2, F_ModPar3);
	DispMod_Par(0x9D, 0x03, 0x00, &Pw_dspDO2OutSource, 2, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspAO1OutSource, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspAO2OutSource, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspDO1OutSource, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_dspDO2OutSource, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspAO1OutSource, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspAO2OutSource, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspDO1OutSource, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_dspDO2OutSource, 2), 1);
	}
}

void Menu_DspSetPar8(void) // 画面 DSP设定参数
{
	if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1)
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "故障复位En");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "故障复位秒");
			LCD12864_String(0x97, "秒");
			// 第三行
			LCD12864_String(0x88, "                ");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(2);
		// 显示和修改参数
		DispMod_Par(0x85, 0x03, 0x00, &Pw_dspFaultAutoResetEn, 2, F_ModPar1);
		DispMod_Par(0x95, 0x03, 0x00, &Pw_dspFaultAutoResetSec, 2, F_ModPar2);

		// DispMod_Par(0x8C, 0x04,0x01, &Pw_ScrEquipPower,4, F_ModPar3);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1
		if (B_ModPar == 1)
		{
			B_ModPar = 2;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
		}
		if (B_ModPar == 2)
		{
			if (S_RdWrNo == 1)
				Write_Stm32Data(Address(&Pw_dspFaultAutoResetEn, 2), Lw_SaveSetValue);
			else if (S_RdWrNo == 2)
				Write_Stm32Data(Address(&Pw_dspFaultAutoResetSec, 2), Lw_SaveSetValue);
		}

		// 读参数
		else if (B_ModPar == 0)
		{
			if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
			{
				ReadBPDJPar();
				return;
			}

			// ZCL 2015.9.1 把数据通讯过来
			if (S_RdWrNo > 2)
				S_RdWrNo = 0;
			if (S_RdWrNo == 0)
			{
				S_RdWrNo = 1;
				S_Com1Send = 0;
				C_Com1Send = 0;
				S_Com1SendNoNum = 0;
				if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
				{
					B_TimeReadBPDJPar = 2;
					return; // ZCL 2019.3.19
				}
			}
			// 按顺序读取数据
			if (S_RdWrNo == 1)
				Read_Stm32Data(Address(&Pw_dspFaultAutoResetEn, 2), 1);
			else if (S_RdWrNo == 2)
				Read_Stm32Data(Address(&Pw_dspFaultAutoResetSec, 2), 1);
		}
	}

	// ZCL 2019.3.19  DCM220 变频器模式
	else if (Pw_ScrMenuMode == 2)
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "故障复位En");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "故障复位秒");
			LCD12864_String(0x97, "秒");
			// 第三行
			LCD12864_String(0x88, "设定复位数");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "已复位数");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(4);
		// 显示和修改参数
		DispMod_Par(0x85, 0x03, 0x00, &Pw_dspFaultAutoResetEn, 2, F_ModPar1);
		DispMod_Par(0x95, 0x03, 0x00, &Pw_dspFaultAutoResetSec, 2, F_ModPar2);
		DispMod_Par(0x8D, 0x03, 0x00, &Pw_dspAutoResetMax, 2, F_ModPar3);
		DispMod_Par(0x9C, 0x05, 0x00, &w_dspAutoResetNum, 3, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1
		if (B_ModPar == 1)
		{
			B_ModPar = 2;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
		}
		if (B_ModPar == 2)
		{
			if (S_RdWrNo == 1)
				Write_Stm32Data(Address(&Pw_dspFaultAutoResetEn, 2), Lw_SaveSetValue);
			else if (S_RdWrNo == 2)
				Write_Stm32Data(Address(&Pw_dspFaultAutoResetSec, 2), Lw_SaveSetValue);
			else if (S_RdWrNo == 3)
				Write_Stm32Data(Address(&Pw_dspAutoResetMax, 2), Lw_SaveSetValue);
			else if (S_RdWrNo == 4)
				Write_Stm32Data(Address(&w_dspAutoResetNum, 3), Lw_SaveSetValue);
		}

		// 读参数
		else if (B_ModPar == 0)
		{
			// ZCL 2015.9.1 把数据通讯过来
			if (S_RdWrNo > 4)
				S_RdWrNo = 0;
			if (S_RdWrNo == 0)
			{
				S_RdWrNo = 1;
				S_Com1Send = 0;
				C_Com1Send = 0;
				S_Com1SendNoNum = 0;
			}
			// 按顺序读取数据
			if (S_RdWrNo == 1)
				Read_Stm32Data(Address(&Pw_dspFaultAutoResetEn, 2), 1);
			else if (S_RdWrNo == 2)
				Read_Stm32Data(Address(&Pw_dspFaultAutoResetSec, 2), 1);
			else if (S_RdWrNo == 3)
				Read_Stm32Data(Address(&Pw_dspAutoResetMax, 2), 1);
			else if (S_RdWrNo == 4)
				Read_Stm32Data(Address(&w_dspAutoResetNum, 3), 1);
		}
	}
}

void Menu_DspSetPar9(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "启动频率停模式");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "测温模式");
		LCD12864_String(0x95, " (0-1)"); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		// 第三行
		LCD12864_String(0x88, "按键模式");
		LCD12864_String(0x8D, " (0-2)");

		// LCD12864_String(0x88, "                ");
		// LCD12864_String(0x8F, "  ");

		// 第四行
		LCD12864_String(0x98, "电流小数位");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x87, 0x01, 0x00, &Pw_dspStartHzStopMode, 2, F_ModPar1);
	DispMod_Par(0x94, 0x01, 0x00, &Pw_dspWenDuSel, 2, F_ModPar2);
	DispMod_Par(0x8C, 0x01, 0x00, &Pw_ScrKeyMode, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
	DispMod_Par(0x9D, 0x03, 0x00, &Pw_ScrCurrentBits, 4, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspStartHzStopMode, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspWenDuSel, 2), Lw_SaveSetValue);

		// ZCL 2018.9.13 Pw_ScrKeyMode不用写，为了完善流程写一个不用的变量 w_dspCommErrCounter ！！！
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&w_dspCommErrCounter, 3), Lw_SaveSetValue);
		// ZCL 2018.9.13 Pw_ScrCurrentBits不用写，为了完善流程写一个不用的变量 w_dspCounter2 ！！！
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_dspCounter2, 3), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0)
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspStartHzStopMode, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspWenDuSel, 2), 1);
		// else if(S_RdWrNo==3)
		// Read_Stm32Data( Address(&Pw_dspStartHzStopMode,2), 1 );
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&w_dspManMadeFault,3), 1 );
	}
}

void Menu_DspSetPar10(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "菜单模式"); // ZCL 2018.3.30
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "(0供水，1=变频) ");
		// LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "(2= DCM220)     ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "变频功率        "); // ZCL 2019.9.11
		LCD12864_String(0x9F, "kW");
	}

	ModParNum(2);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_ScrMenuMode, 4, F_ModPar1);

	DispMod_Par(0x9C, 0x04, 0x01, &Pw_ScrEquipPower, 4, F_ModPar2); // ZCL 2019.9.11

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// ZCL 2018.8.29 文本自己的参数，不用通讯来读写。
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_DspSetPar30(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "定频模式"); // DCM220
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "定频频率");
		LCD12864_String(0x97, "Hz");
		// 第三行
		LCD12864_String(0x88, "频率选择AI");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "最大频率");
		LCD12864_String(0x9F, "Hz");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_dspZDRunMode, 2, F_ModPar1);
	DispMod_Par(0x94, 0x04, 0x01, &Pw_dspRunHz, 2, F_ModPar2);
	DispMod_Par(0x8D, 0x03, 0x00, &Pw_dspHzAISource, 2, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_dspMAXFREQ, 2, F_ModPar4);
	/*			DispMod_Par(0x9C, 0x05,0x00, &Pw_dspSelABCShunXu,2, F_ModPar4); */
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspZDRunMode, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspRunHz, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspHzAISource, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_dspMAXFREQ, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0)
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspZDRunMode, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspRunHz, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspHzAISource, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_dspMAXFREQ, 2), 1);
	}
}

void Menu_DspSetPar31(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "角度补偿");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "角度补2 ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "补偿方向");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "电流顺序");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_dspAngleCompRef, 2, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_dspAngleCompSet, 2, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_dspCompDirection, 2, F_ModPar3);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_dspSelABCShunXu, 2, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_dspAngleCompRef, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspAngleCompSet, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspCompDirection, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_dspSelABCShunXu, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0)
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_dspAngleCompRef, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspAngleCompSet, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspCompDirection, 2), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_dspSelABCShunXu, 2), 1);
	}
}

// ZCL 2017.3.9
void Menu_DspSetPar32(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "震荡抑制参数值：");
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "空载系数");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "比例系数");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "微分系数");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(3);
	// 显示和修改参数
	// DispMod_Par(0x84, 0x05,0x00, &Pw_dspAngleCompRef,2, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_dspICurQsRef, 2, F_ModPar1);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_dspVoltCompUp, 2, F_ModPar2);
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_dspVoltCompUd, 2, F_ModPar3);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			/* 			Write_Stm32Data( Address(&Pw_dspAngleCompRef,2), Lw_SaveSetValue );
					else if(S_RdWrNo==2) */
			Write_Stm32Data(Address(&Pw_dspICurQsRef, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_dspVoltCompUp, 2), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspVoltCompUd, 2), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0)
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			/* 			Read_Stm32Data( Address(&Pw_dspAngleCompRef,2), 1 );
					else if(S_RdWrNo==2) */
			Read_Stm32Data(Address(&Pw_dspICurQsRef, 2), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_dspVoltCompUp, 2), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspVoltCompUd, 2), 1);
	}
}

// ZCL 2020.4.8
void Menu_DspSetPar33(void) // 画面 DSP设定参数
{
	if (Pw_ScrMenuMode == 2) // ZCL 2020.4.8 目前只有DCM220加入电压系数画面，DCM100的DSP还没有
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "电压系数");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "快速过压值");
			LCD12864_String(0x97, " V");
			// 第三行
			LCD12864_String(0x88, "快速欠压值");
			LCD12864_String(0x8F, " V");
			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(3);
		// 显示和修改参数
		DispMod_Par(0x84, 0x04, 0x02, &Pw_dspBusBarVoltageKi, 2, F_ModPar1);
		DispMod_Par(0x95, 0x03, 0x00, &Pw_dspFastOverVoltage, 2, F_ModPar2);
		DispMod_Par(0x8D, 0x03, 0x00, &Pw_dspFastUnderVoltage, 2, F_ModPar3);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1
		if (B_ModPar == 1)
		{
			B_ModPar = 2;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
		}
		if (B_ModPar == 2)
		{
			if (S_RdWrNo == 1)
				Write_Stm32Data(Address(&Pw_dspBusBarVoltageKi, 2), Lw_SaveSetValue);
			else if (S_RdWrNo == 2)
				Write_Stm32Data(Address(&Pw_dspFastOverVoltage, 2), Lw_SaveSetValue);
			else if (S_RdWrNo == 3)
				Write_Stm32Data(Address(&Pw_dspFastUnderVoltage, 2), Lw_SaveSetValue);
		}

		// 读参数
		else if (B_ModPar == 0)
		{
			if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
			{
				ReadBPDJPar();
				return;
			}

			// ZCL 2015.9.1 把数据通讯过来
			if (S_RdWrNo > 3)
				S_RdWrNo = 0;
			if (S_RdWrNo == 0)
			{
				S_RdWrNo = 1;
				S_Com1Send = 0;
				C_Com1Send = 0;
				S_Com1SendNoNum = 0;
				if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
				{
					B_TimeReadBPDJPar = 2;
					return; // ZCL 2019.3.19
				}
			}
			// 按顺序读取数据
			if (S_RdWrNo == 1)
				Read_Stm32Data(Address(&Pw_dspBusBarVoltageKi, 2), 1);
			else if (S_RdWrNo == 2)
				Read_Stm32Data(Address(&Pw_dspFastOverVoltage, 2), 1);
			else if (S_RdWrNo == 3)
				Read_Stm32Data(Address(&Pw_dspFastUnderVoltage, 2), 1);
		}
	}
}

// ZCL 2019.3.15 用于DCM220
void Menu_DspSetPar50(void) // 画面 DSP设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "修改参数");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "恢复参数");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "本机地址");
		LCD12864_String(0x8F, "号");
		// 第四行
		LCD12864_String(0x98, "(DCM220)        ");
	}

	ModParNum(3);
	DispMod_Par(0x84, 0x05, 0x00, &w_dspModPar, 3, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &w_dspParInitial, 3, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_dspAddress, 2, F_ModPar3);
	// DispMod_Par(0x9C, 0x03,0x01, &w_Pump1Current,1, 0);
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_dspModPar, 3), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_dspParInitial, 3), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_dspAddress, 2), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_NoWaterStopP,0), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2016.12.6  添加if(B_ModPar==0)。 后面同样的地方都添加。
	{
		if (B_TimeReadBPDJPar == 2) // ZCL 2019.3.19
		{
			ReadBPDJPar();
			return;
		}

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			if (B_TimeReadBPDJPar == 1) // ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			{
				B_TimeReadBPDJPar = 2;
				return; // ZCL 2019.3.19
			}
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_dspModPar, 3), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_dspParInitial, 3), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_dspAddress, 2), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_NoWaterStopP,0), 1 );
	}
}

void Menu_LoRaMasteSetPar1(void) // 画面 LoRa设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "修改LoRa");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "恢复LoRa");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "主从机设定      ");
		LCD12864_String(0x8D, "  ");
		// 第四行
		LCD12864_String(0x98, "本机地址        ");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2019.
	ModParNum(4);
	DispMod_Par(0x84, 0x05, 0x00, &w_GprsModPar, 4, F_ModPar1);		// ZCL 2019.4.2
	DispMod_Par(0x94, 0x05, 0x00, &w_GprsParInitial, 4, F_ModPar2); // ZCL 2019.4.2
	DispMod_Par(0x8D, 0x03, 0x00, &Pw_LoRaMasterSlaveSel, 4, F_ModPar3);
	DispMod_Par(0x9D, 0x03, 0x00, &Pw_LoRaEquipmentNo, 4, F_ModPar4);
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_LoRaSetPar1(void) // 画面 LoRa设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "从机1 地址      ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "从机2 地址      ");
		LCD12864_String(0x97, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		// 第三行
		LCD12864_String(0x88, "从机3 地址      ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "从机4 地址      ");
		LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &Pw_LoRaModule1Add, 4, F_ModPar1);
	DispMod_Par(0x95, 0x03, 0x00, &Pw_LoRaModule2Add, 4, F_ModPar2);
	DispMod_Par(0x8D, 0x03, 0x00, &Pw_LoRaModule3Add, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
	DispMod_Par(0x9D, 0x03, 0x00, &Pw_LoRaModule4Add, 4, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_LoRaSetPar2(void) // 画面 LoRa设定参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "从机5 地址      ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "从机6 地址      ");
			LCD12864_String(0x97, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
			// 第三行
			LCD12864_String(0x88, "从机7 地址      ");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "从机8 地址      ");
			LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		}

		ModParNum(4);
		// 显示和修改参数
		DispMod_Par(0x85, 0x03, 0x00, &Pw_LoRaModule5Add, 4, F_ModPar1);
		DispMod_Par(0x95, 0x03, 0x00, &Pw_LoRaModule6Add, 4, F_ModPar2);
		DispMod_Par(0x8D, 0x03, 0x00, &Pw_LoRaModule7Add, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
		DispMod_Par(0x9D, 0x03, 0x00, &Pw_LoRaModule8Add, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "LoRa位控制      ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "Gprs位控制      ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "发射测试1       ");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "发射测试2       ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(4);
		// 显示和修改参数
		// 注意：Pw_LoRa		Pw_Gprs		Pw_Scr	w_LoRa		w_Gprs		w_Scr 都是定义在 w_GprsParLst[]数组中的。
		DispMod_Par(0x85, 0x03, 0x00, &Pw_LoRaSet, 4, F_ModPar1);
		DispMod_Par(0x95, 0x03, 0x00, &Pw_GprsSet, 4, F_ModPar2);
		DispMod_Par(0x8D, 0x03, 0x00, &Pw_ScrLoRaTest1, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
		DispMod_Par(0x9D, 0x03, 0x00, &Pw_ScrLoRaSend2, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_LoRaSetPar3(void) // 画面 LoRa设定参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "LoRa位控制      ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "Gprs位控制      ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "发射测试1       ");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "发射测试2       ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(4);
		// 显示和修改参数
		// 注意：Pw_LoRa		Pw_Gprs		Pw_Scr	w_LoRa		w_Gprs		w_Scr 都是定义在 w_GprsParLst[]数组中的。
		DispMod_Par(0x85, 0x03, 0x00, &Pw_LoRaSet, 4, F_ModPar1);
		DispMod_Par(0x95, 0x03, 0x00, &Pw_GprsSet, 4, F_ModPar2);
		DispMod_Par(0x8D, 0x03, 0x00, &Pw_ScrLoRaTest1, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
		DispMod_Par(0x9D, 0x03, 0x00, &Pw_ScrLoRaSend2, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "LoRa频率");
			LCD12864_String(0x87, " M");
			// 第二行
			LCD12864_String(0x90, "LoRa功率");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "LoRa带宽");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "比例因子");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(4);
		// 显示和修改参数
		DispMod_Par(0x84, 0x05, 0x00, &Pw_LoRaSetFreq, 4, F_ModPar1);
		DispMod_Par(0x94, 0x05, 0x00, &Pw_LoRaSetPower, 4, F_ModPar2);
		DispMod_Par(0x8C, 0x05, 0x00, &Pw_LoRaSetSignalBW, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
		DispMod_Par(0x9C, 0x05, 0x00, &Pw_LoRaSetSpreadingFactor, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_LoRaSetPar4(void) // 画面 LoRa设定参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "LoRa频率");
			LCD12864_String(0x87, " M");
			// 第二行
			LCD12864_String(0x90, "LoRa功率");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "LoRa带宽");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "比例因子");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(4);
		// 显示和修改参数
		DispMod_Par(0x84, 0x05, 0x00, &Pw_LoRaSetFreq, 4, F_ModPar1);
		DispMod_Par(0x94, 0x05, 0x00, &Pw_LoRaSetPower, 4, F_ModPar2);
		DispMod_Par(0x8C, 0x05, 0x00, &Pw_LoRaSetSignalBW, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
		DispMod_Par(0x9C, 0x05, 0x00, &Pw_LoRaSetSpreadingFactor, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "错误编码");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "CrcOn   ");
			LCD12864_String(0x97, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
			// 第三行
			LCD12864_String(0x88, "隐式报头");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "接收单个");
			LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		}

		ModParNum(4);
		// 显示和修改参数
		DispMod_Par(0x84, 0x05, 0x00, &Pw_LoRaSetErrorCoding, 4, F_ModPar1);
		DispMod_Par(0x94, 0x05, 0x00, &Pw_LoRaSetCrcOn, 4, F_ModPar2);
		DispMod_Par(0x8C, 0x05, 0x00, &Pw_LoRaSetImplicitHeaderOn, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
		DispMod_Par(0x9C, 0x05, 0x00, &Pw_LoRaSetRxSingleOn, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_LoRaSetPar5(void) // 画面 LoRa设定参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "发送超时      ");
			LCD12864_String(0x87, "ms");
			// 第二行
			LCD12864_String(0x90, "接收超时      ");
			LCD12864_String(0x97, "ms");
			// 第三行
			LCD12864_String(0x88, "空中速率      ");
			LCD12864_String(0x8F, "kb");
			// 第四行
			LCD12864_String(0x98, "                 ");
			LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		}

		ModParNum(2);
		// 显示和修改参数
		DispMod_Par(0x84, 0x05, 0x00, &Pw_LoRaSetTxPacketTimeOut, 4, F_ModPar1);
		DispMod_Par(0x94, 0x05, 0x00, &Pw_LoRaSetRxPacketTimeOut, 4, F_ModPar2);
		DispMod_Par(0x8C, 0x04, 0x02, &w_LoRaDateRate, 5, 0); // 类型4和5，在DispMod_Par()中没有用 ZCL 2018.8.3
		// DispMod_Par(0x9C, 0x05, 0x00, &Pw_LoRaSetRxSingleOn, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "FreqHopOn ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "HopPeriod ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "PayLoadLen");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(3);
		// 显示和修改参数
		DispMod_Par(0x85, 0x03, 0x00, &Pw_LoRaSetFreqHopOn, 4, F_ModPar1);
		DispMod_Par(0x95, 0x03, 0x00, &Pw_LoRaSetHopPeriod, 4, F_ModPar2);
		DispMod_Par(0x8D, 0x03, 0x00, &Pw_LoRaSetPayLoadLength, 4, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
		// DispMod_Par(0x9D, 0x03,0x00, &Pw_LoRaSetRxSingleOn,4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_LoRaSetPar6(void) // 画面 LoRa设定参数
{
	// if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "重启间隔      ");
			LCD12864_String(0x87, "分");
			// 第二行
			LCD12864_String(0x90, "控制信息      ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "设备类型      ");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "(0双驱，1=变频) ");
			// LCD12864_String(0x9F, "  ");
		}

		ModParNum(3);
		// 显示和修改参数
		DispMod_Par(0x84, 0x05, 0x00, &Pw_RebootInterval, 4, F_ModPar1);
		DispMod_Par(0x94, 0x01, 0x00, &Pw_ConsoleInfo, 4, F_ModPar2);
		DispMod_Par(0x8C, 0x01, 0x00, &Pw_EquipmentType, 5, F_ModPar3); // 类型4和5，在DispMod_Par()中没有用 ZCL 2018.8.3
		// DispMod_Par(0x9D, 0x03, 0x00, &Pw_ConsoleInfo, 4, F_ModPar4);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		// 写参数 2015.9.1

		// 读参数
		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	// else // 变频电机
	// {
	// 	if (Lw_SavePageNo != Lw_PageNo)
	// 	{
	// 		Lw_SavePageNo = Lw_PageNo;
	// 		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

	// 		// LCD12864_ClrText();          //清屏
	// 		// 第一行
	// 		LCD12864_String(0x80, "发送超时      ");
	// 		LCD12864_String(0x87, "ms");
	// 		// 第二行
	// 		LCD12864_String(0x90, "接收超时      ");
	// 		LCD12864_String(0x97, "ms");
	// 		// 第三行
	// 		LCD12864_String(0x88, "空中速率      ");
	// 		LCD12864_String(0x8F, "kb");
	// 		// 第四行
	// 		LCD12864_String(0x98, "重启间隔      ");
	// 		LCD12864_String(0x9F, "分");
	// 	}

	// 	ModParNum(4);
	// 	// 显示和修改参数
	// 	DispMod_Par(0x84, 0x05, 0x00, &Pw_LoRaSetTxPacketTimeOut, 4, F_ModPar1);
	// 	DispMod_Par(0x94, 0x05, 0x00, &Pw_LoRaSetRxPacketTimeOut, 4, F_ModPar2);
	// 	DispMod_Par(0x8C, 0x04, 0x02, &w_LoRaDateRate, 5, F_ModPar3);	 // 类型4和5，在DispMod_Par()中没有用 ZCL 2018.8.3
	// 	DispMod_Par(0x9C, 0x05, 0x00, &Pw_RebootInterval, 4, F_ModPar4); //=0，通用协议；=1，舟山特殊协议（通道3和通道4）
	// 	S_DisplayPar = 0;												 // 显示参数结束，清零S_DisplayPar，再次延时

	// 	// 写参数 2015.9.1

	// 	// 读参数
	// 	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	// }
}

void Menu_LoRaSetPar7(void) // 画面 LoRa设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "文本版本");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "编译日期");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "编译日期");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x02, &w_ScrVERSION, 5, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_ScrWriteYear, 1, 0); // ZCL 2019.4.3
	DispMod_Par(0x8C, 0x05, 0x00, &w_ScrWriteDate, 1, 0);
	// DispMod_Par(0x9C, 0x05, 0x00, &w_ScrWriteDate, 1, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_GprsReadPar1(void) // 画面 Gprs参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "    DTU 参数    ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "ID:             ");
		LCD12864_String(0x97, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		// 第三行
		LCD12864_String(0x88, "IP1:            ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "   .   .   .    ");
		LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
	}

	ModParNum(0);
	// 显示和修改参数

	// DispMod_Par(0x92, 0x04,0x00, &w_GprsDtuID1,5, 0);
	// DispMod_Par(0x94, 0x04,0x00, &w_GprsDtuID2,5, 0);
	// DispMod_Par(0x96, 0x03,0x00, &w_GprsDtuID3,5, 0);

	if (Key_Data == 0) // ZCL 2019.4.5 加上按键限制，防止这里面有延时，按键反应缓慢
	{
		DispMod_Par2_String(0x92, (const char *)&GprsPar[DtuNoBase]);

		// ZCL 2019.4.4 连续使用LCD12864_Num时，要加点延时，否则出现过数字重叠。 加1MS延时 OK
		DispMod_Par3_Num(0x98, GprsPar[Ip0Base + 0]);
		DispMod_Par3_Num(0x9A, GprsPar[Ip0Base + 1]);
		DispMod_Par3_Num(0x9C, GprsPar[Ip0Base + 2]);
		DispMod_Par3_Num(0x9E, GprsPar[Ip0Base + 3]);
	}
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_GprsReadPar2(void) // 画面 Gprs参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "IP2-IP4:        ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "   .   .   .    ");
		LCD12864_String(0x97, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		// 第三行
		LCD12864_String(0x88, "   .   .   .    ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "   .   .   .    ");
		LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
	}

	ModParNum(0);
	// 显示和修改参数

	if (Key_Data == 0) // ZCL 2019.4.5 加上按键限制，防止这里面有延时，按键反应缓慢
	{
		// ZCL 2019.4.4 连续使用LCD12864_Num时，要加点延时，否则出现过数字重叠。 加1MS延时 OK
		DispMod_Par3_Num(0x90, GprsPar[Ip0Base + 4]);
		DispMod_Par3_Num(0x92, GprsPar[Ip0Base + 5]);
		DispMod_Par3_Num(0x94, GprsPar[Ip0Base + 6]);
		DispMod_Par3_Num(0x96, GprsPar[Ip0Base + 7]);

		DispMod_Par3_Num(0x88, GprsPar[Ip0Base + 8]);
		DispMod_Par3_Num(0x8A, GprsPar[Ip0Base + 9]);
		DispMod_Par3_Num(0x8C, GprsPar[Ip0Base + 10]);
		DispMod_Par3_Num(0x8E, GprsPar[Ip0Base + 11]);

		DispMod_Par3_Num(0x98, GprsPar[Ip0Base + 12]);
		DispMod_Par3_Num(0x9A, GprsPar[Ip0Base + 13]);
		DispMod_Par3_Num(0x9C, GprsPar[Ip0Base + 14]);
		DispMod_Par3_Num(0x9E, GprsPar[Ip0Base + 15]);
	}

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_GprsReadPar3(void) // 画面 Gprs参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "端口1-端口4:    ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x97, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		// 第三行
		LCD12864_String(0x88, "                ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
	}

	ModParNum(0);
	// 显示和修改参数

	if (Key_Data == 0) // ZCL 2019.4.5 加上按键限制，防止这里面有延时，按键反应缓慢
	{
		// ZCL 2019.4.4 连续使用LCD12864_Num时，要加点延时，否则出现过数字重叠。 加1MS延时 OK
		DispMod_Par3_Num(0x91, (GprsPar[Port0Base + 0] << 8) + GprsPar[Port0Base + 1]);
		DispMod_Par3_Num(0x95, (GprsPar[Port1Base + 0] << 8) + GprsPar[Port1Base + 1]);
		DispMod_Par3_Num(0x89, (GprsPar[Port2Base + 0] << 8) + GprsPar[Port2Base + 1]);
		DispMod_Par3_Num(0x8D, (GprsPar[Port3Base + 0] << 8) + GprsPar[Port3Base + 1]);
	}

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_GprsReadPar4(void) // 画面 Gprs参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "四路协议UDP.TCP:");
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "                ");
		LCD12864_String(0x95, " (0-1)"); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		// 第三行
		LCD12864_String(0x88, "信号强度");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "Xmodem项");
		LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
	}

	ModParNum(0);
	// 显示和修改参数

	if (Key_Data == 0) // ZCL 2019.4.5 加上按键限制，防止这里面有延时，按键反应缓慢
	{
		// ZCL 2019.4.4 连续使用LCD12864_Num时，要加点延时，否则出现过数字重叠。 加1MS延时 OK
		DispMod_Par3_Num(0x91, GprsPar[LinkTCPUDP0Base + 0]);
		DispMod_Par3_Num(0x92, GprsPar[LinkTCPUDP0Base + 1]);
		DispMod_Par3_Num(0x93, GprsPar[LinkTCPUDP0Base + 2]);
		DispMod_Par3_Num(0x94, GprsPar[LinkTCPUDP0Base + 3]);
	}

	// 显示和修改参数
	// 注意：Pw_LoRa		Pw_Gprs		Pw_Scr	w_LoRa		w_Gprs		w_Scr 都是定义在 w_GprsParLst[]数组中的。
	DispMod_Par(0x8C, 0x05, 0x00, &w_GprsCSQ, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_GprsXmodemFTItem, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp1ReadPar1(void) // 画面 DSP读参数1
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   1号高频双驱  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   1号变频电机  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp1ReadPar2_BK(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "当前频率");
		LCD12864_String(0x87, "Hz");

		// 第二行
		LCD12864_String(0x90, "电机电压");
		LCD12864_String(0x97, " V");

		// 第三行
		LCD12864_String(0x88, "电机电流");
		LCD12864_String(0x8F, " A");

		// 第四行
		LCD12864_String(0x98, "模块温度");
		LCD12864_String(0x9F, "度");
	}

	ModParNum(0);
	// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

	DispMod_Par(0x84, 0x04, 0x01, &w_dsp1NowHz, 5, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_dsp1OutVoltage, 5, 0);

	if (Pw_ScrCurrentBits == 1) // ZCL 2018.8.29  电流1位小数
	{
		DispMod_Par(0x8C, 0x04, 0x01, &w_dsp1APhaseCurrent, 5, 0); // ZCL 2018.5.19
	}
	else if (Pw_ScrCurrentBits == 2) // ZCL 2018.8.29  电流2位小数
	{
		DispMod_Par(0x8C, 0x04, 0x02, &w_dsp1APhaseCurrent, 5, 0); // ZCL 2018.5.19
	}

	/* #ifdef CURRENT_OneXiaoShu		//ZCL 2018.1.26  电流1位小数
	#else
	#endif	 */
	// ZCL 2018.8.29 用Pw_ScrCurrentBits 代替

	DispMod_Par(0x9C, 0x04, 0x01, &w_dsp1NowTemperature, 5, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp1ReadPar2(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		//	u32 nl_i;
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "AB~ 电压");
			LCD12864_String(0x87, " V");

			// 第二行
			LCD12864_String(0x90, "A~电流  ");
			LCD12864_String(0x97, " A");

			// 第三行
			LCD12864_String(0x88, "LoRa计数器");
			LCD12864_String(0x8F, "  ");

			if (B_LoRaErrWithModule1 == 0)
			{
				// 第四行
				LCD12864_String(0x98, "DNB 计数器");
				LCD12864_String(0x9F, "  ");
			}
			else
			{
				// 第四行
				LCD12864_String(0x98, "1#泵LoRa通讯异常");
			}
		}

		ModParNum(0);
		DispMod_Par(0x84, 0x04, 0x01, &w_Lora1UabRms, 5, 0);
		// DispMod_Par(0x93, 0x06,0x01, &w_UaRms,5, 0);
		DispMod_Par(0x93, 0x06, 0x02, &w_Lora1IaRms, 5, 0);

		DispMod_Par(0x8D, 0x05, 0x00, &w_LoraCounter1, 5, 0); // YLS 2024.10.20

		if (B_LoRaErrWithModule1 == 0)
			DispMod_Par(0x9D, 0x05, 0x00, &w_Lora1Counter1_BPQ, 5, 0); // ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "1 ");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "  ");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "  ");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "  ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x81, 0x05, 0x00, &w_WaterMeterID1, 5, 0);
		DispMod_Par(0x84, 0x05, 0x00, &w_WaterMeterID2, 5, 0);

		DispMod_Par(0x91, 0x05, 0x00, &w_WaterMeterSum1, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_WaterMeterSum2, 5, 0);

		DispMod_Par(0x89, 0x05, 0x00, &w_WaterMeterInstantFlow1, 5, 0);
		DispMod_Par(0x8C, 0x05, 0x00, &w_WaterMeterInstantFlow2, 5, 0);

		DispMod_Par(0x99, 0x05, 0x00, &w_WaterMeterInSum1, 5, 0);
		DispMod_Par(0x9C, 0x05, 0x00, &w_WaterMeterInSum2, 5, 0);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp1ReadPar3(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "220 计数器");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "远程数据    ");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "按键启停    ");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(1);

		DispMod_Par(0x85, 0x05, 0x00, &w_Lora1Counter2, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x95, 0x05, 0x00, &w_ZhuanFaData, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x8D, 0x05, 0x00, &w_Pump1RemoteStop, 5, F_ModPar1); // ZCL 2018.5.19

		if (F_ModeParLora == 0)
		{
			if (w_Pump1RemoteStop != w_SavePump1RemoteStop)
			{
				if (w_Pump1RemoteStop > 1)
				{
					w_Pump1RemoteStop = 0;
				}
				else
				{
					w_SavePump1RemoteStop = w_Pump1RemoteStop;

					w_ZhuanFaAdd = Addr_Lora1TouchRunStop;

					w_ZhuanFaData = w_Pump1RemoteStop;

					F_ModeParLora = 1;
				}
			}
		}

		// DispMod_Par(0x94, 0x04,0x01, &w_HighV1DispDianLi5,5, 0);
		// DispMod_Par(0x8C, 0x04,0x01, &w_HighV1DispDianLi6,5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1DispDianLi7,5, 0);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "2 ");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "  ");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "  ");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "  ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x81, 0x05, 0x00, &w_WaterMeterOutSum1, 5, 0);
		DispMod_Par(0x84, 0x05, 0x00, &w_WaterMeterOutSum2, 5, 0);

		DispMod_Par(0x91, 0x05, 0x00, &w_WaterMeterRemainMoney1, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_WaterMeterRemainMoney2, 5, 0);

		DispMod_Par(0x89, 0x05, 0x00, &w_WaterMeterOverdraft1, 5, 0);
		DispMod_Par(0x8C, 0x05, 0x00, &w_WaterMeterOverdraft2, 5, 0);

		DispMod_Par(0x99, 0x05, 0x00, &w_WaterMeterStatus1, 5, 0);
		DispMod_Par(0x9C, 0x05, 0x00, &w_WaterMeterStatus2, 5, 0);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp1ReadPar3_BK(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "错误码1 ");
		LCD12864_String(0x87, "  ");

		// 第二行
		LCD12864_String(0x90, "软件版本");
		LCD12864_String(0x97, "  ");

		// 第三行
		LCD12864_String(0x88, "秒计数器");
		LCD12864_String(0x8F, "  ");

		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

	DispMod_Par(0x84, 0x05, 0x00, &w_dsp1SysErrorCode1, 5, 0);
	DispMod_Par(0x94, 0x04, 0x02, &w_dsp1SoftVersion, 5, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_dsp1Counter3, 5, 0); // ZCL 2018.5.19
	// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp1ReadPar4(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "进水压力");
			LCD12864_String(0x87, "Mp");
			// 第二行
			LCD12864_String(0x90, "出水压力");
			LCD12864_String(0x97, "Mp");
			// 第三行
			LCD12864_String(0x88, "设定压力");
			LCD12864_String(0x8F, "Mp");
			// 第四行
			LCD12864_String(0x98, "变频频率");
			LCD12864_String(0x9F, "Hz");
		}

		ModParNum(0); // 指定:修改参数的数量
		// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
		// 显示和修改参数
		DispMod_Par(0x84, 0x04, 0x03, &w_Lora1InP, 5, 0);
		DispMod_Par(0x94, 0x04, 0x03, &w_Lora1OutP, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x03, &w_Lora1SetP, 5, 0);
		DispMod_Par(0x9C, 0x04, 0x01, &w_Lora1VvvfFreq, 5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "3 ");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "  ");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "  ");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "  ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x81, 0x05, 0x00, &w_WaterMeterNoUse1, 5, 0);
		DispMod_Par(0x84, 0x05, 0x00, &w_WaterMeterNoUse2, 5, 0);

		DispMod_Par(0x91, 0x05, 0x00, &w_WaterMeterTime1, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_WaterMeterTime2, 5, 0);

		DispMod_Par(0x89, 0x05, 0x00, &w_WaterMeterTime3, 5, 0);
		DispMod_Par(0x8C, 0x05, 0x00, &w_WaterMeterSoftVer, 5, 0);

		DispMod_Par(0x99, 0x05, 0x00, &w_WaterMeterNoUse3, 5, 0);
		DispMod_Par(0x9C, 0x05, 0x00, &w_WaterMeterNoUse4, 5, 0);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp1ReadPar4_BK(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水压力");
		LCD12864_String(0x87, "Mp");
		// 第二行
		LCD12864_String(0x90, "出水压力");
		LCD12864_String(0x97, "Mp");
		// 第三行
		LCD12864_String(0x88, "设定压力");
		LCD12864_String(0x8F, "Mp");

		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ModParNum(0);
	// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

	// #define	w_1InPDec								w_GprsParLst[128]		//.3 进水压力
	// #define	w_1OutPDec							w_GprsParLst[129]		//.3 出水压力
	// #define	w_1PIDCalcP							w_GprsParLst[130]		//.3 设定压力

	DispMod_Par(0x84, 0x04, 0x03, &w_1InPDec, 5, 0);
	DispMod_Par(0x94, 0x04, 0x03, &w_1OutPDec, 5, 0);
	DispMod_Par(0x8C, 0x04, 0x03, &w_1PIDCalcP, 5, 0); // ZCL 2018.5.19
	// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp1ReadPar5(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0;
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "温度表1 ");
			LCD12864_String(0x87, "℃");
			// 第二行
			LCD12864_String(0x90, "温度表2 ");
			LCD12864_String(0x97, "℃");
			// 第三行
			LCD12864_String(0x88, "软件版本");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(0);

		if (w_Pump1DispWenDu1DecValue >= 10000)
		{
			w_Pump1DispWenDu1DecValue = w_Pump1DispWenDu1DecValue - 10000;
			i1 = 10; // ZCL 2019.10.18 加上负号
		}
		if (w_Pump1DispWenDu2DecValue >= 10000)
		{
			w_Pump1DispWenDu2DecValue = w_Pump1DispWenDu2DecValue - 10000;
			i2 = 10; // ZCL 2019.10.18 加上负号
		}

		DispMod_Par(0x84, 0x04, 0x01, &w_Pump1DispWenDu1DecValue, 5 + i1, 0);
		DispMod_Par(0x94, 0x04, 0x01, &w_Pump1DispWenDu2DecValue, 5 + i2, 0);

		DispMod_Par(0x8C, 0x04, 0x02, &w_Lora1SoftVersion, 5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1Counter,5, 0);		//ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}

	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "供水状态        ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "停机原因        ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "变频功率        ");
			LCD12864_String(0x8F, "kW");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		// #define	w_1EquipOperateStatus  	w_GprsParLst[131]		//.0 供水状态
		// #define	w_1EquipAlarmStatus			w_GprsParLst[132]		//.0 停机原因
		// #define	w_1ScrEquipPower				w_GprsParLst[133]		//.0 设备功率

		DispMod_Par(0x84, 0x05, 0x00, &w_1EquipOperateStatus, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_1EquipAlarmStatus, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x01, &w_1ScrEquipPower, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp2ReadPar1(void) // 画面 DSP读参数1
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   2号高频双驱  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   2号变频电机  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp2ReadPar2(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		//	u32 nl_i;
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "AB~ 电压");
			LCD12864_String(0x87, " V");

			// 第二行
			LCD12864_String(0x90, "A~电流  ");
			LCD12864_String(0x97, " A");

			// 第三行
			LCD12864_String(0x88, "LoRa计数器");
			LCD12864_String(0x8F, "  ");
			if (B_LoRaErrWithModule2 == 0)
			{
				// 第四行
				LCD12864_String(0x98, "DNB 计数器");
				LCD12864_String(0x9F, "  ");
			}
			else
			{
				// 第四行
				LCD12864_String(0x98, "2#泵LoRa通讯异常");
			}
		}

		ModParNum(0);
		DispMod_Par(0x84, 0x04, 0x01, &w_Lora2UabRms, 5, 0);
		// DispMod_Par(0x93, 0x06,0x01, &w_UaRms,5, 0);
		DispMod_Par(0x93, 0x06, 0x02, &w_Lora2IaRms, 5, 0);

		DispMod_Par(0x8D, 0x05, 0x00, &w_LoraCounter2, 5, 0); // YLS 2024.10.20

		if (B_LoRaErrWithModule2 == 0)
			DispMod_Par(0x9D, 0x05, 0x00, &w_Lora2Counter1_BPQ, 5, 0); // ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "当前频率");
			LCD12864_String(0x87, "Hz");

			// 第二行
			LCD12864_String(0x90, "电机电压");
			LCD12864_String(0x97, " V");

			// 第三行
			LCD12864_String(0x88, "电机电流");
			LCD12864_String(0x8F, " A");

			// 第四行
			LCD12864_String(0x98, "模块温度");
			LCD12864_String(0x9F, "度");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x84, 0x04, 0x01, &w_dsp2NowHz, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_dsp2OutVoltage, 5, 0);

		if (Pw_ScrCurrentBits == 1) // ZCL 2018.8.29  电流1位小数
		{
			DispMod_Par(0x8C, 0x04, 0x01, &w_dsp2APhaseCurrent, 5, 0); // ZCL 2018.5.19
		}
		else if (Pw_ScrCurrentBits == 2) // ZCL 2018.8.29  电流2位小数
		{
			DispMod_Par(0x8C, 0x04, 0x02, &w_dsp2APhaseCurrent, 5, 0); // ZCL 2018.5.19
		}

		/* #ifdef CURRENT_OneXiaoShu		//ZCL 2018.1.26  电流1位小数
		#else
		#endif	 */
		// ZCL 2018.8.29 用Pw_ScrCurrentBits 代替

		DispMod_Par(0x9C, 0x04, 0x01, &w_dsp2NowTemperature, 5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp2ReadPar3(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "220 计数器");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "远程数据    ");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "按键启停    ");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(1);

		DispMod_Par(0x85, 0x05, 0x00, &w_Lora2Counter2, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x95, 0x05, 0x00, &w_ZhuanFaData, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x8D, 0x05, 0x00, &w_Pump2RemoteStop, 5, F_ModPar1); // ZCL 2018.5.19

		if (F_ModeParLora == 0)
		{
			if (w_Pump2RemoteStop != w_SavePump2RemoteStop)
			{
				if (w_Pump2RemoteStop > 1)
				{
					w_Pump2RemoteStop = 0;
				}
				else
				{
					w_SavePump2RemoteStop = w_Pump2RemoteStop;

					w_ZhuanFaAdd = Addr_Lora2TouchRunStop;

					w_ZhuanFaData = w_Pump2RemoteStop;

					F_ModeParLora = 1;
				}
			}
		}

		// DispMod_Par(0x94, 0x04,0x01, &w_HighV1DispDianLi5,5, 0);
		// DispMod_Par(0x8C, 0x04,0x01, &w_HighV1DispDianLi6,5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1DispDianLi7,5, 0);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "错误码1 ");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "软件版本");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "秒计数器");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x84, 0x05, 0x00, &w_dsp2SysErrorCode1, 5, 0);
		DispMod_Par(0x94, 0x04, 0x02, &w_dsp2SoftVersion, 5, 0);
		DispMod_Par(0x8C, 0x05, 0x00, &w_dsp2Counter3, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp2ReadPar4(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "进水压力");
			LCD12864_String(0x87, "Mp");
			// 第二行
			LCD12864_String(0x90, "出水压力");
			LCD12864_String(0x97, "Mp");
			// 第三行
			LCD12864_String(0x88, "设定压力");
			LCD12864_String(0x8F, "Mp");
			// 第四行
			LCD12864_String(0x98, "变频频率");
			LCD12864_String(0x9F, "Hz");
		}

		ModParNum(0); // 指定:修改参数的数量
		// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
		// 显示和修改参数
		DispMod_Par(0x84, 0x04, 0x03, &w_Lora2InP, 5, 0);
		DispMod_Par(0x94, 0x04, 0x03, &w_Lora2OutP, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x03, &w_Lora2SetP, 5, 0);
		DispMod_Par(0x9C, 0x04, 0x01, &w_Lora2VvvfFreq, 5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "进水压力");
			LCD12864_String(0x87, "Mp");
			// 第二行
			LCD12864_String(0x90, "出水压力");
			LCD12864_String(0x97, "Mp");
			// 第三行
			LCD12864_String(0x88, "设定压力");
			LCD12864_String(0x8F, "Mp");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		// #define	w_1InPDec								w_GprsParLst[128]		//.3 进水压力
		// #define	w_1OutPDec							w_GprsParLst[129]		//.3 出水压力
		// #define	w_1PIDCalcP							w_GprsParLst[130]		//.3 设定压力

		DispMod_Par(0x84, 0x04, 0x03, &w_2InPDec, 5, 0);
		DispMod_Par(0x94, 0x04, 0x03, &w_2OutPDec, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x03, &w_2PIDCalcP, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp2ReadPar5(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0;
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "温度表1 ");
			LCD12864_String(0x87, "℃");
			// 第二行
			LCD12864_String(0x90, "温度表2 ");
			LCD12864_String(0x97, "℃");
			// 第三行
			LCD12864_String(0x88, "软件版本");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(0);

		if (w_Pump2DispWenDu1DecValue >= 10000)
		{
			w_Pump2DispWenDu1DecValue = w_Pump2DispWenDu1DecValue - 10000;
			i1 = 10; // ZCL 2019.10.18 加上负号
		}
		if (w_Pump2DispWenDu2DecValue >= 10000)
		{
			w_Pump2DispWenDu2DecValue = w_Pump2DispWenDu2DecValue - 10000;
			i2 = 10; // ZCL 2019.10.18 加上负号
		}

		DispMod_Par(0x84, 0x04, 0x01, &w_Pump2DispWenDu1DecValue, 5 + i1, 0);
		DispMod_Par(0x94, 0x04, 0x01, &w_Pump2DispWenDu2DecValue, 5 + i2, 0);

		DispMod_Par(0x8C, 0x04, 0x02, &w_Lora2SoftVersion, 5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1Counter,5, 0);		//ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "供水状态        ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "停机原因        ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "变频功率        ");
			LCD12864_String(0x8F, "kW");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		// #define	w_1EquipOperateStatus  	w_GprsParLst[131]		//.0 供水状态
		// #define	w_1EquipAlarmStatus			w_GprsParLst[132]		//.0 停机原因
		// #define	w_1ScrEquipPower				w_GprsParLst[133]		//.0 设备功率

		DispMod_Par(0x84, 0x05, 0x00, &w_2EquipOperateStatus, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_2EquipAlarmStatus, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x01, &w_2ScrEquipPower, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp3ReadPar1(void) // 画面 DSP读参数1
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   3号高频双驱  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   3号变频电机  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp3ReadPar2(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		//	u32 nl_i;
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "AB~ 电压");
			LCD12864_String(0x87, " V");

			// 第二行
			LCD12864_String(0x90, "A~电流  ");
			LCD12864_String(0x97, " A");

			// 第三行
			LCD12864_String(0x88, "LoRa计数器");
			LCD12864_String(0x8F, "  ");

			if (B_LoRaErrWithModule3 == 0)
			{
				// 第四行
				LCD12864_String(0x98, "DNB 计数器");
				LCD12864_String(0x9F, "  ");
			}
			else
			{
				// 第四行
				LCD12864_String(0x98, "3#泵LoRa通讯异常");
			}
		}

		ModParNum(0);
		DispMod_Par(0x84, 0x04, 0x01, &w_Lora3UabRms, 5, 0);
		// DispMod_Par(0x93, 0x06,0x01, &w_UaRms,5, 0);
		DispMod_Par(0x93, 0x06, 0x02, &w_Lora3IaRms, 5, 0);

		DispMod_Par(0x8D, 0x05, 0x00, &w_LoraCounter3, 5, 0); // YLS 2024.10.20

		if (B_LoRaErrWithModule3 == 0)
			DispMod_Par(0x9D, 0x05, 0x00, &w_Lora3Counter1_BPQ, 5, 0); // ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "当前频率");
			LCD12864_String(0x87, "Hz");

			// 第二行
			LCD12864_String(0x90, "电机电压");
			LCD12864_String(0x97, " V");

			// 第三行
			LCD12864_String(0x88, "电机电流");
			LCD12864_String(0x8F, " A");

			// 第四行
			LCD12864_String(0x98, "模块温度");
			LCD12864_String(0x9F, "度");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x84, 0x04, 0x01, &w_dsp3NowHz, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_dsp3OutVoltage, 5, 0);

		if (Pw_ScrCurrentBits == 1) // ZCL 2018.8.29  电流1位小数
		{
			DispMod_Par(0x8C, 0x04, 0x01, &w_dsp3APhaseCurrent, 5, 0); // ZCL 2018.5.19
		}
		else if (Pw_ScrCurrentBits == 2) // ZCL 2018.8.29  电流2位小数
		{
			DispMod_Par(0x8C, 0x04, 0x02, &w_dsp3APhaseCurrent, 5, 0); // ZCL 2018.5.19
		}

		/* #ifdef CURRENT_OneXiaoShu		//ZCL 2018.1.26  电流1位小数
		#else
		#endif	 */
		// ZCL 2018.8.29 用Pw_ScrCurrentBits 代替

		DispMod_Par(0x9C, 0x04, 0x01, &w_dsp3NowTemperature, 5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp3ReadPar3(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "220 计数器");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "远程数据    ");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "按键启停    ");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(1);

		DispMod_Par(0x85, 0x05, 0x00, &w_Lora3Counter2, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x95, 0x05, 0x00, &w_ZhuanFaData, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x8D, 0x05, 0x00, &w_Pump3RemoteStop, 5, F_ModPar1); // ZCL 2018.5.19

		if (F_ModeParLora == 0)
		{
			if (w_Pump3RemoteStop != w_SavePump3RemoteStop)
			{
				if (w_Pump3RemoteStop > 1)
				{
					w_Pump3RemoteStop = 0;
				}
				else
				{
					w_SavePump3RemoteStop = w_Pump3RemoteStop;

					w_ZhuanFaAdd = Addr_Lora3TouchRunStop;

					w_ZhuanFaData = w_Pump3RemoteStop;

					F_ModeParLora = 1;
				}
			}
		}

		// DispMod_Par(0x94, 0x04,0x01, &w_HighV1DispDianLi5,5, 0);
		// DispMod_Par(0x8C, 0x04,0x01, &w_HighV1DispDianLi6,5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1DispDianLi7,5, 0);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "错误码1 ");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "软件版本");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "秒计数器");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x84, 0x05, 0x00, &w_dsp3SysErrorCode1, 5, 0);
		DispMod_Par(0x94, 0x04, 0x02, &w_dsp3SoftVersion, 5, 0);
		DispMod_Par(0x8C, 0x05, 0x00, &w_dsp3Counter3, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp3ReadPar4(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "进水压力");
			LCD12864_String(0x87, "Mp");
			// 第二行
			LCD12864_String(0x90, "出水压力");
			LCD12864_String(0x97, "Mp");
			// 第三行
			LCD12864_String(0x88, "设定压力");
			LCD12864_String(0x8F, "Mp");
			// 第四行
			LCD12864_String(0x98, "变频频率");
			LCD12864_String(0x9F, "Hz");
		}

		ModParNum(0); // 指定:修改参数的数量
		// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
		// 显示和修改参数
		DispMod_Par(0x84, 0x04, 0x03, &w_Lora3InP, 5, 0);
		DispMod_Par(0x94, 0x04, 0x03, &w_Lora3OutP, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x03, &w_Lora3SetP, 5, 0);
		DispMod_Par(0x9C, 0x04, 0x01, &w_Lora3VvvfFreq, 5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "进水压力");
			LCD12864_String(0x87, "Mp");
			// 第二行
			LCD12864_String(0x90, "出水压力");
			LCD12864_String(0x97, "Mp");
			// 第三行
			LCD12864_String(0x88, "设定压力");
			LCD12864_String(0x8F, "Mp");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		// #define	w_1InPDec								w_GprsParLst[128]		//.3 进水压力
		// #define	w_1OutPDec							w_GprsParLst[129]		//.3 出水压力
		// #define	w_1PIDCalcP							w_GprsParLst[130]		//.3 设定压力

		DispMod_Par(0x84, 0x04, 0x03, &w_3InPDec, 5, 0);
		DispMod_Par(0x94, 0x04, 0x03, &w_3OutPDec, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x03, &w_3PIDCalcP, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp3ReadPar5(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0;
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "温度表1 ");
			LCD12864_String(0x87, "℃");
			// 第二行
			LCD12864_String(0x90, "温度表2 ");
			LCD12864_String(0x97, "℃");
			// 第三行
			LCD12864_String(0x88, "软件版本");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(0);

		if (w_Pump3DispWenDu1DecValue >= 10000)
		{
			w_Pump3DispWenDu1DecValue = w_Pump3DispWenDu1DecValue - 10000;
			i1 = 10; // ZCL 2019.10.18 加上负号
		}
		if (w_Pump3DispWenDu2DecValue >= 10000)
		{
			w_Pump3DispWenDu2DecValue = w_Pump3DispWenDu2DecValue - 10000;
			i2 = 10; // ZCL 2019.10.18 加上负号
		}

		DispMod_Par(0x84, 0x04, 0x01, &w_Pump3DispWenDu1DecValue, 5 + i1, 0);
		DispMod_Par(0x94, 0x04, 0x01, &w_Pump3DispWenDu2DecValue, 5 + i2, 0);

		DispMod_Par(0x8C, 0x04, 0x02, &w_Lora3SoftVersion, 5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1Counter,5, 0);		//ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "供水状态        ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "停机原因        ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "变频功率        ");
			LCD12864_String(0x8F, "kW");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		// #define	w_1EquipOperateStatus  	w_GprsParLst[131]		//.0 供水状态
		// #define	w_1EquipAlarmStatus			w_GprsParLst[132]		//.0 停机原因
		// #define	w_1ScrEquipPower				w_GprsParLst[133]		//.0 设备功率

		DispMod_Par(0x84, 0x05, 0x00, &w_3EquipOperateStatus, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_3EquipAlarmStatus, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x01, &w_3ScrEquipPower, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp4ReadPar1(void) // 画面 DSP读参数1
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   4号高频双驱  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "                ");
			// 第二行
			LCD12864_String(0x90, "                ");
			// 第三行
			LCD12864_String(0x88, "   4号变频电机  ");
			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp4ReadPar2(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		//	u32 nl_i;
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "AB~ 电压");
			LCD12864_String(0x87, " V");

			// 第二行
			LCD12864_String(0x90, "A~电流  ");
			LCD12864_String(0x97, " A");

			// 第三行
			LCD12864_String(0x88, "LoRa计数器");
			LCD12864_String(0x8F, "  ");

			if (B_LoRaErrWithModule4 == 0)
			{
				// 第四行
				LCD12864_String(0x98, "DNB 计数器");
				LCD12864_String(0x9F, "  ");
			}
			else
			{
				// 第四行
				LCD12864_String(0x98, "4#泵LoRa通讯异常");
			}
		}

		ModParNum(0);
		DispMod_Par(0x84, 0x04, 0x01, &w_Lora4UabRms, 5, 0);
		// DispMod_Par(0x93, 0x06,0x01, &w_UaRms,5, 0);
		DispMod_Par(0x93, 0x06, 0x02, &w_Lora4IaRms, 5, 0);

		DispMod_Par(0x8D, 0x05, 0x00, &w_LoraCounter4, 5, 0); // YLS 2024.10.20

		if (B_LoRaErrWithModule4 == 0)
			DispMod_Par(0x9D, 0x05, 0x00, &w_Lora4Counter1_BPQ, 5, 0); // ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "当前频率");
			LCD12864_String(0x87, "Hz");

			// 第二行
			LCD12864_String(0x90, "电机电压");
			LCD12864_String(0x97, " V");

			// 第三行
			LCD12864_String(0x88, "电机电流");
			LCD12864_String(0x8F, " A");

			// 第四行
			LCD12864_String(0x98, "模块温度");
			LCD12864_String(0x9F, "度");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x84, 0x04, 0x01, &w_dsp4NowHz, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_dsp4OutVoltage, 5, 0);

		if (Pw_ScrCurrentBits == 1) // ZCL 2018.8.29  电流1位小数
		{
			DispMod_Par(0x8C, 0x04, 0x01, &w_dsp4APhaseCurrent, 5, 0); // ZCL 2018.5.19
		}
		else if (Pw_ScrCurrentBits == 2) // ZCL 2018.8.29  电流2位小数
		{
			DispMod_Par(0x8C, 0x04, 0x02, &w_dsp4APhaseCurrent, 5, 0); // ZCL 2018.5.19
		}

		/* #ifdef CURRENT_OneXiaoShu		//ZCL 2018.1.26  电流1位小数
		#else
		#endif	 */
		// ZCL 2018.8.29 用Pw_ScrCurrentBits 代替

		DispMod_Par(0x9C, 0x04, 0x01, &w_dsp4NowTemperature, 5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp4ReadPar3(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "220 计数器");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "远程数据    ");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "按键启停    ");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(1);

		DispMod_Par(0x85, 0x05, 0x00, &w_Lora4Counter2, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x95, 0x05, 0x00, &w_ZhuanFaData, 5, 0); // ZCL 2018.5.19

		DispMod_Par(0x8D, 0x05, 0x00, &w_Pump4RemoteStop, 5, F_ModPar1); // ZCL 2018.5.19

		if (F_ModeParLora == 0)
		{
			if (w_Pump4RemoteStop != w_SavePump4RemoteStop)
			{
				if (w_Pump4RemoteStop > 1)
				{
					w_Pump4RemoteStop = 0;
				}
				else
				{
					w_SavePump4RemoteStop = w_Pump4RemoteStop;

					w_ZhuanFaAdd = Addr_Lora4TouchRunStop;

					w_ZhuanFaData = w_Pump4RemoteStop;

					F_ModeParLora = 1;
				}
			}
		}

		// DispMod_Par(0x94, 0x04,0x01, &w_HighV1DispDianLi5,5, 0);
		// DispMod_Par(0x8C, 0x04,0x01, &w_HighV1DispDianLi6,5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1DispDianLi7,5, 0);

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "错误码1 ");
			LCD12864_String(0x87, "  ");

			// 第二行
			LCD12864_String(0x90, "软件版本");
			LCD12864_String(0x97, "  ");

			// 第三行
			LCD12864_String(0x88, "秒计数器");
			LCD12864_String(0x8F, "  ");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		DispMod_Par(0x84, 0x05, 0x00, &w_dsp4SysErrorCode1, 5, 0);
		DispMod_Par(0x94, 0x04, 0x02, &w_dsp4SoftVersion, 5, 0);
		DispMod_Par(0x8C, 0x05, 0x00, &w_dsp4Counter3, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp4ReadPar4(void) // 画面 DSP读参数
{
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "进水压力");
			LCD12864_String(0x87, "Mp");
			// 第二行
			LCD12864_String(0x90, "出水压力");
			LCD12864_String(0x97, "Mp");
			// 第三行
			LCD12864_String(0x88, "设定压力");
			LCD12864_String(0x8F, "Mp");
			// 第四行
			LCD12864_String(0x98, "变频频率");
			LCD12864_String(0x9F, "Hz");
		}

		ModParNum(0); // 指定:修改参数的数量
		// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
		// 显示和修改参数
		DispMod_Par(0x84, 0x04, 0x03, &w_Lora4InP, 5, 0);
		DispMod_Par(0x94, 0x04, 0x03, &w_Lora4OutP, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x03, &w_Lora4SetP, 5, 0);
		DispMod_Par(0x9C, 0x04, 0x01, &w_Lora4VvvfFreq, 5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "进水压力");
			LCD12864_String(0x87, "Mp");
			// 第二行
			LCD12864_String(0x90, "出水压力");
			LCD12864_String(0x97, "Mp");
			// 第三行
			LCD12864_String(0x88, "设定压力");
			LCD12864_String(0x8F, "Mp");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		// #define	w_1InPDec								w_GprsParLst[128]		//.3 进水压力
		// #define	w_1OutPDec							w_GprsParLst[129]		//.3 出水压力
		// #define	w_1PIDCalcP							w_GprsParLst[130]		//.3 设定压力

		DispMod_Par(0x84, 0x04, 0x03, &w_4InPDec, 5, 0);
		DispMod_Par(0x94, 0x04, 0x03, &w_4OutPDec, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x03, &w_4PIDCalcP, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp4ReadPar5(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0;
	if (Pw_EquipmentType == 0) // 双驱泵
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "温度表1 ");
			LCD12864_String(0x87, "℃");
			// 第二行
			LCD12864_String(0x90, "温度表2 ");
			LCD12864_String(0x97, "℃");
			// 第三行
			LCD12864_String(0x88, "软件版本");
			LCD12864_String(0x8F, "  ");
			// 第四行
			LCD12864_String(0x98, "                ");
			LCD12864_String(0x9F, "  ");
		}

		ModParNum(0);

		if (w_Pump4DispWenDu1DecValue >= 10000)
		{
			w_Pump4DispWenDu1DecValue = w_Pump4DispWenDu1DecValue - 10000;
			i1 = 10; // ZCL 2019.10.18 加上负号
		}
		if (w_Pump4DispWenDu2DecValue >= 10000)
		{
			w_Pump4DispWenDu2DecValue = w_Pump4DispWenDu2DecValue - 10000;
			i2 = 10; // ZCL 2019.10.18 加上负号
		}

		DispMod_Par(0x84, 0x04, 0x01, &w_Pump4DispWenDu1DecValue, 5 + i1, 0);
		DispMod_Par(0x94, 0x04, 0x01, &w_Pump4DispWenDu2DecValue, 5 + i2, 0);

		DispMod_Par(0x8C, 0x04, 0x02, &w_Lora4SoftVersion, 5, 0);
		// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1Counter,5, 0);		//ZCL 2018.5.19

		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
	else // 变频电机
	{
		if (Lw_SavePageNo != Lw_PageNo)
		{
			Lw_SavePageNo = Lw_PageNo;
			EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

			// LCD12864_ClrText();          //清屏
			// 第一行
			LCD12864_String(0x80, "供水状态        ");
			LCD12864_String(0x87, "  ");
			// 第二行
			LCD12864_String(0x90, "停机原因        ");
			LCD12864_String(0x97, "  ");
			// 第三行
			LCD12864_String(0x88, "变频功率        ");
			LCD12864_String(0x8F, "kW");

			// 第四行
			LCD12864_String(0x98, "                ");
		}

		ModParNum(0);
		// w_dspAPhaseCurrent = w_dspAPhaseCurrent/10;		//ZCL 2016.2.20 原先1位小数，去掉1位

		// #define	w_1EquipOperateStatus  	w_GprsParLst[131]		//.0 供水状态
		// #define	w_1EquipAlarmStatus			w_GprsParLst[132]		//.0 停机原因
		// #define	w_1ScrEquipPower				w_GprsParLst[133]		//.0 设备功率

		DispMod_Par(0x84, 0x05, 0x00, &w_4EquipOperateStatus, 5, 0);
		DispMod_Par(0x94, 0x05, 0x00, &w_4EquipAlarmStatus, 5, 0);
		DispMod_Par(0x8C, 0x04, 0x01, &w_4ScrEquipPower, 5, 0); // ZCL 2018.5.19
		// DispMod_Par(0x9C, 0x04,0x01, &w_dspNowTemperature,5, 0);
		S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

		ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
	}
}

void Menu_Dsp5ReadPar1(void) // 画面 DSP读参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "   5号高频双驱  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp5ReadPar2(void) // 画面 DSP读参数
{
	//	u32 nl_i;
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "AB~ 电压");
		LCD12864_String(0x87, " V");

		// 第二行
		LCD12864_String(0x90, "A~电流  ");
		LCD12864_String(0x97, " A");

		// 第三行
		LCD12864_String(0x88, "LoRa计数器");
		LCD12864_String(0x8F, "  ");

		if (B_LoRaErrWithModule4 == 0)
		{
			// 第四行
			LCD12864_String(0x98, "DNB 计数器");
			LCD12864_String(0x9F, "  ");
		}
		else
		{
			// 第四行
			LCD12864_String(0x98, "5#泵LoRa通讯异常");
		}
	}

	ModParNum(0);
	DispMod_Par(0x84, 0x04, 0x01, &w_Lora5UabRms, 5, 0);
	// DispMod_Par(0x93, 0x06,0x01, &w_UaRms,5, 0);
	DispMod_Par(0x93, 0x06, 0x02, &w_Lora5IaRms, 5, 0);

	DispMod_Par(0x8D, 0x05, 0x00, &w_LoraCounter5, 5, 0);

	if (B_LoRaErrWithModule5 == 0)
		DispMod_Par(0x9D, 0x05, 0x00, &w_Lora5Counter1_BPQ, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp5ReadPar3(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "220 计数器");
		LCD12864_String(0x87, "  ");

		// 第二行
		LCD12864_String(0x90, "远程数据    ");
		LCD12864_String(0x97, "  ");

		// 第三行
		LCD12864_String(0x88, "按键启停    ");
		LCD12864_String(0x8F, "  ");

		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(1);

	DispMod_Par(0x85, 0x05, 0x00, &w_Lora5Counter2, 5, 0); // ZCL 2018.5.19

	DispMod_Par(0x95, 0x05, 0x00, &w_ZhuanFaData, 5, 0); // ZCL 2018.5.19

	DispMod_Par(0x8D, 0x05, 0x00, &w_Pump5RemoteStop, 5, F_ModPar1); // ZCL 2018.5.19

	if (F_ModeParLora == 0)
	{
		if (w_Pump5RemoteStop != w_SavePump5RemoteStop)
		{
			if (w_Pump5RemoteStop > 1)
			{
				w_Pump5RemoteStop = 0;
			}
			else
			{
				w_SavePump5RemoteStop = w_Pump5RemoteStop;

				w_ZhuanFaAdd = Addr_Lora5TouchRunStop;

				w_ZhuanFaData = w_Pump5RemoteStop;

				F_ModeParLora = 1;
			}
		}
	}

	// DispMod_Par(0x94, 0x04,0x01, &w_HighV1DispDianLi5,5, 0);
	// DispMod_Par(0x8C, 0x04,0x01, &w_HighV1DispDianLi6,5, 0);
	// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1DispDianLi7,5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp5ReadPar4(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "进水压力");
		LCD12864_String(0x87, "Mp");
		// 第二行
		LCD12864_String(0x90, "出水压力");
		LCD12864_String(0x97, "Mp");
		// 第三行
		LCD12864_String(0x88, "设定压力");
		LCD12864_String(0x8F, "Mp");
		// 第四行
		LCD12864_String(0x98, "变频频率");
		LCD12864_String(0x9F, "Hz");
	}

	ModParNum(0); // 指定:修改参数的数量
	// ZCL 2018.5.16 修改参数不延时，加快速度保证切换；显示参数延时，否则液晶显示跟不上，显示错乱
	// 显示和修改参数
	DispMod_Par(0x84, 0x04, 0x03, &w_Lora5InP, 5, 0);
	DispMod_Par(0x94, 0x04, 0x03, &w_Lora5OutP, 5, 0);
	DispMod_Par(0x8C, 0x04, 0x03, &w_Lora5SetP, 5, 0);
	DispMod_Par(0x9C, 0x04, 0x01, &w_Lora5VvvfFreq, 5, 0);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp5ReadPar5(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0;
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "温度表1 ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "温度表2 ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "软件版本");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);

	if (w_Pump5DispWenDu1DecValue >= 10000)
	{
		w_Pump5DispWenDu1DecValue = w_Pump5DispWenDu1DecValue - 10000;
		i1 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_Pump5DispWenDu2DecValue >= 10000)
	{
		w_Pump5DispWenDu2DecValue = w_Pump5DispWenDu2DecValue - 10000;
		i2 = 10; // ZCL 2019.10.18 加上负号
	}

	DispMod_Par(0x84, 0x04, 0x01, &w_Pump5DispWenDu1DecValue, 5 + i1, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_Pump5DispWenDu2DecValue, 5 + i2, 0);

	DispMod_Par(0x8C, 0x04, 0x02, &w_Lora5SoftVersion, 5, 0);
	// DispMod_Par(0x9C, 0x05,0x00, &w_HighV1Counter,5, 0);		//ZCL 2018.5.19

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp6ReadPar1(void) // 画面 DSP读参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "   6号高频双驱  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp6ReadPar2(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "电机频率");
		LCD12864_String(0x87, "Hz");

		// 第二行
		LCD12864_String(0x90, "线电压12");
		LCD12864_String(0x97, " V");

		// 第三行
		LCD12864_String(0x88, "线电压23");
		LCD12864_String(0x8F, " V");

		// 第四行
		LCD12864_String(0x98, "线电压31");
		LCD12864_String(0x9F, " V");
	}

	ModParNum(0);

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV6DispDianLi8, 5, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_HighV6DispDianLi1, 5, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_HighV6DispDianLi2, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV6DispDianLi3, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp6ReadPar3(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, " A相电流");
		LCD12864_String(0x87, " A");

		// 第二行
		LCD12864_String(0x90, " B相电流");
		LCD12864_String(0x97, " A");

		// 第三行
		LCD12864_String(0x88, " C相电流");
		LCD12864_String(0x8F, " A");

		// 第四行
		LCD12864_String(0x98, "有功功率");
		LCD12864_String(0x9F, "kW");
	}

	ModParNum(0);

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV6DispDianLi4, 5, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV6DispDianLi5, 5, 0);
	DispMod_Par(0x8C, 0x04, 0x01, &w_HighV6DispDianLi6, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV6DispDianLi7, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp6ReadPar4(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0, i3 = 0, i4 = 0;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "温度表1 ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "温度表2 ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "温度表3 ");
		LCD12864_String(0x8F, "℃");

		// 第四行
		LCD12864_String(0x98, "温度表4 ");
		LCD12864_String(0x9F, "℃");
	}

	ModParNum(0);

	if (w_HighV6DispWenDu1DecValue >= 10000)
	{
		w_HighV6DispWenDu1DecValue = w_HighV6DispWenDu1DecValue - 10000;
		i1 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV6DispWenDu2DecValue >= 10000)
	{
		w_HighV6DispWenDu2DecValue = w_HighV6DispWenDu2DecValue - 10000;
		i2 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV6DispWenDu3DecValue >= 10000)
	{
		w_HighV6DispWenDu3DecValue = w_HighV6DispWenDu3DecValue - 10000;
		i3 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV6DispWenDu4DecValue >= 10000)
	{
		w_HighV6DispWenDu4DecValue = w_HighV6DispWenDu4DecValue - 10000;
		i4 = 10; // ZCL 2019.10.18 加上负号
	}

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV6DispWenDu1DecValue, 5 + i1, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV6DispWenDu2DecValue, 5 + i2, 0);
	DispMod_Par(0x8C, 0x04, 0x01, &w_HighV6DispWenDu3DecValue, 5 + i3, 0);
	DispMod_Par(0x9C, 0x04, 0x01, &w_HighV6DispWenDu4DecValue, 5 + i4, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp6ReadPar5(void) // 画面 DSP读参数
{
	u8 i5 = 0, i6 = 0;
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "温度表5 ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "温度表6 ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "软件版本");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "秒计数器");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);

	if (w_HighV6DispWenDu5DecValue >= 10000)
	{
		w_HighV6DispWenDu5DecValue = w_HighV6DispWenDu5DecValue - 10000;
		i5 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV6DispWenDu6DecValue >= 10000)
	{
		w_HighV6DispWenDu6DecValue = w_HighV6DispWenDu6DecValue - 10000;
		i6 = 10; // ZCL 2019.10.18 加上负号
	}

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV6DispWenDu5DecValue, 5 + i5, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV6DispWenDu6DecValue, 5 + i6, 0);

	DispMod_Par(0x8C, 0x04, 0x02, &w_HighV6SoftVersion, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV6Counter, 5, 0); // ZCL 2018.5.19

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp7ReadPar1(void) // 画面 DSP读参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "   7号高频双驱  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp7ReadPar2(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "电机频率");
		LCD12864_String(0x87, "Hz");

		// 第二行
		LCD12864_String(0x90, "线电压12");
		LCD12864_String(0x97, " V");

		// 第三行
		LCD12864_String(0x88, "线电压23");
		LCD12864_String(0x8F, " V");

		// 第四行
		LCD12864_String(0x98, "线电压31");
		LCD12864_String(0x9F, " V");
	}

	ModParNum(0);

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV7DispDianLi8, 5, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_HighV7DispDianLi1, 5, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_HighV7DispDianLi2, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV7DispDianLi3, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp7ReadPar3(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, " A相电流");
		LCD12864_String(0x87, " A");

		// 第二行
		LCD12864_String(0x90, " B相电流");
		LCD12864_String(0x97, " A");

		// 第三行
		LCD12864_String(0x88, " C相电流");
		LCD12864_String(0x8F, " A");

		// 第四行
		LCD12864_String(0x98, "有功功率");
		LCD12864_String(0x9F, "kW");
	}

	ModParNum(0);

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV7DispDianLi4, 5, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV7DispDianLi5, 5, 0);
	DispMod_Par(0x8C, 0x04, 0x01, &w_HighV7DispDianLi6, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV7DispDianLi7, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp7ReadPar4(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0, i3 = 0, i4 = 0;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "温度表1 ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "温度表2 ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "温度表3 ");
		LCD12864_String(0x8F, "℃");

		// 第四行
		LCD12864_String(0x98, "温度表4 ");
		LCD12864_String(0x9F, "℃");
	}

	ModParNum(0);

	if (w_HighV7DispWenDu1DecValue >= 10000)
	{
		w_HighV7DispWenDu1DecValue = w_HighV7DispWenDu1DecValue - 10000;
		i1 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV7DispWenDu2DecValue >= 10000)
	{
		w_HighV7DispWenDu2DecValue = w_HighV7DispWenDu2DecValue - 10000;
		i2 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV7DispWenDu3DecValue >= 10000)
	{
		w_HighV7DispWenDu3DecValue = w_HighV7DispWenDu3DecValue - 10000;
		i3 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV7DispWenDu4DecValue >= 10000)
	{
		w_HighV7DispWenDu4DecValue = w_HighV7DispWenDu4DecValue - 10000;
		i4 = 10; // ZCL 2019.10.18 加上负号
	}

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV7DispWenDu1DecValue, 5 + i1, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV7DispWenDu2DecValue, 5 + i2, 0);
	DispMod_Par(0x8C, 0x04, 0x01, &w_HighV7DispWenDu3DecValue, 5 + i3, 0);
	DispMod_Par(0x9C, 0x04, 0x01, &w_HighV7DispWenDu4DecValue, 5 + i4, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp7ReadPar5(void) // 画面 DSP读参数
{
	u8 i5 = 0, i6 = 0;
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "温度表5 ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "温度表6 ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "软件版本");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "秒计数器");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);

	if (w_HighV7DispWenDu5DecValue >= 10000)
	{
		w_HighV7DispWenDu5DecValue = w_HighV7DispWenDu5DecValue - 10000;
		i5 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV7DispWenDu6DecValue >= 10000)
	{
		w_HighV7DispWenDu6DecValue = w_HighV7DispWenDu6DecValue - 10000;
		i6 = 10; // ZCL 2019.10.18 加上负号
	}

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV7DispWenDu5DecValue, 5 + i5, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV7DispWenDu6DecValue, 5 + i6, 0);

	DispMod_Par(0x8C, 0x04, 0x02, &w_HighV7SoftVersion, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV7Counter, 5, 0); // ZCL 2018.5.19

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp8ReadPar1(void) // 画面 DSP读参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "   8号高频双驱  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

// ZCL 2019.3.8 主机显示的内容
void Menu_Dsp8ReadPar2(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "电机频率");
		LCD12864_String(0x87, "Hz");

		// 第二行
		LCD12864_String(0x90, "线电压12");
		LCD12864_String(0x97, " V");

		// 第三行
		LCD12864_String(0x88, "线电压23");
		LCD12864_String(0x8F, " V");

		// 第四行
		LCD12864_String(0x98, "线电压31");
		LCD12864_String(0x9F, " V");
	}

	ModParNum(0);

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV8DispDianLi8, 5, 0);
	DispMod_Par(0x94, 0x05, 0x00, &w_HighV8DispDianLi1, 5, 0);
	DispMod_Par(0x8C, 0x05, 0x00, &w_HighV8DispDianLi2, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV8DispDianLi3, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp8ReadPar3(void) // 画面 DSP读参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, " A相电流");
		LCD12864_String(0x87, " A");

		// 第二行
		LCD12864_String(0x90, " B相电流");
		LCD12864_String(0x97, " A");

		// 第三行
		LCD12864_String(0x88, " C相电流");
		LCD12864_String(0x8F, " A");

		// 第四行
		LCD12864_String(0x98, "有功功率");
		LCD12864_String(0x9F, "kW");
	}

	ModParNum(0);

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV8DispDianLi4, 5, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV8DispDianLi5, 5, 0);
	DispMod_Par(0x8C, 0x04, 0x01, &w_HighV8DispDianLi6, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV8DispDianLi7, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp8ReadPar4(void) // 画面 DSP读参数
{
	u8 i1 = 0, i2 = 0, i3 = 0, i4 = 0;

	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "温度表1 ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "温度表2 ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "温度表3 ");
		LCD12864_String(0x8F, "℃");

		// 第四行
		LCD12864_String(0x98, "温度表4 ");
		LCD12864_String(0x9F, "℃");
	}

	ModParNum(0);

	if (w_HighV8DispWenDu1DecValue >= 10000)
	{
		w_HighV8DispWenDu1DecValue = w_HighV8DispWenDu1DecValue - 10000;
		i1 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV8DispWenDu2DecValue >= 10000)
	{
		w_HighV8DispWenDu2DecValue = w_HighV8DispWenDu2DecValue - 10000;
		i2 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV8DispWenDu3DecValue >= 10000)
	{
		w_HighV8DispWenDu3DecValue = w_HighV8DispWenDu3DecValue - 10000;
		i3 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV8DispWenDu4DecValue >= 10000)
	{
		w_HighV8DispWenDu4DecValue = w_HighV8DispWenDu4DecValue - 10000;
		i4 = 10; // ZCL 2019.10.18 加上负号
	}

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV8DispWenDu1DecValue, 5 + i1, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV8DispWenDu2DecValue, 5 + i2, 0);
	DispMod_Par(0x8C, 0x04, 0x01, &w_HighV8DispWenDu3DecValue, 5 + i3, 0);
	DispMod_Par(0x9C, 0x04, 0x01, &w_HighV8DispWenDu4DecValue, 5 + i4, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_Dsp8ReadPar5(void) // 画面 DSP读参数
{
	u8 i5 = 0, i6 = 0;
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "温度表5 ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "温度表6 ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "软件版本");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "秒计数器");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(0);

	if (w_HighV8DispWenDu5DecValue >= 10000)
	{
		w_HighV8DispWenDu5DecValue = w_HighV8DispWenDu5DecValue - 10000;
		i5 = 10; // ZCL 2019.10.18 加上负号
	}
	if (w_HighV8DispWenDu6DecValue >= 10000)
	{
		w_HighV8DispWenDu6DecValue = w_HighV8DispWenDu6DecValue - 10000;
		i6 = 10; // ZCL 2019.10.18 加上负号
	}

	DispMod_Par(0x84, 0x04, 0x01, &w_HighV8DispWenDu5DecValue, 5 + i5, 0);
	DispMod_Par(0x94, 0x04, 0x01, &w_HighV8DispWenDu6DecValue, 5 + i6, 0);

	DispMod_Par(0x8C, 0x04, 0x02, &w_HighV8SoftVersion, 5, 0);
	DispMod_Par(0x9C, 0x05, 0x00, &w_HighV8Counter, 5, 0); // ZCL 2018.5.19

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_YW310Par1(void) // 画面 DSP读参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "   YW310参数    ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

// ZCL 2019.10.19 YW310
void Menu_YW310Par2(void) // 画面 YW310设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "修改YW310 ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "恢复YW310       ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "温感地址        ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "计数器          ");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2019.
	ModParNum(3);
	DispMod_Par(0x85, 0x04, 0x00, &Pw_YW310ModPar, 8, F_ModPar1);	  // ZCL 2019.4.2  2019.10.19 使用 8类型！
	DispMod_Par(0x95, 0x04, 0x00, &Pw_YW310ParInitial, 8, F_ModPar2); // ZCL 2019.4.2
	DispMod_Par(0x8D, 0x04, 0x00, &Pw_YW310EquipmentNo, 8, F_ModPar3);
	DispMod_Par(0x9D, 0x05, 0x00, &w_YW310Counter, 8, 0);
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_YW310ModPar, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_YW310ParInitial, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_YW310EquipmentNo, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_YW310Counter, 8), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_YW310ModPar, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_YW310ParInitial, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_YW310EquipmentNo, 8), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_YW310Counter, 8), 1);
	}
}

void Menu_YW310Par3(void) // 画面 YW310设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "压力温度选择    ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "压温系数        ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "AI1 初值        ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, " YW310版本      ");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2019.
	ModParNum(3);
	DispMod_Par(0x86, 0x03, 0x00, &Pw_YW310PressureWenDuSel, 8, F_ModPar1);	  // ZCL 2019.4.2  2019.10.19 使用 8类型！
	DispMod_Par(0x94, 0x04, 0x03, &Pw_YW310PressureWenDuXiShu, 8, F_ModPar2); // ZCL 2019.4.2
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_YW310AI1SensorZero, 8, F_ModPar3);
	DispMod_Par(0x9D, 0x04, 0x02, &w_YW310SoftVer, 8, 0);
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_YW310PressureWenDuSel, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_YW310PressureWenDuXiShu, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_YW310AI1SensorZero, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_YW310SoftVer, 8), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_YW310PressureWenDuSel, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_YW310PressureWenDuXiShu, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_YW310AI1SensorZero, 8), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_YW310SoftVer, 8), 1);
	}
}

void Menu_YW310Par4(void) // 画面 YW310设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "正温量程        ");
		LCD12864_String(0x87, "℃");
		// 第二行
		LCD12864_String(0x90, "负温量程        ");
		LCD12864_String(0x97, "℃");
		// 第三行
		LCD12864_String(0x88, "最大AI值        ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2019.
	ModParNum(3);
	DispMod_Par(0x84, 0x05, 0x00, &Pw_YW310ZWenDecMax, 8, F_ModPar1); // ZCL 2019.4.2  2019.10.19 使用 8类型！
	DispMod_Par(0x94, 0x05, 0x00, &Pw_YW310FWenDecMax, 8, F_ModPar2); // ZCL 2019.4.2
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_YW310MaxAIHex, 8, F_ModPar3);
	// DispMod_Par(0x9C, 0x04,0x02, &w_YW310SoftVer,8, 0);
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_YW310ZWenDecMax, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_YW310FWenDecMax, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_YW310MaxAIHex, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&w_YW310SoftVer, 8), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_YW310ZWenDecMax, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_YW310FWenDecMax, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_YW310MaxAIHex, 8), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&w_YW310SoftVer, 8), 1);
	}
}

void Menu_SZM220Par1(void) // 画面 DSP读参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "                ");
		// 第二行
		LCD12864_String(0x90, "                ");
		// 第三行
		LCD12864_String(0x88, "   SZM220 参数  ");
		// 第四行
		LCD12864_String(0x98, "                ");
	}

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_SZM220Par2(void) // 画面 SZM220设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "修改ZM220");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "恢复ZM220");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "主从机设定      ");
		LCD12864_String(0x8D, "  ");
		// 第四行
		LCD12864_String(0x98, "220-LORA地址    ");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2019.
	ModParNum(4);
	DispMod_Par(0x85, 0x04, 0x00, &Pw_ZM220ModPar, 8, F_ModPar1);	  // ZCL 2019.4.2  2019.10.19 使用 8类型！
	DispMod_Par(0x95, 0x04, 0x00, &Pw_ZM220ParInitial, 8, F_ModPar2); // ZCL 2019.4.2
	DispMod_Par(0x8D, 0x03, 0x00, &Pw_ZM220LoRaMasterSlaveSel, 8, F_ModPar3);
	DispMod_Par(0x9E, 0x02, 0x00, &Pw_ZM220LoRaEquipmentNo, 8, F_ModPar4);
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ZM220ModPar, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_ZM220ParInitial, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_ZM220LoRaMasterSlaveSel, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_ZM220LoRaEquipmentNo, 8), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ZM220ModPar, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_ZM220ParInitial, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_ZM220LoRaMasterSlaveSel, 8), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_ZM220LoRaEquipmentNo, 8), 1);
	}
}

void Menu_SZM220Par3(void) // 画面 SZM220设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "LoRa频率");
		LCD12864_String(0x87, " M");
		// 第二行
		LCD12864_String(0x90, "LoRa功率");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "LoRa带宽");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "比例因子");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2019.10.19
	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_ZM220LoRaSetFreq, 8, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_ZM220LoRaSetPower, 8, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_ZM220LoRaSetSignalBW, 8, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_ZM220LoRaSetSpreadingFactor, 8, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetFreq, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetPower, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetSignalBW, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetSpreadingFactor, 8), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetFreq, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetPower, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetSignalBW, 8), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetSpreadingFactor, 8), 1);
	}
}

void Menu_SZM220Par4(void) // 画面 SZM220设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "错误编码");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "CrcOn   ");
		LCD12864_String(0x97, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
		// 第三行
		LCD12864_String(0x88, "隐式报头");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "接收单个");
		LCD12864_String(0x9F, "  "); // ZCL 2018.7.30 原先0x97不好，这里因为(01)占2个字，应该用0x96
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_ZM220LoRaSetErrorCoding, 8, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_ZM220LoRaSetCrcOn, 8, F_ModPar2);
	DispMod_Par(0x8C, 0x05, 0x00, &Pw_ZM220LoRaSetImplicitHeaderOn, 8, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
	DispMod_Par(0x9C, 0x05, 0x00, &Pw_ZM220LoRaSetRxSingleOn, 8, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetErrorCoding, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetCrcOn, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetImplicitHeaderOn, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetRxSingleOn, 8), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetErrorCoding, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetCrcOn, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetImplicitHeaderOn, 8), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetRxSingleOn, 8), 1);
	}
}

void Menu_SZM220Par5(void) // 画面 SZM220设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "FreqHopOn ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "HopPeriod ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "PayLoadLen");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "LoRa位控制      ");
		LCD12864_String(0x9F, "  ");
	}

	ModParNum(4);
	// 显示和修改参数
	DispMod_Par(0x85, 0x03, 0x00, &Pw_ZM220LoRaSetFreqHopOn, 8, F_ModPar1);
	DispMod_Par(0x95, 0x03, 0x00, &Pw_ZM220LoRaSetHopPeriod, 8, F_ModPar2);
	DispMod_Par(0x8D, 0x03, 0x00, &Pw_ZM220LoRaSetPayLoadLength, 8, F_ModPar3); // 类型4，在DispMod_Par()中没有用 ZCL 2018.8.3
	DispMod_Par(0x9D, 0x03, 0x00, &Pw_ZM220LoRaSet, 8, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetFreqHopOn, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetHopPeriod, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetPayLoadLength, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 4)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSet, 8), Lw_SaveSetValue);
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 4)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetFreqHopOn, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetHopPeriod, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetPayLoadLength, 8), 1);
		else if (S_RdWrNo == 4)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSet, 8), 1);
	}
}

void Menu_SZM220Par6(void) // 画面 SZM220设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "发送超时      ");
		LCD12864_String(0x87, "ms");
		// 第二行
		LCD12864_String(0x90, "接收超时      ");
		LCD12864_String(0x97, "ms");
		// 第三行
		LCD12864_String(0x88, "空中速率      ");
		LCD12864_String(0x8F, "kb");
		// 第四行
		LCD12864_String(0x98, "SZM220--LORA参数");
		// LCD12864_String(0x9F, "  ");
	}

	ModParNum(2);
	// 显示和修改参数
	DispMod_Par(0x84, 0x05, 0x00, &Pw_ZM220LoRaSetTxPacketTimeOut, 8, F_ModPar1);
	DispMod_Par(0x94, 0x05, 0x00, &Pw_ZM220LoRaSetRxPacketTimeOut, 8, F_ModPar2);
	DispMod_Par(0x8C, 0x04, 0x02, &w_ZM220LoRaDateRate, 8, 0); // 类型4和5，在DispMod_Par()中没有用 ZCL 2018.8.3
	// DispMod_Par(0x9D, 0x03,0x00, &w_LoRaDateRate,4, F_ModPar4);
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetTxPacketTimeOut, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&Pw_ZM220LoRaSetRxPacketTimeOut, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 3)
			Write_Stm32Data(Address(&w_ZM220LoRaDateRate, 8), Lw_SaveSetValue);
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_ZM220LoRaSetRxSingleOn,8), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 3)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetTxPacketTimeOut, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&Pw_ZM220LoRaSetRxPacketTimeOut, 8), 1);
		else if (S_RdWrNo == 3)
			Read_Stm32Data(Address(&w_ZM220LoRaDateRate, 8), 1);
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_ZM220LoRaSetRxSingleOn,8), 1 );
	}
}

void Menu_SZM220Par7(void) // 画面 YW310设定参数
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "SZM220计数");
		// LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "SZM220版本      ");
		LCD12864_String(0x97, "  ");
		// 第三行
		LCD12864_String(0x88, "                ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "                ");
		LCD12864_String(0x9F, "  ");
	}

	// ZCL 2019.
	ModParNum(0);
	DispMod_Par(0x85, 0x05, 0x00, &w_ZM220Counter, 8, 0); // ZCL 2019.4.2  2019.10.19 使用 8类型！
	DispMod_Par(0x95, 0x04, 0x02, &w_ZM220SoftVer, 8, 0); // ZCL 2019.4.2
	// DispMod_Par(0x8D, 0x04,0x02, &w_YW310SoftVer,8, 0);
	// DispMod_Par(0x9D, 0x03,0x00, &Pw_YW310EquipmentNo,8, F_ModPar4);
	//
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	// 写参数 2015.9.1

	// 读参数
	// ReadBPDJPar2();			//ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数

	// ZCL 2019.10.19 修改

	// 写参数 2015.9.1
	if (B_ModPar == 1)
	{
		B_ModPar = 2;
		S_Com1Send = 0;
		C_Com1Send = 0;
		S_Com1SendNoNum = 0;
	}
	if (B_ModPar == 2)
	{
		if (S_RdWrNo == 1)
			Write_Stm32Data(Address(&w_ZM220Counter, 8), Lw_SaveSetValue);
		else if (S_RdWrNo == 2)
			Write_Stm32Data(Address(&w_ZM220SoftVer, 8), Lw_SaveSetValue);
		// else if(S_RdWrNo==3)
		// Write_Stm32Data( Address(&w_YW310SoftVer,8), Lw_SaveSetValue );
		// else if(S_RdWrNo==4)
		// Write_Stm32Data( Address(&Pw_YW310EquipmentNo,8), Lw_SaveSetValue );
	}

	// 读参数
	else if (B_ModPar == 0) // ZCL 2018.5.15 +if(B_ModPar==0)  没有解决串行改参数，但更完整
	{
		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		// {
		// ReadBPDJPar();
		// return;
		// }

		// ZCL 2015.9.1 把数据通讯过来
		if (S_RdWrNo > 2)
			S_RdWrNo = 0;
		if (S_RdWrNo == 0)
		{
			S_RdWrNo = 1;
			S_Com1Send = 0;
			C_Com1Send = 0;
			S_Com1SendNoNum = 0;
			// if(B_TimeReadBPDJPar==1)		//ZCL 2019.3.19 =1 要定时读，变频电机常规参数（非本画面）
			// {
			// B_TimeReadBPDJPar=2;
			// return;										//ZCL 2019.3.19
			// }
		}
		// 按顺序读取数据
		if (S_RdWrNo == 1)
			Read_Stm32Data(Address(&w_ZM220Counter, 8), 1);
		else if (S_RdWrNo == 2)
			Read_Stm32Data(Address(&w_ZM220SoftVer, 8), 1);
		// else if(S_RdWrNo==3)
		// Read_Stm32Data( Address(&w_YW310SoftVer,8), 1 );
		// else if(S_RdWrNo==4)
		// Read_Stm32Data(Address(&Pw_YW310EquipmentNo,8), 1 );
	}
}

void Menu_GpsReadPar1(void) // 画面 读参数  			//ZCL 2016.10.6 增加此画面
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		// LCD12864_ClrText();          //清屏
		// 第一行
		LCD12864_String(0x80, "纬度:           ");
		LCD12864_String(0x87, "  ");
		// 第二行
		LCD12864_String(0x90, "    D   .       ");
		LCD12864_String(0x97, "F ");
		// 第三行
		LCD12864_String(0x88, "经度:           ");
		LCD12864_String(0x8F, "  ");
		// 第四行
		LCD12864_String(0x98, "     D  .       ");
		LCD12864_String(0x9F, "F "); // ZCL 2018.9.21
	}

	ModParNum(0);
	// 显示和修改参数
	DispMod_Par(0x83, 0x05, 0x02, &w_ScrGpsLatAllDu1, 5, 0); // ZCL 2019.9.24    ,4,
	DispMod_Par(0x86, 0x04, 0x00, &w_ScrGpsLatAllDu2, 5, 0);

	DispMod_Par(0x91, 0x02, 0x00, &w_ScrGpsLatDu, 5, 0);
	DispMod_Par(0x93, 0x02, 0x00, &w_ScrGpsLatFen1, 5, 0);
	DispMod_Par(0x95, 0x04, 0x00, &w_ScrGpsLatFen2, 5, 0);

	DispMod_Par(0x8B, 0x05, 0x02, &w_ScrGpsLonAllDu1, 5, 0);
	DispMod_Par(0x8E, 0x04, 0x00, &w_ScrGpsLonAllDu2, 5, 0);

	DispMod_Par(0x99, 0x03, 0x00, &w_ScrGpsLonDu, 5, 0);
	DispMod_Par(0x9B, 0x02, 0x00, &w_ScrGpsLonFen1, 5, 0);
	DispMod_Par(0x9D, 0x04, 0x00, &w_ScrGpsLonFen2, 5, 0);

	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	/* #define	w_ScrGpsLatDu			w_ScrParLst[18]		//.0 纬度 度；	ddmm.mmmm之dd
	#define	w_ScrGpsLatFen1		w_ScrParLst[19]		//.0 纬度 分；		ddmm.mmmm之mm
	#define	w_ScrGpsLatFen2		w_ScrParLst[20]		//.0 纬度 分；		ddmm.mmmm之mmmm
	#define	w_ScrGpsLonDu			w_ScrParLst[21]		//.0 经度 度；	dddmm.mmmm之ddd
	#define	w_ScrGpsLonFen1		w_ScrParLst[22]		//.0 经度 分；		dddmm.mmmm之mm
	#define	w_ScrGpsLonFen2		w_ScrParLst[23]		//.0 经度 分；		dddmm.mmmm之mmmm

	#define	w_ScrGpsLatAllDu1		w_ScrParLst[24]		//.0 纬度 度1； 35.44
	#define	w_ScrGpsLatAllDu2		w_ScrParLst[25]		//.0 纬度 度2；	00.00****
	#define	w_ScrGpsLonAllDu1		w_ScrParLst[26]		//.0 经度 度1；
	#define	w_ScrGpsLonAllDu2		w_ScrParLst[27]		//.0 经度 度2； */

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

void Menu_END(void) // 画面 DSP设定参数1
{
	if (Lw_SavePageNo != Lw_PageNo)
	{
		Lw_SavePageNo = Lw_PageNo;
		EnterMenu_InitPar(); // 进入菜单初始化参数； ZCL 2018.5.15

		LCD12864_ClrText(); // 清屏
		// 第一行
		LCD12864_String(0x80, "青岛三利智能动力");
		// 第二行
		LCD12864_String(0x90, "    欢迎您！    ");
		// 第三行
		LCD12864_String(0x88, "                ");
		// 第四行
		LCD12864_String(0x98, "             End");
	}

	// ZCL 2017.3.21 必须加上下面一段，否则在没有参数的画面按OK键，会出现进入参数修改状态，出不来。

	ModParNum(0);
	// 显示和修改参数
	S_DisplayPar = 0; // 显示参数结束，清零S_DisplayPar，再次延时

	ReadBPDJPar2(); // ZCL 2019.4.3 在无参数画面，液晶屏也可以读DSP,ARM板参数
}

// ZCL 2019.3.7 读变频电机常规参数！
/* void ReadBPDJPar(void)
{
	if(S_RdWrNo>7)
	{
		S_RdWrNo=0;
		B_TimeReadBPDJPar=0;
	}

	//按顺序读取数据	//软件版本，A相电流，临时频率，目标频率
	if(S_RdWrNo==1)
		Read_Stm32Data( Address(&w_dspSoftVersion,3), 1 );
	else if(S_RdWrNo==2)
		Read_Stm32Data( Address(&w_dspAPhaseCurrent,3), 1 );
	else if(S_RdWrNo==3)
		Read_Stm32Data( Address(&w_dspTmpTargetHz,3), 6 );		//ZCL 2019.2.18 根据调试，正常后，改成多个
	else if(S_RdWrNo==4)
		Read_Stm32Data( Address(&w_dspCounter3,3), 5 );		//.0 DCM220 DCM100: dsp计数器，秒		//w_dspTargetHz
	else if(S_RdWrNo==5)
		Read_Stm32Data( Address(&w_dspModPar,3), 7 );		//
	else if(S_RdWrNo==6)
		Read_Stm32Data( Address(&w_dspWriteYear,3), 8 );		//


	else if(S_RdWrNo==7)
		Read_Stm32Data( Address(&Pw_dspAddSpeedTime,2), 10 );		// */
// ZCL 测试 2019.3.7 Pw_dsp读写参数的读，多个参数的读OK
/* 	if(S_RdWrNo==1)
		Read_Stm32Data( Address(&Pw_dspAddSpeedTime,2), 6 );
	else if(S_RdWrNo==2)
		Read_Stm32Data( Address(&Pw_dspDeadSupplement,2), 1 );
	else if(S_RdWrNo==3)
		Read_Stm32Data( Address(&Pw_dspOCPCurrent,2), 1 );
	else if(S_RdWrNo==4)
		Read_Stm32Data(Address(&Pw_dspOVPVoltage,2), 1 );		 */
//}

// ZCL 2019.3.7 读变频电机常规参数！
//  备注：在当前画面，后台读取常规参数，LORA远传用！
void ReadBPDJPar(void)
{
	if (S_RdWrNo > 12) // ZCL 2019.9.11 7
	{
		S_RdWrNo = 0;
		B_TimeReadBPDJPar = 0;
	}

	// 按顺序读取数据	//软件版本，A相电流，临时频率，目标频率
	if (S_RdWrNo == 1)
		Read_Stm32Data(Address(&w_dspSoftVersion, 3), 1);
	else if (S_RdWrNo == 2)
		Read_Stm32Data(Address(&w_dspNowHz, 3), 1);
	else if (S_RdWrNo == 3)
		Read_Stm32Data(Address(&w_dspAPhaseCurrent, 3), 1);
	else if (S_RdWrNo == 4)
		Read_Stm32Data(Address(&w_dspOutVoltage, 3), 1); // ZCL 2019.2.18 根据调试，正常后，改成多个
	else if (S_RdWrNo == 5)
		Read_Stm32Data(Address(&w_dspNowTemperature, 3), 1);
	else if (S_RdWrNo == 6)
		Read_Stm32Data(Address(&w_dspCounter3, 3), 1); // ZCL 2019.2.18 根据调试，正常后，改成多个
	else if (S_RdWrNo == 7)
		Read_Stm32Data(Address(&w_dspSysErrorCode1, 3), 1); // ZCL 2019.2.18 根据调试，正常后，改成多个

	// ZCL 2019.9.11 增加：进水压力，出水压力，设定压力
	else if (S_RdWrNo == 8)
		Read_Stm32Data(Address(&w_InPDec, 1), 1);
	else if (S_RdWrNo == 9)
		Read_Stm32Data(Address(&w_OutPDec, 1), 1);
	else if (S_RdWrNo == 10)
		Read_Stm32Data(Address(&w_PIDCalcP, 1), 1);
	else if (S_RdWrNo == 11)
		Read_Stm32Data(Address(&w_EquipOperateStatus, 1), 1);
	else if (S_RdWrNo == 12)
		Read_Stm32Data(Address(&w_EquipAlarmStatus, 1), 1);
	else if (S_RdWrNo == 13)
		Read_Stm32Data(Address(&Pw_ScrEquipPower, 4), 1); // SCR参数不需要读。

	/* 	else if(S_RdWrNo==4)
			Read_Stm32Data( Address(&w_dspCounter3,3), 5 );		//.0 DCM220 DCM100: dsp计数器，秒		//w_dspTargetHz
		else if(S_RdWrNo==5)
			Read_Stm32Data( Address(&w_dspModPar,3), 7 );		//
		else if(S_RdWrNo==6)
			Read_Stm32Data( Address(&w_dspWriteYear,3), 8 );		//


		else if(S_RdWrNo==7)
			Read_Stm32Data( Address(&Pw_dspAddSpeedTime,2), 10 );		// */
	// ZCL 测试 2019.3.7 Pw_dsp读写参数的读，多个参数的读OK
	/* 	if(S_RdWrNo==1)
			Read_Stm32Data( Address(&Pw_dspAddSpeedTime,2), 6 );
		else if(S_RdWrNo==2)
			Read_Stm32Data( Address(&Pw_dspDeadSupplement,2), 1 );
		else if(S_RdWrNo==3)
			Read_Stm32Data( Address(&Pw_dspOCPCurrent,2), 1 );
		else if(S_RdWrNo==4)
			Read_Stm32Data(Address(&Pw_dspOVPVoltage,2), 1 );		 */
}

// ZCL 2019.3.7 读变频电机常规参数！ ReadBPDJPar（在当前画面）
// ZCL 2019.4.3 备注：在无参数画面，液晶屏也可以读DSP,ARM板参数	 ReadBPDJPar2  LORA远传用！
void ReadBPDJPar2(void)
{
	// ZCL 2019.4.6
	if (((Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1) && B_ModPar == 0) // DCM100
		|| ((Pw_ScrMenuMode == 2) && B_PressedStopRunKey == 0))			// DCM220
	{
		if (S_RdWrNo == 0) // ZCL 2019.4.4 解决 ReadBPDJPar2 下不读取数据问题
			S_RdWrNo = 1;

		// if(B_TimeReadBPDJPar==2)		//ZCL 2019.3.19
		//{
		if (Pw_LoRaMasterSlaveSel == 0) // ZCL 2019.4.3 从机读连接的DSP,ARM参数
			ReadBPDJPar();
		//}
	}
}

void Menu_Delay(void) // 画面延时
{
	// ZCL 2018.5.16 延时加到这里
	if (S_DisplayPar == 0 && T_LcdDisplay != SClk10Ms)
	{
		T_LcdDisplay = SClk10Ms;
		C_LcdDisplay++;
		S_DisplayPar = 1;
		if (C_LcdDisplay > 15)
		{
			C_LcdDisplay = 0;
			S_DisplayPar = 2;
		}
	}
}

void FaultDisplay(u16 FaultValue, u8 Addr) // 故障显示		故障值，显示地址
{
	if (S_DisplayPar == 2) // ZCL 2018.5.19
	{
		switch (FaultValue)
		{
		case 0:
			LCD12864_String(Addr, "空记录        ");
			break; //
		case 1:
			LCD12864_String(Addr, "过流保护      ");
			break; // 1
		case 2:
			LCD12864_String(Addr, "过压保护      ");
			break; // 2
		case 3:
			LCD12864_String(Addr, "欠压保护      ");
			break; // 3
		case 4:
			LCD12864_String(Addr, "变频器报警    ");
			break; //
		case 5:
			LCD12864_String(Addr, "电源故障      ");
			break; //
		case 6:
			LCD12864_String(Addr, "液位超高      ");
			break; //
		case 7:
			LCD12864_String(Addr, "管网超压      ");
			break; //
		case 8:
			LCD12864_String(Addr, "              ");
			break; //
		case 9:
			LCD12864_String(Addr, "              ");
			break; //
		case 10:
			LCD12864_String(Addr, "压力传感器坏  ");
			break; //
		case 11:
			LCD12864_String(Addr, "缺相保护      ");
			break; // 11
		case 12:
			LCD12864_String(Addr, "IPM 故障      ");
			break; // 12
		case 13:
			LCD12864_String(Addr, "变频器复位失败");
			break; //
		case 14:
			LCD12864_String(Addr, "电流互感器故障");
			break; // 14
		case 15:
			LCD12864_String(Addr, "进水压高保护  ");
			break; //
		case 16:
			LCD12864_String(Addr, "定时交换      ");
			break; //
		case 17:
			LCD12864_String(Addr, "IPM 过热      ");
			break; // 17
		case 18:
			LCD12864_String(Addr, "              ");
			break; //
		case 19:
			LCD12864_String(Addr, "              ");
			break; //
		case 20:
			LCD12864_String(Addr, "              ");
			break; //
		case 21:
			LCD12864_String(Addr, "              ");
			break; //
		case 22:
			LCD12864_String(Addr, "              ");
			break; //
		case 23:
			LCD12864_String(Addr, "软件超压保护！");
			break; //
		case 24:
			LCD12864_String(Addr, "电机过热      ");
			break; // 24
		case 25:
			LCD12864_String(Addr, "风扇故障      ");
			break; // 25
		case 26:
			LCD12864_String(Addr, "EEPROM故障    ");
			break; // 26
		case 27:
			LCD12864_String(Addr, "DSP 处理器故障");
			break; // 27
		case 28:
			LCD12864_String(Addr, "STM 处理器故障");
			break; // 28
		case 29:
			LCD12864_String(Addr, "接地保护      ");
			break; // 29
		case 30:
			LCD12864_String(Addr, "加速中欠电压  ");
			break; // 30
		case 31:
			LCD12864_String(Addr, "减速中过电压  ");
			break; // 31
		default:
			return;
		}
	}
}

void DspFaultDisplay(u16 FaultValue, u8 Addr) // ZCL 2018.5.18
{
	if (S_DisplayPar == 2) // ZCL 2018.5.19
	{
		switch (FaultValue) // ZCL 2017.6.5 改成只有变频器故障代码
		{
		case 0:
			LCD12864_String(Addr, "无故障- 正常  ");
			break; //
		case 1:
			LCD12864_String(Addr, "过流保护      ");
			break; //
		case 2:
			LCD12864_String(Addr, "过压保护      ");
			break; //
		case 3:
			LCD12864_String(Addr, "欠压保护      ");
			break; //
		case 4:
			LCD12864_String(Addr, "缺相保护      ");
			break; //
		case 5:
			LCD12864_String(Addr, "IPM 故障      ");
			break; //
		case 6:
			LCD12864_String(Addr, "电流互感器故障");
			break; //
		case 7:
			LCD12864_String(Addr, "IPM 过热      ");
			break; //
		case 8:
			LCD12864_String(Addr, "电机过热      ");
			break; //
		case 9:
			LCD12864_String(Addr, "风扇故障      ");
			break; //
		case 10:
			LCD12864_String(Addr, "EEPROM故障    ");
			break; //
		case 11:
			LCD12864_String(Addr, "DSP 处理器故障");
			break; //
		case 12:
			LCD12864_String(Addr, "STM 处理器故障");
			break; //
		case 13:
			LCD12864_String(Addr, "接地保护      ");
			break; //
		case 14:
			LCD12864_String(Addr, "加速中欠电压  ");
			break; //
		case 15:
			LCD12864_String(Addr, "减速中过电压");
			break; //

		default:
			return;
		}
	}
}

// 显示参数.修改参数  周成磊 2013.12.6-12.8  非常重要的一个函数！！！
// p:页  x:列总坐标(h+l)(最低地址是->最高位)  *Address:要显示和修改的参数  length:参数显示长度
//	DispMod_Par(2,0x48,&Pw_YeWeiStartFirstPump,4,0);
// LCD12864_Num2(0x96, 3, MainWindow_OutPressure);
// void DispMod_Par(u8 x_pos,u8 length,u16 *Address, u8 ParType, u8 nb_modpar)
// u8 ParType  ZCL 2019.3.8  这个形参没有用到。因为显示和修改都用到的是液晶ARM的参数，通讯才需要给出参数区间。

void DispMod_Par(u8 x_pos, u8 length, u8 XiaoShuBits, u16 *Address, u8 ParType, u8 nb_modpar)
{
	u16 w; // 相对地址,RelativeAddress

	// u16 *p1;						//ZCL 观看 Pw_ScrKeyMode 地址值
	// p1=&Pw_ScrKeyMode;

	u8 nB_Sign; // ZCL 2019.10.18 符号标志

	if (ParType >= 10)
	{
		ParType = ParType - 10;
		nB_Sign = 1;
	}
	else
		nB_Sign = 0;

	// 注意：RUN . STOP 按键的操作时，B_ModPar=3， 特殊按键立即写参数！
	if (B_ModPar < 3)
	{
		w = *Address; // 值
		//
		// ZCL 2018.5.17 降低10倍
		if (Address == &Pw_Time1SetP || Address == &Pw_Time2SetP || Address == &Pw_Time3SetP || Address == &Pw_Time4SetP || Address == &Pw_HaveWaterRunP || Address == &Pw_NoWaterStopP || Address == &Pw_SmallHoldP || Address == &Pw_SetP)
			w = w / 10;

		if (!nb_modpar) // 分解万.千.百.十.个位，在下面进行显示和修改，修改完要合并
		{
			Value[4] = w / 10000;		   // 万位
			Value[3] = (w % 10000) / 1000; // 千位
			Value[2] = (w % 1000) / 100;   // 百位
			Value[1] = (w % 100) / 10;	   // 十位
			Value[0] = (w % 10);		   // 个位
		}
		// ZCL 2018.5.14 注释掉，放到下面。否则Lw_TmpModParValue里面没值，会串行赋值错误。
		/* 	else
				{
					Value[4]=Lw_TmpModParValue/10000;					//万位
					Value[3]=(Lw_TmpModParValue%10000)/1000;		//千位
					Value[2]=(Lw_TmpModParValue%1000)/100;			//百位
					Value[1]=(Lw_TmpModParValue%100)/10;				//十位
					Value[0]=(Lw_TmpModParValue%10);				 		//个位
				}	*/

		// 1.修改参数  在先  ------------------------------
		if (nb_modpar)
		{
			// ZCL 2015.9.1 刚进入修改参数状态，先把参数值送到临时变量，用临时变量来修改
			if (S_ModParStatus == 1) // 刚进入修改参数项，把参数保存到Lw_TmpModParValue，以此来修改
			{
				S_ModParStatus = 2;
				Lw_TmpModParValue = w;
				Lw_OldModParValue = w;
				//
			}

			// ZCL 2018.5.14 移到这里，在上面不对
			Value[4] = Lw_TmpModParValue / 10000;		   // 万位
			Value[3] = (Lw_TmpModParValue % 10000) / 1000; // 千位
			Value[2] = (Lw_TmpModParValue % 1000) / 100;   // 百位
			Value[1] = (Lw_TmpModParValue % 100) / 10;	   // 十位
			Value[0] = (Lw_TmpModParValue % 10);		   // 个位

			if (S_ModParStatus == 2) // 刚进入修改参数项，把参数保存到Lw_TmpModParValue，以此来修改
			{
				if (Sa_ModParKeyData == UPKEY) // 上升键 11	每次修改后，合并保存值 ZCL 2018.5.14
				{
					if (Value[ModParBitPos] < 9)
						Value[ModParBitPos]++;
					else
						Value[ModParBitPos] = 0; // 2015.9.6

					Lw_TmpModParValue = Value[4] * 10000 + Value[3] * 1000 + Value[2] * 100 + Value[1] * 10 + Value[0];
				}
				else if (Sa_ModParKeyData == DOWNKEY) // 下降键 8	每次修改后，合并保存值
				{
					if (Value[ModParBitPos] > 0)
						Value[ModParBitPos]--;
					else
						Value[ModParBitPos] = 9; // 2015.9.6
					Lw_TmpModParValue = Value[4] * 10000 + Value[3] * 1000 + Value[2] * 100 + Value[1] * 10 + Value[0];
				}

				else if (Sa_ModParKeyData == LEFTKEY) // 向左键 9  移动参数位置
				{
					if (ModParBitPos + 1 < length) // 只能移动到length规定的位置
						ModParBitPos++;
				}
				else if (Sa_ModParKeyData == RIGHTKEY) // 向右键 7  移动参数位置
				{
					if (ModParBitPos > 0) // 只要位置不是0，减1
						ModParBitPos--;
				}

				else if (Sa_ModParKeyData == ESCKEY) // ESC取消键  10
				{
					S_ModParStatus = 0; // 不修改参数，取消
					ModParNo = 0;		// 修改参数序号=0
					ModParBitPos = 0;	// 修改参数位位置=0
				}

				else if (Sa_ModParKeyData == OKKEY) // ENTER键 6
				{

					// 用指针修改参数
					Lw_TmpModParValue = Value[4] * 10000 + Value[3] * 1000 + Value[2] * 100 + Value[1] * 10 + Value[0];

					// ZCL 2019.4.2 没有参数口令，参数不改变。 w_GprsModPar 除外
					//  可以没有口令，就可以修改的参数如下：
					if (Address == &w_ModPar || Address == &w_dspModPar || Address == &w_GprsModPar || Address == &w_TouchRunStop || Address == &w_dspTouchRunStop //) ZCL 2019.9.11
																																								   // ZCL 2019.4.16 液晶屏本身的参数，不需要口令，防止板子不对时，改不过来！
						|| Address == &Pw_ScrKeyMode || Address == &Pw_ScrMenuMode																				   // ZCL 2019.4.16
						|| Address == &Pw_ScrCurrentBits																										   // ZCL 2019.4.16
						|| B_ModYW310SZM220 == 1)																												   // ZCL 2019.10.19  YW310，SZM220的 修改参数，又作为单独的参数，必须可以直接修改。
						;
					// ZCL 2019.4.2 没有参数口令，参数不改变
					else if (w_ModPar != 2000 && w_ModPar != 3000 && w_dspModPar != 2000 && w_dspModPar != 3000 && w_GprsModPar != 2000 && w_GprsModPar != 3000)
					{
						Lw_TmpModParValue = Lw_OldModParValue;
					}

					// ZCL  2018.5.14  参数值没有改变的话，就不用修改
					//  != 参数改变
					if (Lw_OldModParValue != Lw_TmpModParValue) // ZCL 2018.5.14  这个加上还不起作用，快速按OK键，还是会串行修改参数
					{
						// ZCL 2015.9.1 有此标志，串口通讯修改STM32里面的参数
						// ZCL 2019.4.6 新加内容，只有0-3才串口读取数据。 4,5类型 都是液晶屏本身参数。
						// if(ParType>3)  ZCL 2019.10.19
						if (ParType == 4 || ParType == 5) // ZCL 2019.10.19
							B_ModPar = 0;
						else if (ParType == 0 || ParType == 1 || ParType == 2 || ParType == 3 || ParType == 8) // ZCL 2019.10.19
							// ZCL 2019.4.6 上面是新加内容，只有0-3才串口读取数据。 4,5类型 都是液晶屏本身参数。
							B_ModPar = 1; //=1修改参数，不再读参数！ 但修改参数中一直读参数

						// ZCL 2018.5.17 乘以10倍
						if (Address == &Pw_Time1SetP || Address == &Pw_Time2SetP || Address == &Pw_Time3SetP || Address == &Pw_Time4SetP || Address == &Pw_HaveWaterRunP || Address == &Pw_NoWaterStopP || Address == &Pw_SmallHoldP || Address == &Pw_SetP)
							Lw_SaveSetValue = Lw_TmpModParValue * 10;
						else
							Lw_SaveSetValue = Lw_TmpModParValue;

						// ZCL 2018.8.4 调试Pw_ScrKeyMode时，需要w_dspModPar=2000或者3000；2018.8.6 取消
						/* 						if(Address==&Pw_ScrKeyMode && w_ModPar!=2000 && w_ModPar!=3000 )
													*Address=w_ScrSaveKeyMode;
												else		//正常顺序 */

						*Address = Lw_SaveSetValue;

						EnterMenu_InitPar(); // ZCL 2018.5.17 测试

						// S_RdWrNo = nb_modpar;		//! 告诉要修改哪一个参数 ZCL 2018.5.14
						S_RdWrNo = ModParNo; // ZCL 2018.5.14
						// S_DisplayPar=2;						//ZCL 2018.5.17 改变了设定，立即刷新显示

						// ZCL 2019.3.22 出现问题：修改完一个修改，按OK键进入下一下，还在最后修改位位置。解决办法：加上此句好了。
						// 又出现问题：但一会自动退出修改状态？答：因为没有连接ARM板，导致Write_Stm32Data()中超过3次清S_ModParStatus  解决办法：注释掉S_ModParStatus=0;  还注释掉：ModParNo=0; ModParBitPos=0;
						ModParBitPos = 0; // 修改参数位位置=0
					}

					//== 参数没有改变
					// ZCL 2018.5.14  这个加上还不起作用，快速按OK键，还是会串行修改参数。
					// 备注：根本原因，读前一个串口返回值，送给了当前读参数
					else
					{
						S_ModParStatus = 1; // 修改参数确认后，指向下一个参数
						ModParNo++;			// 修改参数序号++
						ModParBitPos = 0;	// 修改参数位位置=0
					}

					// ZCL 2018.5.14
					//  S_ModParStatus=1;					//修改参数确认后，指向下一个参数
					//  ModParNo++;					//修改参数序号++
					//  ModParBitPos=0;			//修改参数位位置=0
				}
			}

			// 每次按键用完，就清零，等待重新按键。 周成磊 2013.12.8
			Sa_ModParKeyData = 0;
		}

		// 2.显示参数 在后 -----------------------------------------
		// ZCL 2018.5.17
		Menu_Delay(); // 延时
		if (S_DisplayPar == 2)
		{
			if (nb_modpar) //&& S_ModParStatus>0
				w = Lw_TmpModParValue;

			if (nB_Sign == 1)
				nb_modpar = nb_modpar + 10; // ZCL 2019.10.18  +10 代表有负号！

			LCD12864_Num3(x_pos, length, XiaoShuBits, w, nb_modpar);

			// 这个延时必须加！！！ 否则显示错乱。 ZCL 2015.8.31
			// LCD_DLY_ms(1);	 //1MS就可以
			LCD12864_DelayUS(1000); // 50不行，100有一点点闪！200可以了  1000
		}
	}
}

// ZCL 2022.5.21
void DispMod_Par2(u8 x_pos, u8 length, u8 XiaoShuBits, u32 l_value, u8 ParType, u8 nb_modpar)
{
	u32 nl_i; // 相对地址,RelativeAddress

	// u16 *p1;						//ZCL 观看 Pw_ScrKeyMode 地址值
	// p1=&Pw_ScrKeyMode;

	nl_i = l_value; // 值

	// 注意：RUN . STOP 按键的操作时，B_ModPar=3， 特殊按键立即写参数！
	if (B_ModPar < 3)
	{

		// 2.显示参数 在后 -----------------------------------------
		// ZCL 2018.5.17
		Menu_Delay(); // 延时
		if (S_DisplayPar == 2)
		{

			// ZCL 2022.3.10 流量脉冲  长数据显示用。w32_PulseCouter 在这里直接赋值了！w_ScrPulseSumH w_ScrPulseSumH 没有用到！
			//..

			LCD12864_Num3(x_pos, length, XiaoShuBits, nl_i, nb_modpar);

			// 这个延时必须加！！！ 否则显示错乱。 ZCL 2015.8.31
			// LCD_DLY_ms(1);	 //1MS就可以
			LCD12864_DelayUS(1000); // 50不行，100有一点点闪！200可以了  1000
		}
	}
}

// ZCL 2019.4.5
void DispMod_Par2_String(unsigned int Address, const char *Str)
{
	// 2.显示参数 在后 -----------------------------------------
	// ZCL 2018.5.17
	Menu_Delay(); // 延时
	if (S_DisplayPar == 2)
	{
		LCD12864_String(Address, Str);

		// 这个延时必须加！！！ 否则显示错乱。 ZCL 2015.8.31
		// LCD_DLY_ms(1);	 //1MS就可以
		LCD12864_DelayUS(1000); // 50不行，100有一点点闪！200可以了  1000
	}
}

// ZCL 2019.4.5
void DispMod_Par3_Num(unsigned int Address, long Num)
{
	// 2.显示参数 在后 -----------------------------------------
	// ZCL 2018.5.17
	Menu_Delay(); // 延时
	if (S_DisplayPar == 2)
	{

		LCD12864_Num(Address, Num);

		// 这个延时必须加！！！ 否则显示错乱。 ZCL 2015.8.31
		// LCD_DLY_ms(1);	 //1MS就可以
		LCD12864_DelayUS(1000); // 50不行，100有一点点闪！200可以了  1000
	}
}

void ModParNum(u8 num) // 指定:修改参数的数量
{
	F_ModPar1 = 0;
	F_ModPar2 = 0;
	F_ModPar3 = 0;
	F_ModPar4 = 0;
	F_ModPar5 = 0;
	F_ModPar6 = 0;
	if (ModParNo > num)
	{
		ModParNo = 0;		// 参数不修改
		S_ModParStatus = 0; // 这句很重要！！保证参数修改完毕，若已经到本页最后的参数，退出修改状态
	}
	else if (ModParNo == 1)
		F_ModPar1 = 1;
	else if (ModParNo == 2)
		F_ModPar2 = 2;
	else if (ModParNo == 3)
		F_ModPar3 = 3;
	else if (ModParNo == 4)
		F_ModPar4 = 4;
	else if (ModParNo == 5)
		F_ModPar5 = 5;
	else if (ModParNo == 6)
		F_ModPar6 = 6;
}

//********************下面为按键处理****************************
// 六个按键分别为： 向上键   CANCEL
//								向左键		向右键
//								向下键		ENTER
/*********************键值扫描************************************/
void GetKey(void) // ZCL 得到按键的键值 2013.12.07
{
	// 1.键值
	Key_Data = 0;
	if (KEY1 == 0) // KEY==0按下键
		Key_Data = 1;
	else if (KEY2 == 0) //
		Key_Data = 2;
	// else if( KEY3==0 )		//ZCL 2018.11.13  COM_KEY_3线调试时割断了，用于M35 STATUS检测。这里注释掉
	// Key_Data = 3;
	// else if( KEY4==0 )		//ZCL 2018.10.19
	// Key_Data = 4;
	// else if( KEY5==0 )		//ZCL 2018.12.06
	// Key_Data = 5;
	else if (KEY6 == 0) //
		Key_Data = 6;
	else if (KEY7 == 0) //
		Key_Data = 7;
	else if (KEY8 == 0) //
		Key_Data = 8;
	else if (KEY9 == 0) //
		Key_Data = 9;
	else if (KEY10 == 0) //
		Key_Data = 10;
	else if (KEY11 == 0) //
		Key_Data = 11;

	// 2.防抖
	if (Sa_OldKeyData != Key_Data) // 按键变化，保存键值，清除计数器
	{
		Sa_OldKeyData = Key_Data;
		Lw_KEYStableCounter = 0;
	}
	else // 按键不变化－－当达到稳定数目
	{
		if (Lw_KEYStableCounter++ > 10) // 用循环次数过滤
		{
			Lw_KEYStableCounter = 0;
			if (Key_Data == 0) // 按键弹起来
			{
				Key_Pressed = 0;
				F_KeyExec = 0;
			}
			else // 按键按下		//ZCL 2018.9.29  else 不小心删掉（V138---V139-2），这里加上！
			{
				Key_Pressed = 1;
				C_LCDBackLight = 1;

				// ZCL 2018.8.3 加按键模式，对按键不同进行兼容，改变按键定义值。
				if (Pw_ScrKeyMode == 0) //=0，8个按键，正常模式
					;
				else if (Pw_ScrKeyMode == 1) //=1，7个按键
				{
					if (Key_Data == DOWNKEY)
						Key_Data = UPKEY;
					else if (Key_Data == UPKEY)
						Key_Data = DOWNKEY;
					else if (Key_Data == SETKEY)
						Key_Data = LEFTKEY;
				}
				else if (Pw_ScrKeyMode == 2) //=2，8个按键，上下键反着
				{
					if (Key_Data == DOWNKEY)
						Key_Data = UPKEY;
					else if (Key_Data == UPKEY)
						Key_Data = DOWNKEY;
				}
			}
		}
	}

	if (Pw_EquipmentType == 0) // 双驱泵
	{
		// 3.按键改变画面的处理
		if (S_ModParStatus == 0)
		{
			if (Key_Pressed == 1 && Key_Data != 0 && F_KeyExec == 0)
			{
				F_KeyExec = 1;

				// ZCL 2019.3.8 LORA从机
				if (Pw_LoRaMasterSlaveSel == 0) // 下面是原先的内容
				{

					// zcl 2014.3.26 >=3画面后，必须口令正确才可以显示 && (Lw_PageNo<4 || Pw_ModPar==10000 || Lw_PageNo>4 )
					if (Key_Data == DOWNKEY && Lw_PageNo < 999) // Lw_PageNo<11  ZCL 2018.8.28 308
					{
						Lw_PageNo++;

						if (Pw_ScrMenuMode == 0)
						{
							if (Lw_PageNo == 20 + 1) // ZCL 2018.8.21 18;  2019.8.7 19
								Lw_PageNo = 100;
							else if (Lw_PageNo == 135 + 1) // ZCL 2018.8.21 125;		2019.8.31 133
								Lw_PageNo = 199;		   // ZCL 2018.8.28 200;
							else if (Lw_PageNo == 207 + 1) // ZCL 2018.8.28 =0，控制器模式
								Lw_PageNo = 300;
							else if (Lw_PageNo == 310) // ZCL 2019.4.15 定频有重复，310跳过
								Lw_PageNo = 543;

							// ZCL 2018.4.19 跳过 7: Menu_StmReadPar8
							else if (Lw_PageNo == 7)
								Lw_PageNo = 8;
							else if (Lw_PageNo == 544) // ZCL 2019.4.9 子机跳过544页
								Lw_PageNo = 546;
							else if (Lw_PageNo == 550 + 1) // ZCL 2019.4.5
								Lw_PageNo = 999;
						}

						else if (Pw_ScrMenuMode == 1)
						{
							if (Lw_PageNo == 221 + 1) // ZCL 2019.3.19 =1，变频器模式
								Lw_PageNo = 300;
							else if (Lw_PageNo == 310 + 1)
								Lw_PageNo = 543; // ZCL 2019.3.23

							else if (Lw_PageNo == 544) // ZCL 2019.4.9 子机跳过544页
								Lw_PageNo = 546;
							else if (Lw_PageNo == 550 + 1) // ZCL 2019.4.5
								Lw_PageNo = 999;

							else if (Lw_PageNo == 210) // ZCL 2019.4.15 跳过210：Menu_StmReadPar8_NoUse()
								Lw_PageNo = 211;
						}

						else if (Pw_ScrMenuMode == 2) // ZCL 2019.3.19 =11，DCM220变频器模式
						{
							if (Lw_PageNo == 207 + 1)
								Lw_PageNo = 250;
							else if (Lw_PageNo == 261 + 1)
								Lw_PageNo = 300;
							else if (Lw_PageNo == 310 + 1)
								Lw_PageNo = 330;
							else if (Lw_PageNo == 331 + 1) // ZCL 2019.3.23
								Lw_PageNo = 543;
							//

							else if (Lw_PageNo == 544) // ZCL 2019.4.9 子机跳过544页
								Lw_PageNo = 546;
							else if (Lw_PageNo == 550 + 1) // ZCL 2019.4.5
								Lw_PageNo = 999;
						}
					}

					else if (Key_Data == UPKEY && Lw_PageNo > 0)
					{
						Lw_PageNo--;

						if (Pw_ScrMenuMode == 0) // ZCL 2018.8.28 =0，控制器模式
						{
							if (Lw_PageNo == 543 - 1) // ZCL 2019.4.2  544
								Lw_PageNo = 310 - 1;  // ZCL 2019.4.15 定频有重复，310跳过
							else if (Lw_PageNo == 300 - 1)
								Lw_PageNo = 207;
							else if (Lw_PageNo == 199 - 1) // ZCL 2018.8.28 =0，控制器模式
								Lw_PageNo = 135;		   // ZCL 2019.8.31 133
							else if (Lw_PageNo == 100 - 1)
								Lw_PageNo = 20; // ZCL 2019.8.7

							// ZCL 2018.4.19 可以跳过 7: Menu_StmReadPar8
							// ZCL 2019.4.11 备注：跳过的页，直接写数字，不用+1，-1（第7页，第544页跳过）
							else if (Lw_PageNo == 7)
								Lw_PageNo = 6;
							else if (Lw_PageNo == 544) // ZCL 2019.4.9 子机跳过544页
								Lw_PageNo = 543;
							else if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
								Lw_PageNo = 551 - 1;
						}

						else if (Pw_ScrMenuMode == 1) // ZCL 2019.3.19 =1，变频器模式
						{
							if (Lw_PageNo == 543 - 1) // ZCL 2019.4.2  544
								Lw_PageNo = 310;
							else if (Lw_PageNo == 300 - 1)
								Lw_PageNo = 221;
							else if (Lw_PageNo == 200 - 1)
								Lw_PageNo = 200;

							else if (Lw_PageNo == 544) // ZCL 2019.4.9 子机跳过544页
								Lw_PageNo = 543;
							else if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
								Lw_PageNo = 551 - 1;
							else if (Lw_PageNo == 210) // ZCL 2019.4.15 跳过210：Menu_StmReadPar8_NoUse()
								Lw_PageNo = 209;
						}

						else if (Pw_ScrMenuMode == 2) // ZCL 2019.3.19 =11，DCM220变频器模式
						{
							if (Lw_PageNo == 543 - 1) // ZCL 2019.3.23
								Lw_PageNo = 331;
							else if (Lw_PageNo == 330 - 1)
								Lw_PageNo = 310;
							else if (Lw_PageNo == 300 - 1)
								Lw_PageNo = 261;
							else if (Lw_PageNo == 250 - 1)
								Lw_PageNo = 207;
							else if (Lw_PageNo == 200 - 1)
								Lw_PageNo = 200;

							else if (Lw_PageNo == 544) // ZCL 2019.4.9 子机跳过544页
								Lw_PageNo = 543;
							else if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
								Lw_PageNo = 551 - 1;
						}
					}

					else if (Key_Data == ESCKEY) // CANCEL键，返回
						Lw_PageNo = 0;
					else if (Key_Data == OKKEY) // ENTER键  进入参数修改状态
					{
						S_ModParStatus = 1; // 进入参数修改状态
						ModParNo++;			// 修改参数序号++
					}
					else if (Key_Data == RIGHTKEY) // 右键
					{
						B_RightKeyCount++;
					}
					else if (Key_Data == LEFTKEY) // 左键
					{
						B_LeftKeyCount++;
					}
				}

				// ZCL 2019.3.8 新加 LORA主机
				else if (Pw_LoRaMasterSlaveSel == 1) // 下面是原先的内容
				{
					if (Key_Data == UPKEY && Lw_PageNo > 500) // Lw_PageNo>0  ZCL 2016.5.27 修改
					{
						// if(Lw_PageNo==300)
						// Lw_PageNo=218;								//2016.5.27 209   			//ZCL 2016.10.6 208改成209
						/* 				else if(Lw_PageNo==200)			//2016.5.23 ZCL 取消
											Lw_PageNo=123;
										else if(Lw_PageNo==100)
											Lw_PageNo=17;	 */
						//
						// else
						Lw_PageNo--;

						// ZCL 2018.4.19 可以跳过 Menu_DspReadPar5 =205
						//  if(Lw_PageNo==205)
						//  Lw_PageNo=204;

						if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
							Lw_PageNo = 565;

						if (Lw_PageNo == 543 - 1) // ZCL 2019.9.12
							Lw_PageNo = 525;	  // ZCL 2019.10.18  520		540
					}

					// zcl 2014.3.26 >=3画面后，必须口令正确才可以显示 && (Lw_PageNo<4 || Pw_ModPar==10000 || Lw_PageNo>4 )
					else if (Key_Data == DOWNKEY && Lw_PageNo < 999) // Lw_PageNo<11
					{
						// if(Lw_PageNo==218)						//2016.5.27 209					//ZCL 2016.10.6 208改成209
						// Lw_PageNo=300;
						/* 				else if(Lw_PageNo==123)		//2016.5.23 ZCL 取消
											Lw_PageNo=200;
										else if(Lw_PageNo==17)
											Lw_PageNo=100;	 */
						//
						// else
						Lw_PageNo++;

						// ZCL 2018.4.19 可以跳过 Menu_DspReadPar5 =205
						//  if(Lw_PageNo==205)
						//  Lw_PageNo=206;

						if (Lw_PageNo == 565 + 1) // ZCL 2019.4.5
							Lw_PageNo = 999;

						if (Lw_PageNo == 525 + 1) // ZCL 2019.9.12 520  540
							Lw_PageNo = 543;
					}
					else if (Key_Data == ESCKEY) // CANCEL键，返回
						// Lw_PageNo=0;			//2016.5.23 ZCL 修改
						Lw_PageNo = 500;		// 2016.5.23 ZCL 修改
					else if (Key_Data == OKKEY) // ENTER键
					{
						S_ModParStatus = 1;
						ModParNo++;
						// w_KeyPressDelay=1805;
					}
					else if (Key_Data == RIGHTKEY) // 右键
					{
						B_RightKeyCount++;
					}
					else if (Key_Data == LEFTKEY) // 左键
					{
						B_LeftKeyCount++;
					}
				}
			}
		}
		// 4.按键修改参数的处理
		else
		{
			if (Key_Pressed == 1 && Key_Data != 0 && F_KeyExec == 0)
			{
				F_KeyExec = 1;
				Sa_ModParKeyData = Key_Data;
			}
		}

		// 5. RUN . STOP 按键的操作。 特殊按键立即写参数 ZCL  2015.9.5  2016.12.6
		// ZCL 2018.5.19 这一段移植 V145-3。 原因：DSP串口不允许打断，不像ARM那样。有启停按键指令也得等通讯结束再进行。
		if (Key_Pressed == 1 && Key_Data == STOPKEY)
		{
			if (Pw_ScrMenuMode == 2)
				w_dspTouchRunStop = 1;							 // ZCL 2016.12.5 改成DSP中的参数
			else if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1) // ZCL 2019.4.6 加DCM100中的按键启停参数
				w_TouchRunStop = 1;
			// B_ModPar=3;
			B_PressedStopRunKey = 1;
		}
		else if (Key_Pressed == 1 && Key_Data == RUNKEY)
		{
			if (Pw_ScrMenuMode == 2)
				w_dspTouchRunStop = 0;							 // ZCL 2016.12.5 改成DSP中的参数
			else if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1) // ZCL 2019.4.6 加DCM100中的按键启停参数
				w_TouchRunStop = 0;
			// B_ModPar=3;
			B_PressedStopRunKey = 1;
		}

		// 特殊按键立即写参数 2015.9.5
		if (B_ModPar == 100)
		{
			/* 		B_ModPar=4;
					S_Com1Send=0;
					C_Com1Send=0;
					S_Com1SendNoNum=0;

				}
				if(B_ModPar==4)
				{ */
			if (Pw_ScrMenuMode == 2)
				Write_Stm32Data(Address(&w_dspTouchRunStop, 3), w_dspTouchRunStop); // ZCL 2016.12.5 改成DSP中的参数
			else if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1)					// ZCL 2019.4.6 加DCM100中的按键启停参数
				Write_Stm32Data(Address(&w_TouchRunStop, 1), w_TouchRunStop);
		}

		// 6.快捷键 ZCL 2015.9.6  快按2次向右键 或者 向左键；只按一次超时清除快捷键计数器
		if (Pw_LoRaMasterSlaveSel == 0) // LORA从机
		{
			if (Pw_ScrMenuMode == 0) //=0 DCM100 控制器模式	 2019.3.19
			{
				// 向右键
				if (Lw_PageNo == 0 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 5;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.3.19
				else if (Lw_PageNo == 5 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 10;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 10 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 17;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 17 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 101;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 101 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 103;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 103 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 108;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 108 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 113;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 113 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 117;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 117 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 121;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 121 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 125;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 125 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 129;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				else if (Lw_PageNo == 129 && B_RightKeyCount == 2) // ZCL 2018.8.21
				{
					Lw_PageNo = 135;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.8.31 133
				else if (Lw_PageNo == 135 && B_RightKeyCount == 2) // ZCL 2018.8.21 2019.8.31 133
				{
					Lw_PageNo = 200;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				else if (Lw_PageNo == 200 && B_RightKeyCount == 2) // ZCL 2018.8.28 =0控制器
				{
					Lw_PageNo = 205;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 205 && B_RightKeyCount == 2) // ZCL 2018.8.28 =0控制器
				{
					Lw_PageNo = 301;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28
				else if (Lw_PageNo == 305 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 543;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 543 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 548;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 548 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				// 向左键
				else if (Lw_PageNo == 0 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 200;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 200 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 202;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 202 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 205;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 205 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 305 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 543;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 543 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 548;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 548 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}

				//
				else if (Lw_PageNo == 101 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				} // ZCL 2018.8.29 变频加速时间画面
			}

			else if (Pw_ScrMenuMode == 1) //=1 DCM100 变频器模式	2019.3.19
			{
				// 向右键
				if (Lw_PageNo == 200 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 208;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 208 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 300;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 300 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 301 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 305 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 543;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 543 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 548;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 548 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				// ZCL 2018.8.28 变频器模式
				// 向左键
				else if (Lw_PageNo == 200 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 213;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 213 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 220;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 220 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 221;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 221 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 305 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 543;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 543 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 548;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 548 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
			}

			else if (Pw_ScrMenuMode == 2) //=11 DCM220 变频器模式
			{
				// 向右键
				if (Lw_PageNo == 200 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 300;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 300 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 301 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 305 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308；2019.9.11 310
				else if (Lw_PageNo == 309 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 310;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 310 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 330;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				// ZCL 2019.3.23
				else if (Lw_PageNo == 330 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 543;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 543 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 548;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 548 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				// ZCL 2018.8.28 变频器模式
				// 向左键
				else if (Lw_PageNo == 200 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 250;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 250 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 255;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 255 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 261;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}

				// ZCL 2019.4.16 增加个“修改参数”，7个键的液晶屏，只有左键
				else if (Lw_PageNo == 261 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 300;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}

				else if (Lw_PageNo == 300 && B_LeftKeyCount == 2) // 261
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 305 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 309 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 310;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.10
				else if (Lw_PageNo == 310 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 330;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				// ZCL 2019.3.23
				else if (Lw_PageNo == 330 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 543;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 543 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 548;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 548 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
			}
		}

		// ZCL 2019.3.14 增加 LORA主机的时候，快捷键
		else if (Pw_LoRaMasterSlaveSel == 1) // LORA主机
		{
			// 向右键 ZCL 2017.3.17
			if (Lw_PageNo == 500 && B_RightKeyCount == 2) // ZCL 2016.10.6  初始画面按快捷键跳到 变频设定参数
			{
				Lw_PageNo = 543;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 543 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 544;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 544 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 548;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 548 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 551;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}

			// ZCL 2019.10.19 YW310 SZM220 画面的快捷键
			else if (Lw_PageNo == 551 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 555;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 555 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 559;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 559 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 565;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}

			else if (Lw_PageNo == 565 && B_RightKeyCount == 2) // ZCL 2019.10.19 551
			{
				Lw_PageNo = 999;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}

			// 向左键 ZCL 2017.3.17
			else if (Lw_PageNo == 500 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 501;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 501 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 506;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 506 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 511;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 511 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 516;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}

			// ZCL 2019.10.18 5-8号电机的快捷键
			else if (Lw_PageNo == 516 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 521;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 521 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 543;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}

			else if (Lw_PageNo == 543 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 544;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 544 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 548;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 548 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 551;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}

			// ZCL 2019.10.19 YW310 SZM220 画面的快捷键
			else if (Lw_PageNo == 551 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 555;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 555 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 559;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 559 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 565;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}

			else if (Lw_PageNo == 565 && B_LeftKeyCount == 2) // ZCL 2019.10.19 551
			{
				Lw_PageNo = 999;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
		}
	}
	else // 变频电机
	{

		// 3.按键改变画面的处理
		if (S_ModParStatus == 0)
		{
			if (Key_Pressed == 1 && Key_Data != 0 && F_KeyExec == 0)
			{
				F_KeyExec = 1;

				// ZCL 2019.3.8 LORA从机
				if (Pw_LoRaMasterSlaveSel == 0) // 下面是原先的内容
				{

					// zcl 2014.3.26 >=3画面后，必须口令正确才可以显示 && (Lw_PageNo<4 || Pw_ModPar==10000 || Lw_PageNo>4 )
					if (Key_Data == DOWNKEY && Lw_PageNo < 999) // Lw_PageNo<11  ZCL 2018.8.28 308
					{
						Lw_PageNo++;

						if (Pw_ScrMenuMode == 0)
						{
							if (Lw_PageNo == 20 + 1) // ZCL 2018.8.21 18;  2019.8.7 19
								Lw_PageNo = 100;
							else if (Lw_PageNo == 135 + 1) // ZCL 2018.8.21 125;		2019.8.31 133
								Lw_PageNo = 199;		   // ZCL 2018.8.28 200;
							else if (Lw_PageNo == 207 + 1) // ZCL 2018.8.28 =0，控制器模式
								Lw_PageNo = 300;
							else if (Lw_PageNo == 310) // ZCL 2019.4.15 定频有重复，310跳过
								Lw_PageNo = 523;

							// ZCL 2018.4.19 跳过 7: Menu_StmReadPar8
							else if (Lw_PageNo == 7)
								Lw_PageNo = 8;
							else if (Lw_PageNo == 524) // ZCL 2019.4.9 子机跳过524页
								Lw_PageNo = 525;
							else if (Lw_PageNo == 529 + 1) // ZCL 2019.4.5
								Lw_PageNo = 999;
						}

						else if (Pw_ScrMenuMode == 1)
						{
							if (Lw_PageNo == 221 + 1) // ZCL 2019.3.19 =1，变频器模式
								Lw_PageNo = 300;
							else if (Lw_PageNo == 310 + 1)
								Lw_PageNo = 523; // ZCL 2019.3.23

							else if (Lw_PageNo == 524) // ZCL 2019.4.9 子机跳过524页
								Lw_PageNo = 525;
							else if (Lw_PageNo == 529 + 1) // ZCL 2019.4.5
								Lw_PageNo = 999;

							else if (Lw_PageNo == 210) // ZCL 2019.4.15 跳过210：Menu_StmReadPar8_NoUse()
								Lw_PageNo = 211;
						}

						else if (Pw_ScrMenuMode == 2) // ZCL 2019.3.19 =11，DCM220变频器模式
						{
							if (Lw_PageNo == 207 + 1)
								Lw_PageNo = 250;
							else if (Lw_PageNo == 261 + 1)
								Lw_PageNo = 300;
							else if (Lw_PageNo == 310 + 1)
								Lw_PageNo = 330;
							else if (Lw_PageNo == 332 + 1) // ZCL 2020.4.8
								Lw_PageNo = 523;
							//

							else if (Lw_PageNo == 524) // ZCL 2019.4.9 子机跳过524页
								Lw_PageNo = 525;
							else if (Lw_PageNo == 529 + 1) // ZCL 2019.4.5
								Lw_PageNo = 999;
						}
					}

					else if (Key_Data == UPKEY && Lw_PageNo > 0)
					{
						Lw_PageNo--;

						if (Pw_ScrMenuMode == 0) // ZCL 2018.8.28 =0，控制器模式
						{
							if (Lw_PageNo == 523 - 1) // ZCL 2019.4.2  524
								Lw_PageNo = 310 - 1;  // ZCL 2019.4.15 定频有重复，310跳过
							else if (Lw_PageNo == 300 - 1)
								Lw_PageNo = 207;
							else if (Lw_PageNo == 199 - 1) // ZCL 2018.8.28 =0，控制器模式
								Lw_PageNo = 135;		   // ZCL 2019.8.31 133
							else if (Lw_PageNo == 100 - 1)
								Lw_PageNo = 20; // ZCL 2019.8.7

							// ZCL 2018.4.19 可以跳过 7: Menu_StmReadPar8
							// ZCL 2019.4.11 备注：跳过的页，直接写数字，不用+1，-1（第7页，第524页跳过）
							else if (Lw_PageNo == 7)
								Lw_PageNo = 6;
							else if (Lw_PageNo == 524) // ZCL 2019.4.9 子机跳过524页
								Lw_PageNo = 523;
							else if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
								Lw_PageNo = 530 - 1;
						}

						else if (Pw_ScrMenuMode == 1) // ZCL 2019.3.19 =1，变频器模式
						{
							if (Lw_PageNo == 523 - 1) // ZCL 2019.4.2  524
								Lw_PageNo = 310;
							else if (Lw_PageNo == 300 - 1)
								Lw_PageNo = 221;
							else if (Lw_PageNo == 200 - 1)
								Lw_PageNo = 200;

							else if (Lw_PageNo == 524) // ZCL 2019.4.9 子机跳过524页
								Lw_PageNo = 523;
							else if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
								Lw_PageNo = 530 - 1;
							else if (Lw_PageNo == 210) // ZCL 2019.4.15 跳过210：Menu_StmReadPar8_NoUse()
								Lw_PageNo = 209;
						}

						else if (Pw_ScrMenuMode == 2) // ZCL 2019.3.19 =11，DCM220变频器模式
						{
							if (Lw_PageNo == 523 - 1) // ZCL 2019.3.23
								Lw_PageNo = 332;
							else if (Lw_PageNo == 330 - 1)
								Lw_PageNo = 310;
							else if (Lw_PageNo == 300 - 1)
								Lw_PageNo = 261;
							else if (Lw_PageNo == 250 - 1)
								Lw_PageNo = 207;
							else if (Lw_PageNo == 200 - 1)
								Lw_PageNo = 200;

							else if (Lw_PageNo == 524) // ZCL 2019.4.9 子机跳过524页
								Lw_PageNo = 523;
							else if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
								Lw_PageNo = 530 - 1;
						}
					}

					else if (Key_Data == ESCKEY) // CANCEL键，返回
						Lw_PageNo = 0;
					else if (Key_Data == OKKEY) // ENTER键  进入参数修改状态
					{
						S_ModParStatus = 1; // 进入参数修改状态
						ModParNo++;			// 修改参数序号++
					}
					else if (Key_Data == RIGHTKEY) // 右键
					{
						B_RightKeyCount++;
					}
					else if (Key_Data == LEFTKEY) // 左键
					{
						B_LeftKeyCount++;
					}
				}

				// ZCL 2019.3.8 新加 LORA主机
				else if (Pw_LoRaMasterSlaveSel == 1) // 下面是原先的内容
				{
					if (Key_Data == UPKEY && Lw_PageNo > 500) // Lw_PageNo>0  ZCL 2016.5.27 修改
					{
						Lw_PageNo--;

						if (Lw_PageNo == 999 - 1) // ZCL 2019.4.5
							Lw_PageNo = 534;

						if (Lw_PageNo == 523 - 1) // ZCL 2019.9.12
							Lw_PageNo = 520;
					}

					// zcl 2014.3.26 >=3画面后，必须口令正确才可以显示 && (Lw_PageNo<4 || Pw_ModPar==10000 || Lw_PageNo>4 )
					else if (Key_Data == DOWNKEY && Lw_PageNo < 999) // Lw_PageNo<11
					{
						Lw_PageNo++;

						if (Lw_PageNo == 534 + 1) // ZCL 2019.4.5
							Lw_PageNo = 999;

						if (Lw_PageNo == 520 + 1) // ZCL 2019.9.12
							Lw_PageNo = 523;
					}
					else if (Key_Data == ESCKEY) // CANCEL键，返回
						Lw_PageNo = 500;		 // 2016.5.23 ZCL 修改
					else if (Key_Data == OKKEY)	 // ENTER键
					{
						S_ModParStatus = 1;
						ModParNo++;
						// w_KeyPressDelay=1805;
					}
					else if (Key_Data == RIGHTKEY) // 右键
					{
						B_RightKeyCount++;
					}
					else if (Key_Data == LEFTKEY) // 左键
					{
						B_LeftKeyCount++;
					}
				}
			}
		}
		// 4.按键修改参数的处理
		else
		{
			if (Key_Pressed == 1 && Key_Data != 0 && F_KeyExec == 0)
			{
				F_KeyExec = 1;
				Sa_ModParKeyData = Key_Data;
			}
		}

		// 5. RUN . STOP 按键的操作。 特殊按键立即写参数 ZCL  2015.9.5  2016.12.6
		// ZCL 2018.5.19 这一段移植 V145-3。 原因：DSP串口不允许打断，不像ARM那样。有启停按键指令也得等通讯结束再进行。
		if (Key_Pressed == 1 && Key_Data == STOPKEY)
		{
			if (Pw_ScrMenuMode == 2)
				w_dspTouchRunStop = 1;							 // ZCL 2016.12.5 改成DSP中的参数
			else if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1) // ZCL 2019.4.6 加DCM100中的按键启停参数
				w_TouchRunStop = 1;
			// B_ModPar=3;
			B_PressedStopRunKey = 1;
		}
		else if (Key_Pressed == 1 && Key_Data == RUNKEY)
		{
			if (Pw_ScrMenuMode == 2)
				w_dspTouchRunStop = 0;							 // ZCL 2016.12.5 改成DSP中的参数
			else if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1) // ZCL 2019.4.6 加DCM100中的按键启停参数
				w_TouchRunStop = 0;
			// B_ModPar=3;
			B_PressedStopRunKey = 1;
		}

		// 特殊按键立即写参数 2015.9.5
		if (B_ModPar == 100)
		{
			/* 		B_ModPar=4;
					S_Com1Send=0;
					C_Com1Send=0;
					S_Com1SendNoNum=0;

				}
				if(B_ModPar==4)
				{ */
			if (Pw_ScrMenuMode == 2)
				Write_Stm32Data(Address(&w_dspTouchRunStop, 3), w_dspTouchRunStop); // ZCL 2016.12.5 改成DSP中的参数
			else if (Pw_ScrMenuMode == 0 || Pw_ScrMenuMode == 1)					// ZCL 2019.4.6 加DCM100中的按键启停参数
				Write_Stm32Data(Address(&w_TouchRunStop, 1), w_TouchRunStop);
		}

		// 6.快捷键 ZCL 2015.9.6  快按2次向右键 或者 向左键；只按一次超时清除快捷键计数器
		if (Pw_LoRaMasterSlaveSel == 0) // LORA从机
		{
			if (Pw_ScrMenuMode == 0) //=0 DCM100 控制器模式	 2019.3.19
			{
				// 向右键
				if (Lw_PageNo == 0 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 5;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.3.19
				else if (Lw_PageNo == 5 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 10;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 10 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 17;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 17 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 101;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 101 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 103;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 103 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 108;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 108 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 113;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 113 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 117;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 117 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 121;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 121 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 125;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 125 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 129;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				else if (Lw_PageNo == 129 && B_RightKeyCount == 2) // ZCL 2018.8.21
				{
					Lw_PageNo = 135;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.8.31 133
				else if (Lw_PageNo == 135 && B_RightKeyCount == 2) // ZCL 2018.8.21 2019.8.31 133
				{
					Lw_PageNo = 200;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				else if (Lw_PageNo == 200 && B_RightKeyCount == 2) // ZCL 2018.8.28 =0控制器
				{
					Lw_PageNo = 205;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 205 && B_RightKeyCount == 2) // ZCL 2018.8.28 =0控制器
				{
					Lw_PageNo = 301;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28
				else if (Lw_PageNo == 305 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 523;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 523 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 527;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 527 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				// 向左键
				else if (Lw_PageNo == 0 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 200;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 200 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 202;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 202 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 205;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 205 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 305 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 523;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 523 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 527;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 527 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}

				//
				else if (Lw_PageNo == 101 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				} // ZCL 2018.8.29 变频加速时间画面
			}

			else if (Pw_ScrMenuMode == 1) //=1 DCM100 变频器模式	2019.3.19
			{
				// 向右键
				if (Lw_PageNo == 200 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 208;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 208 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 300;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 300 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 301 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 305 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 523;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 523 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 527;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 527 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				// ZCL 2018.8.28 变频器模式
				// 向左键
				else if (Lw_PageNo == 200 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 213;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 213 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 220;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 220 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 221;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 221 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 305 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 309 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 523;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 523 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 527;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 527 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
			}

			else if (Pw_ScrMenuMode == 2) //=11 DCM220 变频器模式
			{
				// 向右键
				if (Lw_PageNo == 200 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 300;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 300 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 301;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 301 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 305 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308；2019.9.11 310
				else if (Lw_PageNo == 309 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 310;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.11
				else if (Lw_PageNo == 310 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 330;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				// ZCL 2019.3.23
				else if (Lw_PageNo == 330 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 523;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 523 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 527;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}
				else if (Lw_PageNo == 527 && B_RightKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				}

				// ZCL 2018.8.28 变频器模式
				// 向左键
				else if (Lw_PageNo == 200 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 207;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 207 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 250;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 250 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 255;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 255 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 261;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}

				// ZCL 2019.4.16 增加个“修改参数”，7个键的液晶屏，只有左键
				else if (Lw_PageNo == 261 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 300;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}

				else if (Lw_PageNo == 300 && B_LeftKeyCount == 2) // 261
				{
					Lw_PageNo = 301;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 301 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 305;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 305 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 309;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				else if (Lw_PageNo == 309 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 310;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				} // ZCL 2019.4.15 310
				// ZCL 2019.4.15 2019.9.10
				else if (Lw_PageNo == 310 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 330;
					B_RightKeyCount = 0;
					B_LeftKeyCount = 0;
				} // ZCL 2018.8.28 308
				// ZCL 2019.3.23
				else if (Lw_PageNo == 330 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 523;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 523 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 527;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
				else if (Lw_PageNo == 527 && B_LeftKeyCount == 2)
				{
					Lw_PageNo = 999;
					B_LeftKeyCount = 0;
					B_RightKeyCount = 0;
				}
			}
		}

		// ZCL 2019.3.14 增加 LORA主机的时候，快捷键
		else if (Pw_LoRaMasterSlaveSel == 1) // LORA主机
		{
			// 向右键 ZCL 2017.3.17
			if (Lw_PageNo == 500 && B_RightKeyCount == 2) // ZCL 2016.10.6  初始画面按快捷键跳到 变频设定参数
			{
				Lw_PageNo = 523;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 523 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 524;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 524 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 527;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 527 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 531;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
			else if (Lw_PageNo == 531 && B_RightKeyCount == 2)
			{
				Lw_PageNo = 999;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}

			// 向左键 ZCL 2017.3.17
			else if (Lw_PageNo == 500 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 501;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 501 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 506;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 506 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 511;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 511 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 516;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 516 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 523;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 523 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 524;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 524 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 527;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 527 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 531;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
			else if (Lw_PageNo == 531 && B_LeftKeyCount == 2)
			{
				Lw_PageNo = 999;
				B_LeftKeyCount = 0;
				B_RightKeyCount = 0;
			}
		}
	}

	// 只按一次超时清除快捷键计数器
	if (T_KeyPressOverTime != SClk1Ms)
	{
		T_KeyPressOverTime = SClk1Ms;
		if (B_RightKeyCount > 0 || B_LeftKeyCount > 0)
		{
			C_KeyPressOverTime++;
			if (C_KeyPressOverTime > Pw_FastKeyBetweenMS)
			{
				C_KeyPressOverTime = 0;
				B_RightKeyCount = 0;
				B_LeftKeyCount = 0;
			}
		}
		else if (C_KeyPressOverTime > 0)
			C_KeyPressOverTime--;
	}

	// ZCL 2015.9.17 定时刷新页面，防止出现乱码
	// ZCL 2018.5.16 加 && S_ModParStatus==0  在修改参数时，不刷新页面，防止修改状态下“>” 可以切换页面
	if (Lw_SavePageNo == Lw_PageNo && S_ModParStatus == 0)
	{
		if (T_TimeUpdatePage != SClk10Ms)
		{
			T_TimeUpdatePage = SClk10Ms; //
			C_TimeUpdatePage++;
			if (C_TimeUpdatePage > 1500) // 15秒
			{
				T_TimeUpdatePage = 100;
				C_TimeUpdatePage = 0;
				Lw_SavePageNo = 1000; // ZCL 2018.5.16 测试，刷新页面要及时清除串口接收计数器，否则偶尔参数串行
			}
		}
	}
	else
	{
		C_TimeUpdatePage = 0;
	}

	// ZCL 2019.3.8  =0 从机需要读RS232常规变频参数
	//=2 下一步再补=2 地上泵房，当主机，还当从机1 ！
	if (Pw_LoRaMasterSlaveSel == 0) //=0从机
	{
		// ZCL 2019.2.18 每3S，读取变频电机常规参数1次
		if (B_TimeReadBPDJPar == 0) //|| B_TimeReadBPDJPar==2
		{
			if (T_TimeReadBPDJPar != SClk10Ms)
			{
				T_TimeReadBPDJPar = SClk10Ms;
				C_TimeReadBPDJPar++;
				if (C_TimeReadBPDJPar > 300)
				{
					C_TimeReadBPDJPar = 0;

					B_TimeReadBPDJPar = 1;
				}
			}
		}
	}
	else
	{
		C_TimeReadBPDJPar = 0;
		B_TimeReadBPDJPar = 0;
	}
}

/*********************画面菜单改变********************************/
void Menu_Change(void) // 根据页序号，显示相应的画面
{
	if (Lw_PageNo <= 200 && (Pw_ScrMenuMode == 1 || Pw_ScrMenuMode == 2)) // ZCL 2018.8.28 =0，变频器模式
		Lw_PageNo = 200;												  // ZCL 2018.8.28 变频器模式下，最低画面号200

	// 1. 注意修改GetKey()中画面切换的键值 ZCL 2015.9.6
	// 2. 注意时间画面的键值定义，TIMEPAGE
	switch (Lw_PageNo)
	{
	// 0. ARM读参数
	case 0:
		Menu_StmReadPar1();
		break; //
	case 1:
		Menu_StmReadPar2();
		break; //
	case 2:
		Menu_StmReadPar3();
		break; //
	case 3:
		Menu_StmReadPar4();
		break; //
	case 4:
		Menu_StmReadPar5();
		break; //
	case 5:
		Menu_StmReadPar6();
		break; //	供水版本
	case 6:
		Menu_StmReadPar7();
		break; //
	case 7:
		Menu_StmReadPar8_NoUse();
		break; // ZCL 2018.4.19 暂时不显示此画面
	case 8:
		Menu_StmReadPar9();
		break; //
	case 9:
		Menu_StmReadPar10();
		break; //
	case 10:
		Menu_StmReadPar11();
		break; //
	case 11:
		Menu_StmReadPar12();
		break; //
	case 12:
		Menu_StmReadPar13();
		break; //
	case 13:
		Menu_StmReadPar14();
		break; //
	case 14:
		Menu_StmReadPar15();
		break; //
	case 15:
		Menu_StmReadPar16();
		break; //
	case 16:
		Menu_StmReadPar17();
		break; //
	case 17:
		Menu_StmReadPar18();
		break; //
	case 18:
		Menu_StmReadPar19();
		break; // ZCL 2017.9.12 加入AI1-AI4读值，方便调试。
	case 19:
		Menu_StmReadPar20();
		break; // ZCL 2019.8.7	DI状态，DO状态
	case 20:
		Menu_StmReadPar21();
		break; // ZCL 2018.8.21	加电动阀状态

	// 1. ARM写参数
	case 100:
		Menu_StmSetPar1();
		break; //
	case 101:
		Menu_StmSetPar2();
		break; // 修改参数
	case 102:
		Menu_StmSetPar3();
		break; //
	case 103:
		Menu_StmSetPar4();
		break; //
	case 104:
		Menu_StmSetPar5();
		break; //
	case 105:
		Menu_StmSetPar6();
		break; //
	case 106:
		Menu_StmSetPar7();
		break; //
	case 107:
		Menu_StmSetPar8();
		break; //
	case 108:
		Menu_StmSetPar9();
		break; //
	case 109:
		Menu_StmSetPar10();
		break; //
	case 110:
		Menu_StmSetPar11();
		break; //
	case 111:
		Menu_StmSetPar12();
		break; //
	case 112:
		Menu_StmSetPar13();
		break; //
	case 113:
		Menu_StmSetPar14();
		break; //
	case 114:
		Menu_StmSetPar15();
		break; //
	case 115:
		Menu_StmSetPar16();
		break; //
	case 116:
		Menu_StmSetPar17();
		break; //
	case 117:
		Menu_StmSetPar18();
		break; //
	case 118:
		Menu_StmSetPar19();
		break; //
	case 119:
		Menu_StmSetPar20();
		break; //
	case 120:
		Menu_StmSetPar21();
		break; //
	case 121:
		Menu_StmSetPar22();
		break; //
	case 122:
		Menu_StmSetPar23();
		break; //
	case 123:
		Menu_StmSetPar24();
		break; //
	case 124:
		Menu_StmSetPar25();
		break; //
	case 125:
		Menu_StmSetPar26();
		break; //

	case 126:
		Menu_StmSetPar27();
		break; //	ZCL 2018.8.21	加双水源和电动阀参数
	case 127:
		Menu_StmSetPar28();
		break; //
	case 128:
		Menu_StmSetPar29();
		break; //
	case 129:
		Menu_StmSetPar30();
		break; //
	case 130:
		Menu_StmSetPar31();
		break; //
	case 131:
		Menu_StmSetPar32();
		break; //
	case 132:
		Menu_StmSetPar33();
		break; //	ZCL 2018.8.21	加双水源和电动阀参数
	case 133:
		Menu_StmSetPar34();
		break; //	ZCL 2018.8.22	加双水源和电动阀参数

	case 134:
		Menu_StmSetPar35();
		break; //	ZCL 2019.8.30 特殊定时停机
	case 135:
		Menu_StmSetPar36();
		break; //	ZCL 2019.8.31 设备类型、供水变频模式

	// 2. DSP读参数
	case 199:
		Menu_DspReadPar0();
		break; //	ZCL 2018.8.28 控制器模式时使用，变频模式时不需要“变频观看参数”

	case 200:
		Menu_DspReadPar1();
		break; //
	case 201:
		Menu_DspReadPar2();
		break; //
	case 202:
		Menu_DspReadPar3();
		break; //
	case 203:
		Menu_DspReadPar4();
		break; //
	case 204:
		Menu_DspReadPar5();
		break; //

	case 205:
		if (Pw_ScrMenuMode == 0) // ZCL 2018.8.28 =0，控制器模式
			// Menu_StmReadPar4();
			Menu_DspReadPar6();								 // ZCL 2018.8.28 2019.9.6 换成此画面
		else if (Pw_ScrMenuMode == 1 || Pw_ScrMenuMode == 2) // ZCL 2018.8.28 =1，变频器模式
			Menu_DspReadPar6();								 // ZCL 2018.8.28
		break;

	case 206:
		Menu_DspReadPar7();
		break; //
	case 207:
		Menu_DspReadPar8();
		break; //

	// ZCL 2018.8.28 DCM100变频模式时使用。
	//  备注：DCM100控制器模式时，不需要 208-221画面，因为在Menu_StmReadPar中有。
	case 208:
		Menu_StmReadPar6();
		break; //	供水版本
	case 209:
		Menu_StmReadPar7();
		break; //
	case 210:
		Menu_StmReadPar8_NoUse();
		break; //
	case 211:
		Menu_StmReadPar9();
		break; //
	case 212:
		Menu_StmReadPar10();
		break; //

	case 213:
		Menu_StmReadPar11();
		break; // ZCL 2018.8.28 最近六次故障记录 1	DCM100变频模式时使用。
	case 214:
		Menu_StmReadPar12();
		break; //
	case 215:
		Menu_StmReadPar13();
		break; //
	case 216:
		Menu_StmReadPar14();
		break; //
	case 217:
		Menu_StmReadPar15();
		break; //
	case 218:
		Menu_StmReadPar16();
		break; //
	case 219:
		Menu_StmReadPar17();
		break; // 选择故障记录
	case 220:
		Menu_StmReadPar18();
		break; // 时间
	case 221:
		Menu_StmReadPar19();
		break; // ZCL 2018.8.28 加入AI1-AI4读值，方便调试。

	//----------------ZCL 2019.3.18 DCM220 变频器模式
	case 250:
		Menu_DspReadPar29();
		break; //
	case 251:
		Menu_DspReadPar30();
		break; //
	case 252:
		Menu_DspReadPar31();
		break; //
	case 253:
		Menu_DspReadPar32();
		break; //
	case 254:
		Menu_DspReadPar33();
		break; //
	case 255:
		Menu_DspReadPar34();
		break; // ZCL 2018.8.28 最近六次故障记录 1	DCM220变频模式时使用。
	case 256:
		Menu_DspReadPar35();
		break; //
	case 257:
		Menu_DspReadPar36();
		break; //
	case 258:
		Menu_DspReadPar37();
		break; //
	case 259:
		Menu_DspReadPar38();
		break; //
	case 260:
		Menu_DspReadPar39();
		break; //
	case 261:
		Menu_DspReadPar40();
		break; // 选择故障记录

	// case 262: Menu_GpsReadPar1();  break;		// 			//ZCL 2017.6.5 经纬度页

	// 3. DSP写参数
	case 300:
		if (Pw_ScrMenuMode == 0)	  // ZCL 2018.8.28 =0，控制器模式
			Menu_DspSetPar1();		  // 变频设定参数
		else if (Pw_ScrMenuMode == 1) // ZCL 2018.8.28 =1，变频器模式
			Menu_StmSetPar2();		  // 修改参数
		else if (Pw_ScrMenuMode == 2) // ZCL 2019.3.18 =11，DCM220变频器模式
			Menu_DspSetPar50();		  // 修改参数
		break;						  // 修改参数画面

	case 301:
		Menu_DspSetPar2();
		break; // 加速时间
	case 302:
		Menu_DspSetPar3();
		break; //
	case 303:
		Menu_DspSetPar4();
		break; //
	case 304:
		Menu_DspSetPar5();
		break; //
	case 305:
		Menu_DspSetPar6();
		break; //
	case 306:
		Menu_DspSetPar7();
		break; //
	case 307:
		Menu_DspSetPar8();
		break; //
	case 308:
		Menu_DspSetPar9();
		break; //	ZCL 2018.8.28

	case 309:
		Menu_DspSetPar10();
		break; //
	case 310:
		if (Pw_ScrMenuMode == 0)	  // ZCL 2018.8.28 =0，控制器模式
			Menu_StmSetPar24();		  // 定频模式
		else if (Pw_ScrMenuMode == 1) // ZCL 2018.8.28 =1，变频器模式
			Menu_StmSetPar24();		  // 定频模式
		else if (Pw_ScrMenuMode == 2) // ZCL 2019.3.18 =11，DCM220变频器模式
			Menu_DspSetPar30();		  // 定频模式
		break;
	// 跳到 543-5**结束		ZCL 2019.3.23

	//----------------ZCL 2019.3.18 DCM220 变频器模式，多加2行
	case 330:
		Menu_DspSetPar31();
		break; //
	case 331:
		Menu_DspSetPar32();
		break; //
	case 332:
		Menu_DspSetPar33();
		break; //	ZCL 2020.4.8
	// 跳到 523-5**结束		ZCL 2019.3.23

	// 5. LORA主机显示从机的参数	ZCL 2019.3.8
	case 500:
		Menu_GpsReadPar1(); // ZCL 2017.6.5 经纬度页
		break;
	case 501:
		Menu_Dsp1ReadPar1();
		break; //
	case 502:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp1ReadPar2();
		}
		else // 变频电机
		{
			Menu_Dsp1ReadPar2_BK();
		}
		break; //
	case 503:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp1ReadPar3();
		}
		else // 变频电机
		{
			Menu_Dsp1ReadPar3_BK();
		}
		break; //
	case 504:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp1ReadPar4();
		}
		else // 变频电机
		{
			Menu_Dsp1ReadPar4_BK();
		}
		break; //	ZCL 2019.9.12
	case 505:
		Menu_Dsp1ReadPar5();
		break; //	ZCL 2019.9.12

	case 506:
		Menu_Dsp2ReadPar1();
		break; //
	case 507:
		Menu_Dsp2ReadPar2();
		break; //
	case 508:
		Menu_Dsp2ReadPar3();
		break; //
	case 509:
		Menu_Dsp2ReadPar4();
		break; //	ZCL 2019.9.12
	case 510:
		Menu_Dsp2ReadPar5();
		break; //	ZCL 2019.9.12

	case 511:
		Menu_Dsp3ReadPar1();
		break; //
	case 512:
		Menu_Dsp3ReadPar2();
		break; //
	case 513:
		Menu_Dsp3ReadPar3();
		break; //
	case 514:
		Menu_Dsp3ReadPar4();
		break; //	ZCL 2019.9.12
	case 515:
		Menu_Dsp3ReadPar5();
		break; //	ZCL 2019.9.12

	case 516:
		Menu_Dsp4ReadPar1();
		break; //
	case 517:
		Menu_Dsp4ReadPar2();
		break; //
	case 518:
		Menu_Dsp4ReadPar3();
		break; //
	case 519:
		Menu_Dsp4ReadPar4();
		break; //	ZCL 2019.9.12
	case 520:
		Menu_Dsp4ReadPar5();
		break; //	ZCL 2019.9.12

	// ZCL 2019.10.18
	case 521:
		Menu_Dsp5ReadPar1();
		break; //
	case 522:
		Menu_Dsp5ReadPar2();
		break; //
	case 523:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp5ReadPar3();
		}
		else // 变频电机
		{
			Menu_LoRaMasteSetPar1();
		}
		break; //
	case 524:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp5ReadPar4();
		}
		else // 变频电机
		{
			Menu_LoRaSetPar1();
		}
		break; //	ZCL 2019.9.12
	case 525:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp5ReadPar5();
		}
		else // 变频电机
		{
			Menu_LoRaSetPar2();
		}
		break; //	ZCL 2019.9.12

	case 526:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp6ReadPar1();
		}
		else // 变频电机
		{
			Menu_LoRaSetPar3();
		}
		break; //
	case 527:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp6ReadPar2();
		}
		else // 变频电机
		{
			Menu_LoRaSetPar4();
		}
		break; //
	case 528:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp6ReadPar3();
		}
		else // 变频电机
		{
			Menu_LoRaSetPar5();
		}
		break; //
	case 529:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp6ReadPar4();
		}
		else // 变频电机
		{
			Menu_LoRaSetPar6();
		}
		break; //	ZCL 2019.9.12
	case 530:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp6ReadPar5();
		}
		else // 变频电机
		{
			Menu_LoRaSetPar7();
		}
		break; //	ZCL 2019.9.12

	case 531:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp7ReadPar1();
		}
		else // 变频电机
		{
			Menu_GprsReadPar1();
		}
		break; //
	case 532:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp7ReadPar2();
		}
		else // 变频电机
		{
			Menu_GprsReadPar2();
		}
		break; //
	case 533:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp7ReadPar3();
		}
		else // 变频电机
		{
			Menu_GprsReadPar3();
		}
		break; //
	case 534:
		if (Pw_EquipmentType == 0) // 双驱泵
		{
			Menu_Dsp7ReadPar4();
		}
		else // 变频电机
		{
			Menu_GprsReadPar4();
		}
		break; //	ZCL 2019.9.12
	case 535:
		Menu_Dsp7ReadPar5();
		break; //	ZCL 2019.9.12

	case 536:
		Menu_Dsp8ReadPar1();
		break; //
	case 537:
		Menu_Dsp8ReadPar2();
		break; //
	case 538:
		Menu_Dsp8ReadPar3();
		break; //
	case 539:
		Menu_Dsp8ReadPar4();
		break; //	ZCL 2019.9.12
	case 540:
		Menu_Dsp8ReadPar5();
		break; //	ZCL 2019.9.12

	case 543:
		Menu_LoRaMasteSetPar1();
		break; //

	case 544:
		Menu_LoRaSetPar1();
		break; // ZCL 2018.11.13 LoRa参数
	case 545:
		Menu_LoRaSetPar2();
		break; //
	case 546:
		Menu_LoRaSetPar3();
		break; //
	case 547:
		Menu_LoRaSetPar4();
		break; //
	case 548:
		Menu_LoRaSetPar5();
		break; // ZCL 2019.3.22 LoRa射频参数
	case 549:
		Menu_LoRaSetPar6();
		break; //

	case 550:
		Menu_LoRaSetPar7();
		break; // ZCL 2019.9.24

	case 551:
		Menu_GprsReadPar1();
		break; //
	case 552:
		Menu_GprsReadPar2();
		break; //
	case 553:
		Menu_GprsReadPar3();
		break; //
	case 554:
		Menu_GprsReadPar4();
		break; //

	// ZCL 2019.10.19 增加可以改动 YW310 SZM220参数
	case 555:
		Menu_YW310Par1();
		break; //
	case 556:
		Menu_YW310Par2();
		break; //
	case 557:
		Menu_YW310Par3();
		break; //

	case 558:
		Menu_YW310Par4();
		break; //		ZCL 2019.10.21

	case 559:
		Menu_SZM220Par1();
		break; //
	case 560:
		Menu_SZM220Par2();
		break; //
	case 561:
		Menu_SZM220Par3();
		break; //
	case 562:
		Menu_SZM220Par4();
		break; //
	case 563:
		Menu_SZM220Par5();
		break; //
	case 564:
		Menu_SZM220Par6();
		break; //
	case 565:
		Menu_SZM220Par7();
		break; //

	case 999:
		Menu_END();
		break; //

	default:
		return;
	}
}
