#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#define ZHONGZHI 3095
#define Position_Zero 0

int myabs(int a);
void TIM8_PWM_Init(void);
void	control_init(void);
void Xianfu_Pwm(int amplitude);
void Xianfu_Velocity(int amplitude_A,int amplitude_B,int amplitude_C,int amplitude_D);
int Incremental_PI_D (int Encoder,int Target);
int Position_PID (int Encoder,int Target);
void Set_Pwm(int motor_d);
int balance(float Angle);//Çã½ÇPD¿ØÖÆ
int Position_PID (int Encoder,int target);
#endif
