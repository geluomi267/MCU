#include <reg52.h>

#define sgm_dx P0

typedef unsigned char u8;
typedef unsigned int u16;

u8 sgm_num[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
u8 sec1=0,sec10=0,min1=0,min10=0,hour1=0,hour10=0;
u8 state=0;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit KEY3=P3^2;
sbit KEY4=P3^3;

void delay_10us(u16 t)
{
	while(t--);
}

void Timer0_Set()
{
	TMOD&=0xF0;
	TMOD|=0x01;
	TH0=0x4B;
	TL0=0xFD;
	ET0=1;
	EA=1;
	TR0=1;
}

void Exit0_Set()
{
	IT0=1;
	EX0=1;
	EA=1;
}

void Exit1_Set()
{
	IT1=1;
	EX1=1;
	EA=1;
}

void display_sgm()
{
	u8 i=1;
	for(i=1;i<=6;i++)
	{
		switch(i)
		{
			case 1: LSC=0;LSB=0;LSA=0;sgm_dx=sgm_num[sec1];break;
			case 2: LSC=0;LSB=0;LSA=1;sgm_dx=sgm_num[sec10];break;
			case 3: LSC=0;LSB=1;LSA=0;sgm_dx=sgm_num[min1];break;
			case 4: LSC=0;LSB=1;LSA=1;sgm_dx=sgm_num[min10];break;
			case 5: LSC=1;LSB=0;LSA=0;sgm_dx=sgm_num[hour1];break;
			case 6: LSC=1;LSB=0;LSA=1;sgm_dx=sgm_num[hour10];break;
		}
		delay_10us(100);
		sgm_dx=0x00;
		delay_10us(100);
	}	
}

void main()
{
	Timer0_Set();
	Exit0_Set();
	Exit1_Set();
	while(1)
	{
		display_sgm();
	}
}

void Timer0() interrupt 1
{
	static u16 j=0;
	TH0=0x4B;
	TL0=0xFD;
	j++;
	if(j==20)
	{
		j=0;
		sec1++;
		if(sec1==10)
		{
			sec1=0;
			sec10++;
			if(sec10==6)
			{
				sec10=0;
				min1++;
				if(min1==10)
				{
					min1=0;
					min10++;
					if(min10==6)
					{
						min10=0;
						hour1++;
						if(hour1==10)
						{
							hour1=0;
							hour10++;
						}
						if(hour1==4 && hour10==2)
						{
							hour1=0;
							hour10=0;
						}
					}
				}
			}
		}
	}
}

void Exit0() interrupt 0
{
	delay_10us(1000);
	if(KEY3==0)
	{
		state++;
		TR0=0;
		if(state==7) 
		{
			state=0;
			TR0=1;
		}	
	}
}

void Exit1() interrupt 2
{
	delay_10us(1000);
	if(KEY4==0)
	{
		if(state==1) 
		{
			sec1++;
			if(sec1==10) sec1=0;
		}
		if(state==2) 
		{
			sec10++;
			if(sec10==6) sec10=0;
		}
		if(state==3) 
		{
			min1++;
			if(min1==10) min1=0;
		}
		if(state==4) 
		{
			min10++;
			if(min10==6) min10=0;
		}
		if(state==5) 
		{
			hour1++;
			if(hour1==10) hour1=0;
		}
		if(state==6) 
		{
			hour10++;
			if(hour10==3) hour10=0;
			if(hour10==2 && hour1>=4) hour10=0;	
		}		
	}
}