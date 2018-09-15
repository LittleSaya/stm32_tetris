#include "tetris.h"
#include "lcd.h"
#include "adc_rand.h"
#include "ext_button.h"
#include "systick.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// 120byte, 4bit 一个点， 1bit 表示是否有方块， 3bit 表示方块颜色（的索引）
u8 tetrisPlayground[120]; // playground 10x24=240bit=30byte
u8 tetrisPreview[8]; // preview 4x4=16bit=2byte
Falling tetrisFalling; // 坠落物
NextFalling tetrisNextFalling; // 下一个坠落物
u32 tetrisScore; // 分数
u32 tetrisRowCount; // 消去的行数计数
u8 tetrisCombo; // 连击
u16 tetrisLevel; // 当前等级
u8 tetrisIsPause; // 暂停
u8 tetrisGameOver;

Coord tetrisMoveTemp[4]; // 移动位置暂存
Coord tetrisRotateTemp[4]; // 旋转位置暂存

// 颜色数组
u16 tetrisColorArray[8] = {
    // 坠落物颜色
    LCD_COLOR_BROWN,
    LCD_COLOR_BLUE,
    LCD_COLOR_RED,
    LCD_COLOR_GREEN,
    LCD_COLOR_BRED,
    LCD_COLOR_YELLOW,
    LCD_COLOR_CYAN,
    // 阴影颜色
    LCD_COLOR_LIGHTGRAY
};

