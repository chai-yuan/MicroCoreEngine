#ifndef PLATFORM_PORTING_H
#define PLATFORM_PORTING_H

#include "GameEngineDef.h"

// 平台层管理的图形句柄
typedef void *platform_image_t;

typedef struct {
    // --- 内存管理 ---
    void *(*mem_alloc)(size_t size);
    void (*mem_free)(void *ptr);

    // --- 时间 & 输入 ---
    unsigned int (*get_ticks_ms)(void);     // 获取毫秒级时间戳
    unsigned int (*get_button_state)(void); // 获取按钮状态位掩码

    // --- 图形资源管理 ---
    platform_image_t (*gfx_create_image)(int width, int height, const void *data); // 从二进制数据中创建纹理
    void (*gfx_update_image)(platform_image_t image, Rect update_rect, const void *data); // 更新指定范围的纹理
    platform_image_t (*gfx_create_render_target)(int width, int height);                  // 创建一块纹理
    void (*gfx_destroy_image)(platform_image_t image);                                    // 移除一块纹理

    // --- 渲染状态控制 ---
    void (*gfx_set_render_target)(platform_image_t image); // 设置渲染目标，传入NULL则渲染到主屏幕
    void (*gfx_set_clip_rect)(Rect rect);                  // 限制渲染区域
    // void (*gfx_set_draw_mode)(ImageDrawMode mode);         // 设置渲染模式

    // --- 绘图操作 ---
    void (*gfx_clear)(Color color); // 清除屏幕，设置背景颜色
    void (*gfx_present)(void);      // 将后台缓冲区的内容显示到屏幕上
    // 将src_rect矩形区域内的图像显示到x,y 如果src_rect为NULL则显示整张图像
    void (*gfx_draw_image)(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip);
    void (*gfx_draw_line)(Point p1, Point p2, int width, Color color); // 绘制线条
    void (*gfx_draw_rect)(Rect rect, Color color);                     // 绘制填充矩形

    // --- 显示信息 ---
    int (*display_get_width)(void);  // 获得屏幕宽度
    int (*display_get_height)(void); // 获得屏幕高度
} platform_api_t;

void platform_register_api(const platform_api_t api);
void game_init(void);
void game_loop(void);

#endif // PLATFORM_PORTING_H
