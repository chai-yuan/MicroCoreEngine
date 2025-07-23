#ifndef PLATFORM_SDL2_H
#define PLATFORM_SDL2_H

#include "MinimalPorting.h"

// 定义一个函数指针类型，用于指向游戏的主循环函数
typedef void (*game_loop_func_t)(void);

minimal_api_t platform_sdl2_init(int target_fps);

void platform_sdl2_run_main_loop(game_loop_func_t game_loop);

void platform_sdl2_shutdown(void);

#endif // PLATFORM_SDL2_H
