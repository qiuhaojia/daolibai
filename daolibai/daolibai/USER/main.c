#include "key.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "control.h"

int key_num = 0;	//ҪΪint��

int Encoder_A,Encoder_B,Encoder_C,Encoder_D; 
long int Position_A,Position_B,Position_C,Position_D,Rate_A,Rate_B,Rate_C,Rate_D; //PID������ر��� 
long int Target_A,Target_B,Target_C,Target_D; //���Ŀ��ֵ 
long int Motor_A,Motor_B,Motor_C,Motor_D; //���PWM����

float Velocity_KP=10,Velocity_KI=10;	          //�ٶȿ���PID����

int main(void)
{	
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(115200);		//��ʼ�����ڲ�����
	KEY_Init();
	TIM1_Int_Init();
	Encode_int_tim2();
	Encode_int_tim3();
	Encode_int_tim4();
	Encode_int_tim5();
	TIM8_PWM_Init();	//PWM�����ʼ��
	control_init();	//��������ʼ��
	
	TIM_SetCompare2(TIM8,1600);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
	
	while(1)
	{
		

		//printf("OK\r\n");
		key_num = KEY_Scan(1);
	}
	
}
