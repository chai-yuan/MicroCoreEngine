#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "GameEngineDef.h"

/**
 * @brief   游戏入口函数。
 * @details 游戏初始化函数，在游戏第一次开始运行时被执行。
 */
void game_init(void);

/**
 * @brief   游戏循环函数。
 * @details 根据当前设定的 fps ，每间隔一段时间被调用一次，调用结束后会自动刷新图像。
 */
void game_loop(void);

//=============================================================================
// 系统 API (System API)
//=============================================================================

/**
 * @brief   分配指定大小的内存。
 * @details 这是引擎的内存分配函数，推荐在游戏逻辑中使用此函数代替标准`malloc`，
 *          以便引擎进行统一的内存跟踪或管理。
 * @param   size 要分配的内存字节数。
 * @return  成功时返回指向已分配内存的指针，失败时返回NULL。
 * @see     system_free()
 */
void *system_malloc(size_t size);

/**
 * @brief   释放由`system_malloc`分配的内存。
 * @param   ptr 要释放的内存指针。必须是由`system_malloc`返回的指针。
 * @see     system_malloc()
 */
void system_free(void *ptr);

/**
 * @brief   设置游戏的目标帧率（FPS）。
 * @details 引擎会尝试将游戏逻辑和渲染更新维持在这个速率。
 * @param   fps 目标帧率，例如 30 或 60。
 */
void system_setFPS(int fps);

/**
 * @brief   获取当前所有按钮的按下状态。
 * @details 返回一个位掩码（bitmask），每一位代表一个按钮的状态。
 *          通常需要与预定义的按钮常量（如 `BUTTON_A`, `BUTTON_B`）进行位与运算来检查特定按钮。
 * @return  一个代表按钮状态的无符号整数位掩码。
 */
unsigned int system_getButtonState(void);

/**
 * @brief   获取自引擎启动以来经过的时间。
 * @return  经过的时间，单位通常是毫秒（ms）。
 */
unsigned int system_getCurrentTime(void);

/**
 * @brief   为引擎的伪随机数生成器设置种子。
 * @param   seed 随机数种子。
 * @see     system_rand()
 */
void system_srand(unsigned int seed);

/**
 * @brief   生成一个伪随机整数。
 * @details 在调用此函数前，应先使用`system_srand`设置种子以获得不同的随机序列。
 * @return  一个伪随机整数。
 * @see     system_srand()
 */
int system_rand(void);

//=============================================================================
// 显示 API (Display API)
//=============================================================================

/**
 * @brief   获取显示屏幕或窗口的高度。
 * @return  屏幕高度，单位为像素。
 */
int display_getHeight(void);

/**
 * @brief   获取显示屏幕或窗口的宽度。
 * @return  屏幕宽度，单位为像素。
 */
int display_getWidth(void);

//=============================================================================
// 图形 API (Graphics API)
//=============================================================================

/**
 * @brief   用指定颜色清空当前的绘制目标（通常是屏幕）。
 * @param   color 用于清屏的颜色。
 */
void graphics_clear(Color color);

/**
 * @brief   根据原始像素数据创建一个新的图像资源。
 * @details 图像可以包含多个帧，用于实现动画。所有帧的尺寸必须相同。
 * @param   count  图像中的帧数（对于静态图，此值为1）。
 * @param   width  单帧图像的宽度（像素）。
 * @param   height 单帧图像的高度（像素）。
 * @param   format 像素数据的格式。
 * @param   data   指向原始像素数据的指针。
 * @return  一个指向新创建图像的句柄（ImageHandle）。如果创建失败则返回NULL。
 * @see     graphics_freeImage()
 */
ImageHandle graphics_newImage(int count, int width, int height, PixelFormat format, const uint8_t *data);

/**
 * @brief   将绘制目标推入一个栈，并将一个新的图像设置为当前绘制目标。
 * @details 这允许实现“渲染到纹理”（Render-to-Texture）的功能。后续的绘制操作将作用于`target`图像上，
 *          直到调用`graphics_popContext`。
 * @param   target 一个将作为新绘制目标的图像句柄。
 * @see     graphics_popContext()
 */
void graphics_pushContext(ImageHandle target);

