#ifndef PLATFORM_PORTING_H
#define PLATFORM_PORTING_H

/**
 * @brief 平台功能接口结构体
 *
 * 包含一系列函数指针，指向平台相关的具体实现。
 *   游戏逻辑在使用前应检查其是否为 NULL。
 */
typedef struct {
    int (*getTime)();
    void (*drawScreen)();
    unsigned char (*getKey)();
    void (*setFramerate)(int fps);
    void (*loadPalette)(int *palette);
    void (*loadImage)(const unsigned char *image, int addr, int len);
    void (*drawImage)(int addr, int x, int y, int w, int h);
} platform_api_t;

/**
 * @brief 指向已注册平台 API 实现的全局指针。
 * 游戏逻辑应通过此指针调用平台功能。
 */
extern platform_api_t g_platform_api;

/**
 * @brief 平台实现者必须调用的注册函数。
 * 在调用 game_main() 之前，平台代码必须创建并填充一个 `platform_api_t` 结构体，
 * 然后调用此函数将其注册到引擎核心。
 * @param api 指向平台 API 结构体实例的指针。
 */
void platform_register_api(const platform_api_t api);

#endif // PLATFORM_PORTING_H
