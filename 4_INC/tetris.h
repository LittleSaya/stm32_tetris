#ifndef _TETRIS_H_
#define _TETRIS_H_

#include "lcd.h"

#define TETRIS_PLAYGROUND_W 10
#define TETRIS_PLAYGROUND_H 24
#define TETRIS_PLAYGROUND_VISIBLE_H 20

#define TETRIS_PREVIEW_SIZE 4

#define TETRIS_PLAYGROUND_BLOCK_SIZE 6
#define TETRIS_PREVIEW_BLOCK_SIZE 10

#define TETRIS_COLOR_EDGE LCD_COLOR_WHITE
#define TETRIS_COLOR_LETTER LCD_COLOR_GRAY
#define TETRIS_COLOR_NUMBER LCD_COLOR_RED
#define TETRIS_COLOR_BACK LCD_COLOR_BLACK
#define TETRIS_COLOR_DEBUG LCD_COLOR_GREEN

#define TETRIS_BASIC_FALL_INTERVAL 1000
#define TETRIS_FALL_INTERVAL_LV_STEP -10
#define TETRIS_MIN_FALL_INTERVAL 125

typedef struct _type_Coord_
{
    s16 x;
    s16 y;
} Coord;

typedef struct _type_Falling_
{
    Coord blocks[4];
    Coord center;
    u8 type; // 坠落物的类型，用于访问颜色数组
} Falling;

typedef struct _type_NextFalling_
{
    u8 type;
    u8 state;
} NextFalling;

// 底层函数

/**
 *  @biref 将 playground 上的方块坐标转换为 playground 对象中对应 4bit 的位置
 *  @param x 方块的 x 坐标
 *  @param y 方块的 y 坐标
 *  @return 对应 4bit 的位置
 */
u16 tetrisPlayground2Dto1D(u16 x, u16 y);

/**
 *  @biref 将 playground 对象中对应 4bit 的位置转换为 playground 上的方块坐标 
 *  @param bit 对应 4bit 的位置
 *  @param x 方块的 x 坐标
 *  @param y 方块的 y 坐标
 */
void tetrisPlayground1Dto2D(u16 bit, u16 *x, u16 *y);

/**
 *  @biref 将 preview 上的方块坐标转换为 preview 对象中对应 4bit 的位置
 *  @param x 方块的 x 坐标
 *  @param y 方块的 y 坐标
 *  @return 对应 4bit 的位置
 */
u16 tetrisPreview2Dto1D(u16 x, u16 y);

/**
 *  @biref 将 preview 对象中对应 4bit 的位置转换为 preview 上的方块坐标 
 *  @param bit 对应 4bit 的位置
 *  @param x 方块的 x 坐标
 *  @param y 方块的 y 坐标
 */
void tetrisPreview1Dto2D(u16 bit, u16 *x, u16 *y);

/**
 *  @biref 计算 b 相对于 a 的坐标
 *  @param a 坐标 a
 *  @param b 坐标 b
 *  @param result 存储相对坐标结果
 */
void tetrisRelativeCoord(Coord *a, Coord *b, Coord *result);

/**
 *  @biref b 为相对于 a 的坐标，计算 b 的绝对坐标
 *  @param a 坐标 a
 *  @param b 坐标 b
 *  @param result 存储绝对坐标结果
 */
void tetrisAbsoluteCoord(Coord *a, Coord *b, Coord *result);

/**
 *  @biref 将指定坐标按照指定中心顺时针旋转 90 度
 *      该屏幕坐标系的 Y 轴正方向与笛卡尔坐标系的 Y 轴正方向相反
 *  @param center 旋转中心
 *  @param point 待旋转坐标
 *  @param result 旋转后的坐标
 */
void tetrisRotatePoint90(Coord *center, Coord *point, Coord *result);

/**
 *  @biref 将指定坐标按照指定中心顺时针旋转 180 度
 *      该屏幕坐标系的 Y 轴正方向与笛卡尔坐标系的 Y 轴正方向相反
 *  @param center 旋转中心
 *  @param point 待旋转坐标
 *  @param result 旋转后的坐标
 */
void tetrisRotatePoint180(Coord *center, Coord *point, Coord *result);

/**
 *  @biref 将指定坐标按照指定中心顺时针旋转 270 度
 *      该屏幕坐标系的 Y 轴正方向与笛卡尔坐标系的 Y 轴正方向相反
 *  @param center 旋转中心
 *  @param point 待旋转坐标
 *  @param result 旋转后的坐标
 */
void tetrisRotatePoint270(Coord *center, Coord *point, Coord *result);

/**
 *  @biref 复制坐标
 *  @param dst 目标坐标
 *  @param src 源坐标
 */
void tetrisCoordCopy(Coord *dst, Coord *src);

// 屏幕操作函数

/**
 *  @biref 绘制 playground 窗口的边框
 */
void tetrisDrawPlaygroundOutline(void);

