#ifndef _ADC_RAND_H_
#define _ADC_RAND_H_

#include "stm32f10x.h"

/**
 *  @biref 初始化 ADC 随机数生成器
 */
void initAdcRand(void);

/**
 *  @biref 使用 ADC 生成一个新的随机数种子，并用这个种子填充 srand
 */
void adcRandFeedSeed(void);

/**
 *  @biref 获取一次 AD 转换的值
 *  @return AD 转换的结果，一个 16bit 的无符号整数
 */
u16 adcRandReadAdc(void);

/**
 *  @biref 获取一个随机数
 *  @return 随机数
 */
int adcRand(void);

#endif
