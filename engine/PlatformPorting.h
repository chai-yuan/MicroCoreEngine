#ifndef PLATFORM_PORTING_H
#define PLATFORM_PORTING_H

#include "GameEngineDef.h"

// 平台层管理的图形句柄
typedef void *platform_image_t;

typedef struct {
    // --- 生命周期 & 系统 ---
    // --- 内存管理 ---
    void *(*mem_alloc)(size_t size);
    void (*mem_free)(void *ptr);
    // --- 时间 & 输入 ---
    unsigned int (*get_ticks_ms)(void);     // 获取毫秒级时间戳
    unsigned int (*get_button_state)(void); // 获取按钮状态位掩码
    // --- 图形资源管理 ---
    platform_image_t (*gfx_create_image)(int width, int height, const void *data);
    platform_image_t (*gfx_create_render_target)(int width, int height);
    void (*gfx_destroy_image)(platform_image_t image);
    // --- 渲染状态控制 ---
    void (*gfx_set_render_target)(platform_image_t image); // 设置渲染目标，传入NULL则渲染到主屏幕
    void (*gfx_set_clip_rect)(Rect rect);
    // void (*gfx_set_draw_mode)(ImageDrawMode mode);
    // --- 绘图操作 ---
    void (*gfx_clear)(Color color);
    void (*gfx_present)(void); // 将后台缓冲区的内容显示到屏幕上
    void (*gfx_draw_image)(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip);
    void (*gfx_draw_line)(Point p1, Point p2, int width, Color color);
    void (*gfx_draw_rect)(Rect rect, Color color); // 绘制填充矩形
    // --- 显示信息 ---
    int (*display_get_width)(void);
    int (*display_get_height)(void);
} platform_api_t;

extern platform_api_t platform;

void platform_register_api(const platform_api_t api);

#endif // PLATFORM_PORTING_H
