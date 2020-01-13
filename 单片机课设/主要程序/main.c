#include "lcd12864.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "at24c02.h"
#include "config.h"
#include <stdlib.h>
#define SWAP(a,b); {a=a^b;b=a^b;a=a^b;}//宏函数，交换值
//按键,LED，蜂鸣器等声明放在了config.h
bit IsAlarm;//闹钟时间到了？判断这个标志用来启动一系列动作
bit OutTemp;//温度超出范围？同样用来启动一系列动作
uint cnt;//中断计数用的，达到两种不同的声音频率
bit AlarmOn;		//闹钟开启则置1，关则0，方便其他部分判断闹钟是否开启	
uchar AlarmTime[3]={0x00,0x00,0x18};	//闹钟设置的时间,秒，分，时			
int temp;//读取到的温度
float tp;
int tempH=12500;																			
int tempL=-5500;//温度上限值和下限值变量，百倍的			
uchar buff[10];//存储设置时选的值
uchar IndexHor=0;//横向移动的箭头提示符的位置
uchar IndexVer=0;//竖向移动的箭头提示符的位置
uchar ui=0;
/**********************************************
变量ui表示的含义:
	0:默认的显示信息的界面，有时间、温度、日期、周、闹钟情况等信息。
	1:设置界面

	2:日期设置界面
	3:时间设置界面
	4:温度上下限设置界面
	5:闹钟设置界面
**********************************************/

int TempPros(int temp)
{
	if(temp<0)
		{
			tp=~(temp-1);				//负数的时候把温度-1再取反	
			temp=tp*0.0625*100+0.5;//得到真实温度值的百倍的绝对值，整型方便后面比较和调整
			temp=-temp;						//再转为带负号的百倍的值
		}else
		{
			tp=temp;					//正数就是本身，转为浮点型方便运算
			temp=tp*0.0625*100+0.5;
		}
		return temp;
}

