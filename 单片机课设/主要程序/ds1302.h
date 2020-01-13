#ifndef _DS1302_H_
#define _DS1302_H_

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

void Ds1302Write(uchar addr, uchar dat);//写数据到指定地址
uchar Ds1302Read(uchar addr);//读指定地址的数据
//void Ds1302Init();//初始化
void Ds1302ReadTime();//读取时间到TIME[]

extern uchar TIME[7];

#endif