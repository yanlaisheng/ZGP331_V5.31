/*******************************************************************************
                             智能电极驱动程序
							 版权所有 @ 2012



设计:张彦欣
日期:2014-03-11
********************************************************************************/
#ifndef  _LCD12864_GRAPHIC_H_
#define  _LCD12864_GRAPHIC_H_


#include "stm32f10x.h"


u8 LCD12864_BUFF_GetChangeFlg(void);

void  LCD12864_BUFF_RefreshScreen(void);

void  LCD12864_BUFF_Clear(void);

void  LCD12864_BUFF_SetPixelIndex(u8 x, u8 y, u8 Color);
u8 LCD12864_BUFF_GetPixelIndex(u8 x, u8 y);

void  LCD_BUFF_DrawBMP(u8 x, u8 y, u8 xSize, u8 ySize, const u8 *myData);

void  LCD12864_BUFF_Num(u8 x, u8 y, u8 xSize, u8 ySize, long Num);
void  LCD12864_BUFF_Char(u8 x, u8 y, u8 xSize, u8 ySize, u8 Index);
void LCD12864_BUFF_String(u8 x, u8 y, u8 xSize, u8 ySize, u8 *String);


void LCD12864_DrawRect(u8 x, u8 y, u8 xSize, u8 ySize);
void LCD12864_DrawProgressbar(u8 x, u8 y, u8 xSize, u8 ySize, u8 Value);




#endif



