#include "timer.h"
#include "sys.h"
#include	"delay.h"  
#include "Send_Data.h"
#include "control.h"
#include "adc.h"
#include <stdlib.h>
#define ENCODER_TIM_PERIOD (u16)(65535)   //不可大于65535 因为F103的定时器是16位的。

int read_encode = 0;
int read_encode_motor = 0;
int times_flag = 0;

//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}

void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//取低四位  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//使能中断位(要清除的话,相反操作就OK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;//设置响应优先级和抢断优先级   	    	  				   
}

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

///定时器1按一定计数读取编码值
void TIM1_Int_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 99; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到100为10ms
	TIM_TimeBaseStructure.TIM_Prescaler =7199; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM1, //TIM1
		TIM_IT_Update,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设
							 
}


void TIM1_UP_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
//		    Read_Encoder(2); 	//左前
//				Read_Encoder(3);	//右前
			  read_encode = Read_Angle_Encoder(5);	//位置式
//			read_encode_motor=Read_Encoder(5);  //===读取编码器的值//右后
	
			if(times_flag == 5)
			{
				////电机相关				
				Position_PWM =  Position_PID(read_encode,Position_Zero);
				Send_Speed2(read_encode, Position_Zero,adc_value, ZHONGZHI);
				Xianfu_Pwm(3000);
				Set_Pwm(Position_PWM);
				times_flag = 0;
			}
			times_flag++;
//角位移传感器相关		
			adc_value = Get_Adc_Average(13,8); 
			Balance_PWM = balance(adc_value);
			Xianfu_Pwm(3000);
			Set_Pwm(Balance_PWM);		
			
			
////最终PWM			
			Motor_D = -Position_PWM +Balance_PWM;
			Xianfu_Pwm(3000);
			Set_Pwm(Motor_D);
			
				TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 

		}
}
//定时器2的编码模式初始化
void	Encode_int_tim2(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//开启重映射时钟
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);            // 重映射TIM2的CH1、CH2到PA15和PB3
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //使能TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PB
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	//PA6he7 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//PA6he7 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOB, &GPIO_InitStructure);		         //初始化	
	
 	//TIM2初始化
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 不分频
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //重装载处置
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //选择时钟分频，不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 
	 //编码器的配置
	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); //清楚TIM的更新标志位
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	//Reset counter
	TIM_SetCounter(TIM2,0);
	TIM_Cmd(TIM2, ENABLE);
}

////定时器3的编码模式初始化
void	Encode_int_tim3(void)
{
	 //GPIO的配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//PA6he7 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //初始化
	
 	//TIM2初始化
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; //0x0// 不分频
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //重装载处置   0xffff
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //选择时钟分频，不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
 
	 //编码器的配置
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;//10
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); //清楚TIM的更新标志位
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	//Reset counter
	TIM_SetCounter(TIM3,0);
	TIM_Cmd(TIM3, ENABLE);
}

//定时器4的编码模式初始化
void	Encode_int_tim4(void)
{
	 //GPIO的配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOB, &GPIO_InitStructure);		         //初始化
	
 	//TIM2初始化
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 不分频
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //重装载处置
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //选择时钟分频，不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 
	 //编码器的配置
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update); //清楚TIM的更新标志位
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM_SetCounter(TIM4,0);
	TIM_Cmd(TIM4, ENABLE);
}
//定时器5的编码模式初始化
void	Encode_int_tim5(void)
{
	 //GPIO的配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //使能TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0;	 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //初始化
	
 	//TIM5初始化
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 不分频
	TIM_TimeBaseStructure.TIM_Period = 0xffff;  //重装载处置
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //选择时钟分频，不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
 
	 //编码器的配置
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); //清楚TIM的更新标志位
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	//Reset counter
	TIM_SetCounter(TIM5,0);
	TIM_Cmd(TIM5, ENABLE);
}


//读取电机编码器值
int Read_Encoder(int TIMX)
{
	s32 Encoder_TIM;    
	switch(TIMX)
	{
		case 2:  Encoder_TIM= (short)TIM2 -> CNT;  TIM2 -> CNT=0;break;//左前
		case 3:  Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0;break;//右前	
		case 4:  Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0;break;//左后	
		case 5:  Encoder_TIM= (short)TIM5 -> CNT;  TIM5 -> CNT=0;break;//右后	
		default:  Encoder_TIM=0;
	}
	return Encoder_TIM;
}

//读取角位移编码器值
int Read_Angle_Encoder(int TIMX)
{
	s32 Encoder_TIM;    
	switch(TIMX)
	{
		case 2:  Encoder_TIM= (short)TIM2 -> CNT;  break;//左前
		case 3:  Encoder_TIM= (short)TIM3 -> CNT;  break;//右前	
		case 4:  Encoder_TIM= (short)TIM4 -> CNT;  break;//左后	
		case 5:  Encoder_TIM= (short)TIM5 -> CNT;  break;//右后	
		default:  Encoder_TIM=0;
	}
	return Encoder_TIM;
}
//中断函数
void TIM2_IRQHandler(void)
{                                   
 if(TIM2->SR&0X0001)//中断溢出
 {           

 }                  
 TIM2->SR&=~(1<<0);//清除中断标志位
}

//中断函数
void TIM3_IRQHandler(void)
{                                   
 if(TIM3->SR&0X0001)//中断溢出
 {           

 }                  
 TIM3->SR&=~(1<<0);//清除中断标志位
}

//中断函数
void TIM4_IRQHandler(void)
{                                   
 if(TIM4->SR&0X0001)//中断溢出
 {           

 }                  
 TIM4->SR&=~(1<<0);//清除中断标志位
}

//中断函数
void TIM5_IRQHandler(void)
{                                   
 if(TIM5->SR&0X0001)//中断溢出
 {           

 }                  
 TIM5->SR&=~(1<<0);//清除中断标志位
}