// 预定义的坠落物
Falling tetrisPredefinedFallings[7] = {
    { { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 1, 2 } }, { 0, 2 }, 0 }, // L 型
    { { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 0, 2 } }, { 1, 2 }, 1 }, // J 型
    { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 } }, { 1, 0 }, 2 }, // Z 型
    { { { 2, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, { 1, 0 }, 3 }, // S 型
    { { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } }, { 1, 1 }, 4 }, // T 型
    { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, { 0, 0 }, 5 }, // O 型
    { { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, { 2, 0 }, 6 }  // I 型
};

// 预定义的 preview
u8 tetrisPredefinedPreview[7][4][2] = {
    {
        /* L 型
         * OOOO
         * OXOO
         * OXOO
         * OXXO
         */
        { 0x04, 0x46 },
        
        /* L 型 1 次顺时针旋转
         * OOOO
         * OXXX
         * OXOO
         * OOOO
         */
        { 0x07, 0x40 },
        
        /* L 型 2 次顺时针旋转
         * OOOO
         * OXXO
         * OOXO
         * OOXO
         */
        { 0x06, 0x22 },
        
        /* L 型 3 次顺时针旋转
         * OOOO
         * OOXO
         * XXXO
         * OOOO
         */
        { 0x02, 0xE0 },
    },
    {
        /* J 型
         * OOOO
         * OOXO
         * OOXO
         * OXXO
         */
        { 0x02, 0x26 },
        
        /* J 型 1 次顺时针旋转
         * OOOO
         * OXOO
         * OXXX
         * OOOO
         */
        { 0x04, 0x70 },
        
        /* J 型 2 次顺时针旋转
         * OOOO
         * OXXO
         * OXOO
         * OXOO
         */
        { 0x06, 0x44 },
        
        /* L 型 3 次顺时针旋转
         * OOOO
         * XXXO
         * OOXO
         * OOOO
         */
        { 0x0E, 0x20 },
    },
    {
        /* Z 型
         * OOOO
         * XXOO
         * OXXO
         * OOOO
         */
        { 0x0C, 0x60 },
        
        /* Z 型 1 次顺时针旋转
         * OOOO
         * OOXO
         * OXXO
         * OXOO
         */
        { 0x02, 0x64 },
        
        /* Z 型 2 次顺时针旋转
         * OOOO
         * XXOO
         * OXXO
         * OOOO
         */
        { 0x0C, 0x60 },
        
        /* Z 型 3 次顺时针旋转
         * OOOO
         * OOXO
         * OXXO
         * OXOO
         */
        { 0x02, 0x64 },
    },
    {
        /* S 型
         * OOOO
         * OXXO
         * XXOO
         * OOOO
         */
        { 0x06, 0xC0 },
        
        /* S 型 1 次顺时针旋转
         * OOOO
         * OXOO
         * OXXO
         * OOXO
         */
        { 0x04, 0x62 },
        
        /* S 型 2 次顺时针旋转
         * OOOO
         * OXXO
         * XXOO
         * OOOO
         */
        { 0x06, 0xC0 },
        
        /* S 型 3 次顺时针旋转
         * OOOO
         * OXOO
         * OXXO
         * OOXO
         */
        { 0x04, 0x62 },
    },
    {
        /* T 型
         * OOOO
         * OXOO
         * XXXO
         * OOOO
         */
        { 0x04, 0xE0 },
        
        /* T 型 1 次顺时针旋转
         * OOOO
         * OXOO
         * OXXO
         * OXOO
         */
        { 0x04, 0x64 },
        
        /* T 型 2 次顺时针旋转
         * OOOO
         * OOOO
         * XXXO
         * OXOO
         */
        { 0x00, 0xE4 },
        
        /* T 型 3 次顺时针旋转
         * OOOO
         * OOXO
         * OXXO
         * OOXO
         */
        { 0x02, 0x62 },
    },
    {
        /* O 型
         * OOOO
         * OXXO
         * OXXO
         * OOOO
         */
        { 0x06, 0x60 },
        
        /* O 型 1 次顺时针旋转
         * OOOO
         * OXXO
         * OXXO
         * OOOO
         */
        { 0x06, 0x60 },
        
        /* O 型 2 次顺时针旋转
         * OOOO
         * OXXO
         * OXXO
         * OOOO
         */
        { 0x06, 0x60 },
        
        /* O 型 3 次顺时针旋转
         * OOOO
         * OXXO
         * OXXO
         * OOOO
         */
        { 0x06, 0x60 },
    },
    {
        /* I 型
         * OOOO
         * XXXX
         * OOOO
         * OOOO
         */
        { 0x0F, 0x00 },
        
        /* I 型 1 次顺时针旋转
         * OXOO
         * OXOO
         * OXOO
         * OXOO
         */
        { 0x44, 0x44 },
        
        /* I 型 2 次顺时针旋转
         * OOOO
         * XXXX
         * OOOO
         * OOOO
         */
        { 0x0F, 0x00 },
        
        /* I 型 3 次顺时针旋转
         * OXOO
         * OXOO
         * OXOO
         * OXOO
         */
        { 0x44, 0x44 },
    }
};

// 底层函数

u16 tetrisPlayground2Dto1D(u16 x, u16 y)
{
    return y * TETRIS_PLAYGROUND_W + x;
}

void tetrisPlayground1Dto2D(u16 bit, u16 *x, u16 *y)
{
    *y = bit / TETRIS_PLAYGROUND_W;
    *x = bit % TETRIS_PLAYGROUND_W;
}

u16 tetrisPreview2Dto1D(u16 x, u16 y)
{
    return y * TETRIS_PREVIEW_SIZE + x;
}

void tetrisPreview1Dto2D(u16 bit, u16 *x, u16 *y)
{
    *y = bit / TETRIS_PREVIEW_SIZE;
    *x = bit % TETRIS_PREVIEW_SIZE;
}

void tetrisRelativeCoord(Coord *a, Coord *b, Coord *result)
{
    result->x = b->x - a->x;
    result->y = b->y - a->y;
}

void tetrisAbsoluteCoord(Coord *a, Coord *b, Coord *result)
{
    result->x = b->x + a->x;
    result->y = b->y + a->y;
}

void tetrisRotatePoint90(Coord *center, Coord *point, Coord *result)
{
    static Coord temp;
    
    tetrisRelativeCoord(center, point, &temp);
    result->x = -temp.y;
    result->y = temp.x;
    tetrisAbsoluteCoord(center, result, result);
}

void tetrisRotatePoint180(Coord *center, Coord *point, Coord *result)
{
    static Coord temp;
    
    tetrisRelativeCoord(center, point, &temp);
    result->x = -temp.x;
    result->y = -temp.y;
    tetrisAbsoluteCoord(center, result, result);
}

void tetrisRotatePoint270(Coord *center, Coord *point, Coord *result)
{
    static Coord temp;
    
    tetrisRelativeCoord(center, point, &temp);
    result->x = temp.y;
    result->y = -temp.x;
    tetrisAbsoluteCoord(center, result, result);
}

void tetrisCoordCopy(Coord *dst, Coord *src)
{
    static u8 i;
    
    for (i = 0; i < 4; ++i)
    {
        dst[i] = src[i];
    }
}

// 屏幕操作函数

void tetrisDrawPlaygroundOutline(void)
{
    // 边框的宽度是 4 个像素
    lcdSetBrushColor(TETRIS_COLOR_EDGE);
    lcdDrawFillRectangle(0, 0, 3, 127);
    lcdDrawFillRectangle(64, 0, 67, 127);
    lcdDrawFillRectangle(4, 0, 63, 3);
    lcdDrawFillRectangle(4, 124, 63, 127);
}

void tetrisDrawPreviewOutline(void)
{
    // 边框的宽度是 3 个像素，中空
    // 边框的内外边距都是 2 个像素
    lcdSetBrushColor(TETRIS_COLOR_EDGE);
    lcdDrawFillRectangle(70, 2, 125, 2);
    lcdDrawFillRectangle(70, 2, 70, 57);
    lcdDrawFillRectangle(70, 57, 125, 57);
    lcdDrawFillRectangle(125, 2, 125, 57);
    lcdDrawFillRectangle(72, 4, 123, 4);
    lcdDrawFillRectangle(72, 4, 72, 55);
    lcdDrawFillRectangle(72, 55, 123, 55);
    lcdDrawFillRectangle(123, 4, 123, 55);
}

void tetrisDrawLogo(void)
{
    lcdSetBrushColor(TETRIS_COLOR_LETTER);
    lcdDrawString(74, 60, (u8*)"TETRIS", 16);
}

void tetrisDrawScoreTitle(void)
{
    lcdSetBrushColor(TETRIS_COLOR_LETTER);
    lcdDrawString(78, 76, (u8*)"score", 16);
}

void tetrisDrawScore(void)
{
    static char str[7];
    
    lcdSetBrushColor(TETRIS_COLOR_BACK);
    lcdDrawFillRectangle(74, 92, 121, 107);
    lcdSetBrushColor(TETRIS_COLOR_NUMBER);
    sprintf(str, "%06u", tetrisScore);
    lcdDrawString(74, 92, (u8*)str, 16);
}

void tetrisDrawPlayground(u16 x, u16 y)
{
    static u8 exist, color;
    
    // 获取 exist 和 color
    exist = tetrisGetPlaygroundExist(x, y);
    color = tetrisGetPlaygroundColor(x, y);
    // 不绘制隐藏的那 4 行
    if (y < TETRIS_PLAYGROUND_H - TETRIS_PLAYGROUND_VISIBLE_H)
    {
        return;
    }
    // 将 playground 的原点和屏幕原点对齐
    y -= TETRIS_PLAYGROUND_H - TETRIS_PLAYGROUND_VISIBLE_H;
    lcdSetBrushColor(exist ? tetrisColorArray[color] : TETRIS_COLOR_BACK);
    lcdDrawFillRectangle(
        4 + x * TETRIS_PLAYGROUND_BLOCK_SIZE,
        4 + y * TETRIS_PLAYGROUND_BLOCK_SIZE,
        4 + x * TETRIS_PLAYGROUND_BLOCK_SIZE + TETRIS_PLAYGROUND_BLOCK_SIZE - 1,
        4 + y * TETRIS_PLAYGROUND_BLOCK_SIZE + TETRIS_PLAYGROUND_BLOCK_SIZE - 1);
}

void tetrisDrawPreview(u16 x, u16 y)
{
    static u8 exist, color;
    
    // 获取 exist 和 color
    exist = tetrisGetPreviewExist(x, y);
    color = tetrisGetPreviewColor(x, y);
    lcdSetBrushColor(exist ? tetrisColorArray[color] : TETRIS_COLOR_BACK);
    lcdDrawFillRectangle(
        75 + x * (TETRIS_PREVIEW_BLOCK_SIZE + 2),
        7 + y * (TETRIS_PREVIEW_BLOCK_SIZE + 2),
        75 + x * (TETRIS_PREVIEW_BLOCK_SIZE + 2) + TETRIS_PREVIEW_BLOCK_SIZE - 1,
        7 + y * (TETRIS_PREVIEW_BLOCK_SIZE + 2) + TETRIS_PREVIEW_BLOCK_SIZE - 1);
}

void tetrisDrawPreviewFromPredefined(u8 type, u8 state)
{
    static u8 exist;
    static u16 x, y;
    
    for (y = 0; y < 4; ++y)
    {
        for (x = 0; x < 4; ++x)
        {
            exist = tetrisGetPredefinedPreview(type, state, x, y);
            tetrisSetPreviewExist(x, y, exist);
            tetrisSetPreviewColor(x, y, type);
            tetrisDrawPreview(x, y);
        }
    }
}

void tetrisDrawFalling(void)
{
    static u8 i;
    
    for (i = 0; i < 4; ++i)
    {
        tetrisDrawPlayground(tetrisFalling.blocks[i].x, tetrisFalling.blocks[i].y);
    }
}

// 内存操作函数

void tetrisSetPlaygroundExist(u16 x, u16 y, u8 bit)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPlayground2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    // 改变 playground 在内存中的状态
    if (bit)
    {
        tetrisPlayground[byteLoc] |= (0x80 >> (bitLoc * 4));
    }
    else
    {
        tetrisPlayground[byteLoc] &= ~(0x80 >> (bitLoc * 4));
    }
}