/**
 * @brief   从栈中弹出绘制目标，恢复到上一个绘制状态。
 * @details 如果栈为空，则恢复到默认的屏幕绘制目标。
 * @see     graphics_pushContext()
 */
void graphics_popContext(void);

/**
 * @brief   在当前绘制目标上绘制一个指定的图像帧。
 * @param   image 要绘制的图像句柄。
 * @param   x     绘制位置的X坐标（左上角）。
 * @param   y     绘制位置的Y坐标（左上角）。
 * @param   idx   要绘制的图像帧的索引（从0开始）。
 * @param   flip  图像的翻转方式。
 */
void graphics_drawImage(ImageHandle image, int x, int y, int idx, ImageFlip flip);

/**
 * @brief   设置一个全局的绘制偏移量（用于实现镜头/相机）。
 * @details 所有后续的绘制坐标 (x, y) 都会自动加上这个偏移量 (dx, dy)。
 *          例如，要实现相机向右移动，可以设置一个负的dx值。
 * @param   dx X方向的偏移量。
 * @param   dy Y方向的偏移量。
 */
void graphics_setDrawOffset(int dx, int dy);

/**
 * @brief   设置裁剪矩形区域。
 * @details 所有后续的绘制操作都将被限制在该矩形区域内，超出部分将被裁掉。
 *          要取消裁剪，可以设置一个覆盖整个屏幕的矩形。
 * @param   rect 定义裁剪区域的矩形。
 */
void graphics_setClipRect(Rect rect);

/**
 * @brief   绘制一条线段。
 * @param   p1    线段的起点。
 * @param   p2    线段的终点。
 * @param   width 线段的宽度（像素）。
 * @param   color 线段的颜色。
 */
void graphics_drawLine(Point p1, Point p2, int width, Color color);

/**
 * @brief   绘制一个填充矩形。
 * @param   rect  要绘制的矩形的位置和尺寸。
 * @param   color 矩形的填充颜色。
 */
void graphics_drawRect(Rect rect, Color color);

/**
 * @brief   获取指定图像单帧的尺寸。
 * @param   image  图像句柄。
 * @param   width  [out] 用于接收图像宽度的指针。
 * @param   height [out] 用于接收图像高度的指针。
 */
void graphics_getImageSize(ImageHandle image, int *width, int *height);

/**
 * @brief   释放一个图像资源，回收其占用的内存。
 * @param   image 要释放的图像句柄。
 * @see     graphics_newImage()
 */
void graphics_freeImage(ImageHandle image);

//=============================================================================
// 瓦片地图 API (TileMap API)
//=============================================================================

/**
 * @brief   创建一个新的瓦片地图实例。
 * @param   tilesWide 地图的宽度（图块数）。
 * @param   tilesHigh 地图的高度（图块数）。
 * @return  一个指向新创建瓦片地图的句柄（TileMapHandle）。
 * @see     tilemap_freeTilemap()
 */
TileMapHandle tilemap_newTilemap(int tilesWide, int tilesHigh);

/**
 * @brief   释放一个瓦片地图实例及其相关资源。
 * @param   tilemap 要释放的瓦片地图句柄。
 * @see     tilemap_newTilemap()
 */
void tilemap_freeTilemap(TileMapHandle tilemap);

/**
 * @brief   为瓦片地图设置用于渲染的图块集（tileset）图像。
 * @param   tilemap 瓦片地图句柄。
 * @param   image   包含所有图块的图像句柄。该图像通常被视为一个网格。
 */
void tilemap_setImage(TileMapHandle tilemap, ImageHandle image);

/**
 * @brief   获取瓦片地图当前使用的图块集图像。
 * @param   tilemap 瓦片地图句柄。
 * @return  图块集图像的句柄。
 */
ImageHandle tilemap_getImage(TileMapHandle tilemap);

/**
 * @brief   设置瓦片地图的尺寸（以图块为单位）。
 * @param   tilemap   瓦片地图句柄。
 * @param   tilesWide 地图的宽度（图块数）。
 * @param   tilesHigh 地图的高度（图块数）。
 */
void tilemap_setSize(TileMapHandle tilemap, int tilesWide, int tilesHigh);

/**
 * @brief   获取瓦片地图的尺寸（以图块为单位）。
 * @param   tilemap   瓦片地图句柄。
 * @param   tilesWide [out] 用于接收地图宽度的指针。
 * @param   tilesHigh [out] 用于接收地图高度的指针。
 */
