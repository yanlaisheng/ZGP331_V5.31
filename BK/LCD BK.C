/*********************LCD 延时1ms************************************/
void LCD_DLY_ms(u32 Time)
{
	u32 tick;
	tick = sysTickValue;
	/* Wait for specified Time */
	while ((u32)(sysTickValue - tick) < Time);
}

/*********************LCD写数据************************************/
void LCD_WrDat(u8 dat)
{
	u8 i;
	LCD_DC = 1;
	for(i = 0; i < 8; i++) //发送一个八位数据
	{
		if((dat << i) & 0x80)
		{
			LCD_SDA  = 1;
		}
		else  LCD_SDA  = 0;
		LCD_SCL = 0;
		LCD_SCL = 1;
	}
}

/*********************LCD写命令************************************/
void LCD_WrCmd(u8 cmd)
{
	u8 i;
	LCD_DC = 0;
	for(i = 0; i < 8; i++) //发送一个八位数据
	{
		if((cmd << i) & 0x80)
		{
			LCD_SDA  = 1;
		}
		else  LCD_SDA  = 0;
		LCD_SCL = 0;
		LCD_SCL = 1;
	}
}

/*********************LCD 设置坐标************************************/
void LCD_Set_Pos(u8 x, u8 y)		//x的范围0～127，y为页的范围0～7
{
	LCD_WrCmd(0xb0 + y);
	LCD_WrCmd(((x & 0xf0) >> 4) | 0x10);
	LCD_WrCmd((x & 0x0f) | SEGBEGIN );						//0x01?  周成磊
}

/*********************LCD全屏************************************/
void LCD_Fill(u8 bmp_dat)
{
	u8 y, x;
	for(y = 0; y < 8; y++)     // 8页，每页128个数据
	{
		LCD_WrCmd(0xb0 + y);				//set page address 
		LCD_WrCmd(SEGBEGIN);						//0x01?  周成磊
		LCD_WrCmd(0x10);
		for(x = 0; x < X_WIDTH; x++)
			LCD_WrDat(bmp_dat);
	}
}

/*********************LCD复位************************************/
void LCD_CLS(void)
{
	u8 y, x;
	for(y = 0; y < 8; y++)     // 8页，每页128个数据
	{
		LCD_WrCmd(0xb0 + y);				//set page address 
		LCD_WrCmd(SEGBEGIN);						//0x01?  周成磊
		LCD_WrCmd(0x10);
		for(x = 0; x < X_WIDTH; x++)
			LCD_WrDat(0);
	}
}

/***********功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7***********/
void LCD_P6x8Str(u8 x, u8 y, u8 ch[])
{
	u8 c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;				//去掉控制字符，从空格符开始
		if(x > 126)
		{
			x = 0;
			y++;
		}
		LCD_Set_Pos(x, y);		//设置坐标
		for(i = 0; i < 6; i++)		//写6*8点阵的6个字节数据
			LCD_WrDat(F6x8[c][i]);
		x += 6;								//列地址+6
		j++;									//字符+1
	}
}

/**************功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～7**********/
void LCD_P8x16Str(u8 x, u8 y, u8 ch[])
{
	u8 c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;				//去掉控制字符，从空格符开始
		if(x > 120)
		{
			x = 0;
			y++;
		}
		LCD_Set_Pos(x, y);						//设置坐标 上一页  一个字符16个字节，分布在两页中
		for(i = 0; i < 8; i++)
			LCD_WrDat(F8X16[c*16+i]);		//顺序写前8个字节
		LCD_Set_Pos(x, y + 1);				//设置坐标 下一页  一个字符16个字节，分布在两页中
		for(i = 0; i < 8; i++)		
			LCD_WrDat(F8X16[c*16+i+8]);	//顺序写后8个字节
		x += 8;
		j++;
	}
}

/***************功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7*************************/
void LCD_P16x16Ch(u8 x, u8 y, u8 N)
{
	u8 wm = 0;
	unsigned int addre = 32 * N; //
	LCD_Set_Pos(x , y);
	for(wm = 0; wm < 16; wm++) //
	{
		LCD_WrDat(F16x16[addre]);
		addre += 1;
	}
	LCD_Set_Pos(x, y + 1);
	for(wm = 0; wm < 16; wm++) //
	{
		LCD_WrDat(F16x16[addre]);
		addre += 1;
	}
}

/*********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7***********/
void Draw_BMP(u8 x0, u8 y0, u8 x1, u8 y1, uc8 BMP[])
{
	unsigned int j = 0;
	u8 x, y;

	if(y1 % 8 == 0) y = y1 / 8;
	else y = y1 / 8 + 1;
	for(y = y0; y < y1; y++)
	{
		LCD_Set_Pos(x0, y);
		for(x = x0; x < x1; x++)
		{
			LCD_WrDat(BMP[j++]);
		}
	}
}

