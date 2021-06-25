#include "timer.h"
#include "sys.h"
#include	"delay.h"  
#include "Send_Data.h"
#include "control.h"
#include "adc.h"
#include <stdlib.h>
#define ENCODER_TIM_PERIOD (u16)(65535)   //���ɴ���65535 ��ΪF103�Ķ�ʱ����16λ�ġ�

int read_encode = 0;
int read_encode_motor = 0;
int times_flag = 0;

//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}

void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//ȡ����λ  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;//������Ӧ���ȼ����������ȼ�   	    	  				   
}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

///��ʱ��1��һ��������ȡ����ֵ
void TIM1_Int_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 99; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������100Ϊ10ms
	TIM_TimeBaseStructure.TIM_Prescaler =7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM1, //TIM1
		TIM_IT_Update,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx����
							 
}


void TIM1_UP_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
//		    Read_Encoder(2); 	//��ǰ
//				Read_Encoder(3);	//��ǰ
			  read_encode = Read_Angle_Encoder(5);	//λ��ʽ
//			read_encode_motor=Read_Encoder(5);  //===��ȡ��������ֵ//�Һ�
	
			if(times_flag == 5)
			{
				////������				
				Position_PWM =  Position_PID(read_encode,Position_Zero);
				Send_Speed2(read_encode, Position_Zero,adc_value, ZHONGZHI);
				Xianfu_Pwm(3000);
				Set_Pwm(Position_PWM);
				times_flag = 0;
			}
			times_flag++;
//��λ�ƴ��������		
			adc_value = Get_Adc_Average(13,8); 
			Balance_PWM = balance(adc_value);
			Xianfu_Pwm(3000);
			Set_Pwm(Balance_PWM);		
			
			
////����PWM			
			Motor_D = -Position_PWM +Balance_PWM;
			Xianfu_Pwm(3000);
			Set_Pwm(Motor_D);
			
				TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 

		}
}
//��ʱ��2�ı���ģʽ��ʼ��
void	Encode_int_tim2(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//������ӳ��ʱ��
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);            // ��ӳ��TIM2��CH1��CH2��PA15��PB3
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʹ��TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PB
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	//PA6he7 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //��ʼ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//PA6he7 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOB, &GPIO_InitStructure);		         //��ʼ��	
	
 	//TIM2��ʼ��
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // ����Ƶ
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //��װ�ش���
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 
	 //������������
	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	//Reset counter
	TIM_SetCounter(TIM2,0);
	TIM_Cmd(TIM2, ENABLE);
}

////��ʱ��3�ı���ģʽ��ʼ��
void	Encode_int_tim3(void)
{
	 //GPIO������
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ��TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//PA6he7 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //��ʼ��
	
 	//TIM2��ʼ��
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; //0x0// ����Ƶ
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //��װ�ش���   0xffff
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
 
	 //������������
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;//10
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	//Reset counter
	TIM_SetCounter(TIM3,0);
	TIM_Cmd(TIM3, ENABLE);
}

//��ʱ��4�ı���ģʽ��ʼ��
void	Encode_int_tim4(void)
{
	 //GPIO������
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOB, &GPIO_InitStructure);		         //��ʼ��
	
 	//TIM2��ʼ��
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // ����Ƶ
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //��װ�ش���
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 
	 //������������
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM_SetCounter(TIM4,0);
	TIM_Cmd(TIM4, ENABLE);
}
//��ʱ��5�ı���ģʽ��ʼ��
void	Encode_int_tim5(void)
{
	 //GPIO������
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʹ��TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0;	 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //��ʼ��
	
 	//TIM5��ʼ��
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // ����Ƶ
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //��װ�ش���
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
 
	 //������������
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	//Reset counter
	TIM_SetCounter(TIM5,0);
	TIM_Cmd(TIM5, ENABLE);
}


//��ȡ���������ֵ
int Read_Encoder(int TIMX)
{
	s32 Encoder_TIM;    
	switch(TIMX)
	{
		case 2:  Encoder_TIM= (short)TIM2 -> CNT;  TIM2 -> CNT=0;break;//��ǰ
		case 3:  Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0;break;//��ǰ	
		case 4:  Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0;break;//���	
		case 5:  Encoder_TIM= (short)TIM5 -> CNT;  TIM5 -> CNT=0;break;//�Һ�	
		default:  Encoder_TIM=0;
	}
	return Encoder_TIM;
}

//��ȡ��λ�Ʊ�����ֵ
int Read_Angle_Encoder(int TIMX)
{
	s32 Encoder_TIM;    
	switch(TIMX)
	{
		case 2:  Encoder_TIM= (short)TIM2 -> CNT;  break;//��ǰ
		case 3:  Encoder_TIM= (short)TIM3 -> CNT;  break;//��ǰ	
		case 4:  Encoder_TIM= (short)TIM4 -> CNT;  break;//���	
		case 5:  Encoder_TIM= (short)TIM5 -> CNT;  break;//�Һ�	
		default:  Encoder_TIM=0;
	}
	return Encoder_TIM;
}
//�жϺ���
void TIM2_IRQHandler(void)
{                                   
 if(TIM2->SR&0X0001)//�ж����
 {           

 }                  
 TIM2->SR&=~(1<<0);//����жϱ�־λ
}

//�жϺ���
void TIM3_IRQHandler(void)
{                                   
 if(TIM3->SR&0X0001)//�ж����
 {           

 }                  
 TIM3->SR&=~(1<<0);//����жϱ�־λ
}

//�жϺ���
void TIM4_IRQHandler(void)
{                                   
 if(TIM4->SR&0X0001)//�ж����
 {           

 }                  
 TIM4->SR&=~(1<<0);//����жϱ�־λ
}

//�жϺ���
void TIM5_IRQHandler(void)
{                                   
 if(TIM5->SR&0X0001)//�ж����
 {           

 }                  
 TIM5->SR&=~(1<<0);//����жϱ�־λ
}