void tilemap_getSize(TileMapHandle tilemap, int *tilesWide, int *tilesHigh);

/**
 * @brief   获取瓦片地图的总像素尺寸。
 * @details 该尺寸等于 (地图宽度 * 图块宽度, 地图高度 * 图块高度)。
 * @param   tilemap 瓦片地图句柄。
 * @param   width   [out] 用于接收地图总像素宽度的指针。
 * @param   height  [out] 用于接收地图总像素高度的指针。
 */
void tilemap_getPixelSize(TileMapHandle tilemap, uint32_t *width, uint32_t *height);

/**
 * @brief   批量设置瓦片地图的图块数据。
 * @param   tilemap   瓦片地图句柄。
 * @param   tiles     一个一维数组，存储了地图上每个位置的图块索引。
 * @param   tilesWide `tiles`数组对应的宽度。
 * @param   tilesHigh `tiles`数组对应的高度。
 */
void tilemap_setTiles(TileMapHandle tilemap, uint8_t *tiles);

/**
 * @brief   设置地图上指定位置的图块索引。
 * @param   tilemap 瓦片地图句柄。
 * @param   tilex   目标位置的X坐标（图块单位）。
 * @param   tiley   目标位置的Y坐标（图块单位）。
 * @param   idx     要设置的图块索引，该索引对应于图块集图像中的某个图块。
 */
void tilemap_setTileAtPosition(TileMapHandle tilemap, int tilex, int tiley, uint8_t idx);

/**
 * @brief   获取地图上指定位置的图块索引。
 * @param   tilemap 瓦片地图句柄。
 * @param   tilex   目标位置的X坐标（图块单位）。
 * @param   tiley   目标位置的Y坐标（图块单位）。
 * @return  该位置的图块索引。如果坐标无效，则返回-1。
 */
int tilemap_getTileAtPosition(TileMapHandle tilemap, int tilex, int tiley);

/**
 * @brief   批量设置瓦片地图的碰撞数据。
 * @param   tilemap   瓦片地图句柄。
 * @param   collision 一个一维数组，存储了地图上每个位置的碰撞掩码。
 * @param   tilesWide `tiles`数组对应的宽度。
 * @param   tilesHigh `tiles`数组对应的高度。
 */
void tilemap_setCollision(TileMapHandle tilemap, uint8_t *collision);

/**
 * @brief   设置瓦片地图的绘制位置（卷轴位置）。
 * @details 这决定了瓦片地图在屏幕上的哪个位置开始绘制，常用于实现镜头滚动。
 * @param   m 瓦片地图句柄。
 * @param   x 屏幕上的X坐标。
 * @param   y 屏幕上的Y坐标。
 */
void tilemap_setPosition(TileMapHandle m, int x, int y);

//=============================================================================
// 精灵 API (Sprite API)
//=============================================================================

/**
 * @brief   创建一个新的精灵实例。
 * @return  一个指向新创建精灵的句柄（SpriteHandle）。
 * @see     sprite_freeSprite()
 */
SpriteHandle sprite_newSprite(void);

/**
 * @brief   释放一个精灵实例。
 * @param   sprite 要释放的精灵句柄。
 * @see     sprite_newSprite()
 */
void sprite_freeSprite(SpriteHandle sprite);

/**
 * @brief   设置精灵在世界坐标系中的位置。
 * @param   sprite 精灵句柄。
 * @param   x      X坐标。
 * @param   y      Y坐标。
 */
void sprite_setPosition(SpriteHandle sprite, int x, int y);

/**
 * @brief   获取精灵在世界坐标系中的位置。
 * @param   sprite 精灵句柄。
 * @param   x      [out] 用于接收精灵X坐标的指针。
 * @param   y      [out] 用于接收精灵Y坐标的指针。
 */
void sprite_getPosition(SpriteHandle sprite, int *x, int *y);

/**
 * @brief   为精灵设置要显示的图像。
 * @param   sprite 精灵句柄。
 * @param   image  要绑定的图像句柄。
 */
void sprite_setImage(SpriteHandle sprite, ImageHandle image);

/**
 * @brief   获取精灵当前绑定的图像。
 * @param   sprite 精灵句柄。
 * @return  绑定的图像句柄。
 */