//显示参数.修改参数  周成磊 2013.12.6-12.8  非常重要的一个函数！！！
//p:页  x:列总坐标(h+l)(最低地址是->最高位)  *Address:要显示和修改的参数  length:参数显示长度
void DrawMod_Par(u8 p,u8 x,u16 *Address,u8 length,u8 f_modpar)
{
  u8 i,j,c,x0;
	u8 zimo;
	u8 secondbit=0;		//第二位是零要显示，第一位是零不显示（在前是高位）
	u16 w,RelativeAddress;		//相对地址
	u16 *p_head;
	
	w=*Address;
	//2014.3.26 ZCL 
	p_head=w_ParLst;
	RelativeAddress=Address-p_head;	
	if(S_ModPar==1)				//刚进入修改参数项，把参数保存到w_TmpModParValue，以此来修改
	{
		S_ModPar=2;
		w_TmpModParValue=w;			
	}
	
	if(!f_modpar)					//分解万.千.百.十.个位，在下面进行显示和修改，修改完要合并
	{
		Value[4]=w/10000;						//万位
		Value[3]=(w%10000)/1000;		//千位
		Value[2]=(w%1000)/100;			//百位	
		Value[1]=(w%100)/10;				//十位	
		Value[0]=(w%10);				 		//个位		
	}
	else
	{
		Value[4]=w_TmpModParValue/10000;					//万位
		Value[3]=(w_TmpModParValue%10000)/1000;		//千位
		Value[2]=(w_TmpModParValue%1000)/100;			//百位	
		Value[1]=(w_TmpModParValue%100)/10;				//十位	
		Value[0]=(w_TmpModParValue%10);				 		//个位			
	}

	//1.修改参数  在先
	if(f_modpar)
	{
		if(Sa_ModParKeyData==1)				//上升键	每次修改后，合并保存值
		{
			if(Value[ModParBitPos]<9)
				Value[ModParBitPos]++;

			w_TmpModParValue=Value[4]*10000+Value[3]*1000+Value[2]*100+Value[1]*10+Value[0];
		}
		else if(Sa_ModParKeyData==5)	//下降键	每次修改后，合并保存值
		{
			if(Value[ModParBitPos]>0)
				Value[ModParBitPos]--;
			w_TmpModParValue=Value[4]*10000+Value[3]*1000+Value[2]*100+Value[1]*10+Value[0];
		}	
		else if(Sa_ModParKeyData==3)	//向左键  移动参数位置
		{
			if(ModParBitPos+1<length)		//只能移动到length规定的位置
				ModParBitPos++;
		}
		else if(Sa_ModParKeyData==4)	//向右键  移动参数位置
		{
			if(ModParBitPos>0)					//只要位置不是0，减1
				ModParBitPos--;
		}			
		else if(Sa_ModParKeyData==2)	//ESC取消键  
		{
			S_ModPar=0;					//不修改参数，取消
			ModParNo=0;					//修改参数序号=0
			ModParBitPos=0;			//修改参数位位置=0
		}		
		else if(Sa_ModParKeyData==6)	//ENTER回车键  
		{
			if((Address==&Pw_ModPar) || (Pw_ModPar==10000) //Pw_ModPar可以直接修改 
				|| (Address>=&Pw_SetSecond && Address<=&Pw_SetYear) )		//时间参数可以直接修改 
			{										//Pw_ModPar=10000可以修改其他参数 2013.12.10 周成磊
				//用指针修改参数
				w_TmpModParValue=Value[4]*10000+Value[3]*1000+Value[2]*100+Value[1]*10+Value[0];
				*Address=w_TmpModParValue;			
				//2014.3.26 ZCL 
				if(RelativeAddress>=10 && RelativeAddress<=16)
				{
					w_ModTimeNo=RelativeAddress-10;					
					F_ModTime=1;
				}
			}
			//
			S_ModPar=1;					//修改参数确认后，指向下一个参数
			ModParNo++;					//修改参数序号++	
			ModParBitPos=0;			//修改参数位位置=0
		}
		//每次按键用完，就清零，等待重新按键。 周成磊 2013.12.8
		Sa_ModParKeyData=0;
	}
	
	//2.显示参数 在后
	j=length;
	for(;j>0;j--)			//length次显示，每次一个位，从最低地址是->最高位开始，=0不显示
	{
		if( Value[j-1]>0 												//>0显示
			|| (Value[j-1]==0 && secondbit)				//=0，但是高位有非0数字显示后，显示
			|| (Value[j-1]==0 && (j-1)==0 ) 			//=0，但是末尾0，显示
			|| (Value[j-1]==0 && j-1==ModParBitPos && f_modpar) 
			|| RelativeAddress==86 || RelativeAddress==87		//开关量输入状态
			|| RelativeAddress==88 || RelativeAddress==89		//开关量输出状态		
			|| RelativeAddress==93								//水泵状态 
			)		//=0，但是在修改位，显示		
		{
			c=Value[j-1]+48-32;						//在字符表中的位置！		
			secondbit=1;			
		}
		else
			c=0;													//空的字模点阵，清空
		//
		x0=x+(length-j)*8;							//计算显示位置	
		
		LCD_Set_Pos(x0, p);						//设置坐标 上一页  一个字符16个字节，分布在两页中
		for(i = 0; i < 8; i++)
		{
			zimo=F8X16[c*16+i];
			if(f_modpar && (j-1!=ModParBitPos || length==1 ))		//修改参数状态字模取反 2013.12.8
																					//加length==1 2013.12.9
				zimo=(~zimo);
			LCD_WrDat(zimo);		//顺序写前8个字节
		}	
		
		LCD_Set_Pos(x0, p + 1);				//设置坐标 下一页  一个字符16个字节，分布在两页中
		for(i = 0; i < 8; i++)
		{
			zimo=F8X16[c*16+i+8];
			if(f_modpar && (j-1!=ModParBitPos || length==1 ))		//修改参数状态字模取反 2013.12.8
																					//加length==1 2013.12.9
				zimo=(~zimo);
			LCD_WrDat(zimo);		//顺序写后8个字节		
		}	
	}	
	
}

