#include "lcd.h"
#include "font.h"
#include "systick.h"

u16 brushColor; // 画刷颜色

// 底层函数

void initLcdSpi(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//配置SPI2管脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_12); // 片选信号为低
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);  // RS 信号为低

	//SPI2配置选项
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	//使能SPI2
	SPI_Cmd(SPI2, ENABLE);
}

u8 spiWriteByte(SPI_TypeDef* spiX, u8 byte)
{
	while ((spiX->SR & SPI_I2S_FLAG_TXE) == RESET);	//等待发送区空
	spiX->DR = byte; //发送一个byte
	while ((spiX->SR & SPI_I2S_FLAG_RXNE) == RESET); // 等待接收完一个 byte
	return spiX->DR; //返回收到的数据
}

void lcdWriteReg(u8 data)
{
   M_lcdCsClr();
   M_lcdRsClr();
   spiWriteByte(SPI2, data);
   M_lcdCsSet();
}

void lcdWriteData(u8 data)
{
   M_lcdCsClr();
   M_lcdRsSet();
   spiWriteByte(SPI2, data);
   M_lcdCsSet();
}

void lcdWriteData16(u16 data)
{
   M_lcdCsClr();
   M_lcdRsSet();
   spiWriteByte(SPI2, data >> 8);
   spiWriteByte(SPI2, data);
   M_lcdCsSet();
}

// 中层函数

void lcdSetWindow(u16 xs, u16 ys, u16 xe, u16 ye)
{
#if LCD_SCREEN_DIR == LCD_SCREEN_DIR_H // 横屏
	lcdWriteReg(LCD_CMD_SET_X);
	lcdWriteData(xs >> 8);
	lcdWriteData(0x00FF & xs + 3); // 这里为什么要 +3 ？
	lcdWriteData(xe >> 8);
	lcdWriteData(0x00FF & xe + 3); // 这里为什么呀 +3 ？

	lcdWriteReg(LCD_CMD_SET_Y);
	lcdWriteData(ys >> 8);
	lcdWriteData(0x00FF & ys + 2);
	lcdWriteData(ye >> 8);
	lcdWriteData(0x00FF & ye + 2);
#else
	lcdWriteReg(LCD_CMD_SET_X);
	lcdWriteData(xs >> 8);
	lcdWriteData(0x00FF & xs + 2);
	lcdWriteData(xe >> 8);
	lcdWriteData(0x00FF & xe + 2);

	lcdWriteReg(LCD_CMD_SET_Y);
	lcdWriteData(ys >> 8);
	lcdWriteData(0x00FF & ys + 3);
	lcdWriteData(ye >> 8);
	lcdWriteData(0x00FF & ye + 3);
#endif
	lcdWriteReg(LCD_CMD_WRITE); // 准备写 GRAM
}

void lcdDrawCircle8(s16 xc, s16 yc, s16 x, s16 y)
{
	lcdDrawPoint(xc + x, yc + y);
	lcdDrawPoint(xc - x, yc + y);
	lcdDrawPoint(xc + x, yc - y);
	lcdDrawPoint(xc - x, yc - y);
	lcdDrawPoint(xc + y, yc + x);
	lcdDrawPoint(xc - y, yc + x);
	lcdDrawPoint(xc + y, yc - x);
	lcdDrawPoint(xc - y, yc - x);
}

void lcdDrawChar(u16 x, u16 y, u8 ascii, u8 size)
{
    u8 temp, pos, t;

	ascii = ascii - ' '; // 得到偏移后的值
	lcdSetWindow(x, y, x + size / 2 - 1, y + size - 1); // 设置单个文字显示窗口
    for(pos = 0; pos < size; ++pos)
    {
        if (size == 12)
        {
            temp = asc2_1206[ascii][pos]; //调用1206字体
        }
        else
        {
            temp = asc2_1608[ascii][pos]; //调用1608字体
        }
        for(t = 0; t < size / 2; ++t)
        {
            if (temp & 0x01)
            {
                lcdDrawPoint(x + t, y + pos); //画一个点
            }
            temp >>= 1;
        }
    }
	lcdSetWindow(0, 0, LCD_SCREEN_W - 1, LCD_SCREEN_H - 1); //恢复窗口为全屏
}

// 高层函数

