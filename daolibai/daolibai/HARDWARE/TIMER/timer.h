#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ͨ�ö�ʱ�� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  




int Read_Encoder(int TIMX);
int Read_Angle_Encoder(int TIMX);
void 	TIM1_Int_Init(void);
void	Encode_int_tim2(void);
void	Encode_int_tim3(void);
void	Encode_int_tim4(void);
void	Encode_int_tim5(void);


 
#endif