void tetrisSetPlaygroundColor(u16 x, u16 y, u8 color)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPlayground2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    // 改变 playground 在内存中的状态
    if (0 == bitLoc)
    {
        color <<= 4;
        color &= 0x70;
        tetrisPlayground[byteLoc] &= 0x8F;
        tetrisPlayground[byteLoc] |= color;
    }
    else
    {
        color &= 0x07;
        tetrisPlayground[byteLoc] &= 0xF8;
        tetrisPlayground[byteLoc] |= color;
    }
}

u8 tetrisGetPlaygroundExist(u16 x, u16 y)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPlayground2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    return ((tetrisPlayground[byteLoc] & (0x80 >> (bitLoc * 4))) ? 1 : 0);
}

u8 tetrisGetPlaygroundColor(u16 x, u16 y)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPlayground2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    // 获取指定位置的颜色索引
    if (0 == bitLoc)
    {
        return ((tetrisPlayground[byteLoc] & 0x70) >> 4) & 0x07;
    }
    else
    {
        return (tetrisPlayground[byteLoc] & 0x07);
    }
}

void tetrisSetPreviewExist(u16 x, u16 y, u8 bit)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPreview2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    // 改变 preview 在内存中的状态
    if (bit)
    {
        tetrisPreview[byteLoc] |= (0x80 >> (bitLoc * 4));
    }
    else
    {
        tetrisPreview[byteLoc] &= ~(0x80 >> (bitLoc * 4));
    }
}