void initLcd(void)
{
    // 初始化 SPI
	initLcdSpi();

	// 开始初始化 ST7735R
	lcdWriteReg(0x11); // Sleep exit
	delayMs (120);

	// Frame Rate
	lcdWriteReg(0xB1); // normal mode frame rate
	lcdWriteData(0x01);
	lcdWriteData(0x2C);
	lcdWriteData(0x2D);

	lcdWriteReg(0xB2); // idle mode frame rate
	lcdWriteData(0x01);
	lcdWriteData(0x2C);
	lcdWriteData(0x2D);

	lcdWriteReg(0xB3); // partial mode frame rate
	lcdWriteData(0x01);
	lcdWriteData(0x2C);
	lcdWriteData(0x2D);
	lcdWriteData(0x01);
	lcdWriteData(0x2C);
	lcdWriteData(0x2D);

	lcdWriteReg(0xB4); //Column inversion
	lcdWriteData(0x07); // normal & idle & partial mode
    // 0x01 partial mode (column inversion)
    // 0x02 idle mode (column inversion)
    // 0x04 normal mode (column inversion)

	// ST7735R Power Sequence
	lcdWriteReg(0xC0); // power control 1
	lcdWriteData(0xA2);
	lcdWriteData(0x02);
	lcdWriteData(0x84);

	lcdWriteReg(0xC1); // power control 2
	lcdWriteData(0xC5);

	lcdWriteReg(0xC2); // power control in normal mode
	lcdWriteData(0x0A);
	lcdWriteData(0x00);

	lcdWriteReg(0xC3); // power control in idle mode
	lcdWriteData(0x8A);
	lcdWriteData(0x2A);

	lcdWriteReg(0xC4); // power control in partial mode
	lcdWriteData(0x8A);
	lcdWriteData(0xEE);

	lcdWriteReg(0xC5); // VCOM control
	lcdWriteData(0x0E);

	lcdWriteReg(0x36); // memory data access control MX, MY, RGB mode
	lcdWriteData(0xC8); // 11001000

	// ST7735R Gamma Sequence
	lcdWriteReg(0xe0);
	lcdWriteData(0x0f);
	lcdWriteData(0x1a);
	lcdWriteData(0x0f);
	lcdWriteData(0x18);
	lcdWriteData(0x2f);
	lcdWriteData(0x28);
	lcdWriteData(0x20);
	lcdWriteData(0x22);
	lcdWriteData(0x1f);
	lcdWriteData(0x1b);
	lcdWriteData(0x23);
	lcdWriteData(0x37);
	lcdWriteData(0x00);
	lcdWriteData(0x07);
	lcdWriteData(0x02);
	lcdWriteData(0x10);

	lcdWriteReg(0xe1);
	lcdWriteData(0x0f);
	lcdWriteData(0x1b);
	lcdWriteData(0x0f);
	lcdWriteData(0x17);
	lcdWriteData(0x33);
	lcdWriteData(0x2c);
	lcdWriteData(0x29);
	lcdWriteData(0x2e);
	lcdWriteData(0x30);
	lcdWriteData(0x30);
	lcdWriteData(0x39);
	lcdWriteData(0x3f);
	lcdWriteData(0x00);
	lcdWriteData(0x07);
	lcdWriteData(0x03);
	lcdWriteData(0x10);

	lcdWriteReg(0x2a); // column address set
	lcdWriteData(0x00);
	lcdWriteData(0x00); // X start = 0
	lcdWriteData(0x00);
	lcdWriteData(0x7f); // X end = 127

	lcdWriteReg(0x2b); // row address set
	lcdWriteData(0x00);
	lcdWriteData(0x00); // Y start = 0
	lcdWriteData(0x00);
	lcdWriteData(0x9f); // Y end = 159

	lcdWriteReg(0xF0); // Enable test command
	lcdWriteData(0x01);
	lcdWriteReg(0xF6); // Disable ram power save mode
	lcdWriteData(0x00);

	lcdWriteReg(0x3A); //65k mode
	lcdWriteData(0x05);
	lcdWriteReg(0x29); // Display on

#if LCD_SCREEN_DIR == LCD_SCREEN_DIR_H //横屏
    lcdWriteReg(0x36);
    lcdWriteData(0xA8);
#else // 竖屏
    lcdWriteReg(0x36);
    lcdWriteData(0xC8);
#endif
	lcdClear(LCD_COLOR_BLACK);
    lcdSetBrushColor(LCD_COLOR_WHITE);
}