//---------------------------移植 OLED2.7寸程序
void Clear_screen(void)   //清屏函数 128*64
{
  u8 j,page;
	for(page=0;page<8;page++)     // 8页，每页128个数据
	{
		LCD_WrCmd(0xB0+page);       //set page address   0xB0:176
		LCD_WrCmd(0x10);            //set higher address    High低位+Low低位组合成1个字节，这里为0。    
		LCD_WrCmd(SEGBEGIN);        //set low address

		for(j=0;j<128;j++)
		{
      LCD_WrDat(0x00);            //写数据0x00，清屏 
		}	       
  }
}

void Clear_page(u8 page)   //清指定页函数 
{
  u8 j;

	LCD_WrCmd(0xB0+page);       //set page address   0xB0:176
	LCD_WrCmd(0x10);            //set higher address    High低位+Low低位组合成1个字节，这里为0。    
	LCD_WrCmd(SEGBEGIN);            //set low address

	for(j=0;j<128;j++)
	{
		LCD_WrDat(0x00);            //写数据0x00，清屏 
	}	       
}

// OLED显示函数（图片） 可指定起始页，结束页
void Oled_display(uc8 picture[],u8 startpage,u8 endpage)
{
	u8 j,page;
	for(page=startpage;page<endpage+1;page++)     // 8页，每页128个数据
	{
    LCD_WrCmd(0xB0+page); //set page address
    LCD_WrCmd(0x10);      //set higher address     High低位+Low低位组合成1个字节，这里为0。    
    LCD_WrCmd(SEGBEGIN);      //set low address 02h! 128*64屏，使用128*64的控制器SSD1306 
    
    for(j=0;j<128;j++)
    {
      LCD_WrDat(picture[j+page*128]);      //从第1页开始写，但取数据还是从数组[0]开始      
    }
  }
}

/******************************************************************/
void Show_cat(uc8 picture[])
{
  u8 j,page;
	for(page=0;page<8;page++)     // 8页，每页58个数据
	{
    LCD_WrCmd(0xB0+page); //set page address
    LCD_WrCmd(0x10);      //set higher address     High低位+Low低位组合成1个字节，这里为0。    
    LCD_WrCmd(SEGBEGIN);      //set low address 02h! 128*64屏，使用128*64的控制器SSD1306 
    
		for(j=0;j<58;j++)   //picture is 58*64 像素
		{
      LCD_WrDat(picture[j+page*58]);      //从第1页开始写，但取数据还是从数组[0]开始  		  
		}	   
  }
}

/***************************************************************************/
void Cat_offon(void)
{
  u8 i;
  Show_cat(cat_off);    //显示猫关
	for(i=0;i<5;i++)
	{
	  LCD_DLY_ms(18);  			 //延时18MS;
	}
	Show_cat(cat_on);     //显示猫开
	for(i=0;i<5;i++)
	{
	  LCD_DLY_ms(18);  			 //延时18MS;
	}
}

