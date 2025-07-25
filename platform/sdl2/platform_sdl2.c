#include "platform_sdl2.h"
#include "PlatformPorting.h"
#include <SDL2/SDL.h>
#include <stdlib.h> // For malloc/free

// --- 全局变量 ---
static SDL_Window   *g_window                = NULL;
static SDL_Renderer *g_renderer              = NULL;
static SDL_Texture  *g_current_render_target = NULL; // 用于跟踪当前渲染目标
static int           g_target_frame_time     = 0;    // 每帧的目标毫秒数

// --- 平台API的实现函数 ---

// 内存
static void *_sdl_mem_alloc(size_t size) { return malloc(size); }
static void  _sdl_mem_free(void *ptr) { free(ptr); }

// 时间 & 输入
static unsigned int _sdl_get_ticks_ms() { return SDL_GetTicks(); }

static unsigned int _sdl_get_button_state() {
    unsigned int state          = 0;
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    if (keyboard_state[SDL_SCANCODE_LEFT])
        state |= buttonLeft;
    if (keyboard_state[SDL_SCANCODE_RIGHT])
        state |= buttonRight;
    if (keyboard_state[SDL_SCANCODE_UP])
        state |= buttonUp;
    if (keyboard_state[SDL_SCANCODE_DOWN])
        state |= buttonDown;
    if (keyboard_state[SDL_SCANCODE_Z])
        state |= buttonA;
    if (keyboard_state[SDL_SCANCODE_X])
        state |= buttonB;
    return state;
}

// 图形资源
static platform_image_t _sdl_gfx_create_image(int width, int height, const void *data) {
    SDL_Texture *texture =
        SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return NULL;
    }
    if (data) {
        SDL_UpdateTexture(texture, NULL, data, width * 2); // 2 bytes per pixel (RGBA)
    }
    // 设置默认混合模式为透明
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return (platform_image_t)texture;
}

static platform_image_t _sdl_gfx_create_render_target(int width, int height) {
    SDL_Texture *texture =
        SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_TARGET, width, height);
    if (!texture) {
        SDL_Log("Failed to create render target texture: %s", SDL_GetError());
        return NULL;
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return (platform_image_t)texture;
}

static void _sdl_gfx_destroy_image(platform_image_t image) { SDL_DestroyTexture((SDL_Texture *)image); }

// 渲染状态
static void _sdl_gfx_set_render_target(platform_image_t image) {
    g_current_render_target = (SDL_Texture *)image;
    SDL_SetRenderTarget(g_renderer, g_current_render_target);
}

static void _sdl_gfx_set_clip_rect(Rect rect) {
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderSetClipRect(g_renderer, &sdl_rect);
}

// 绘图操作
static void _sdl_gfx_clear(Color color) {
    SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(g_renderer);
}

static void _sdl_gfx_present(void) { SDL_RenderPresent(g_renderer); }

static void _sdl_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip) {
    SDL_Rect  s_rect, d_rect;
    SDL_Rect *s_rect_ptr = NULL;
    if (src_rect) {
        s_rect.x   = src_rect->x;
        s_rect.y   = src_rect->y;
        s_rect.w   = src_rect->w;
        s_rect.h   = src_rect->h;
        s_rect_ptr = &s_rect;
        d_rect.w   = src_rect->w;
        d_rect.h   = src_rect->h;
    } else {
        // 如果源矩形为NULL，则绘制整个纹理
        SDL_QueryTexture((SDL_Texture *)image, NULL, NULL, &d_rect.w, &d_rect.h);
    }
    d_rect.x                  = x;
    d_rect.y                  = y;
    SDL_RendererFlip sdl_flip = SDL_FLIP_NONE;
    if (flip == imageFlippedX)
        sdl_flip = SDL_FLIP_HORIZONTAL;
    if (flip == imageFlippedY)
        sdl_flip = SDL_FLIP_VERTICAL;
    if (flip == imageFlippedXY)
        sdl_flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);

    SDL_RenderCopyEx(g_renderer, (SDL_Texture *)image, s_rect_ptr, &d_rect, 0.0, NULL, sdl_flip);
}

static void _sdl_gfx_draw_line(Point p1, Point p2, int width, Color color) {
    // 注意: SDL_RenderDrawLine 不支持线条宽度。为了实现宽线，需要更复杂的几何绘制。
    // 这里我们为了简洁而忽略 width 参数。
    SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(g_renderer, p1.x, p1.y, p2.x, p2.y);
}

static void _sdl_gfx_draw_rect(Rect rect, Color color) {
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(g_renderer, &sdl_rect);
}

// 显示信息
static int _sdl_display_get_width() {
    int w;
    if (g_current_render_target) {
        SDL_QueryTexture(g_current_render_target, NULL, NULL, &w, NULL);
    } else {
        SDL_GetRendererOutputSize(g_renderer, &w, NULL);
    }
    return w;
}

static int _sdl_display_get_height() {
    int h;
    if (g_current_render_target) {
        SDL_QueryTexture(g_current_render_target, NULL, NULL, NULL, &h);
    } else {
        SDL_GetRendererOutputSize(g_renderer, NULL, &h);
    }
    return h;
}

// --- 平台生命周期函数 ---

platform_api_t platform_sdl2_init(const char *title, int width, int height, int target_fps) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    g_window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (g_window == NULL) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    g_renderer = SDL_CreateRenderer(g_window, -1, 0);
    if (g_renderer == NULL) {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }
    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    if (target_fps > 0) {
        g_target_frame_time = 1000 / target_fps;
    }

    // 填充API结构体
    platform_api_t sdl_api = {.mem_alloc                = _sdl_mem_alloc,
                              .mem_free                 = _sdl_mem_free,
                              .get_ticks_ms             = _sdl_get_ticks_ms,
                              .get_button_state         = _sdl_get_button_state,
                              .gfx_create_image         = _sdl_gfx_create_image,
                              .gfx_create_render_target = _sdl_gfx_create_render_target,
                              .gfx_destroy_image        = _sdl_gfx_destroy_image,
                              .gfx_set_render_target    = _sdl_gfx_set_render_target,
                              .gfx_set_clip_rect        = _sdl_gfx_set_clip_rect,
                              .gfx_clear                = _sdl_gfx_clear,
                              .gfx_present              = _sdl_gfx_present,
                              .gfx_draw_image           = _sdl_gfx_draw_image,
                              .gfx_draw_line            = _sdl_gfx_draw_line,
                              .gfx_draw_rect            = _sdl_gfx_draw_rect,
                              .display_get_width        = _sdl_display_get_width,
                              .display_get_height       = _sdl_display_get_height};

    return sdl_api;
}

void platform_sdl2_run_main_loop(game_loop_func_t game_loop) {
    SDL_Event e;
    int       quit = 0;

    while (!quit) {
        Uint32 frame_start_time = SDL_GetTicks();

        // 处理事件
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // 调用游戏的主循环
        game_loop();

        // 帧率控制
        if (g_target_frame_time > 0) {
            Uint32 frame_time = SDL_GetTicks() - frame_start_time;
            if (frame_time < g_target_frame_time) {
                SDL_Delay(g_target_frame_time - frame_time);
            }
        }
    }
}

void platform_sdl2_shutdown(void) {
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    g_renderer = NULL;
    g_window   = NULL;

    SDL_Quit();
}
