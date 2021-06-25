#include "key.h"
#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "control.h"

int key_num = 0;	//要为int型

float Velocity_KP=3.2,Velocity_KI=1.01;	          //速度控制PID参数
float Position_KP=10,Position_KD=130;	//位置控制PID
int Encoder_A,Encoder_B,Encoder_C,Encoder_D; 
float Balance_KP=35,Balance_KD=100;
long int Position_A,Position_B,Position_C,Position_D = 10000; //PID控制相关变量 
long int Target_A,Target_B,Target_C,Target_D = 50; //电机目标值 
long int Motor_A,Motor_B,Motor_C,Motor_D; //电机PWM变量
long int Position_PWM,Balance_PWM; //电机PWM变量
u16 adc_value;


int main(void)
{	
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);		//初始化串口波特率
	KEY_Init();
	Adc_Init();	//角位移传感器
	control_init();	//电机方向初始化
	TIM1_Int_Init();
	Encode_int_tim2();
	Encode_int_tim3();
	Encode_int_tim4();
	Encode_int_tim5();
	TIM8_PWM_Init();	//PWM输出初始化
	
//			TIM_SetCompare2(TIM8,myabs(1000));
//			GPIO_SetBits(GPIOB,GPIO_Pin_12);
//			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
//	
	while(1)
	{
		
		
		//printf("OK\r\n");
		key_num = KEY_Scan(1);
	}
	
}