/**
 *  @biref 绘制 preview 窗口的边框
 */
void tetrisDrawPreviewOutline(void);

/**
 *  @biref 绘制 logo "TETRIS"
 */
void tetrisDrawLogo(void);

/**
 *  @biref 绘制分数标题 "score"
 */
void tetrisDrawScoreTitle(void);

/**
 *  @biref 绘制分数值，范围 [0, 999999]
 *  @param score 分数值
 */
void tetrisDrawScore(void);

/**
 *  @biref 在 playground 上绘制/擦除一个方块，以 playground 的左上角为原点，坐标轴方向与屏幕相同，
 *      只有屏幕下方的 20 格才会被绘制。方块与边框之间没有间隙，方块与方块之间无间隙。
 *  @param x 方块的 x 坐标，大于等于 0 且小于 10
 *  @param y 方块的 y 坐标，大于等于 0 且小于 24
 */
void tetrisDrawPlayground(u16 x, u16 y);

/**
 *  @biref 在 preview 上绘制/擦除一个方块，以 preview 的左上角为原点，坐标轴方向与屏幕相同
 *  @param x 方块的 x 坐标，大于等于 0 且小于 4
 *  @param y 方块的 y 坐标，大于等于 0 且小于 4
 *  @param flag 绘制还是擦除
 *      0 ：擦除
 *      非 0 ：绘制
 */
void tetrisDrawPreview(u16 x, u16 y);

/**
 *  @biref 根据指定的方块类型和旋转状态在 preview 中绘制坠落物
 *  @param type 方块类型
 *  @param state 旋转状态
 */
void tetrisDrawPreviewFromPredefined(u8 type, u8 state);

/**
 *  @biref 绘制 falling 所指坐标
 */
void tetrisDrawFalling(void);

// 内存操作函数

/**
 *  @biref 设置 playground 上某个坐标处是否有方块
 *  @param x x 坐标
 *  @param y y 坐标
 *  @param bit (x, y) 上是否有方块
 *      0 ：无方块
 *      非 0 ：有方块
 */
void tetrisSetPlaygroundExist(u16 x, u16 y, u8 bit);

/**
 *  @biref 设置 playground 上某个坐标处的颜色
 *  @param x x 坐标
 *  @param y y 坐标
 *  @param color (x, y) 上的颜色索引
 */
void tetrisSetPlaygroundColor(u16 x, u16 y, u8 color);

/**
 *  @biref 获取 playground 上某个坐标处是否有方块
 *  @param x x 坐标
 *  @param y y 坐标
 *  @return (x, y) 上是否有方块
 *      0 ：无方块
 *      非 0 ：有方块
 */
u8 tetrisGetPlaygroundExist(u16 x, u16 y);

/**
 *  @biref 获取 playground 上某个坐标处的颜色索引
 *  @param x x 坐标
 *  @param y y 坐标
 *  @return (x, y) 上的颜色索引
 */
u8 tetrisGetPlaygroundColor(u16 x, u16 y);

/**
 *  @biref 设置 preview 上某个坐标处是否有方块，不会真的绘制方块
 *  @param x x 坐标
 *  @param y y 坐标
 *  @param bit (x, y) 上是否有方块
 *      0 ：无方块
 *      非 0 ：有方块
 */
void tetrisSetPreviewExist(u16 x, u16 y, u8 bit);

/**
 *  @biref 设置 preview 上某个坐标处的颜色
 *  @param x x 坐标
 *  @param y y 坐标
 *  @param color (x, y) 上的颜色索引
 */
void tetrisSetPreviewColor(u16 x, u16 y, u8 color);

/**
 *  @biref 获取 preview 上某个坐标处是否有方块
 *  @param x x 坐标
 *  @param y y 坐标
 *  @return (x, y) 上是否有方块
 *      0 ：无方块
 *      非 0 ：有方块
 */
u8 tetrisGetPreviewExist(u16 x, u16 y);

/**
 *  @biref 获取 preview 上某个坐标处的颜色索引
 *  @param x x 坐标
 *  @param y y 坐标
 *  @return (x, y) 上的颜色索引
 */
u8 tetrisGetPreviewColor(u16 x, u16 y);

/**
 *  @biref 获取预定义 preview 上某个类型的坠落物在某个旋转状态下的某个位置上的方块存在状态
 *  @param type 方块类型（ 0..6 ）
 *  @param state 方块状态（ 0..3 ）
 *  @param x x 坐标
 *  @param y y 坐标
 *  @return (x, y) 上是否有方块
 *      0 ：无方块
 *      非 0 ：有方块
 */
u8 tetrisGetPredefinedPreview(u8 type, u8 state, u16 x, u16 y);

/**
 *  @biref 移动 falling 到临时位置（不改变旋转中心的位置）
 *  @param dx 在 x 轴正方向上的移动量
 *  @param dy 在 y 轴正方向上的移动量
 */
