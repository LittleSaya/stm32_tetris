#ifndef _EXT_BUTTON_
#define _EXT_BUTTON_

#include "stm32f10x.h"

/**
 *  键盘引脚 - 核心板引脚
 *  COM - PA0
 *  S8  - PA1
 *  S7  - PA2
 *  S6  - PA3
 *  S5  - PA4
 *  S4  - PA5
 *  S3  - PA6
 *  S2  - PA7
 *  S1  - PB2
 */

// 按键名 - 状态位 - 键盘引脚
#define EXT_BTN_R       0x01 // S1
#define EXT_BTN_NO      0x02 // S2
#define EXT_BTN_L       0x04 // S3
#define EXT_BTN_YES     0x08 // S4
#define EXT_BTN_RIGHT   0x10 // S5
#define EXT_BTN_DOWN    0x20 // S6
#define EXT_BTN_LEFT    0x40 // S7
#define EXT_BTN_UP      0x80 // S8

// 按键名（引脚） - 核心板引脚
#define EXT_BTN_R_PIN       GPIO_Pin_2
#define EXT_BTN_NO_PIN      GPIO_Pin_7
#define EXT_BTN_L_PIN       GPIO_Pin_6
#define EXT_BTN_YES_PIN     GPIO_Pin_5
#define EXT_BTN_RIGHT_PIN   GPIO_Pin_4
#define EXT_BTN_DOWN_PIN    GPIO_Pin_3
#define EXT_BTN_LEFT_PIN    GPIO_Pin_2
#define EXT_BTN_UP_PIN      GPIO_Pin_1

// 按键编号
#define EXT_BTN_R_IDX     0
#define EXT_BTN_NO_IDX    1
#define EXT_BTN_L_IDX     2
#define EXT_BTN_YES_IDX   3
#define EXT_BTN_RIGHT_IDX 4
#define EXT_BTN_DOWN_IDX  5
#define EXT_BTN_LEFT_IDX  6
#define EXT_BTN_UP_IDX    7

// 命令值
#define EXT_BTN_CMD_NONE        0x00
#define EXT_BTN_CMD_DOWN        0x01
#define EXT_BTN_CMD_PRESS       0x02
#define EXT_BTN_CMD_LONG_PRESS  0x03
#define EXT_BTN_CMD_UP          0x04

// 长按阀限，按键按下超过该时间则视为长按，单位 ms
#define EXT_BTN_LONG_PRESS_LIMIT 500

/**
 *  @brief 初始化按键 GPIO
 *      COM 端推挽输出，高电平
 *      S1 - S8 端下拉输入，高电平按下，低电平松开
 */
void initExtBtn(void);

/**
 *  @biref 扫描按钮，为 extBtnState 赋值
 */
void extBtnScan(void);

/**
 *  @biref 根据参数给出的按键状态，检查某个（些）按钮是否按下
 *  @param state 状态
 *  @param btn 按钮名称（或按钮名称的组合），可以使用 '|' 符号组合多个按钮
 *  @return 按钮（或按钮的组合）是否被按下
 *      0 ：该按钮未被按下（或该按钮组合中的按钮未被全部按下）
 *      1 ：该按钮被按下（或该按钮组合中的按钮被全部按下）
 */
u8 extBtnCheckState(u8 state, u8 btn);

/**
 *  @biref 比较新旧按键状态，更新按键指令
 */
void extBtnCmdScan(void);

/**
 *  @biref 获取某个按键的参数
 *  @param btn 按键编号
 *  @return 该按键上的命令
 */
u8 extBtnGetCmd(u8 btn);

#endif