void ShowInformation()//ui==0
{
		/***温度显示部分***/
//		LcdPrint(2,0,"t=");			//写入温度显示的位置以及提示字
		WriteCom(0x88);
		if(temp<0)
			WriteDat('-');	  		//  温度为负则显示 -
		else
			WriteDat((temp/10000)+'0');	//百位只在正温度显示。因为ds18b20测负温度最低-55度,没必要显示负数的百位
		WriteDat((abs(temp)%10000/1000)+'0');//LCD显示十位
		WriteDat((abs(temp)%1000/100)+'0');	//LCD显示个位
		WriteDat('.');					//LCD显示小数点
		WriteDat((abs(temp)%100/10)+'0');	//LCD显示小数点后第一位
		WriteDat((abs(temp)%10)+'0');		//LCD显示小数点后第二位
		LcdPrint(2,3,"℃");
		/***显示温度上下限***/
		WriteCom(0x90+4);				//设置LCD显示地址在第2排倒数第4格
		WriteDat(' ');
		WriteDat('H');					//写入H，温度上限提示符
		if(tempH<0)
			WriteDat('-');//负数应该写入符号位
		else
			WriteDat(' ');//否则写个空格，和负号对齐会好看点
		WriteDat((abs(tempH)/10000)+'0');	
		WriteDat((abs(tempH)%10000/1000)+'0');//LCD显示十位
		WriteDat((abs(tempH)%1000/100)+'0');	//LCD显示个位
		WriteDat('.');					//LCD显示小数点
		WriteDat((abs(tempH)%100/10)+'0');	//LCD显示小数点后第一位
		//低温度
		WriteCom(0x88+4);				//设置LCD显示地址在第3排倒数第4格
		WriteDat(' ');
		WriteDat('L');					//写入L，温度下限提示符
		if(tempL<0)
			WriteDat('-');//负数应该写入符号位
		else
			WriteDat(' ');//否则写个空格，和负号对齐会好看点
		WriteDat((abs(tempL)/10000)+'0');	
		WriteDat((abs(tempL)%10000/1000)+'0');//LCD显示十位
		WriteDat((abs(tempL)%1000/100)+'0');	//LCD显示个位
		WriteDat('.');					//LCD显示小数点
		WriteDat((abs(tempL)%100/10)+'0');	//LCD显示小数点后第一位
		/***时间显示部分***/
		WriteCom(0x90);					//设置LCD显示地址在第2排起始
		WriteDat((TIME[2]/16)+'0');		//显示小时的十位
		WriteDat((TIME[2]&0x0f)+'0');	//显示小时的个位
		WriteDat(':');
		WriteDat((TIME[1]/16)+'0');		//显示分的十位
		WriteDat((TIME[1]&0x0f)+'0');	//显示分的个位
		WriteDat(':');
		WriteDat((TIME[0]/16)+'0');		//显示秒的十位
		WriteDat((TIME[0]&0x0f)+'0');	//显示秒的个位
		/***日期显示部分***/
		WriteCom(0x80);					//设置LCD显示地址在第1排起始
		WriteDat((TIME[6]/16)+'0');		//显示年的十位
		WriteDat((TIME[6]&0x0f)+'0');	//显示年时的个位
		LcdPrint(0,1,"年");
		WriteDat((TIME[4]/16)+'0');		//显示月的十位
		WriteDat((TIME[4]&0x0f)+'0');	//显示月的个位
		LcdPrint(0,3,"月");
		WriteDat((TIME[3]/16)+'0');		//显示日的十位
		WriteDat((TIME[3]&0x0f)+'0');	//显示日的个位
		LcdPrint(0,5,"日");
		switch(TIME[5])
		{
			case 1: LcdPrint(0,6,"周一");break;
			case 2: LcdPrint(0,6,"周二");break;
			case 3: LcdPrint(0,6,"周三\xFD");break;
			case 4: LcdPrint(0,6,"周四");break;
			case 5: LcdPrint(0,6,"周五");break;
			case 6: LcdPrint(0,6,"周六");break;
			case 7: LcdPrint(0,6,"周日");break;
		}
		/***显示闹钟时间及设置***/
		if(AlarmOn)
		{
			LcdPrint(3,0,"闹钟: ");
			WriteDat((AlarmTime[2]/16)+'0');		//显示闹钟小时的十位
			WriteDat((AlarmTime[2]&0x0f)+'0');		//显示闹钟小时的个位
			WriteDat(':');
			WriteDat((AlarmTime[1]/16)+'0');		//显示闹钟分的十位
			WriteDat((AlarmTime[1]&0x0f)+'0');		//显示闹钟分的个位
			WriteDat(':');
			WriteDat((AlarmTime[0]/16)+'0');		//显示闹钟秒的十位
			WriteDat((AlarmTime[0]&0x0f)+'0');		//显示闹钟秒的个位
		}
		else
			LcdPrint(3,0,"闹钟: 关");
}

void ShowSetting()//ui==1
{
	LcdPrint(0,0,"设置日期      ");
	LcdPrint(1,0,"设置时间      ");
	LcdPrint(2,0,"设置温控范围  ");
	LcdPrint(3,0,"设置闹钟      ");
	switch(IndexVer)//打印提示符
	{
		case 0:
			WriteCom(0x80+7);
			break;
		case 1:
			WriteCom(0x90+7);
			break;
		case 2:
			WriteCom(0x88+7);
			break;
		case 3:
			WriteCom(0x98+7);
			break;
	}
	WriteDat(0x1b);//显示一个左箭头
	WriteDat(' ');
}

