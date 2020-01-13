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
/***����***/
sbit _UP=P3^0;//��
sbit _DOWN=P3^1;//��
sbit _LEFT=P3^2;//��
sbit _RIGHT=P3^3;//�� 
sbit _BACK=P2^0;//����
sbit _OK=P2^1;//ȷ��
/***LED***/
sbit D0=P2^2;
sbit D1=P2^3;
sbit D2=P2^4;
sbit D3=P2^5;
sbit D4=P2^6;
/***������***/
sbit beep=P2^7;
/***�������***/
sbit Htemp=P3^4;
/***�������***/
sbit Ltemp=P3^5;
void sleep(uint t);
#endif