/***************************************************************************/
//真空度(原先:物理量)监测计时+数值显示  效果图见文件
void Zhenkongdu_SelfCheck(void)
{
  u8 i,j,k,l;
  HZ_1616(1,0,8,0);//真    第1页，第0x8列，第0个汉字   16*16
  HZ_1616(1,1,8,1);//空    第1页，第0x18列，第1个汉字
	HZ_1616(1,2,8,2);//度    第1页，第0x28列，第2个汉字
	HZ_1616(1,3,8,3);//监    第1页，第0x38列，第3个汉字
	HZ_1616(1,4,8,4);//测    第1页，第0x48列，第4个汉字
	//HZ_1616(1,5,6,5);
	HZ_1616(1,5,8,6);//计    第1页，第0x58列，第6个汉字
	HZ_1616(1,6,8,7);//时    第1页，第0x68列，第7个汉字

	//第4页，第0x16列，第i个数字(0-1)  16*32
	//第4页，第0x26列，第i个数字(0-1)  16*32
	//第4页，第0x46列，第i个数字(0-1)  16*32
	//第4页，第0x56列，第i个数字(0-9)  16*32  
  for(l=0;l<2;l++)
	{
    Num_1632(4,1,6,l);    //第4页，第0x16列，第i个数字(0-1)  16*32
		for(k=0;k<3;k++)
		{		
			Num_1632(4,2,6,k);    //第4页，第0x26列，第i个数字(0-1)  16*32		
			for(j=0;j<6;j++)
			{
				Num_1632(4,4,6,j);    //第4页，第0x46列，第i个数字(0-1)  16*32		
				for(i=0;i<10;i++)
				{
					Num_1632(4,5,6,i);    //第4页，第0x56列，第i个数字(0-9)  16*32
					LCD_DLY_ms(18);  			 //延时18mS;		
				}
			}
		}
	}
}

/************************************************************************/ 
//写HZ of 16*16=32字节
//p:页  a:写的汉字编码数组 编程思路同w_word
void HZ_1616(u8 p,u8 h,u8 l,u8 a)  
{
  u8 i,j,k,m=0;
	k=p+2;
  for(i=p;i<k;i++)        //写2页，每页16个字节，1个汉字32个字节
  {
    LCD_WrCmd(0xB0+i);      //set page address
    LCD_WrCmd(0x10+h);      //set higher address     High低位+Low低位组合成1个字节
    LCD_WrCmd(SEGBEGIN+l);      //set low address  02h! 128*64屏，使用128*64的控制器SSD1306 
    
		for(j=0;j<16;j++)   //16个字节
		{
      LCD_WrDat(HZ1616[a][j+m*16]);       
		}	    
    m++;
  }
}

/************************************************************************/ 
//p:第*页  从指定页.指定行写指定长度的数组（数组16点阵，需要写2页，每页的长度为数组长度/2）
//write 任意长度的字符图形（可指定页和位置）
void HZ_16x(u8 p,u8 h,u8 l,uc8 *pic,u16 Arraylength)   
{
  u8 i,j,k,m=0;
	k=p+2;
  for(i=p;i<k;i++)        //写2页，每页16个字节，1个汉字32个字节
  {
    LCD_WrCmd(0xB0+i);      //set page address
    LCD_WrCmd(0x10+h);      //set higher address     High低位+Low低位组合成1个字节
    LCD_WrCmd(SEGBEGIN+l);    //set low address 02h!128*64屏,使用128*64的控制器SSD1306
    
		for(j=0;j<Arraylength/2;j++)   //
		{
      LCD_WrDat(pic[j+m*Arraylength/2]);       
		}	    
    m++;
  }
}

/***************************************************************************/
//现在加P参数（p:页）  a:写的24点阵字母编码数组 编程思路与word_1616稍有不同
//word_2424在指定页写，word_1616可以在指定页写
void HZ_2424(u8 p,u8 h,u8 l,u8 a)    //24*24汉字    未调用 p必须<=5
{
  u8 i,j,k,m=0;
  k=p+3;        
  for(i=p;i<k;i++)   //page number，固定页位置
  {
    LCD_WrCmd(0xB0+i);      //set page address
    LCD_WrCmd(0x10+h);      //set higher address     High低位+Low低位组合成1个字节
    LCD_WrCmd(SEGBEGIN+l);      //set low address  02h! 128*64屏，使用128*64的控制器SSD1306
    
    for(j=0;j<24;j++)   //24个字节
    {
      LCD_WrDat(HZ2424[a][j+m*24]);  
    }
    m++;
  }
}