ImageHandle sprite_getImage(SpriteHandle sprite);

/**
 * @brief   为精灵设置动画参数。
 * @details 精灵将在`begin`和`end`帧之间循环播放动画。
 * @param   sprite 精灵句柄。
 * @param   begin  动画的起始帧索引。
 * @param   end    动画的结束帧索引。
 * @param   div    动画速度分频值。值越大，动画播放越慢。例如，div=4表示每4个游戏帧才切换一帧动画。
 */
void sprite_setAnimation(SpriteHandle sprite, int begin, int end, int div);

/**
 * @brief   设置精灵的Z-Index（绘制深度）。
 * @details Z-Index越大的精灵会越晚被绘制，从而显示在Z-Index较小的精灵之上。
 * @param   sprite 精灵句柄。
 * @param   zIndex Z-Index值。
 */
void sprite_setZIndex(SpriteHandle sprite, int8_t zIndex);

/**
 * @brief   设置精灵图像的翻转状态。
 * @param   sprite 精灵句柄。
 * @param   flip   翻转方式。
 */
void sprite_setImageFlip(SpriteHandle sprite, ImageFlip flip);

/**
 * @brief   获取精灵图像的当前翻转状态。
 * @param   sprite 精灵句柄。
 * @return  当前的翻转方式。
 */
ImageFlip sprite_getImageFlip(SpriteHandle sprite);

/**
 * @brief   为精灵设置一个自定义的更新函数。
 * @details 这个函数会在每一游戏帧被引擎自动调用，用于实现精灵的自定义逻辑（如AI、玩家控制等）。
 * @param   sprite 精灵句柄。
 * @param   func   一个函数指针，指向更新函数。
 * @see     SpriteUpdateFunction
 */
void sprite_setUpdateFunction(SpriteHandle sprite, SpriteUpdateFunction func);

/**
 * @brief   获取精灵的自定义更新函数。
 * @param   sprite 精灵句柄。
 * @return  指向更新函数的函数指针。
 */
SpriteUpdateFunction sprite_getUpdateFunction(SpriteHandle sprite);

/**
 * @brief   设置精灵的碰撞矩形。
 * @details 这个矩形是相对于精灵左上角(0,0)的偏移。
 * @param   sprite      精灵句柄。
 * @param   collideRect 碰撞矩形。
 */
void sprite_setCollideRect(SpriteHandle sprite, Rect collideRect);

/**
 * @brief   获取精灵的碰撞矩形。
 * @param   sprite 精灵句柄。
 * @return  碰撞矩形。
 */
Rect sprite_getCollideRect(SpriteHandle sprite);

/**
 * @brief   获取精灵的碰撞掩码。
 * @details 碰撞掩码用于过滤碰撞检测。只有当两个精灵的碰撞掩码进行位与运算结果不为0时，才会进行碰撞检测。
 * @param   sprite 精灵句柄。
 * @return  碰撞掩码。
 */
uint8_t sprite_getCollideMask(SpriteHandle sprite);

/**
 * @brief   设置精灵的碰撞掩码。
 * @param   sprite 精灵句柄。
 * @param   mask   要设置的碰撞掩码。
 */
void sprite_setCollideMask(SpriteHandle sprite, uint8_t mask);

/**
 * @brief   为精灵设置一个碰撞响应函数。
 * @details 当此精灵与其他精灵发生碰撞时，该函数会被引擎调用。
 * @param   sprite 精灵句柄。
 * @param   func   一个函数指针，指向碰撞响应函数。
 * @see     SpriteCollisionFunction
 */
void sprite_setCollisionResponseFunction(SpriteHandle sprite, SpriteCollisionFunction func);

/**
 * @brief   移动精灵，并处理与其它精灵的碰撞。
 * @details 这是一个高级移动函数。它会尝试将精灵移动(dx, dy)的距离，但如果途中遇到
 *          会与之发生碰撞的其它精灵，移动将会被阻挡。
 * @param   s  要移动的精灵句柄。
 * @param   dx X方向的移动距离。
 * @param   dy Y方向的移动距离。
 */
void sprite_moveWithCollisions(SpriteHandle s, int dx, int dy);

#endif // RETRO_ENGINE_H