void ShowDateSet()//ui==2
{
	LcdPrint(0,0,"设置日期: ");
	//显示设置值
	WriteCom(0x90);
	WriteDat(buff[0]+'0');
	WriteDat(buff[1]+'0');
	LcdPrint(1,1,"年");
	WriteDat(buff[2]+'0');
	WriteDat(buff[3]+'0');
	LcdPrint(1,3,"月");
	WriteDat(buff[4]+'0');
	WriteDat(buff[5]+'0');
	LcdPrint(1,5,"日");
	switch(buff[6])
	{
		case 1: LcdPrint(1,6,"周一");break;
		case 2: LcdPrint(1,6,"周二");break;
		case 3: LcdPrint(1,6,"周三\xFD");break;
		case 4: LcdPrint(1,6,"周四");break;
		case 5: LcdPrint(1,6,"周五");break;
		case 6: LcdPrint(1,6,"周六");break;
		case 7: LcdPrint(1,6,"周日");break;
	}
	//显示输入提示符
	switch(IndexHor)
	{
		case 0:WriteCom(0x88);break;
		case 1:
			WriteCom(0x88);
			WriteDat(' ');
			break;
		case 2:WriteCom(0x8a);break;
		case 3:
			WriteCom(0x8a);
			WriteDat(' ');
			break;
		case 4:WriteCom(0x8c);break;
		case 5:
			WriteCom(0x8c);
			WriteDat(' ');
			break;
		case 6:WriteCom(0x8f);break;
	}
	WriteDat(0x1e);//显示一个上箭头
}

void ShowTimeSet()//ui==3
{
	LcdPrint(0,0,"设置时间: ");
	//显示设置值
	WriteCom(0x90);
	WriteDat(buff[0]+'0');
	WriteDat(buff[1]+'0');
	LcdPrint(1,1,"时");
	WriteDat(buff[2]+'0');
	WriteDat(buff[3]+'0');
	LcdPrint(1,3,"分");
	WriteDat(buff[4]+'0');
	WriteDat(buff[5]+'0');
	LcdPrint(1,5,"秒");
	//显示输入提示符
	switch(IndexHor)
	{
		case 0:WriteCom(0x88);break;
		case 1:
			WriteCom(0x88);
			WriteDat(' ');
			break;
		case 2:WriteCom(0x8a);break;
		case 3:
			WriteCom(0x8a);
			WriteDat(' ');
			break;
		case 4:WriteCom(0x8c);break;
		case 5:
			WriteCom(0x8c);
			WriteDat(' ');
			break;
	}
	WriteDat(0x1e);//显示一个上箭头
}

void ShowTempSet()//ui==4
{
	LcdPrint(0,0,"设置温控范围: ");
	WriteCom(0x91);//设置地址
	WriteDat('H');//写个H,用来提示
	WriteCom(0x97);//设置地址
	WriteDat('L');//写个L,用来提示下限值
	WriteCom(0x88);
	if(!buff[0])
		WriteDat('+');
	else
		WriteDat('-');
	WriteDat(buff[1]+'0');
	WriteDat(buff[2]+'0');
	WriteDat(buff[3]+'0');
	WriteDat('.');
	WriteDat(buff[4]+'0');
	WriteCom(0x88+5);
	if(!buff[5])
		WriteDat('+');
	else
		WriteDat('-');
	WriteDat(buff[6]+'0');
	WriteDat(buff[7]+'0');
	WriteDat(buff[8]+'0');
	WriteDat('.');
	WriteDat(buff[9]+'0');
	//显示输入提示符
	switch(IndexHor)
	{
		case 0:WriteCom(0x98);break;//上限温度符号位
		case 1://上限温度百位
			WriteCom(0x98);
			WriteDat(' ');
			break;
		case 2:WriteCom(0x99);break;
		case 3:
			WriteCom(0x99);
			WriteDat(' ');
			break;
		case 4://上限温度小数位
			WriteCom(0x9a);
			WriteDat(' ');break;
		case 5:WriteCom(0x9d);break;//下限温度符号位
		case 6:
			WriteCom(0x9d);
			WriteDat(' ');
			break;
		case 7:WriteCom(0x9e);break;
		case 8:
			WriteCom(0x9e);
			WriteDat(' ');
			break;
		case 9:
			WriteCom(0x9f);
			WriteDat(' ');
			break;
	}
	WriteDat(0x1e);//显示一个上箭头
}