void tetrisSetPreviewColor(u16 x, u16 y, u8 color)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPreview2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    // 改变 preview 在内存中的状态
    if (0 == bitLoc)
    {
        color <<= 4;
        color &= 0x70;
        tetrisPreview[byteLoc] &= 0x8F;
        tetrisPreview[byteLoc] |= color;
    }
    else
    {
        color &= 0x07;
        tetrisPreview[byteLoc] &= 0xF8;
        tetrisPreview[byteLoc] |= color;
    }
}

u8 tetrisGetPreviewExist(u16 x, u16 y)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPreview2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    return ((tetrisPreview[byteLoc] & (0x80 >> (bitLoc * 4))) ? 1 : 0);
}

u8 tetrisGetPreviewColor(u16 x, u16 y)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPreview2Dto1D(x, y);
    byteLoc = loc / 2;
    bitLoc = loc % 2;
    // 获取指定位置的颜色索引
    if (0 == bitLoc)
    {
        return ((tetrisPreview[byteLoc] & 0x70) >> 4) & 0x07;
    }
    else
    {
        return (tetrisPreview[byteLoc] & 0x07);
    }
}

u8 tetrisGetPredefinedPreview(u8 type, u8 state, u16 x, u16 y)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPreview2Dto1D(x, y);
    byteLoc = loc / 8;
    bitLoc = loc % 8;
    return ((tetrisPredefinedPreview[type][state][byteLoc] & (0x80 >> bitLoc)) ? 1 : 0);
}

