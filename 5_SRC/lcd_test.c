#include "lcd.h"

extern u16 brushColor;

void lcdTest_Window(void)
{
    u16 i;
    
    lcdClear(LCD_COLOR_WHITE);
    
    // 11
    lcdSetBrushColor(LCD_COLOR_BLACK);
    lcdSetWindow(13, 1, 23, 11);
    for (i = 0; i < 121; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 10
    lcdSetWindow(13, 12, 22, 21);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 100; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 9
    lcdSetWindow(13, 22, 21, 30);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 81; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 8
    lcdSetWindow(13, 31, 20, 38);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 64; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 7
    lcdSetWindow(13, 39, 19, 45);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 49; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 6
    lcdSetWindow(13, 46, 18, 51);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 36; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 5
    lcdSetWindow(13, 52, 17, 56);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 25; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 4
    lcdSetWindow(13, 57, 16, 60);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 16; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 3
    lcdSetWindow(13, 61, 15, 63);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 9; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 2
    lcdSetWindow(13, 64, 14, 65);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 4; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 1
    lcdSetWindow(13, 66, 13, 66);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 1; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 11
    lcdSetBrushColor(LCD_COLOR_BLACK);
    lcdSetWindow(1, 1, 11, 11);
    for (i = 0; i < 120; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 10
    lcdSetWindow(1, 13, 10, 22);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 100; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 9
    lcdSetWindow(1, 24, 9, 32);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 81; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 8
    lcdSetWindow(1, 34, 8, 41);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 64; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 7
    lcdSetWindow(1, 43, 7, 49);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 49; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 6
    lcdSetWindow(1, 51, 6, 56);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 36; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 5
    lcdSetWindow(1, 58, 5, 62);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 25; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 4
    lcdSetWindow(1, 64, 4, 67);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 16; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 3
    lcdSetWindow(1, 69, 3, 71);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 9; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 2
    lcdSetWindow(1, 73, 2, 74);
    lcdSetBrushColor(LCD_COLOR_BLUE);
    for (i = 0; i < 4; ++i)
    {
        lcdWriteData16(brushColor);
    }
    
    // 1
    lcdSetWindow(1, 76, 1, 76);
    lcdSetBrushColor(LCD_COLOR_BLACK);
    for (i = 0; i < 1; ++i)
    {
        lcdWriteData16(brushColor);
    }
}
