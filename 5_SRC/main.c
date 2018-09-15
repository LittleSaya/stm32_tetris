#include "stm32f10x.h"
#include "systick.h"
#include "ext_button.h"
#include "lcd.h"
#include "adc_rand.h"
#include "tetris.h"
#include <stdio.h>
#include <string.h>

extern Falling tetrisPredefinedFallings[7];
extern Coord tetrisMoveTemp[4]; // 移动位置暂存
extern Coord tetrisRotateTemp[4]; // 旋转位置暂存

int main(void)
{
    u8 show = 0;
    u32 time = 0;
    
    initSysTick(); // 延时
    initExtBtn();  // 按键
    initLcd();     // LCD 屏幕
    initAdcRand(); // 随机数生成
    
    // 显示欢迎界面
    lcdSetBrushColor(LCD_COLOR_RED);
    lcdDrawString(40, 48, (u8*)"TETRIS", 16);
    lcdSetBrushColor(LCD_COLOR_WHITE);
    lcdDrawString(24, 64, (u8*)"Press Confirm", 12);
    show = 1;
    while (1)
    {
        if (getRunningTime() - time > 1000)
        {
            if (show)
            {
                lcdSetBrushColor(LCD_COLOR_BLACK);
                lcdDrawFillRectangle(24, 64, 102, 76);
                show = 0;
            }
            else
            {
                lcdSetBrushColor(LCD_COLOR_WHITE);
                lcdDrawString(24, 64, (u8*)"Press Confirm", 12);
                show = 1;
            }
            time = getRunningTime();
        }
        extBtnCmdScan();
        if (extBtnGetCmd(EXT_BTN_YES_IDX) == EXT_BTN_CMD_DOWN)
        {
            break;
        }
    }
    
    initTetris();  // 俄罗斯方块（该初始化依赖上述部件）
    
    // 进入游戏循环
    while (1)
    {
        tetrisGameLoop();
    }
}