void tetrisMove2Temp(s16 dx, s16 dy)
{
    tetrisMove2TempOri(dx, dy, tetrisFalling.blocks);
}

void tetrisMove2TempOri(s16 dx, s16 dy, Coord *ori)
{
    static u8 i;
    
    for (i = 0; i < 4; ++i)
    {
        tetrisMoveTemp[i].x = ori[i].x + dx;
        tetrisMoveTemp[i].y = ori[i].y + dy;
    }
}

void tetrisRotate2Temp(u8 cnt)
{
    tetrisRotate2TempOri(cnt, &tetrisFalling.center, tetrisFalling.blocks);
}

void tetrisRotate2TempOri(u8 cnt, Coord *center, Coord *ori)
{
    static u8 i;
    
    switch (cnt)
    {
    case 0:
        {
            // 0 次旋转
            for (i = 0; i < 4; ++i)
            {
                tetrisRotateTemp[i] = ori[i];
            }
        }
        break;
    case 1:
        {
            // 1 次旋转，等价于顺时针 90 度旋转
            for (i = 0; i < 4; ++i)
            {
                tetrisRotatePoint90(center, ori + i, tetrisRotateTemp + i);
            }
        }
        break;
    case 2:
        {
            // 2 次旋转，等价于顺时针 180 度旋转
            for (i = 0; i < 4; ++i)
            {
                tetrisRotatePoint180(center, ori + i, tetrisRotateTemp + i);
            }
        }
        break;
    case 3:
        {
            // 3 次旋转，等价于顺时针 270 度旋转
            for (i = 0; i < 4; ++i)
            {
                tetrisRotatePoint270(center, ori + i, tetrisRotateTemp + i);
            }
        }
        break;
    default:
        {
            while (1);
        }
    }
}

u8 tetrisCollisionDetect(Coord *coord)
{
    static u8 i;
    
    for (i = 0; i < 4; ++i)
    {
        // 越界
        if (coord[i].x < 0 || coord[i].x > TETRIS_PLAYGROUND_W - 1 ||
            coord[i].y < 0 || coord[i].y > TETRIS_PLAYGROUND_H - 1)
        {
            return 1;
        }
        // 目标坐标上已经存在方块
        if (tetrisGetPlaygroundExist(coord[i].x, coord[i].y))
        {
            return 1;
        }
    }
    return 0;
}

void tetrisSetPlaygroundExistWithCoord(u8 bit)
{
    tetrisSetPlaygroundExistWithCoordOri(tetrisFalling.blocks, bit);
}

void tetrisSetPlaygroundExistWithCoordOri(Coord *ori, u8 bit)
{
    static u8 i;
    
    for (i = 0; i < 4; ++i)
    {
        tetrisSetPlaygroundExist(ori[i].x, ori[i].y, bit);
    }
}

void tetrisSetPlaygroundColorWithCoord()
{
    tetrisSetPlaygroundColorWithCoordOri(tetrisFalling.blocks, tetrisFalling.type);
}

void tetrisSetPlaygroundColorWithCoordOri(Coord *ori, u8 color)
{
    static u8 i;
    
    for (i = 0; i < 4; ++i)
    {
        tetrisSetPlaygroundColor(ori[i].x, ori[i].y, color);
    }
}

// 混合（内存操作 + 屏幕操作）

