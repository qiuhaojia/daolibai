#ifndef	__KEY_H
#define	__KEY_H
#include	"sys.h"

#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//��ȡ����1
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//��ȡ����1
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//��ȡ����1
 

#define KEY1_PRES	1		//KEY0  
#define KEY2_PRES	2		//KEY1 
#define KEY3_PRES	3		//KEY0  
#define KEY4_PRES	4		//KEY1

void KEY_Init(void);//IO��ʼ��
int KEY_Scan(u8 mode);  	//����ɨ�躯��	

#endif