void lcdClear(u16 color)
{
	u16 i, j;

	lcdSetWindow(0, 0, LCD_SCREEN_W - 1, LCD_SCREEN_H - 1); // 设置窗口为全屏
	for (i = 0; i < LCD_SCREEN_W; ++i)
	{
		for (j = 0; j < LCD_SCREEN_H; ++j)
        {
            lcdWriteData16(color); // 填充颜色
        }
	}
}

void lcdSetCursor(u16 x, u16 y)
{
    lcdSetWindow(x, y, x, y);
}

void lcdSetBrushColor(u16 color)
{
    brushColor = color;
}

void lcdDrawPoint(u16 x,u16 y)
{
	lcdSetCursor(x, y);
    lcdWriteData16(brushColor);
}

void lcdDrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;

	delta_x = x2 - x1; //计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
    {
        incx = 1; //设置单步方向
    }
	else if (delta_x == 0)
    {
        incx = 0; //垂直线
    }
	else
    {
        incx = -1;
        delta_x = -delta_x;
    }
	if (delta_y > 0)
    {
        incy=1;
    }
	else if (delta_y == 0)
    {
        incy = 0; //水平线
    }
	else
    {
        incy = -1;
        delta_y = -delta_y;
    }
	if (delta_x > delta_y)
    {
        distance = delta_x; //选取基本增量坐标轴
    }
	else
    {
        distance = delta_y;
    }
	for(t = 0; t <= distance + 1; ++t) // 画线输出
	{
		lcdDrawPoint(uRow, uCol); //画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

void lcdDrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	lcdDrawLineSimple(x1, y1, LCD_OTH_SIMPLE_LINE_DIR_PX, x2 - x1 + 1);
	lcdDrawLineSimple(x1, y1, LCD_OTH_SIMPLE_LINE_DIR_PY, y2 - y1 + 1);
	lcdDrawLineSimple(x2, y2, LCD_OTH_SIMPLE_LINE_DIR_NX, x2 - x1 + 1);
	lcdDrawLineSimple(x2, y2, LCD_OTH_SIMPLE_LINE_DIR_NY, y2 - y1 + 1);
}

void lcdDrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	static u16 i, steps;

    steps = (x2 - x1 + 1) * (y2 - y1 + 1);
	lcdSetWindow(x1, y1, x2, y2); // 设置显示窗口
	for (i = 0; i < steps; ++i)
	{
        lcdWriteData16(brushColor); // 填充颜色
	}
}

void lcdDrawCircle(s16 xc, s16 yc, s16 r, u8 fill)
{
	s16 x = 0, y = r, yi, d;

	d = 3 - 2 * r;
	if (fill)
	{
		// 如果填充（画实心圆）
		while (x <= y)
        {
			for (yi = x; yi <= y; yi++)
            {
				lcdDrawCircle8(xc, yc, x, yi);
            }
			if (d < 0)
            {
				d = d + 4 * x + 6;
			}
            else
            {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
    else
	{
		// 如果不填充（画空心圆）
		while (x <= y)
        {
			lcdDrawCircle8(xc, yc, x, y);
			if (d < 0)
            {
				d = d + 4 * x + 6;
			}
            else
            {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

void lcdDrawString(u16 x, u16 y, u8 *str, u8 size)
{
    while ((*str <= '~') && (*str >= ' ')) // 判断是不是非法字符
    {
		if (x > (LCD_SCREEN_W - 1) || y > (LCD_SCREEN_H - 1))
		{
            return;
        }
        lcdDrawChar(x, y, *str, size);
        x += size / 2;
        ++str;
    }
}

// 高层函数（第二组）

void lcdDrawLineSimple(u16 x, u16 y, u8 dir, u16 length)
{
    static u16 i;
    
    if (LCD_OTH_SIMPLE_LINE_DIR_PX == dir)
    {
        lcdSetWindow(x, y, x + length - 1, y);
    }
    else if (LCD_OTH_SIMPLE_LINE_DIR_PY == dir)
    {
        lcdSetWindow(x, y, x, y + length - 1);
    }
    else if (LCD_OTH_SIMPLE_LINE_DIR_NX == dir)
    {
        lcdSetWindow(x - length + 1, y, x, y);
    }
    else // LCD_OTH_SIMPLE_LINE_DIR_NY == dir
    {
        lcdSetWindow(x, y - length + 1, x, y);
    }
    for (i = 0; i < length; ++i)
    {
        lcdWriteData16(brushColor);
    }
}