void tetrisGenNextFalling(void)
{
    char buf[12];
    
    tetrisNextFalling.type = adcRand() % 7;
    tetrisNextFalling.state = adcRand() % 4;
    tetrisDrawPreviewFromPredefined(
        tetrisNextFalling.type,
        tetrisNextFalling.state
    );
}

u8 tetrisGenFallingFromNext(void)
{
    static s16 xMin, yMax, dx, dy;
    static u8 i;
    
    tetrisFalling = tetrisPredefinedFallings[tetrisNextFalling.type];
    // 按照给定 state 进行旋转
    tetrisRotate2Temp(tetrisNextFalling.state);
    // 找到 y 的最大值， x 的最小值
    xMin = tetrisRotateTemp[0].x;
    yMax = tetrisRotateTemp[0].y;
    for (i = 1; i < 4; ++i)
    {
        xMin = (tetrisRotateTemp[i].x < xMin ? tetrisRotateTemp[i].x : xMin);
        yMax = (tetrisRotateTemp[i].y > yMax ? tetrisRotateTemp[i].y : yMax);
    }
    // 计算位移量，位移后坠落物的最下沿应该在不可见区域的最后一行
    // 最左沿应该在中轴线左侧那一列
    dy = 3 - yMax;
    dx = 4 - xMin;
    // 移动坐标
    tetrisMove2TempOri(dx, dy, tetrisRotateTemp);
    // 碰撞检测
    if (tetrisCollisionDetect(tetrisMoveTemp))
    {
        return 0;
    }
    // 将 falling 真正地移动到新位置
    tetrisFalling.center.x += dx;
    tetrisFalling.center.y += dy;
    tetrisCoordCopy(tetrisFalling.blocks, tetrisMoveTemp);
    tetrisSetPlaygroundExistWithCoord(1);
    tetrisSetPlaygroundColorWithCoord();
    tetrisDrawFalling();
    return 1;
}

u8 tetrisMove(s16 dx, s16 dy)
{
    // 首先让 falling 所指的坠落物暂时从 playground 上消失
    tetrisSetPlaygroundExistWithCoord(0);
    tetrisMove2Temp(dx, dy);
    if (tetrisCollisionDetect(tetrisMoveTemp))
    {
        // 移动失败
        tetrisSetPlaygroundExistWithCoord(1);
        return 0;
    }
    // 擦去原位置上的方块
    tetrisDrawFalling();
    // 移动 falling 到新的位置
    tetrisFalling.center.x += dx;
    tetrisFalling.center.y += dy;
    tetrisCoordCopy(tetrisFalling.blocks, tetrisMoveTemp);
    // 绘制新的 falling
    tetrisSetPlaygroundExistWithCoord(1);
    tetrisSetPlaygroundColorWithCoord();
    tetrisDrawFalling();
    return 1;
}

u8 tetrisRotate(u8 cnt)
{
    // 踢壁测试列表
    #define TETRIS_KICK_TEST_SIZE 14
    static Coord kickTest[TETRIS_KICK_TEST_SIZE] =
    {
        { 1, 0 }, { -1, 0 }, { 0, -1 }, { 1, -1 }, { -1, -1 },
        { 2, 0 }, { -2, 0 }, { 0, -2 }, { 2, -2 }, { -2, -2 },
        { 2, -1 }, { 1, -2 }, { -2, -1 }, { -1, -2 }
    };
    static u8 i;
    
    tetrisSetPlaygroundExistWithCoord(0);
    tetrisRotate2Temp(cnt);
    if (tetrisCollisionDetect(tetrisRotateTemp))
    {
        // 存在碰撞，尝试踢壁
        for (i = 0; i < TETRIS_KICK_TEST_SIZE; ++i)
        {
            tetrisMove2TempOri(kickTest[i].x, kickTest[i].y, tetrisRotateTemp);
            if (!tetrisCollisionDetect(tetrisMoveTemp))
            {
                break;
            }
        }
        if (TETRIS_KICK_TEST_SIZE == i)
        {
            // 踢壁失败
            tetrisSetPlaygroundExistWithCoord(1);
            return 0;
        }
        // 移动 falling
        tetrisDrawFalling();
        tetrisFalling.center.x += kickTest[i].x;
        tetrisFalling.center.y += kickTest[i].y;
        tetrisCoordCopy(tetrisFalling.blocks, tetrisMoveTemp);
        tetrisSetPlaygroundExistWithCoord(1);
        tetrisSetPlaygroundColorWithCoord();
        tetrisDrawFalling();
        return 1;
    }
    else
    {
        // 无碰撞
        tetrisDrawFalling();
        tetrisCoordCopy(tetrisFalling.blocks, tetrisRotateTemp);
        tetrisSetPlaygroundExistWithCoord(1);
        tetrisSetPlaygroundColorWithCoord();
        tetrisDrawFalling();
        return 1;
    }
}