/***************************************************************************/
//现在加P参数（p:页）  a:写的24点阵字母编码数组 编程思路与word_1616稍有不同
//在指定页写  激光扫描书写
void HZ_2424Laser(u8 p,u8 h,u8 l,u8 a)    //24*24汉字    未调用 p必须<=5
{
  u8 i,j,k;		//1.周成磊 2013.12.6 这三个内部变量可以观看，定位在R5,R6,R7 
	//2.下面定义的内部变量放到WATCH窗口，没法观看，变量太多？周成磊 2013.12.6  已经转成全局变量
	//u8 Remainder,Quotient;			//余数和商数  
	//3.传递参数p,h,l在IAR中观看值也不对，都定位在R8，看来传递参数不能直接用，最好传递给函数内变量。
	//static   u8 p1,h1,l1,a1;			//这样定义也不对！看不了p1,h1,l1,a1  为什么？
	//volatile u8 p1,h1,l1,a1;			//这样定义也不对！看不了p1,h1,l1,a1  为什么？
	//p1=p;h1=h,l1=l;a1=a;						//周成磊！看不了局部变量p1,h1,l1,a1  为什么？

	for(i=0;i<6;i++)			//写6次，每次写3页，每页4个字节
	{
		for(j=0;j<3;j++)		//每次写3页，每页4个字节（写3次）
		{
			LCD_WrCmd(0xB0+p+j);    //set page address
			Remainder=(l+i*4)%16;
			Quotient=(l+i*4)/16;
			LCD_WrCmd(0x10+h+Quotient);      //set higher address     High低位+Low低位组合成1个字节
			LCD_WrCmd(SEGBEGIN+Remainder);   //set low address  02h! 128*64屏，使用128*64的控制器SSD1306
		
			for(k=0;k<4;k++)   //4个字节
			{
				Tmp1=HZ2424[a][k+j*24+i*4];
				//Tmp1=0xff;
				LCD_WrDat(Tmp1);  
			}
			LCD_DLY_ms(40);  			 //延时50MS;			
		}	
	}
}

/***************************************************************************/
//p:页  a:写的16*7点阵数字字母编码数组 编程思路同w_word
//word_1607 p:第*页  从指定页.指定行写指定数字 16*7点阵 14字节
void Num_1607(u8 p,u8 h,u8 l,u8 a)   //write 字母数字 16*7点阵 14字节
{
  u8 i,j;
  for(i=0;i<2;i++)        //写2页，每页8个字节，1个字母16个字节
  {
    LCD_WrCmd(0xB0+i+p);      //set page address
		LCD_WrCmd(0x10+h);      //set higher address     High低位+Low低位组合成1个字节
		LCD_WrCmd(SEGBEGIN+l);      //set low address
	
		for(j=0;j<7;j++)   //7个字节
		{
      LCD_WrDat(Number1607[a*14+j+i*7]);    	//地址会自动顺序加  周成磊 2013.12.6   
		}	    
  }	
}

//p:页  a:写的16*8点阵数字字母编码数组 编程思路同w_word
//word_1608 p:第*页  从指定页.指定行写指定字母 16*8点阵 16字节
void Num_1608(u8 p,u8 h,u8 l,u8 a)   //write 字母数字 16*8点阵 16字节
{
  u8 i,j;
  for(i=0;i<2;i++)        //写2页，每页8个字节，1个字母16个字节
  {
    LCD_WrCmd(0xB0+i+p);      //set page address
		LCD_WrCmd(0x10+h);      //set higher address     High低位+Low低位组合成1个字节
		LCD_WrCmd(SEGBEGIN+l);      //set low address
	
		for(j=0;j<8;j++)   //8个字节
		{
      LCD_WrDat(Number1608[a*16+j+i*8]);    	//地址会自动顺序加  周成磊 2013.12.6   
		}	    
  }
}

/**********************************************************************/
//write 0-9 figure  p:页  a:写的数字编码数组  16×32像素=64字节
void Num_1632(u8 p,u8 h,u8 l,u8 a)    
{
  u8 i,j,k,m=0;
	k=p+4;
  for(i=p;i<k;i++)      // 写4页，每页16个字节，1个数字64个字节
  {
    LCD_WrCmd(0xB0+i);      //set page address
    LCD_WrCmd(0x10+h);      //set higher address     High低位+Low低位组合成1个字节
    LCD_WrCmd(SEGBEGIN+l);      //set low address
    
		for(j=0;j<16;j++)   //16个字节
		{
      LCD_WrDat(number1632[a][j+m*16]);       
		}	    
    m++;
  }
}  