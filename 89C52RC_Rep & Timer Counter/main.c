#include "reg52.h"

#define smg_dx P0

sbit LED1=P2^0;
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit KEY=P3^1;

typedef unsigned char u8;
typedef unsigned int u16;

u8 smg_num[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
u16 turn10=0,turn1=0,sec10=0,sec1=0;
u8 i,j=0;
u8 flag=0;

void delay_10us(u16 t)
{
	while(t--);
}

void Timer0_Set()
{
	TMOD&=0xF0;
	TMOD|=0x01;
	TH0=(int)(65536-50000/1.085)/256;
	TL0=(int)(65536-50000/1.085)%256;
	ET0=1;
	EA=1;
	TR0=0;
}

void smg_wx()
{
	for(i=1;i<=4;i++)
	{
		switch(i)
		{
			case 1:LSC=0;LSB=0;LSA=0;P0=smg_num[sec1];break;
			case 2:LSC=0;LSB=0;LSA=1;P0=smg_num[sec10];break;
			case 3:LSC=0;LSB=1;LSA=0;P0=smg_num[turn1];break;
			case 4:LSC=0;LSB=1;LSA=1;P0=smg_num[turn10];break;
		}
		delay_10us(100);
		P0=0x00;
		delay_10us(100);
	}	
}

void display_mode()
{
	if(flag==0)
	{
		TR0=0;
		sec1=0;sec10=4;turn1=1;turn10=0;
	}
	if(flag==1)
	{
		TR0=1;LED1=0;
		if(sec1==0 && sec10==0) 
		{
			flag=2;
			sec1=0;sec10=2;
		}
	}
	if(flag==2)
	{
		TR0=1;LED1=1;
		if(sec1==0 && sec10==0)
		{
			turn1++;
			if(turn1==10)
			{
				turn1=0;
				turn10++;
			}
			flag=1;
			sec1=0;sec10=4;
		}
	}
}

void SET0()
{
	if(KEY==0)
	{
		delay_10us(1000);
		if(KEY==0 && (flag==1 || flag==2))
		{
			flag=0;	
		}
		if(KEY==0 && flag==0)
		{
			flag=1;
		}
	}
}

void main()
{
	Timer0_Set();
	while(1)
	{
		smg_wx();
		SET0();
		display_mode();		
	}
}

void Timer0() interrupt 1
{
	TH0=(int)(65536-50000/1.085)/256;
	TL0=(int)(65536-50000/1.085)%256;
	j++;
	if(j==20)
	{
		j=0;
		sec1--;
		if(sec1==-1)
		{
			sec1=9;sec10--;
		}
	}	
}