#ifndef _LCD_H_
#define _LCD_H_

#include "stm32f10x.h"

// 屏幕
#define LCD_SCREEN_DIR_H 0 // 横屏
#define LCD_SCREEN_DIR_V 1 // 竖屏

#define LCD_SCREEN_W 128
#define LCD_SCREEN_H 128
#define LCD_SCREEN_DIR LCD_SCREEN_DIR_H

// 命令值
#define LCD_CMD_WRITE 0x2C
#define LCD_CMD_SET_X 0x2A
#define LCD_CMD_SET_Y 0x2B

// 端口
#define LCD_PORT   	  	GPIOB		// 定义 TFT 数据端口
#define LCD_PORT_RS     GPIO_Pin_1	// PB1 连接至  TFT--RS
#define LCD_PORT_CS     GPIO_Pin_12 // PB11 连接至 TFT--CS
#define LCD_PORT_SCL    GPIO_Pin_13	// PB13 连接至 TFT--CLK
#define LCD_PORT_SDA    GPIO_Pin_15	// PB15 连接至 TFT--SDI

// 颜色
#define LCD_COLOR_WHITE         0xFFFF // 白
#define LCD_COLOR_BLACK         0x0000 // 黑
#define LCD_COLOR_BLUE       	0x001F // 蓝
#define LCD_COLOR_BRED          0XF81F // 蓝红？
#define LCD_COLOR_GRED 		    0XFFE0 // 绿红？
#define LCD_COLOR_GBLUE		    0X07FF // 绿蓝？
#define LCD_COLOR_RED           0xF800 // 红
#define LCD_COLOR_MAGENTA       0xF81F // 品红
#define LCD_COLOR_GREEN         0x07E0 // 绿
#define LCD_COLOR_CYAN          0x7FFF // 青
#define LCD_COLOR_YELLOW        0xFFE0 // 黄
#define LCD_COLOR_BROWN 		0XBC40 // 棕
#define LCD_COLOR_BRRED 		0XFC07 // 棕红
#define LCD_COLOR_GRAY  		0X8430 // 灰
#define LCD_COLOR_DARKBLUE      0X01CF // 深蓝
#define LCD_COLOR_LIGHTBLUE     0X7D7C // 浅蓝
#define LCD_COLOR_GRAYBLUE      0X5458 // 灰蓝
#define LCD_COLOR_LIGHTGREEN    0X841F // 浅绿
#define LCD_COLOR_LIGHTGRAY     0XEF5B // 浅灰 (PANNEL)
#define LCD_COLOR_LGRAY 		0XC618 // 浅灰 (PANNEL), 窗体背景色
#define LCD_COLOR_LGRAYBLUE     0XA651 // 浅灰蓝 ( 中间层颜色 )
#define LCD_COLOR_LBBLUE        0X2B12 // 浅棕蓝 ( 选择条目的反色 )

// 宏函数
// 液晶控制口置 1
#define	M_lcdRsSet()  	(LCD_PORT->BSRR=LCD_PORT_RS)
#define	M_lcdCsSet()  	(LCD_PORT->BSRR=LCD_PORT_CS)
#define	M_lcdSclSet()  	(LCD_PORT->BSRR=LCD_PORT_SCL)
#define	M_lcdSdaSet()  	(LCD_PORT->BSRR=LCD_PORT_SDA)

// 液晶控制口置 0
#define	M_lcdRsClr()  	(LCD_PORT->BRR=LCD_PORT_RS)
#define	M_lcdCsClr()  	(LCD_PORT->BRR=LCD_PORT_CS)
#define	M_lcdSclClr()  	(LCD_PORT->BRR=LCD_PORT_SCL)
#define	M_lcdSdaClr()  	(LCD_PORT->BRR=LCD_PORT_SDA)

// 其他
#define LCD_OTH_SIMPLE_LINE_DIR_PX 0 // X 轴正方向
#define LCD_OTH_SIMPLE_LINE_DIR_PY 1 // Y 轴正方向
#define LCD_OTH_SIMPLE_LINE_DIR_NX 2 // X 轴负方向
#define LCD_OTH_SIMPLE_LINE_DIR_NY 3 // Y 轴负方向

// 底层函数
/**
 *  @biref 初始化 LCD SPI （ GPIO 、 SPI ）
 */
void initLcdSpi(void);

/**
 *  @biref 向 SPI 总线发送一个字节的数据
 *  @param spiX SPI 总线
 *  @param byte 数据字节
 *  @return 收到的字节
 */
