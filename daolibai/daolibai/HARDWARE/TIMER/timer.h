#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//通用定时器 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
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
