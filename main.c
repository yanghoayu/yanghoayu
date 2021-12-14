#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "Timer0.h"
#include "DS18B20.h"
#include "OneWire.h"

unsigned char Sec=0;
float T;
float T_sum;
float Rain;
unsigned int Rain_Count=0;

void Out()
{
	LCD_ShowString(2,13,"n=");
	LCD_ShowNum(2,15,Rain_Count,2);
	LCD_ShowString(1,13,"t=");
	LCD_ShowNum(1,15,Sec,2);
		if(T<0)				//如果温度小于0
		{
			LCD_ShowChar(1,6,'-');	//显示负号
			T=-T;			//将温度变为正数
		}
		else				//如果温度大于等于0
		{
			LCD_ShowChar(1,6,'+');	//显示正号
		}
		LCD_ShowNum(1,7,T,2);		//显示温度整数部分
		LCD_ShowChar(1,9,'.');		//显示小数点
		LCD_ShowNum(1,10,(unsigned long)(T*10000)%10000,2);//显示温度小数部分
		LCD_ShowNum(2,7,00,2);		//00
		LCD_ShowChar(2,9,'.');		//显示小数点
		LCD_ShowNum(2,10,Rain,2);//显示降水
}

void Rain_Num()
{
	//Rain_Count++;
	if(P3_1==0)			//如果K1按键按下
		{
			Delay(20);		//延时消抖
		  while(P3_1==0);	//松手检测
			Delay(20);		//延时消抖
			Rain_Count++;		//变量自增
		}
}

void main()
{
	DS18B20_ConvertT();		//上电先转换一次温度，防止第一次读数据错误
	Delay(1000);			//等待转换完成
	LCD_Init();
	Timer0Init();
	
	LCD_ShowString(1,1,"Temp:");	//上电显示静态字符串
	LCD_ShowString(2,1,"Rain:");	//上电显示静态字符串
//LCD_ShowString(2,1,"  :  :");
	
	while(1)
	{
		Out();
		Rain_Num();
		
	}
}

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count;
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count++;
	if(T0Count>=1000)	//定时器分频，1s
	{
		T0Count=0;
		Sec++;			//1秒到，Sec自增
		DS18B20_ConvertT();	//转换温度
		T_sum=T_sum+DS18B20_ReadT();	//读取温度
		//P0_1=~P0_1;
		if(Sec>=10)
		{
			T=T_sum/10;
			Sec=0;
			T_sum=0;
			Rain=Rain_Count*2;
			Rain_Count=0;
		}
	}
}
