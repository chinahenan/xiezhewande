#ifndef _DS1302_H_
#define _DS1302_H_

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

void Ds1302Write(uchar addr, uchar dat);//д���ݵ�ָ����ַ
uchar Ds1302Read(uchar addr);//��ָ����ַ������
//void Ds1302Init();//��ʼ��
void Ds1302ReadTime();//��ȡʱ�䵽TIME[]

extern uchar TIME[7];

#endif