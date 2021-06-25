#include "control.h"
#include "timer.h"	//这个头文件定义了PWMA等变量
#include "math.h"



//电机方向初始化
void	control_init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	//左前轮	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_12|GPIO_Pin_13;	//
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_Init(GPIOB, &GPIO_InitStructure);		         //初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11;	//
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         //初始化
}

//PWM输出
void TIM8_PWM_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  //使能GPIO外设时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);//                          	
   //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM_CHx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出	
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 7200-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set; //输出极性:TIM输出比较极性高
		
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx	
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM8, ENABLE);  //使能TIM1  
	TIM_ARRPreloadConfig(TIM8, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE 主输出使能	  
 
}

/**************************************************************************
函数功能：赋值给PWM寄存器和控制正反转
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_d)
{
		if(motor_d<0) 
		{
			TIM_SetCompare2(TIM8,myabs(motor_d));
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);	
		}	
		if(motor_d>0)  
		{
			TIM_SetCompare2(TIM8,myabs(motor_d));
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
		}	
//		if(motor_d==0)  
//		{
//			TIM_SetCompare2(TIM8,myabs(motor_d));
//			GPIO_SetBits(GPIOB,GPIO_Pin_12);
//			GPIO_ResetBits(GPIOB,GPIO_Pin_13);	
//		}	
}
/**************************************************************************
函数功能：限制PWM赋值
入口参数：幅值
返回  值：无
**************************************************************************/
void Xianfu_Pwm(int amplitude)
{	
	  if(Motor_D<-amplitude) Motor_D=-amplitude;	
	  if(Motor_D>amplitude)  Motor_D=amplitude;	
}

/**************************************************************************
函数功能：位置PID控制过程中速度的设置
入口参数：无、幅值
返回  值：无
**************************************************************************/
void Xianfu_Velocity(int amplitude_A,int amplitude_B,int amplitude_C,int amplitude_D)
{	
    if(Motor_A<-amplitude_A) Motor_A=-amplitude_A;	//位置控制模式中，A电机的运行速度
		if(Motor_A>amplitude_A)  Motor_A=amplitude_A;	  //位置控制模式中，A电机的运行速度
	  if(Motor_B<-amplitude_B) Motor_B=-amplitude_B;	//位置控制模式中，B电机的运行速度
		if(Motor_B>amplitude_B)  Motor_B=amplitude_B;		//位置控制模式中，B电机的运行速度
	  if(Motor_C<-amplitude_C) Motor_C=-amplitude_C;	//位置控制模式中，C电机的运行速度
		if(Motor_C>amplitude_C)  Motor_C=amplitude_C;		//位置控制模式中，C电机的运行速度
		if(Motor_D<-amplitude_D) Motor_D=-amplitude_D;	//位置控制模式中，C电机的运行速度
		if(Motor_D>amplitude_D)  Motor_D=amplitude_D;		//位置控制模式中，C电机的运行速度
}

/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_D (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}

/**************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
根据位置式离散PID公式 
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
//int Position_PID (int Encoder,int Target)
//{ 	
//	 static float Bias,Pwm,Integral_bias,Last_Bias;
//	 Bias=Encoder-Target;                                  //计算偏差
//	 Integral_bias+=Bias;	                                 //求出偏差的积分
//	 Pwm=Position_KP*Bias+Position_KI*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
//	 Last_Bias=Bias;                                       //保存上一次偏差 
//	 return Pwm;                                           //增量输出
//}


int balance(float Angle)
{  
	float Bias;                       //倾角偏差
	static float Last_Bias,D_Bias;    //PID相关变量
	int balance;                      //PWM返回值 
	Bias=Angle-ZHONGZHI;              //求出平衡的角度中值 和机械相关
	D_Bias=Bias-Last_Bias;            //求出偏差的微分 进行微分控制
	/*===计算倾角控制的电机PWM  PD控制===*/
	balance=Balance_KP*Bias+D_Bias*Balance_KD;   
	Last_Bias=Bias;                   //保持上一次的偏差
	return balance;
}


int Position_PID (int Encoder,int target)
{  
   static float Position_PWM,Last_Position=0,Position_Bias,Position_Differential;
	 //static float Position_Least;
  	Position_Bias =Encoder-target;            			 //w位置偏差
    Position_Differential = Position_Bias-Last_Position;//这次位置偏差-上次位置偏差
		Last_Position = Position_Bias;											//更新上次位置偏差
		Position_PWM=-Position_Bias*Position_KP-Position_Differential*Position_KD; //===速度控制	
	  return Position_PWM;
}


int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

