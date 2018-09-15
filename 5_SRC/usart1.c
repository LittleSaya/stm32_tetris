#include "stm32f10x.h"
#include "usart1.h"
#include <string.h>

u8 usart1RxBuf[USART1_BUF_SIZE];
u8 usart1RxState = 0x00;
u8 usart1RxCnt = 0;

void usart1Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); 
  
    // USART1 TX 引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // USART1 RX 引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART1 配置
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE); // 使能
    
    // 中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启接收中断
}

void usart1SendStr(char *str)
{
    u8 idx = 0;
    
    do
    {
        USART_SendData(USART1, str[idx]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
    while (str[idx++] != '\0');  //检查字符串结束标志
}

u8 usart1RecvMsg(char *buf, u8 bufSize)
{
    if (USART1_RX_STATE_CLOSE == usart1RxState)
    {
        // 接受正常关闭， CLOSE 为 1 且 ERROR 为 0
        bufSize = bufSize > USART1_BUF_SIZE ? USART1_BUF_SIZE : bufSize; // 限制拷贝大小
        memcpy(buf, usart1RxBuf, bufSize);
        usart1Clear();
        return USART1_RECV_MSG_SUCCESS;
    }
    else if (usart1RxState == 0x00)
    {
        // 正在等待结束字节， CLOSE 为 0 且 ERROR 为 0
        return USART1_RECV_MSG_WAITING;
    }
    else if ((USART1_RX_STATE_CLOSE | USART1_RX_STATE_ERROR) == usart1RxState)
    {
        // 接收因错关闭， CLOSE 为 1 且 ERROR 为 1
        return USART1_RECV_MSG_ERROR;
    }
    else
    {
        // 意料之外的错误
        while (1);
    }
}

void usart1Clear(void)
{
    usart1RxCnt = usart1RxState = 0x00;
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void usart1IT_RNXE(void)
{
	u8 recv;
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // 进入 RXNE 中断，表示该中断尚未关闭， CLOSE 位尚未置 1
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		recv = USART_ReceiveData(USART1); // 接收数据
        if (USART1_RX_END == recv)
        {
            // 结束字节
            usart1RxBuf[usart1RxCnt++] = '\0'; // 填充空字符
            // 关闭 RX
            usart1RxState |= USART1_RX_STATE_CLOSE;
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // 关闭接收中断
        }
        else if (usart1RxCnt >= USART1_BUF_SIZE - 1)
        {
            // 这是缓冲区所能接收的最后一个字节，但这个字符不是结束字节
            // 关闭 RX ，设置 ERROR 位
            usart1RxState |= (USART1_RX_STATE_CLOSE | USART1_RX_STATE_ERROR);
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // 关闭接收中断
        }
        else
        {
            // 不是结束字节，也不是缓冲区的最后一个字节
            usart1RxBuf[usart1RxCnt++] = recv;
        }
	}
}
