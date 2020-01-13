#include "lcd12864.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "at24c02.h"
#include "config.h"
#include <stdlib.h>
#define SWAP(a,b); {a=a^b;b=a^b;a=a^b;}//�꺯��������ֵ
//����,LED��������������������config.h
bit IsAlarm;//����ʱ�䵽�ˣ��ж������־��������һϵ�ж���
bit OutTemp;//�¶ȳ�����Χ��ͬ����������һϵ�ж���
uint cnt;//�жϼ����õģ��ﵽ���ֲ�ͬ������Ƶ��
bit AlarmOn;		//���ӿ�������1������0���������������ж������Ƿ���	
uchar AlarmTime[3]={0x00,0x00,0x18};	//�������õ�ʱ��,�룬�֣�ʱ			
int temp;//��ȡ�����¶�
float tp;
int tempH=12500;																			
int tempL=-5500;//�¶�����ֵ������ֵ�������ٱ���			
uchar buff[10];//�洢����ʱѡ��ֵ
uchar IndexHor=0;//�����ƶ��ļ�ͷ��ʾ����λ��
uchar IndexVer=0;//�����ƶ��ļ�ͷ��ʾ����λ��
uchar ui=0;
/**********************************************
����ui��ʾ�ĺ���:
	0:Ĭ�ϵ���ʾ��Ϣ�Ľ��棬��ʱ�䡢�¶ȡ����ڡ��ܡ������������Ϣ��
	1:���ý���

	2:�������ý���
	3:ʱ�����ý���
	4:�¶����������ý���
	5:�������ý���
**********************************************/

int TempPros(int temp)
{
	if(temp<0)
		{
			tp=~(temp-1);				//������ʱ����¶�-1��ȡ��	
			temp=tp*0.0625*100+0.5;//�õ���ʵ�¶�ֵ�İٱ��ľ���ֵ�����ͷ������ȽϺ͵���
			temp=-temp;						//��תΪ�����ŵİٱ���ֵ
		}else
		{
			tp=temp;					//�������Ǳ���תΪ�����ͷ�������
			temp=tp*0.0625*100+0.5;
		}
		return temp;
}

