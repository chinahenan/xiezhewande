#ifndef _LCD12864_H_
#define _LCD12864_H_

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

void ReadBusy();
void WriteCom(uchar com);
void WriteDat(uchar dat);
void LcdPrint(uchar y,uchar x,uchar *p);//ָ��λ����ʾ�ַ���
void LcdInit();
//void ShowImg(uchar *DData);
#endif