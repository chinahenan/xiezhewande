#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

#include <reg51.h>
#include <intrins.h>
/***按键***/
sbit _UP=P3^0;//上
sbit _DOWN=P3^1;//下
sbit _LEFT=P3^2;//左
sbit _RIGHT=P3^3;//右 
sbit _BACK=P2^0;//返回
sbit _OK=P2^1;//确认
/***LED***/
sbit D0=P2^2;
sbit D1=P2^3;
sbit D2=P2^4;
sbit D3=P2^5;
sbit D4=P2^6;
/***蜂鸣器***/
sbit beep=P2^7;
/***高温输出***/
sbit Htemp=P3^4;
/***低温输出***/
sbit Ltemp=P3^5;
void sleep(uint t);
#endif