void ShowAlarmSet()//ui==5
{
	LcdPrint(0,0,"设置闹钟: ");
	if(buff[0])//0表示闹钟关
		LcdPrint(1,0,"开  ");
	else
		LcdPrint(1,0,"关  ");
	WriteDat(buff[1]+'0');
	WriteDat(buff[2]+'0');
	LcdPrint(1,3,"时");
	WriteDat(buff[3]+'0');
	WriteDat(buff[4]+'0');
	LcdPrint(1,5,"分");
	WriteDat(buff[5]+'0');
	WriteDat(buff[6]+'0');
	LcdPrint(1,7,"秒");
	//显示输入提示符
	switch(IndexHor)
	{
		case 0:WriteCom(0x88);break;
		case 1:WriteCom(0x8a);break;
		case 2:
			WriteCom(0x8a);
			WriteDat(' ');
			break;
		case 3:WriteCom(0x8c);break;//分
		case 4:
			WriteCom(0x8c);
			WriteDat(' ');
			break;
		case 5:WriteCom(0x8e);break;//秒
		case 6:
			WriteCom(0x8e);
			WriteDat(' ');
			break;
	}
	WriteDat(0x1e);//显示一个上箭头
}

void KeyPros()
{
		if(!_UP)
	{
		sleep(10);
		if(!_UP)
		{
			if(IsAlarm)
				IsAlarm=0;//任意键清除闹铃，且不触发其它功能
			else
			{
				switch(ui)
				{
					case 1://设置界面则竖向提示符移动
						if(IndexVer==0)
							IndexVer=3;
						else 
							IndexVer--;
						break;
					case 2://处于日期设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置
							case 1://年的十位百位允许0~9内变化
							case 3://月的个位也能0~9变化
							case 5://日的个位,0~9变化
								if(buff[IndexHor]==9)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://月,十位只能0,1变化
								if(buff[IndexHor]==1)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 4://日的十位,0~3变化
								if(buff[IndexHor]==3)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 6://周的变化,1~7
								if(buff[IndexHor]==7)
									buff[IndexHor]=1;
								else
									buff[IndexHor]++;
								break;
						}
						break;
					case 3://处于时间设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置，时的十位
								if(buff[IndexHor]==2)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 1://时的个位
							case 3://分的个位
							case 5://秒的个位
								if(buff[IndexHor]==9)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://分的十位
							case 4://秒的十位
								if(buff[IndexHor]==5)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
						}
						break;
					case 4://处于温控范围设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置，上限温度符号
							case 5://下限温度符号
								if(buff[IndexHor]==1)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://十位
							case 3://个位
							case 4://小数后一位
							case 7:
							case 8:
							case 9:
								if(buff[IndexHor]==9)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 1:
							case 6:
								if(buff[IndexHor]==1)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
						}
						break;
					case 5://处于闹钟设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置，开关的设置
								if(buff[IndexHor]==1)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 1://时的十位
								if(buff[IndexHor]==2)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://时的个位
							case 4://分的个位
							case 6://秒的个位
								if(buff[IndexHor]==9)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 3://分的十位
							case 5://秒的十位
								if(buff[IndexHor]==5)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
						}
						break;
				}
			}
			WriteCom(0x01);//清屏
		}
		while(!_UP);
	}
	
	if(!_DOWN)
	{
		sleep(10);
		if(!_DOWN)
		{
			if(IsAlarm)
				IsAlarm=0;//任意键清除闹铃
			else
			{
				switch(ui)
				{
					case 1:
						if(IndexVer==3)
							IndexVer=0;
						else 
							IndexVer++;
						break;
					case 2://处于日期设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置
							case 1://年的十位百位允许0~9内变化
							case 3://月的个位也能0~9变化
							case 5://日的个位,0~9变化
								if(buff[IndexHor]==0)
									buff[IndexHor]=9;
								else
									buff[IndexHor]--;
								break;
							case 2://月,十位只能0,1变化
								if(buff[IndexHor]==0)
									buff[IndexHor]=1;
								else
									buff[IndexHor]--;
								break;
							case 4://日的十位,0~3变化
								if(buff[IndexHor]==0)
									buff[IndexHor]=3;
								else
									buff[IndexHor]--;
								break;
							case 6://周的变化,1~7
								if(buff[IndexHor]==1)
								{
									buff[IndexHor]=7;
								}
								else
								{
									buff[IndexHor]--;
								}
								break;
						}
						break;
					case 3://处于时间设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置，时的十位
								if(buff[IndexHor]==0)
									buff[IndexHor]=2;
								else
									buff[IndexHor]--;
								break;
							case 1://时的个位
							case 3://分的个位
							case 5://秒的个位
								if(buff[IndexHor]==0)
									buff[IndexHor]=9;
								else
									buff[IndexHor]--;
								break;
							case 2://分的十位
							case 4://秒的十位
								if(buff[IndexHor]==0)
									buff[IndexHor]=5;
								else
									buff[IndexHor]--;
								break;
						}
						break;
					case 4://处于温控范围设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置，上限温度符号
							case 5://下限温度符号
								if(buff[IndexHor]==0)
									buff[IndexHor]=1;
								else
									buff[IndexHor]--;
								break;
							case 2://十位
							case 3://个位
							case 4://小数后一位
							case 7:
							case 8:
							case 9:
								if(buff[IndexHor]==0)
									buff[IndexHor]=9;
								else
									buff[IndexHor]--;
								break;
							case 1:
							case 6:
								if(buff[IndexHor]==0)
									buff[IndexHor]=1;
								else
									buff[IndexHor]--;
								break;
						}
						break;
					case 5://处于闹钟设置界面
						switch(IndexHor)
						{
							case 0://若提示符在0位置，开关的设置
								if(buff[IndexHor]==0)
									buff[IndexHor]=1;
								else
									buff[IndexHor]--;
								break;
							case 1://时的十位
								if(buff[IndexHor]==0)
									buff[IndexHor]=2;
								else
									buff[IndexHor]--;
								break;
							case 2://时的个位
							case 4://分的个位
							case 6://秒的个位
								if(buff[IndexHor]==0)
									buff[IndexHor]=9;
								else
									buff[IndexHor]--;
								break;
							case 3://分的十位
							case 5://秒的十位
								if(buff[IndexHor]==0)
									buff[IndexHor]=5;
								else
									buff[IndexHor]--;
						}
						break;
				}
			}
			WriteCom(0x01);//清屏;
		}
		while(!_DOWN);
	}
	
	if(!_LEFT)
	{
		sleep(10);
		if(!_LEFT)
		{
			if(IsAlarm)
				IsAlarm=0;//任意键清除闹铃
			else	
			{
				switch(ui)
				{
					case 2://日期设置中的左右移动
					case 5://2和5都是移动6次，就不重复写了
						if(IndexHor==0)
							IndexHor=6;
						else 
							IndexHor--;
						break;
					case 3://时间
						if(IndexHor==0)
							IndexHor=5;
						else 
							IndexHor--;
						break;
					case 4://温控范围
						if(IndexHor==0)
							IndexHor=9;
						else 
							IndexHor--;
						break;
				}
		}
		WriteCom(0x01);//清屏
		}
		while(!_LEFT);
	}
	
	if(!_RIGHT)
	{
		sleep(10);
		if(!_RIGHT)
		{
			if(IsAlarm)
				IsAlarm=0;//任意键清除闹铃
			else
			{
				switch(ui)
				{
					case 2:
					case 5:
						if(IndexHor==6)
							IndexHor=0;
						else 
							IndexHor++;
						break;
					case 3:
						if(IndexHor==5)
							IndexHor=0;
						else 
							IndexHor++;
						break;
					case 4://温控范围
						if(IndexHor==9)
							IndexHor=0;
						else 
							IndexHor++;
						break;
				}
			}
			WriteCom(0x01);//清屏
		}
		while(!_RIGHT);
	}
	
	if(!_OK)
	{
		sleep(10);
		if(!_OK)
		{
			if(IsAlarm)
				IsAlarm=0;//任意键清除闹铃
			else
			{
				switch(ui)
				{
					case 0://信息界面则
						IndexVer=IndexHor=0;//使得每次重新进入设置，提示符全部重新指向第0个坐标
						ui=1;	//转到设置界面
						break;
					case 1://设置界面则
						ui=2+IndexVer;
						switch(ui)//对各个设置界面的值给个初始的值
						{
							case 2://使日期设置的默认值为当前的进入时的日期
								buff[0]=TIME[6]/16;
								buff[1]=TIME[6]&0x0f;//年
								buff[2]=TIME[4]/16;
								buff[3]=TIME[4]&0x0f;//月
								buff[4]=TIME[3]/16;
								buff[5]=TIME[3]&0x0f;//日
								buff[6]=TIME[5];//周
								break;
							case 3://使时间设置的默认值为当前的时间				
								buff[0]=TIME[2]/16;
								buff[1]=TIME[2]&0x0f;//时
								buff[2]=TIME[1]/16;
								buff[3]=TIME[1]&0x0f;//分
								buff[4]=TIME[0]/16;
								buff[5]=TIME[0]&0x0f;//秒
								break;
							case 4://设置温度上下限时的初始值
								if(tempH<0)
									buff[0]=1;//1表示负号，0表示正号
								else 
									buff[0]=0;
								buff[1]=abs(tempH)/10000;//温度百位
								buff[2]=abs(tempH)%10000/1000;//十位
								buff[3]=abs(tempH)%1000/100;//个位
								buff[4]=abs(tempH)%100/10;//小数后一位
								if(tempL<0)
									buff[5]=1;//1表示负号，0表示正号
								else 
									buff[5]=0;
								buff[6]=abs(tempL)/10000;//温度百位
								buff[7]=abs(tempL)%10000/1000;//十位
								buff[8]=abs(tempL)%1000/100;//个位
								buff[9]=abs(tempL)%100/10;//小数后一位
								break;
							case 5://获取当前设置闹钟值放入设置显示的初始值
								buff[0]=AlarmOn;
								buff[1]=AlarmTime[2]/16;
								buff[2]=AlarmTime[2]&0x0f;
								buff[3]=AlarmTime[1]/16;
								buff[4]=AlarmTime[1]&0x0f;
								buff[5]=AlarmTime[0]/16;
								buff[6]=AlarmTime[0]&0x0f;
								break;
						}
						break;
					case 2://处于日期设置界面按下确定?
						if(((buff[2]*16)|buff[3])<=0x12&&((buff[4]*16)|buff[5])<=0x31&&((buff[2]*16)|buff[3])!=0&&((buff[4]*16)|buff[5])!=0)//日、月不合要求不能确定
						{
							TIME[6]=(buff[0]*16)|buff[1];//把设置的值换算成日期存着,年
							TIME[4]=(buff[2]*16)|buff[3];//月
							TIME[3]=(buff[4]*16)|buff[5];//日
							TIME[5]=buff[6];//周
							Ds1302Write(0x8E,0X00);//关闭写保护
							Ds1302Write(0x86,TIME[3]);//写入日期到DS1302
							Ds1302Write(0x88,TIME[4]);
							Ds1302Write(0x8a,TIME[5]);
							Ds1302Write(0x8c,TIME[6]);
							Ds1302Write(0x8E,0x80);		 //打开写保护功能
							WriteCom(0x01);//提示设置成功
							LcdPrint(1,2,"设置成功！");
							sleep(800);//等待显示
							WriteCom(0x01);
							ui=0;//回到主界面
						}else
						{
							WriteCom(0x01);//清屏
							LcdPrint(1,2,"错误的值！");
							sleep(800);//等待显示
							WriteCom(0x01);
						}
						break;
					case 3://处于时间设置的界面
						if((buff[0]*16|buff[1])<=0x23)//不合要求不能确定，只检查小时，因为分和秒不可能出错
						{
							TIME[2]=(buff[0]*16)|buff[1];//时
							TIME[1]=(buff[2]*16)|buff[3];//分
							TIME[0]=(buff[4]*16)|buff[5];//秒
							Ds1302Write(0x8E,0X00);//关闭写保护
							Ds1302Write(0x80,TIME[0]);//写入时间到DS1302
							Ds1302Write(0x82,TIME[1]);
							Ds1302Write(0x84,TIME[2]);
							Ds1302Write(0x8E,0x80);		 //打开写保护功能
							WriteCom(0x01);//提示设置成功
							LcdPrint(1,2,"设置成功！");
							sleep(800);//等待显示
							WriteCom(0x01);
							ui=0;//回到主界面
						}else
						{
							WriteCom(0x01);//清屏
							LcdPrint(1,2,"错误的值！");
							sleep(800);//等待显示
							WriteCom(0x01);
						}
						break;
					case 4://处于温度上限设置界面
						if(!buff[0])//对温度正负判断
							tempH=buff[1]*10000+buff[2]*1000+buff[3]*100+buff[4]*10;
						else
							tempH=-(buff[1]*10000+buff[2]*1000+buff[3]*100+buff[4]*10);
						if(!buff[5])
							tempL=buff[6]*10000+buff[7]*1000+buff[8]*100+buff[9]*10;
						else
							tempL=-(buff[6]*10000+buff[7]*1000+buff[8]*100+buff[9]*10);
						if(tempH<tempL)//如果大小反了，交换两者
						{
							SWAP(tempH,tempL);
						}
						TR0=0;
						At24c02Write(5,tempH>>8);//存储温度上限高八位
						sleep(10);
						At24c02Write(6,tempH&0xff);//存储温度上限低八位
						sleep(10);
						At24c02Write(7,tempL>>8);//存储温度下限高八位
						sleep(10);
						At24c02Write(8,tempL&0xff);//存储温度下限低八位
						sleep(10);
						TR0=1;
						WriteCom(0x01);//提示设置成功
						LcdPrint(1,2,"设置成功！");
						sleep(800);//等待显示
						WriteCom(0x01);
						ui=0;//回到主界面
						break;
					case 5://处于闹钟设置界面
						if((buff[1]*16|buff[2])<=0x23)//不合要求不能确定，只检查小时，因为分和秒在限制下可能出错
						{
							AlarmOn=buff[0];
							AlarmTime[2]=buff[1]*16|buff[2];
							AlarmTime[1]=buff[3]*16|buff[4];
							AlarmTime[0]=buff[5]*16|buff[6];
							TR0=0;
							At24c02Write(1,AlarmTime[0]);
							sleep(10);
							At24c02Write(2,AlarmTime[1]);
							sleep(10);
							At24c02Write(3,AlarmTime[2]);
							sleep(10);
							At24c02Write(4,AlarmOn);
							sleep(10);
							TR0=1;
							WriteCom(0x01);//提示设置成功
							LcdPrint(1,2,"设置成功！");
							sleep(800);//等待显示
							WriteCom(0x01);
							ui=0;//回到主界面
						}else
						{
							WriteCom(0x01);//清屏
							LcdPrint(1,2,"错误的值！");
							sleep(800);//等待显示
							WriteCom(0x01);
						}
						break;
				}
			}
			WriteCom(0x01);//清屏
		}
		while(!_OK);
	}
	
	if(!_BACK)
	{
		sleep(10);
		if(!_BACK)
		{
			if(IsAlarm)
				IsAlarm=0;//任意键清除闹铃
			else 
			{
				IndexHor=0;//每次返回界面都使横向移动的提示符清零
				switch(ui)
				{
					case 1://若为设置界面
						ui=0;//转为信息界面
						break;
					case 2:
					case 3:
					case 4://2/3/4/5/界面的判断相同
					case 5:ui=1;break;//若在设置什么值，返回到设置界面
				}	
			}
			WriteCom(0x01);//清屏
		}
		while(!_BACK);
	}
}

