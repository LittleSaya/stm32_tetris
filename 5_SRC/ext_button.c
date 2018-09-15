#include "ext_button.h"
#include "systick.h"
#include <string.h>

u8 extBtnStateOld;
u8 extBtnState;
u32 extBtnLongPressCount[8];
u8 extBtnCmd[8];

void initExtBtn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置 GPIOA 和 GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    // PA1 ~ PA7 (S8 ~ S2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // PB2 (S1)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // PA0 (COM)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_0); // 设置 COM 端为高电平
    
    // 初始化按键状态
    extBtnStateOld = extBtnState = 0;
    memset(extBtnLongPressCount, 0, sizeof(extBtnLongPressCount));
    memset(extBtnCmd, 0, sizeof(extBtnCmd));
}

void extBtnScan(void)
{
    static u16 pa, pb; // PA PB
    
    pa = GPIO_ReadInputData(GPIOA);
    pb = GPIO_ReadInputData(GPIOB);
    (pb & EXT_BTN_R_PIN)     ? (extBtnState |= EXT_BTN_R)     : (extBtnState &= ~EXT_BTN_R);
    (pa & EXT_BTN_NO_PIN)    ? (extBtnState |= EXT_BTN_NO)    : (extBtnState &= ~EXT_BTN_NO);
    (pa & EXT_BTN_L_PIN)     ? (extBtnState |= EXT_BTN_L)     : (extBtnState &= ~EXT_BTN_L);
    (pa & EXT_BTN_YES_PIN)   ? (extBtnState |= EXT_BTN_YES)   : (extBtnState &= ~EXT_BTN_YES);
    (pa & EXT_BTN_RIGHT_PIN) ? (extBtnState |= EXT_BTN_RIGHT) : (extBtnState &= ~EXT_BTN_RIGHT);
    (pa & EXT_BTN_DOWN_PIN)  ? (extBtnState |= EXT_BTN_DOWN)  : (extBtnState &= ~EXT_BTN_DOWN);
    (pa & EXT_BTN_LEFT_PIN)  ? (extBtnState |= EXT_BTN_LEFT)  : (extBtnState &= ~EXT_BTN_LEFT);
    (pa & EXT_BTN_UP_PIN)    ? (extBtnState |= EXT_BTN_UP)    : (extBtnState &= ~EXT_BTN_UP);
}

u8 extBtnCheckState(u8 state, u8 btn)
{
    return (btn & state) == btn;
}

void extBtnCmdScan(void)
{
    static u8 localBtnState,
              btnBit, btnIdx,
              oldState, newState;
    
    // 获取按键状态
    localBtnState = extBtnState;
    // 延时 16ms
    delayMs(16);
    // 再次获取按键状态
    if (localBtnState != extBtnState)
    {
        // 本次按键状态不可信，按键命令维持原样
        return;
    }
    for (btnBit = 0x01, btnIdx = 0; btnIdx < 8; btnBit <<= 1, ++btnIdx)
    {
        oldState = extBtnCheckState(extBtnStateOld, btnBit);
        newState = extBtnCheckState(localBtnState, btnBit);
        if (!oldState && !newState)
        {
            // 连续两次松开，视为无命令
            extBtnCmd[btnIdx] = EXT_BTN_CMD_NONE;
        }
        else if (!oldState && newState)
        {
            // 松开 -> 按下
            extBtnCmd[btnIdx] = EXT_BTN_CMD_DOWN;
            extBtnLongPressCount[btnIdx] = getRunningTime(); // 记录按下时间
        }
        else if (oldState && newState)
        {
            if (getRunningTime() - extBtnLongPressCount[btnIdx] >= EXT_BTN_LONG_PRESS_LIMIT)
            {
                // 长按
                extBtnCmd[btnIdx] = EXT_BTN_CMD_LONG_PRESS;
            }
            else
            {
                // 普通按下状态
                extBtnCmd[btnIdx] = EXT_BTN_CMD_PRESS;
            }
        }
        else
        {
            // 按下 -> 松开
            extBtnCmd[btnIdx] = EXT_BTN_CMD_UP;
            // 清空计时
            extBtnLongPressCount[btnIdx] = 0;
        }
    }
    extBtnStateOld = localBtnState;
}

u8 extBtnGetCmd(u8 btn)
{
    return extBtnCmd[btn];
}
