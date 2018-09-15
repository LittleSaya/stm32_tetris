#ifndef _USART1_H_
#define _USART1_H_

#include "stm32f10x.h"

//// Public Start ////

// USART1 的 RX 缓冲区 usart1RxBuf 的大小
#define USART1_BUF_SIZE 32
// 结束字节
#define USART1_RX_END '#'
// RX 关闭位，置 0 ：RX 未关闭，置 1 ：RX 关闭
// 和 RXNE 中断的打开状态为反义词
#define USART1_RX_STATE_CLOSE 0x01
// RX 错误位，置 0 ：RX 无错，置 1 ：RX 出错
#define USART1_RX_STATE_ERROR 0x02

/**
 *  @brief 初始化 GPIO 、 USART1 、 NVIC ，使能 RXNE 中断
 */
void usart1Init(void);

/**
 *  @brief 发送字符串
 *  @param str 待发送的 C 字符串
 */
void usart1SendStr(char *str);

/**
 *  @brief 尝试接收一条消息
 *  @param buf 存储消息的缓冲区指针
 *  @param bufSize 缓冲区 buf 的大小
 *  @return
 *      USART1_RECV_MSG_SUCCESS ：
 *          当 CLOSE 为 1 且 ERROR 为 0 时，从 usart1RxBuf 复制数据到 buf ，并将 CLOSE 置 0 ，usart1RxCnt 置 0
 *          表示消息接收成功
 *      USART1_RECV_MSG_WAITING ：
 *          当 CLOSE 为 0 时（此时 ERROR 必为 0 ），什么都不做
 *          表示正在等待结束字节
 *      USART1_RECV_MSG_ERROR ：
 *          当 CLOSE 为 1 且 ERROR 为 1 时，什么都不做
 *          表示存在错误
 */
#define USART1_RECV_MSG_SUCCESS 0
#define USART1_RECV_MSG_WAITING 1
#define USART1_RECV_MSG_ERROR 2
u8 usart1RecvMsg(char *buf, u8 bufSize);

/**
 *  @brief 重置 USART1 的相关变量
 *      CLOSE 和 ERROR 置 0 ， usart1RxCnt 置 0
 *      重新使能 RXNE 中断
 */
void usart1Clear(void);

//// Public End ////

//// Private Start ////

/**
 *  @brief USART1 RXNE 中断处理
 */
void usart1IT_RNXE(void);

//// Private End ////

#endif