void tetrisMove2Temp(s16 dx, s16 dy);

/**
 *  @biref 移动指定坐标到临时位置
 *  @param dx 在 x 轴正方向上的移动量
 *  @param dy 在 y 轴正方向上的移动量
 *  @param ori 原位置，大小为 4 的 Coord 数组
 */
void tetrisMove2TempOri(s16 dx, s16 dy, Coord *ori);

/**
 *  @biref 顺时针旋转 falling 到临时位置
 *  @param cnt 旋转次数
 */
void tetrisRotate2Temp(u8 cnt);

/**
 *  @biref 按照指定旋转中心顺时针旋转指定坐标到临时位置
 *  @param cnt 旋转次数
 *  @param center 旋转中心
 *  @param ori 原位置，大小为 4 的 Coord 数组
 */
void tetrisRotate2TempOri(u8 cnt, Coord *center, Coord *ori);

/**
 *  @biref 碰撞检测，在 playground 中检查一些坐标
 *      已经存在方块，或者坐标超出 playground 的边界均视为发生碰撞
 *  @param coord 待检测坐标列表
 *  @return 是否产生碰撞
 *      0 ：没有碰撞产生
 *      非 0 ：产生碰撞
 */
u8 tetrisCollisionDetect(Coord *coord);

/**
 *  @biref 设置 playground 的一些由 falling 指定的坐标上是否存在方块
 *  @param bit 是否存在方块
 *      0 ：不存在
 *      非 0 ：存在
 */
void tetrisSetPlaygroundExistWithCoord(u8 bit);

/**
 *  @biref 设置 playground 的一些指定坐标上是否存在方块
 *  @param ori 坐标列表
 *  @param bit 是否存在方块
 *      0 ：不存在
 *      非 0 ：存在
 */
void tetrisSetPlaygroundExistWithCoordOri(Coord *ori, u8 bit);

/**
 *  @biref 设置 playground 的一些由 falling 指定的坐标上的颜色
 */
void tetrisSetPlaygroundColorWithCoord(void);

/**
 *  @biref 设置 playground 的一些指定坐标上的颜色
 *  @param ori 坐标列表
 *  @param color 颜色索引
 */
void tetrisSetPlaygroundColorWithCoordOri(Coord *ori, u8 color);

// 混合（内存操作 + 屏幕操作）

/**
 *  @biref 随机生成一个 next falling，并将其绘制在 preview 上
 */
void tetrisGenNextFalling(void);

/**
 *  @biref 根据 next falling 生成新的 falling ，新的 falling 将会生成在
 *      旧 falling 所指向的坠落物将立即被“固化”
 *  @return 生成是否成功
 *      0 ：生成失败，生成位置超出 playground 的边界，或被其他方块占用
 *      非 0 ：生成成功
 */
u8 tetrisGenFallingFromNext(void);

/**
 *  @biref 移动 falling
 *  @param dx falling 在 x 轴正方向上的移动量
 *  @param dy falling 在 y 轴正方向上的移动量
 *  @return 移动是否成功
 *      0 ：移动失败，目标位置超出 playground 的边界，或者被阻挡
 *      非 0 ：移动成功
 */
u8 tetrisMove(s16 dx, s16 dy);

/**
 *  @biref 顺时针旋转 falling 指定次数
 *  @param cnt 旋转次数
 *  @return 旋转是否成功
 *      0 ： 旋转失败
 *      非 0 ：旋转成功
 */
u8 tetrisRotate(u8 cnt);

// 高层函数

/**
 *  @biref 初始化 tetris
 *      初始化变量，界面
 *      初始化 falling 、 next falling 和对应的 playground 、 preview
 */
void initTetris(void);

/**
 *  @biref 游戏循环
 *      检测并响应用户操作，检测并执行方块下落，检测并执行消行，消行后的分数增加
 */
void tetrisGameLoop(void);

// 游戏循环组件

/**
 *  @biref 响应用户输入
 */
void tetrisUserInput(void);

/**
 *  @biref 根据当前等级计算下落间隔
 */
u32 tetrisGetInterval(void);

/**
 *  @biref 着陆
 *      消行， combo 计算，分数计算，等级计算
 */
void tetrisLand(void);

// 辅助

/**
 *  @biref 判断 y 是否在 playground 的范围内
 *  @param y y 的值
 *  @return 参数是否满足要求
 *      0 ：不满足
 *      非 0 ：满足
 */
inline u8 tetrisCheckY(u16 y)
{
    return (y >= 0 && y < TETRIS_PLAYGROUND_H);
}

/**
 *  @biref 判断 x 是否在 playground 的范围内
 *  @param x x 的值
 *  @return 参数是否满足要求
 *      0 ：不满足
 *      非 0 ：满足
 */
inline u8 tetrisCheckX(u16 x)
{
    return (x >= 0 && x < TETRIS_PLAYGROUND_W);
}

// 其他

void tetrisDebug(char * str);

#endif
