#include "ds1302.h"
#include "config.h"

sbit RST_1302=P1^5;
sbit SCK_1302=P1^7;
sbit IO_1302=P1^6;
										//	秒		分		时		日		月		周		年
uchar code ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};//写的地址，读的话每个加1
uchar TIME[7] = {0x02, 0x01, 0x17, 0x09, 0x12, 0x01, 0x19};//初始化时的默认时间
							//	秒		分		时		日		月		周		年

void Ds1302Write(uchar addr, uchar dat)
{
	uchar n;
	RST_1302 = 0;
	_nop_();

	SCK_1302 = 0;//先将SCLK置低电平。
	_nop_();
	RST_1302 = 1; //然后将RST(CE)置高电平。
	_nop_();

	for (n=0; n<8; n++)//开始传送八位地址命令
	{
		IO_1302 = addr & 0x01;//数据从低位开始传送
		addr >>= 1;
		SCK_1302 = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCK_1302 = 0;
		_nop_();
	}
	for (n=0; n<8; n++)//写入8位数据
	{
		IO_1302 = dat & 0x01;
		dat >>= 1;
		SCK_1302 = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCK_1302 = 0;
		_nop_();	
	}	
		 
	RST_1302 = 0;//传送数据结束
	_nop_();
}

uchar Ds1302Read(uchar addr)
{
	uchar n,dat,dat1;
	RST_1302 = 0;
	_nop_();

	SCK_1302 = 0;//先将SCLK置低电平。
	_nop_();
	RST_1302 = 1;//然后将RST(CE)置高电平。
	_nop_();

	for(n=0; n<8; n++)//开始传送八位地址命令
	{
		IO_1302 = addr & 0x01;//数据从低位开始传送
		addr >>= 1;
		SCK_1302 = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCK_1302 = 0;//DS1302下降沿时，放置数据
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)//读取8位数据
	{
		dat1 = IO_1302;//从最低位开始接收
		dat = (dat>>1) | (dat1<<7);
		SCK_1302 = 1;
		_nop_();
		SCK_1302 = 0;//DS1302下降沿时，放置数据
		_nop_();
	}

	RST_1302 = 0;//有bug的话删这里
	_nop_();	//以下为DS1302复位的稳定时间,必须的。
	SCK_1302 = 1;
	_nop_();
	IO_1302 = 0;
	_nop_();
	IO_1302 = 1;
	_nop_();
	return dat;	
}

//void Ds1302Init()//有电池备用电源，不需要
//{
//	uchar n;
//	Ds1302Write(0x8E,0X00);		 //禁止写保护，就是关闭写保护功能
//	for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
//	{
//		Ds1302Write(ADDR[n],TIME[n]);	
//	}
//	Ds1302Write(0x8E,0x80);		 //打开写保护功能
//}

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<7; n++)//读取7个字节的时钟信号：分秒时日月周年
	{
		TIME[n] = Ds1302Read(ADDR[n]+1);
	}
}