void ShowInformation()//ui==0
{
		/***�¶���ʾ����***/
//		LcdPrint(2,0,"t=");			//д���¶���ʾ��λ���Լ���ʾ��
		WriteCom(0x88);
		if(temp<0)
			WriteDat('-');	  		//  �¶�Ϊ������ʾ -
		else
			WriteDat((temp/10000)+'0');	//��λֻ�����¶���ʾ����Ϊds18b20�⸺�¶����-55��,û��Ҫ��ʾ�����İ�λ
		WriteDat((abs(temp)%10000/1000)+'0');//LCD��ʾʮλ
		WriteDat((abs(temp)%1000/100)+'0');	//LCD��ʾ��λ
		WriteDat('.');					//LCD��ʾС����
		WriteDat((abs(temp)%100/10)+'0');	//LCD��ʾС������һλ
		WriteDat((abs(temp)%10)+'0');		//LCD��ʾС�����ڶ�λ
		LcdPrint(2,3,"��");
		/***��ʾ�¶�������***/
		WriteCom(0x90+4);				//����LCD��ʾ��ַ�ڵ�2�ŵ�����4��
		WriteDat(' ');
		WriteDat('H');					//д��H���¶�������ʾ��
		if(tempH<0)
			WriteDat('-');//����Ӧ��д�����λ
		else
			WriteDat(' ');//����д���ո񣬺͸��Ŷ����ÿ���
		WriteDat((abs(tempH)/10000)+'0');	
		WriteDat((abs(tempH)%10000/1000)+'0');//LCD��ʾʮλ
		WriteDat((abs(tempH)%1000/100)+'0');	//LCD��ʾ��λ
		WriteDat('.');					//LCD��ʾС����
		WriteDat((abs(tempH)%100/10)+'0');	//LCD��ʾС������һλ
		//���¶�
		WriteCom(0x88+4);				//����LCD��ʾ��ַ�ڵ�3�ŵ�����4��
		WriteDat(' ');
		WriteDat('L');					//д��L���¶�������ʾ��
		if(tempL<0)
			WriteDat('-');//����Ӧ��д�����λ
		else
			WriteDat(' ');//����д���ո񣬺͸��Ŷ����ÿ���
		WriteDat((abs(tempL)/10000)+'0');	
		WriteDat((abs(tempL)%10000/1000)+'0');//LCD��ʾʮλ
		WriteDat((abs(tempL)%1000/100)+'0');	//LCD��ʾ��λ
		WriteDat('.');					//LCD��ʾС����
		WriteDat((abs(tempL)%100/10)+'0');	//LCD��ʾС������һλ
		/***ʱ����ʾ����***/
		WriteCom(0x90);					//����LCD��ʾ��ַ�ڵ�2����ʼ
		WriteDat((TIME[2]/16)+'0');		//��ʾСʱ��ʮλ
		WriteDat((TIME[2]&0x0f)+'0');	//��ʾСʱ�ĸ�λ
		WriteDat(':');
		WriteDat((TIME[1]/16)+'0');		//��ʾ�ֵ�ʮλ
		WriteDat((TIME[1]&0x0f)+'0');	//��ʾ�ֵĸ�λ
		WriteDat(':');
		WriteDat((TIME[0]/16)+'0');		//��ʾ���ʮλ
		WriteDat((TIME[0]&0x0f)+'0');	//��ʾ��ĸ�λ
		/***������ʾ����***/
		WriteCom(0x80);					//����LCD��ʾ��ַ�ڵ�1����ʼ
		WriteDat((TIME[6]/16)+'0');		//��ʾ���ʮλ
		WriteDat((TIME[6]&0x0f)+'0');	//��ʾ��ʱ�ĸ�λ
		LcdPrint(0,1,"��");
		WriteDat((TIME[4]/16)+'0');		//��ʾ�µ�ʮλ
		WriteDat((TIME[4]&0x0f)+'0');	//��ʾ�µĸ�λ
		LcdPrint(0,3,"��");
		WriteDat((TIME[3]/16)+'0');		//��ʾ�յ�ʮλ
		WriteDat((TIME[3]&0x0f)+'0');	//��ʾ�յĸ�λ
		LcdPrint(0,5,"��");
		switch(TIME[5])
		{
			case 1: LcdPrint(0,6,"��һ");break;
			case 2: LcdPrint(0,6,"�ܶ�");break;
			case 3: LcdPrint(0,6,"����\xFD");break;
			case 4: LcdPrint(0,6,"����");break;
			case 5: LcdPrint(0,6,"����");break;
			case 6: LcdPrint(0,6,"����");break;
			case 7: LcdPrint(0,6,"����");break;
		}
		/***��ʾ����ʱ�估����***/
		if(AlarmOn)
		{
			LcdPrint(3,0,"����: ");
			WriteDat((AlarmTime[2]/16)+'0');		//��ʾ����Сʱ��ʮλ
			WriteDat((AlarmTime[2]&0x0f)+'0');		//��ʾ����Сʱ�ĸ�λ
			WriteDat(':');
			WriteDat((AlarmTime[1]/16)+'0');		//��ʾ���ӷֵ�ʮλ
			WriteDat((AlarmTime[1]&0x0f)+'0');		//��ʾ���ӷֵĸ�λ
			WriteDat(':');
			WriteDat((AlarmTime[0]/16)+'0');		//��ʾ�������ʮλ
			WriteDat((AlarmTime[0]&0x0f)+'0');		//��ʾ������ĸ�λ
		}
		else
			LcdPrint(3,0,"����: ��");
}

void ShowSetting()//ui==1
{
	LcdPrint(0,0,"��������      ");
	LcdPrint(1,0,"����ʱ��      ");
	LcdPrint(2,0,"�����¿ط�Χ  ");
	LcdPrint(3,0,"��������      ");
	switch(IndexVer)//��ӡ��ʾ��
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
	WriteDat(0x1b);//��ʾһ�����ͷ
	WriteDat(' ');
}

