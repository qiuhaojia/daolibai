#include "Send_Data.h"
#include "usart.h"

u8 data_buffer[50];
u8 data_buffer2[200];

//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
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
        USART_SendData(USART1, data_buffer[t]);//向串口1发送数据
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET); //等待发送结束
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
        USART_SendData(USART1, data_buffer2[t]);//向串口1发送数据
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET); //等待发送结束
    }
}