// 高层函数

void initTetris(void)
{
    // 重置变量
    memset(tetrisPlayground, 0, sizeof(tetrisPlayground));
    memset(tetrisPreview, 0, sizeof(tetrisPreview));
    memset(&tetrisFalling, 0, sizeof(tetrisFalling));
    memset(&tetrisNextFalling, 0, sizeof(tetrisNextFalling));
    tetrisRowCount = tetrisScore = tetrisCombo = tetrisLevel = tetrisIsPause = tetrisGameOver = 0;
    memset(tetrisMoveTemp, 0, sizeof(tetrisMoveTemp));
    memset(tetrisRotateTemp, 0, sizeof(tetrisRotateTemp));
    // 重新绘制边框、 Logo 、分数标题和分数
    lcdClear(TETRIS_COLOR_BACK);
    tetrisDrawPlaygroundOutline();
    tetrisDrawPreviewOutline();
    tetrisDrawLogo();
    tetrisDrawScoreTitle();
    tetrisDrawScore();
    // 生成 preview 和 falling
    tetrisGenNextFalling();
    tetrisGenFallingFromNext();
    tetrisGenNextFalling();
}

void tetrisGameLoop(void)
{
    static u32 currTime = 0,
               lastTime = 0,
               deltaTime = 0,
               accumTime = 0;
    static u8 ret;
    
    // 如果 game over 则游戏不再进行
    if (tetrisGameOver)
    {
        return;
    }
    
    // 获取时间差
    currTime = getRunningTime();
    deltaTime = currTime - lastTime;
    lastTime = currTime;
    
    // 响应用户输入
    tetrisUserInput();
    if (tetrisIsPause)
    {
        // 如果处于暂停状态则退出本次游戏循环
        return;
    }
    
    // 累加时间
    accumTime += deltaTime;
    
    // 计算累加时间是否超过当前下落间隔
    if (tetrisGetInterval() > accumTime)
    {
        // 不需要下落
        return;
    }
    
    // 需要下落，执行一次下落
    accumTime = 0;
    ret = tetrisMove(0, 1);
    if (ret)
    {
        // 下落成功
        return;
    }
    
    // 下落执行失败，执行着陆
    tetrisLand();
    
    // 生成新的 falling
    ret = tetrisGenFallingFromNext();
    tetrisGenNextFalling();
    
    if (!ret)
    {
        // 生成失败，游戏结束
        tetrisDebug("Game Over");
        tetrisGameOver = 1;
    }
}

// 游戏循环组件