void ShowDateSet()//ui==2
{
	LcdPrint(0,0,"��������: ");
	//��ʾ����ֵ
	WriteCom(0x90);
	WriteDat(buff[0]+'0');
	WriteDat(buff[1]+'0');
	LcdPrint(1,1,"��");
	WriteDat(buff[2]+'0');
	WriteDat(buff[3]+'0');
	LcdPrint(1,3,"��");
	WriteDat(buff[4]+'0');
	WriteDat(buff[5]+'0');
	LcdPrint(1,5,"��");
	switch(buff[6])
	{
		case 1: LcdPrint(1,6,"��һ");break;
		case 2: LcdPrint(1,6,"�ܶ�");break;
		case 3: LcdPrint(1,6,"����\xFD");break;
		case 4: LcdPrint(1,6,"����");break;
		case 5: LcdPrint(1,6,"����");break;
		case 6: LcdPrint(1,6,"����");break;
		case 7: LcdPrint(1,6,"����");break;
	}
	//��ʾ������ʾ��
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
	WriteDat(0x1e);//��ʾһ���ϼ�ͷ
}

void ShowTimeSet()//ui==3
{
	LcdPrint(0,0,"����ʱ��: ");
	//��ʾ����ֵ
	WriteCom(0x90);
	WriteDat(buff[0]+'0');
	WriteDat(buff[1]+'0');
	LcdPrint(1,1,"ʱ");
	WriteDat(buff[2]+'0');
	WriteDat(buff[3]+'0');
	LcdPrint(1,3,"��");
	WriteDat(buff[4]+'0');
	WriteDat(buff[5]+'0');
	LcdPrint(1,5,"��");
	//��ʾ������ʾ��
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
	WriteDat(0x1e);//��ʾһ���ϼ�ͷ
}

