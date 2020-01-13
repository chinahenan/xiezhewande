#include "lcd12864.h"
#include "config.h"

sbit LCD_E=P1^2;
sbit LCD_RS=P1^0;
sbit LCD_WR=P1^1;
sbit LCD_RST=P1^3;
#define DAT P0

void ReadBusy()//��æ
{
	uchar timeout =0;
	DAT=0xff;
	LCD_E=0;LCD_RS=0;LCD_WR=1;
	LCD_E=1;
	while(DAT&0x80)
	{
		timeout++;
		sleep(1);
		if(timeout>=60)
			break;
	}
	LCD_E=0;
}

void WriteCom(uchar com)//д����
{
	ReadBusy();
	LCD_E=0;LCD_RS=0;LCD_WR=0;
	_nop_();
	_nop_();
	DAT=com;
	_nop_();
	_nop_();
	LCD_E=1;
	sleep(1);
	LCD_E=0;
}

void WriteDat(uchar dat)//д����
{
	ReadBusy();
	LCD_E=0;LCD_RS=1;LCD_WR=0;
	_nop_();
	_nop_();
	DAT=dat;
	_nop_();
	_nop_();
	LCD_E=1;
	sleep(1);
	LCD_E=0;
}

void LcdPrint(uchar y,uchar x,uchar *p)//��ָ��λ�ô�ӡ�ַ���
{
//	if(y<0||y>3||x<0||x>15)//��ֹ���볬��Χ
//	{
//		y=x=0;
//	}
	switch(y)
	{
		case 0: WriteCom(0x80+x);break;
		case 1:	WriteCom(0x90+x);break;
		case 2:	WriteCom(0x88+x);break;
		case 3:	WriteCom(0x98+x);break;
	}
	while(*p!=0)
	{
		WriteDat(*p);
		p++;
	}
}

void LcdInit()//��ʼ��
{

	LCD_RST=0;
	sleep(50);
	LCD_RST=1;
	sleep(400);//�ȴ����빤��״̬
	WriteCom(0x30);
	sleep(50);
	WriteCom(0x01);
	sleep(50);
	WriteCom(0x06);
	sleep(50);
	WriteCom(0x0c);
	sleep(50);
}

//void ShowImg(uchar *DData)
//{
//	uchar x,y,i;
//	uint tmp=0;
//	for(i=0;i<9;){		//���������ϰ������°�������Ϊ��ʼ��ַ��ͬ����Ҫ�ֿ�
//	for(x=0;x<32;x++){				//ÿ��32��
//		WriteCom(0x34);	//��ͼ�أ����书��
//		WriteCom(0x80+x);//�е�ַ
//		WriteCom(0x80+i);	//���׵�ַ���°������������е�ַ0X88
//		WriteCom(0x30);		
//		for(y=0;y<16;y++)	
//			WriteDat(DData[tmp+y]);//��ȡ����д��LCD��128����
//		tmp+=16;		
//	}
//	i+=8;
//	}
//	WriteCom(0x36);	//���书���趨,��ͼ��
//	WriteCom(0x30);
//}
