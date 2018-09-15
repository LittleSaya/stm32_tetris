#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void initSysTick(void);
void delayMs(u32 nTime);

u32 getRunningTime(void);

#endif /* __SYSTICK_H */