void ShowTempSet()//ui==4
{
	LcdPrint(0,0,"�����¿ط�Χ: ");
	WriteCom(0x91);//���õ�ַ
	WriteDat('H');//д��H,������ʾ
	WriteCom(0x97);//���õ�ַ
	WriteDat('L');//д��L,������ʾ����ֵ
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
	//��ʾ������ʾ��
	switch(IndexHor)
	{
		case 0:WriteCom(0x98);break;//�����¶ȷ���λ
		case 1://�����¶Ȱ�λ
			WriteCom(0x98);
			WriteDat(' ');
			break;
		case 2:WriteCom(0x99);break;
		case 3:
			WriteCom(0x99);
			WriteDat(' ');
			break;
		case 4://�����¶�С��λ
			WriteCom(0x9a);
			WriteDat(' ');break;
		case 5:WriteCom(0x9d);break;//�����¶ȷ���λ
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
	WriteDat(0x1e);//��ʾһ���ϼ�ͷ
}

void ShowAlarmSet()//ui==5
{
	LcdPrint(0,0,"��������: ");
	if(buff[0])//0��ʾ���ӹ�
		LcdPrint(1,0,"��  ");
	else
		LcdPrint(1,0,"��  ");
	WriteDat(buff[1]+'0');
	WriteDat(buff[2]+'0');
	LcdPrint(1,3,"ʱ");
	WriteDat(buff[3]+'0');
	WriteDat(buff[4]+'0');
	LcdPrint(1,5,"��");
	WriteDat(buff[5]+'0');
	WriteDat(buff[6]+'0');
	LcdPrint(1,7,"��");
	//��ʾ������ʾ��
	switch(IndexHor)
	{
		case 0:WriteCom(0x88);break;
		case 1:WriteCom(0x8a);break;
		case 2:
			WriteCom(0x8a);
			WriteDat(' ');
			break;
		case 3:WriteCom(0x8c);break;//��
		case 4:
			WriteCom(0x8c);
			WriteDat(' ');
			break;
		case 5:WriteCom(0x8e);break;//��
		case 6:
			WriteCom(0x8e);
			WriteDat(' ');
			break;
	}
	WriteDat(0x1e);//��ʾһ���ϼ�ͷ
}

void KeyPros()
{
		if(!_UP)
	{
		sleep(10);
		if(!_UP)
		{
			if(IsAlarm)
				IsAlarm=0;//�����������壬�Ҳ�������������
			else
			{
				switch(ui)
				{
					case 1://���ý�����������ʾ���ƶ�
						if(IndexVer==0)
							IndexVer=3;
						else 
							IndexVer--;
						break;
					case 2://�����������ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ��
							case 1://���ʮλ��λ����0~9�ڱ仯
							case 3://�µĸ�λҲ��0~9�仯
							case 5://�յĸ�λ,0~9�仯
								if(buff[IndexHor]==9)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://��,ʮλֻ��0,1�仯
								if(buff[IndexHor]==1)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 4://�յ�ʮλ,0~3�仯
								if(buff[IndexHor]==3)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 6://�ܵı仯,1~7
								if(buff[IndexHor]==7)
									buff[IndexHor]=1;
								else
									buff[IndexHor]++;
								break;
						}
						break;
					case 3://����ʱ�����ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ�ã�ʱ��ʮλ
								if(buff[IndexHor]==2)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 1://ʱ�ĸ�λ
							case 3://�ֵĸ�λ
							case 5://��ĸ�λ
								if(buff[IndexHor]==9)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://�ֵ�ʮλ
							case 4://���ʮλ
								if(buff[IndexHor]==5)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
						}
						break;
					case 4://�����¿ط�Χ���ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ�ã������¶ȷ���
							case 5://�����¶ȷ���
								if(buff[IndexHor]==1)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://ʮλ
							case 3://��λ
							case 4://С����һλ
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
					case 5://�����������ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ�ã����ص�����
								if(buff[IndexHor]==1)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 1://ʱ��ʮλ
								if(buff[IndexHor]==2)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 2://ʱ�ĸ�λ
							case 4://�ֵĸ�λ
							case 6://��ĸ�λ
								if(buff[IndexHor]==9)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
								break;
							case 3://�ֵ�ʮλ
							case 5://���ʮλ
								if(buff[IndexHor]==5)
									buff[IndexHor]=0;
								else
									buff[IndexHor]++;
						}
						break;
				}
			}
			WriteCom(0x01);//����
		}
		while(!_UP);
	}
	
	if(!_DOWN)
	{
		sleep(10);
		if(!_DOWN)
		{
			if(IsAlarm)
				IsAlarm=0;//������������
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
					case 2://�����������ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ��
							case 1://���ʮλ��λ����0~9�ڱ仯
							case 3://�µĸ�λҲ��0~9�仯
							case 5://�յĸ�λ,0~9�仯
								if(buff[IndexHor]==0)
									buff[IndexHor]=9;
								else
									buff[IndexHor]--;
								break;
							case 2://��,ʮλֻ��0,1�仯
								if(buff[IndexHor]==0)
									buff[IndexHor]=1;
								else
									buff[IndexHor]--;
								break;
							case 4://�յ�ʮλ,0~3�仯
								if(buff[IndexHor]==0)
									buff[IndexHor]=3;
								else
									buff[IndexHor]--;
								break;
							case 6://�ܵı仯,1~7
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
					case 3://����ʱ�����ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ�ã�ʱ��ʮλ
								if(buff[IndexHor]==0)
									buff[IndexHor]=2;
								else
									buff[IndexHor]--;
								break;
							case 1://ʱ�ĸ�λ
							case 3://�ֵĸ�λ
							case 5://��ĸ�λ
								if(buff[IndexHor]==0)
									buff[IndexHor]=9;
								else
									buff[IndexHor]--;
								break;
							case 2://�ֵ�ʮλ
							case 4://���ʮλ
								if(buff[IndexHor]==0)
									buff[IndexHor]=5;
								else
									buff[IndexHor]--;
								break;
						}
						break;
					case 4://�����¿ط�Χ���ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ�ã������¶ȷ���
							case 5://�����¶ȷ���
								if(buff[IndexHor]==0)
									buff[IndexHor]=1;
								else
									buff[IndexHor]--;
								break;
							case 2://ʮλ
							case 3://��λ
							case 4://С����һλ
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
					case 5://�����������ý���
						switch(IndexHor)
						{
							case 0://����ʾ����0λ�ã����ص�����
								if(buff[IndexHor]==0)
									buff[IndexHor]=1;
								else
									buff[IndexHor]--;
								break;
							case 1://ʱ��ʮλ
								if(buff[IndexHor]==0)
									buff[IndexHor]=2;
								else
									buff[IndexHor]--;
								break;
							case 2://ʱ�ĸ�λ
							case 4://�ֵĸ�λ
							case 6://��ĸ�λ
								if(buff[IndexHor]==0)
									buff[IndexHor]=9;
								else
									buff[IndexHor]--;
								break;
							case 3://�ֵ�ʮλ
							case 5://���ʮλ
								if(buff[IndexHor]==0)
									buff[IndexHor]=5;
								else
									buff[IndexHor]--;
						}
						break;
				}
			}
			WriteCom(0x01);//����;
		}
		while(!_DOWN);
	}
	
	if(!_LEFT)
	{
		sleep(10);
		if(!_LEFT)
		{
			if(IsAlarm)
				IsAlarm=0;//������������
			else	
			{
				switch(ui)
				{
					case 2://���������е������ƶ�
					case 5://2��5�����ƶ�6�Σ��Ͳ��ظ�д��
						if(IndexHor==0)
							IndexHor=6;
						else 
							IndexHor--;
						break;
					case 3://ʱ��
						if(IndexHor==0)
							IndexHor=5;
						else 
							IndexHor--;
						break;
					case 4://�¿ط�Χ
						if(IndexHor==0)
							IndexHor=9;
						else 
							IndexHor--;
						break;
				}
		}
		WriteCom(0x01);//����
		}
		while(!_LEFT);
	}
	
	if(!_RIGHT)
	{
		sleep(10);
		if(!_RIGHT)
		{
			if(IsAlarm)
				IsAlarm=0;//������������
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
					case 4://�¿ط�Χ
						if(IndexHor==9)
							IndexHor=0;
						else 
							IndexHor++;
						break;
				}
			}
			WriteCom(0x01);//����
		}
		while(!_RIGHT);
	}
	
	if(!_OK)
	{
		sleep(10);
		if(!_OK)
		{
			if(IsAlarm)
				IsAlarm=0;//������������
			else
			{
				switch(ui)
				{
					case 0://��Ϣ������
						IndexVer=IndexHor=0;//ʹ��ÿ�����½������ã���ʾ��ȫ������ָ���0������
						ui=1;	//ת�����ý���
						break;
					case 1://���ý�����
						ui=2+IndexVer;
						switch(ui)//�Ը������ý����ֵ������ʼ��ֵ
						{
							case 2://ʹ�������õ�Ĭ��ֵΪ��ǰ�Ľ���ʱ������
								buff[0]=TIME[6]/16;
								buff[1]=TIME[6]&0x0f;//��
								buff[2]=TIME[4]/16;
								buff[3]=TIME[4]&0x0f;//��
								buff[4]=TIME[3]/16;
								buff[5]=TIME[3]&0x0f;//��
								buff[6]=TIME[5];//��
								break;
							case 3://ʹʱ�����õ�Ĭ��ֵΪ��ǰ��ʱ��				
								buff[0]=TIME[2]/16;
								buff[1]=TIME[2]&0x0f;//ʱ
								buff[2]=TIME[1]/16;
								buff[3]=TIME[1]&0x0f;//��
								buff[4]=TIME[0]/16;
								buff[5]=TIME[0]&0x0f;//��
								break;
							case 4://�����¶�������ʱ�ĳ�ʼֵ
								if(tempH<0)
									buff[0]=1;//1��ʾ���ţ�0��ʾ����
								else 
									buff[0]=0;
								buff[1]=abs(tempH)/10000;//�¶Ȱ�λ
								buff[2]=abs(tempH)%10000/1000;//ʮλ
								buff[3]=abs(tempH)%1000/100;//��λ
								buff[4]=abs(tempH)%100/10;//С����һλ
								if(tempL<0)
									buff[5]=1;//1��ʾ���ţ�0��ʾ����
								else 
									buff[5]=0;
								buff[6]=abs(tempL)/10000;//�¶Ȱ�λ
								buff[7]=abs(tempL)%10000/1000;//ʮλ
								buff[8]=abs(tempL)%1000/100;//��λ
								buff[9]=abs(tempL)%100/10;//С����һλ
								break;
							case 5://��ȡ��ǰ��������ֵ����������ʾ�ĳ�ʼֵ
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
					case 2://�����������ý��水��ȷ��?
						if(((buff[2]*16)|buff[3])<=0x12&&((buff[4]*16)|buff[5])<=0x31&&((buff[2]*16)|buff[3])!=0&&((buff[4]*16)|buff[5])!=0)//�ա��²���Ҫ����ȷ��
						{
							TIME[6]=(buff[0]*16)|buff[1];//�����õ�ֵ��������ڴ���,��
							TIME[4]=(buff[2]*16)|buff[3];//��
							TIME[3]=(buff[4]*16)|buff[5];//��
							TIME[5]=buff[6];//��
							Ds1302Write(0x8E,0X00);//�ر�д����
							Ds1302Write(0x86,TIME[3]);//д�����ڵ�DS1302
							Ds1302Write(0x88,TIME[4]);
							Ds1302Write(0x8a,TIME[5]);
							Ds1302Write(0x8c,TIME[6]);
							Ds1302Write(0x8E,0x80);		 //��д��������
							WriteCom(0x01);//��ʾ���óɹ�
							LcdPrint(1,2,"���óɹ���");
							sleep(800);//�ȴ���ʾ
							WriteCom(0x01);
							ui=0;//�ص�������
						}else
						{
							WriteCom(0x01);//����
							LcdPrint(1,2,"�����ֵ��");
							sleep(800);//�ȴ���ʾ
							WriteCom(0x01);
						}
						break;
					case 3://����ʱ�����õĽ���
						if((buff[0]*16|buff[1])<=0x23)//����Ҫ����ȷ����ֻ���Сʱ����Ϊ�ֺ��벻���ܳ���
						{
							TIME[2]=(buff[0]*16)|buff[1];//ʱ
							TIME[1]=(buff[2]*16)|buff[3];//��
							TIME[0]=(buff[4]*16)|buff[5];//��
							Ds1302Write(0x8E,0X00);//�ر�д����
							Ds1302Write(0x80,TIME[0]);//д��ʱ�䵽DS1302
							Ds1302Write(0x82,TIME[1]);
							Ds1302Write(0x84,TIME[2]);
							Ds1302Write(0x8E,0x80);		 //��д��������
							WriteCom(0x01);//��ʾ���óɹ�
							LcdPrint(1,2,"���óɹ���");
							sleep(800);//�ȴ���ʾ
							WriteCom(0x01);
							ui=0;//�ص�������
						}else
						{
							WriteCom(0x01);//����
							LcdPrint(1,2,"�����ֵ��");
							sleep(800);//�ȴ���ʾ
							WriteCom(0x01);
						}
						break;
					case 4://�����¶��������ý���
						if(!buff[0])//���¶������ж�
							tempH=buff[1]*10000+buff[2]*1000+buff[3]*100+buff[4]*10;
						else
							tempH=-(buff[1]*10000+buff[2]*1000+buff[3]*100+buff[4]*10);
						if(!buff[5])
							tempL=buff[6]*10000+buff[7]*1000+buff[8]*100+buff[9]*10;
						else
							tempL=-(buff[6]*10000+buff[7]*1000+buff[8]*100+buff[9]*10);
						if(tempH<tempL)//�����С���ˣ���������
						{
							SWAP(tempH,tempL);
						}
						TR0=0;
						At24c02Write(5,tempH>>8);//�洢�¶����޸߰�λ
						sleep(10);
						At24c02Write(6,tempH&0xff);//�洢�¶����޵Ͱ�λ
						sleep(10);
						At24c02Write(7,tempL>>8);//�洢�¶����޸߰�λ
						sleep(10);
						At24c02Write(8,tempL&0xff);//�洢�¶����޵Ͱ�λ
						sleep(10);
						TR0=1;
						WriteCom(0x01);//��ʾ���óɹ�
						LcdPrint(1,2,"���óɹ���");
						sleep(800);//�ȴ���ʾ
						WriteCom(0x01);
						ui=0;//�ص�������
						break;
					case 5://�����������ý���
						if((buff[1]*16|buff[2])<=0x23)//����Ҫ����ȷ����ֻ���Сʱ����Ϊ�ֺ����������¿��ܳ���
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
							WriteCom(0x01);//��ʾ���óɹ�
							LcdPrint(1,2,"���óɹ���");
							sleep(800);//�ȴ���ʾ
							WriteCom(0x01);
							ui=0;//�ص�������
						}else
						{
							WriteCom(0x01);//����
							LcdPrint(1,2,"�����ֵ��");
							sleep(800);//�ȴ���ʾ
							WriteCom(0x01);
						}
						break;
				}
			}
			WriteCom(0x01);//����
		}
		while(!_OK);
	}
	
	if(!_BACK)
	{
		sleep(10);
		if(!_BACK)
		{
			if(IsAlarm)
				IsAlarm=0;//������������
			else 
			{
				IndexHor=0;//ÿ�η��ؽ��涼ʹ�����ƶ�����ʾ������
				switch(ui)
				{
					case 1://��Ϊ���ý���
						ui=0;//תΪ��Ϣ����
						break;
					case 2:
					case 3:
					case 4://2/3/4/5/������ж���ͬ
					case 5:ui=1;break;//��������ʲôֵ�����ص����ý���
				}	
			}
			WriteCom(0x01);//����
		}
		while(!_BACK);
	}
}

