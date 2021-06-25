#include "Send_Data.h"
#include "usart.h"

u8 data_buffer[50];
u8 data_buffer2[200];

//���ݲ�ֺ궨�壬�ڷ��ʹ���1�ֽڵ���������ʱ������int16��float�ȣ���Ҫ�����ݲ�ֳɵ����ֽڽ��з���
#define BYTE0(dwTemp)       (*((u8 *)(&dwTemp)))
#define BYTE1(dwTemp)       (*((u8 *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((u8 *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((u8 *)(&dwTemp) + 3))


void Send_Speed(s16 A, s16 Expect)
{
    u8 cnt = 0, t;
    float temp;
    temp = A;
    data_buffer[cnt++] = BYTE0(temp);
    data_buffer[cnt++] = BYTE1(temp);
    data_buffer[cnt++] = BYTE2(temp);
    data_buffer[cnt++] = BYTE3(temp);

    temp = Expect;
    data_buffer[cnt++] = BYTE0(temp);
    data_buffer[cnt++] = BYTE1(temp);
    data_buffer[cnt++] = BYTE2(temp);
    data_buffer[cnt++] = BYTE3(temp);

    data_buffer[cnt++] = 0x00;
    data_buffer[cnt++] = 0x00;
    data_buffer[cnt++] = 0x80;
    data_buffer[cnt++] = 0x7f;

    for (t = 0; t < cnt; t++)
    {
        USART_SendData(USART1, data_buffer[t]);//�򴮿�1��������
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET); //�ȴ����ͽ���
    }
}

void Send_Speed2(s16 A, s16 B, s16 C, s16 D)
{
    u8 cnt = 0, t;
    float temp;
    temp = A;
    data_buffer2[cnt++] = BYTE0(temp);
    data_buffer2[cnt++] = BYTE1(temp);
    data_buffer2[cnt++] = BYTE2(temp);
    data_buffer2[cnt++] = BYTE3(temp);

    temp = B;
    data_buffer2[cnt++] = BYTE0(temp);
    data_buffer2[cnt++] = BYTE1(temp);
    data_buffer2[cnt++] = BYTE2(temp);
    data_buffer2[cnt++] = BYTE3(temp);
	
		temp = C;
    data_buffer2[cnt++] = BYTE0(temp);
    data_buffer2[cnt++] = BYTE1(temp);
    data_buffer2[cnt++] = BYTE2(temp);
    data_buffer2[cnt++] = BYTE3(temp);
	
	  temp = D;
    data_buffer2[cnt++] = BYTE0(temp);
    data_buffer2[cnt++] = BYTE1(temp);
    data_buffer2[cnt++] = BYTE2(temp);
    data_buffer2[cnt++] = BYTE3(temp);
		

    data_buffer2[cnt++] = 0x00;
    data_buffer2[cnt++] = 0x00;
    data_buffer2[cnt++] = 0x80;
    data_buffer2[cnt++] = 0x7f;

    for (t = 0; t < cnt; t++)
    {
        USART_SendData(USART1, data_buffer2[t]);//�򴮿�1��������
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET); //�ȴ����ͽ���
    }
}

