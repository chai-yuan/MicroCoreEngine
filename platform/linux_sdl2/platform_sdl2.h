#ifndef PLATFORM_SDL2_H
#define PLATFORM_SDL2_H

// 定义一个函数指针类型，用于指向游戏的主循环函数
typedef void (*game_loop_func_t)(void);

/**
 * @brief 初始化SDL2，创建窗口和渲染器，并注册平台API。
 * @param title 窗口标题。
 * @param width 窗口宽度。
 * @param height 窗口高度。
 * @param target_fps 目标帧率。
 * @return 成功返回0，失败返回-1。
 */
int platform_sdl2_init(const char* title, int width, int height, int target_fps);

/**
 * @brief 运行主事件循环。
 *
 * 该函数会持续运行，处理系统事件（如关闭窗口），并以目标帧率重复调用
 * game_loop 回调函数。
 *
 * @param game_loop 一个指向游戏主循环逻辑的函数指针。
 */
void platform_sdl2_run_main_loop(game_loop_func_t game_loop);

/**
 * @brief 清理并关闭SDL2相关资源。
 */
void platform_sdl2_shutdown(void);

#endif // PLATFORM_SDL2_H
