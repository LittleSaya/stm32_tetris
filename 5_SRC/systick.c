
#include "systick.h"

u32 timingDelay;

u32 runningTime;

void initSysTick(void)
{
    /* SystemFrequency / 10000   1ms
     * SystemFrequency / 100000	 100us
     * SystemFrequency / 1000000 10us
     */
    if (SysTick_Config(SystemFrequency / 10000))
    { 
        /* Capture error */
        while (1);
    }
    runningTime = 0;
}

void delayMs(u32 ms)
{
    timingDelay = ms;
    while (timingDelay != 0);
}

void timingDelayDecrement(void)
{
    if (timingDelay != 0x0000)
    {
        --timingDelay;
    }
}

u32 getRunningTime(void)
{
    return runningTime;
}