void InitTimer0()//��ʱ��0���жϣ�1ms
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
	/***��ʼ�����Լ���ʾ��ӭ����***/
	LcdInit();						//LCD��ʼ��
	InitTimer0();		//��ʼ���ж϶�ʱ��0
	LcdPrint(0,3,"��ӭ!");
	LcdPrint(1,2,"Welcome!");		//��ʾ��ӭ����
	LcdPrint(2,0,"������Ա����κ��");
	LcdPrint(3,0,"�޺���    �����");
	/***��ȡ�洢���е���Ϣ***/
	AlarmTime[0]=At24c02Read(1);sleep(1);
	AlarmTime[1]=At24c02Read(2);sleep(1);
	AlarmTime[2]=At24c02Read(3);sleep(1);
	AlarmOn=At24c02Read(4);sleep(1);
	tempH=At24c02Read(5)<<8|At24c02Read(6);
	sleep(1);
	tempL=At24c02Read(7)<<8|At24c02Read(8);
	sleep(1);
	temp=Ds18b20ReadTemp();	//�ȶ�ȡһ���¶ȣ����⿪��ʱ��ʾ�����¶ȵ��±���
	sleep(1500);						//��ӭ�������1.5s��������
	WriteCom(0x01);					//LCD���Ͳ�����Ļ��ָ��
	while(1)
	{
		TR0=0;//��ֹ�жϴ�ϣ����¶�ȡ�¶Ȳ�����
		Ds1302ReadTime();			//��ȡ��ǰʱ�䵽 TIME[7]�ı���
		temp=Ds18b20ReadTemp();		//��ȡ�¶�
		TR0=1;
		temp=TempPros(temp);		//�����¶ȣ���Ϊʵ��ֵ�İٱ���������
		KeyPros();					//������
		if(IsAlarm)		//������һ����ͬ�Ľ���
		{
			LcdPrint(0,0,"    ����ʱ�䵽��");
			LcdPrint(1,0,"������          ");
			LcdPrint(2,0,"  ");
			WriteDat((TIME[2]/16)+'0');
			WriteDat((TIME[2]&0x0f)+'0');
			LcdPrint(2,2,"ʱ");
			WriteDat((TIME[1]/16)+'0');
			WriteDat((TIME[1]&0x0f)+'0');
			LcdPrint(2,4,"��");
			WriteDat((TIME[0]/16)+'0');
			WriteDat((TIME[0]&0x0f)+'0');
			LcdPrint(2,6,"��  ");
			LcdPrint(3,0,"��������ر�����");
			sleep(10);
		}
		else 
		{
			switch(ui)
			{
				case 0:ShowInformation();			//��ʾ��Ϣ����
					break;
				case 1:ShowSetting();					//��ʾ���ý���
					break;
				case 2:ShowDateSet();					//�������ڵĽ���
					break;
				case 3:ShowTimeSet();					//����ʱ��Ľ���
					break;
				case 4:ShowTempSet();					//�����¿ط�Χ�Ľ���
					break;
				case 5:ShowAlarmSet();				//�������ӵĽ���
					break;
			}
		}
		/***�����ж�***/
		if(AlarmOn&&AlarmTime[0]==TIME[0]&&AlarmTime[1]==TIME[1]&&AlarmTime[2]==TIME[2])
		{
			IsAlarm=1;
		}
		/***�¶ȱ������ж�***/
		if(temp>tempH)
		{
			OutTemp=1;//�¶ȱ���
			Htemp=0;//ʹ�����������
			Ltemp=1;//��������ر�
		}
		else if(temp<tempL)
		{
			OutTemp=1;//�¶ȱ���
			Ltemp=0;	//ʹ�����豸����
			Htemp=1;//�����豸�ر�
		}
		else
		{
			D0=D1=D2=D3=D4=1;//LEDȫ����
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
	if(cnt>200&&OutTemp)//�¿�ʱ��LED��˸
	{
		D0=D1=D2=D3=D4=0;

	}
	if(cnt>400&&OutTemp)
	{
		cnt=0;
		D0=D1=D2=D3=D4=1;
	}
}