void tetrisUserInput(void)
{
    extBtnCmdScan();
    // 旋转和移动只有在不暂停时才会正常执行
    if (extBtnGetCmd(EXT_BTN_UP_IDX) == EXT_BTN_CMD_DOWN)
    {
        // 上键旋转
        if (!tetrisIsPause)
        {
            tetrisRotate(1);
        }
    }
    else if (extBtnGetCmd(EXT_BTN_LEFT_IDX) == EXT_BTN_CMD_DOWN ||
             extBtnGetCmd(EXT_BTN_LEFT_IDX) == EXT_BTN_CMD_LONG_PRESS)
    {
        // 左键左移
        if (!tetrisIsPause)
        {
            tetrisMove(-1, 0);
        }
    }
    else if (extBtnGetCmd(EXT_BTN_RIGHT_IDX) == EXT_BTN_CMD_DOWN ||
             extBtnGetCmd(EXT_BTN_RIGHT_IDX) == EXT_BTN_CMD_LONG_PRESS)
    {
        // 右键右移
        if (!tetrisIsPause)
        {
            tetrisMove(1, 0);
        }
    }
    else if (extBtnGetCmd(EXT_BTN_DOWN_IDX) == EXT_BTN_CMD_DOWN ||
             extBtnGetCmd(EXT_BTN_DOWN_IDX) == EXT_BTN_CMD_LONG_PRESS)
    {
        // 下键下移
        if (!tetrisIsPause)
        {
            tetrisMove(0, 1);
        }
    }
    else if (extBtnGetCmd(EXT_BTN_NO_IDX) == EXT_BTN_CMD_DOWN)
    {
        // 暂停操作的执行不会受到当前暂停状态的影响（废话）
        tetrisIsPause ^= 0x01;
    }
}

u32 tetrisGetInterval(void)
{
    static s32 result;
    
    result = TETRIS_BASIC_FALL_INTERVAL + tetrisLevel * TETRIS_FALL_INTERVAL_LV_STEP;
    return (result < TETRIS_MIN_FALL_INTERVAL ? TETRIS_MIN_FALL_INTERVAL : result);
}

void tetrisLand(void)
{
    static u16 yStart, yEnd;
    static u16 x, y, y2, i;
    static u8 lineFull;
    static u8 count; // 一次消了多少行
    
    // 计算 yStart 和 yEnd
    yStart = yEnd = tetrisFalling.blocks[0].y;
    for (i = 1; i < 4; ++i)
    {
        yStart = (tetrisFalling.blocks[i].y > yStart ? tetrisFalling.blocks[i].y : yStart);
        yEnd = (tetrisFalling.blocks[i].y < yEnd ? tetrisFalling.blocks[i].y : yEnd);
    }
    
    // 自底向上进行消行
    count = 0;
    for (y = yStart; y >= yEnd;)
    {
        // 判断该行是否已满
        lineFull = 1;
        for (x = 0; x < TETRIS_PLAYGROUND_W; ++x)
        {
            if (!tetrisGetPlaygroundExist(x, y))
            {
                lineFull = 0;
                break;
            }
        }
        
        // 未满则跳过该行
        if (!lineFull)
        {
            --y;
            continue;
        }
        
        // 已满，执行消行
        ++count;
        for (x = 0; x < TETRIS_PLAYGROUND_W; ++x)
        {
            tetrisSetPlaygroundExist(x, y, 0);
            tetrisDrawPlayground(x, y);
        }
        // 下落
        for (y2 = y; y2 > 0; --y2)
        {
            for (x = 0; x < TETRIS_PLAYGROUND_W; ++x)
            {
                tetrisSetPlaygroundExist(x, y2,
                    tetrisGetPlaygroundExist(x, y2 - 1)
                );
                tetrisDrawPlayground(x, y2);
            }
        }
    }
    
    // combo 计算
    tetrisRowCount += count;
    if (count > 0)
    {
        ++tetrisCombo;
    
        // 分数计算
        tetrisScore += tetrisCombo * (2 * count - 1);
        tetrisDrawScore();
        
        // 等级计算
        tetrisLevel = (tetrisRowCount / 10);
    }
    else
    {
        tetrisCombo = 0;
    }
}

// 其他

void tetrisDebug(char *str)
{
    lcdSetBrushColor(TETRIS_COLOR_BACK);
    lcdDrawFillRectangle(69, 108, 127, 127);
    lcdSetBrushColor(TETRIS_COLOR_DEBUG);
    lcdDrawString(69, 108, (u8*)str, 12);
}

// 废弃

u8 tetrisGetPreview(u16 x, u16 y)
{
    static u16 loc;
    static u8 byteLoc, bitLoc;
    
    loc = tetrisPreview2Dto1D(x, y);
    byteLoc = loc / 8;
    bitLoc = loc % 8;
    return ((tetrisPreview[byteLoc] & (0x80 >> bitLoc)) ? 1 : 0);
}