u8 spiWriteByte(SPI_TypeDef* spiX, u8 byte);

/**
 *  @biref 向 LCD SPI 总线写入 8bit 指令
 *  @param data 待写入的 8bit 指令
 */
void lcdWriteReg(u8 data);

/**
 *  @biref 向 LCD SPI 总线写入 8bit 数据
 *  @param data 待写入的 8bit 数据
 */
void lcdWriteData(u8 data);

/**
 *  @biref 向 LCD SPI 总线写入 16bit 数据
 *  @param data 待写入的 16bit 数据
 */
void lcdWriteData16(u16 data);

// 中层函数
/**
 *  @biref 设置 LCD 的显示窗口，在此区域写点数据自动换行
 *  @param xs x 的起点
 *  @param ys y 的起点
 *  @param xe x 的终点
 *  @param ye y 的终点
 */
void lcdSetWindow(u16 xs, u16 ys, u16 xe, u16 ye);

/**
 *  @biref 8 对称性画圆算法（被 lcdDrawCircle 调用的内部函数）
 *  @param xc 圆心 x 坐标
 *  @param yc 圆心 y 坐标
 *  @param x 某点相对于圆心的 x 坐标
 *  @param y 某点相对于圆心的 y 坐标
 */
void lcdDrawCircle8(s16 xc, s16 yc, s16 x, s16 y);

/**
 *  @biref 在某个位置显示一个字符
 *  @param x 字符起始 x 坐标
 *  @param y 字符起始 y 坐标
 *  @param ascii 字符的 ascii 码
 *  @param size 字体大小
 */
void lcdDrawChar(u16 x, u16 y, u8 ascii, u8 size);

// 高层函数
/**
 *  @biref 初始化 LCD （ GPIO 、 SPI 、 LCD 配置 ）
 */
void initLcd(void);

/**
 *  @biref 清屏函数（全屏填充）
 *  @param color 填充颜色
 */
void lcdClear(u16 color);

/**
 *  @biref 设置光标位置
 *  @param x 光标 x 坐标
 *  @param y 光标 y 坐标
 */
void lcdSetCursor(u16 x, u16 y);

/**
 *  @biref 设置画刷颜色
 *  @param color 新的画刷颜色
 */
void lcdSetBrushColor(u16 color);

/**
 *  @biref 画点
 *  @param x 点的 x 坐标
 *  @param y 点的 y 坐标
 */
void lcdDrawPoint(u16 x,u16 y);

/**
 *  @biref 画线
 *  @param x1 起点的 x 坐标
 *  @param y1 起点的 y 坐标
 *  @param x2 终点的 x 坐标
 *  @param y2 终点的 y 坐标
 */
void lcdDrawLine(u16 x1, u16 y1, u16 x2, u16 y2);

/**
 *  @biref 画矩形
 *  @param x1 起点的 x 坐标
 *  @param y1 起点的 y 坐标
 *  @param x2 终点的 x 坐标
 *  @param y2 终点的 y 坐标
 */
void lcdDrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);

/**
 *  @biref 画填充矩形，填充颜色为画刷颜色
 *  @param x1 起点的 x 坐标
 *  @param y1 起点的 y 坐标
 *  @param x2 终点的 x 坐标
 *  @param y2 终点的 y 坐标
 */
void lcdDrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);

/**
 *  @biref 在指定位置画（填充）一个指定大小的圆
 *  @param xc 圆心 x 坐标
 *  @param yc 圆心 y 坐标
 *  @param r 圆半径
 *  @param fill 是否填充（ 0 ：不填充， 1 ：填充）
 */
void lcdDrawCircle(s16 xc, s16 yc, s16 r, u8 fill);

/**
 *  @biref 在某个位置打印一行英文字符串
 *  @param x 字符串起始 x 坐标
 *  @param y 字符串起始 y 坐标
 *  @param str ascii 编码的 C 字符串
 *  @param size 字体大小
 */
void lcdDrawString(u16 x, u16 y, u8 *str, u8 size);

// 高层函数（第二组）

/**
 *  @biref 更简单的线条绘制函数，只能画垂直线和水平线
 *  @param x 起点 x 坐标
 *  @param x 起点 y 坐标
 *  @param dir 方向
 *  @param length 长度
 */
void lcdDrawLineSimple(u16 x, u16 y, u8 dir, u16 length);

#endif