void InitTimer0()//定时器0的中断，1ms
{
	TH0=0xFC;
	TL0=0x66;
	TMOD=0x01;
	EA=1;
	ET0=1;
	TR0=1;
}

void main()
{
	/***初始化，以及显示欢迎界面***/
	LcdInit();						//LCD初始化
	InitTimer0();		//初始化中断定时器0
	LcdPrint(0,3,"欢迎!");
	LcdPrint(1,2,"Welcome!");		//显示欢迎界面
	LcdPrint(2,0,"制作人员：万魏琪");
	LcdPrint(3,0,"罗好仪    吴承宪");
	/***读取存储器中的信息***/
	AlarmTime[0]=At24c02Read(1);sleep(1);
	AlarmTime[1]=At24c02Read(2);sleep(1);
	AlarmTime[2]=At24c02Read(3);sleep(1);
	AlarmOn=At24c02Read(4);sleep(1);
	tempH=At24c02Read(5)<<8|At24c02Read(6);
	sleep(1);
	tempL=At24c02Read(7)<<8|At24c02Read(8);
	sleep(1);
	temp=Ds18b20ReadTemp();	//先读取一次温度，避免开机时显示错误温度导致报警
	sleep(1500);						//欢迎界面持续1.5s将被擦除
	WriteCom(0x01);					//LCD发送擦除屏幕的指令
	while(1)
	{
		TR0=0;//防止中断打断，导致读取温度不正常
		Ds1302ReadTime();			//读取当前时间到 TIME[7]的变量
		temp=Ds18b20ReadTemp();		//读取温度
		TR0=1;
		temp=TempPros(temp);		//处理温度，变为实际值的百倍，带符号
		KeyPros();					//处理按键
		if(IsAlarm)		//给闹钟一个不同的界面
		{
			LcdPrint(0,0,"    闹钟时间到了");
			LcdPrint(1,0,"现在是          ");
			LcdPrint(2,0,"  ");
			WriteDat((TIME[2]/16)+'0');
			WriteDat((TIME[2]&0x0f)+'0');
			LcdPrint(2,2,"时");
			WriteDat((TIME[1]/16)+'0');
			WriteDat((TIME[1]&0x0f)+'0');
			LcdPrint(2,4,"分");
			WriteDat((TIME[0]/16)+'0');
			WriteDat((TIME[0]&0x0f)+'0');
			LcdPrint(2,6,"秒  ");
			LcdPrint(3,0,"按任意键关闭提醒");
			sleep(10);
		}
		else 
		{
			switch(ui)
			{
				case 0:ShowInformation();			//显示信息界面
					break;
				case 1:ShowSetting();					//显示设置界面
					break;
				case 2:ShowDateSet();					//设置日期的界面
					break;
				case 3:ShowTimeSet();					//设置时间的界面
					break;
				case 4:ShowTempSet();					//设置温控范围的界面
					break;
				case 5:ShowAlarmSet();				//设置闹钟的界面
					break;
			}
		}
		/***闹钟判断***/
		if(AlarmOn&&AlarmTime[0]==TIME[0]&&AlarmTime[1]==TIME[1]&&AlarmTime[2]==TIME[2])
		{
			IsAlarm=1;
		}
		/***温度报警的判断***/
		if(temp>tempH)
		{
			OutTemp=1;//温度报警
			Htemp=0;//使降温输出开启
			Ltemp=1;//高温输出关闭
		}
		else if(temp<tempL)
		{
			OutTemp=1;//温度报警
			Ltemp=0;	//使加温设备开启
			Htemp=1;//低温设备关闭
		}
		else
		{
			D0=D1=D2=D3=D4=1;//LED全部灭
			Htemp=Ltemp=1;
			OutTemp=0;
		}
	}
}

void Timer0() interrupt 1
{
	TH0=0xFC;
	TL0=0x66;
	cnt++;
	if(OutTemp||IsAlarm)
	{
		beep=~beep;
	}
	if(cnt>200&&OutTemp)//温控时的LED闪烁
	{
		D0=D1=D2=D3=D4=0;

	}
	if(cnt>400&&OutTemp)
	{
		cnt=0;
		D0=D1=D2=D3=D4=1;
	}
}