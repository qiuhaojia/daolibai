#include "key.h"
#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "control.h"

int key_num = 0;	//ҪΪint��

float Velocity_KP=3.2,Velocity_KI=1.01;	          //�ٶȿ���PID����
float Position_KP=10,Position_KD=130;	//λ�ÿ���PID
int Encoder_A,Encoder_B,Encoder_C,Encoder_D; 
float Balance_KP=35,Balance_KD=100;
long int Position_A,Position_B,Position_C,Position_D = 10000; //PID������ر��� 
long int Target_A,Target_B,Target_C,Target_D = 50; //���Ŀ��ֵ 
long int Motor_A,Motor_B,Motor_C,Motor_D; //���PWM����
long int Position_PWM,Balance_PWM; //���PWM����
u16 adc_value;


int main(void)
{	
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(115200);		//��ʼ�����ڲ�����
	KEY_Init();
	Adc_Init();	//��λ�ƴ�����
	control_init();	//��������ʼ��
	TIM1_Int_Init();
	Encode_int_tim2();
	Encode_int_tim3();
	Encode_int_tim4();
	Encode_int_tim5();
	TIM8_PWM_Init();	//PWM�����ʼ��
	
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
