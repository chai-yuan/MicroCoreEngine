#include "platform_sdl2.h"
#include "PlatformPorting.h"
#include <SDL2/SDL.h>
#include <stdlib.h> // For malloc/free

// --- 全局变量 ---
static SDL_Window   *g_window                = NULL;
static SDL_Surface *g_screen_surface         = NULL; // NEW: The primary surface tied to the window.
static SDL_Surface *g_current_render_target  = NULL; // CHANGED: Now tracks the destination SDL_Surface.
static int           g_target_frame_time     = 0;    // 每帧的目标毫秒数
static uint16_t      g_current_palette[16]   = {0};

// --- 辅助函数 ---
static SDL_Color _get_sdl_color_from_rgb565(uint16_t color565) {
    SDL_Color color;
    color.r = (color565 & 0xF800) >> 8; // RRRRR... -> RRRRR000
    color.g = (color565 & 0x07E0) >> 3; // ...GGGGGG... -> GGGGGG00
    color.b = (color565 & 0x001F) << 3; // ...BBBBB -> BBBBB000
    color.a = 255;
    return color;
}

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

static platform_image_t _sdl_gfx_create_image(int width, int height, PixelFormat format, const void *data) {
    if (format != pixelFormatPalette) {
        SDL_Log("Warning: This implementation is optimized for pixelFormatPalette. Other formats are not supported.");
        return NULL;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 8, SDL_PIXELFORMAT_INDEX8);
    if (!surface) {
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        return NULL;
    }

    if (data) {
        SDL_LockSurface(surface);
        uint8_t *src_pixels = (uint8_t*)data;
        uint8_t *dst_pixels = (uint8_t*)surface->pixels;
        int num_pixels = width * height;
        for (int i = 0; i < num_pixels / 2; ++i) {
            uint8_t packed_byte = src_pixels[i];
            dst_pixels[i * 2]     = packed_byte >> 4;   // High nibble
            dst_pixels[i * 2 + 1] = packed_byte & 0x0F; // Low nibble
        }
        if (num_pixels % 2 != 0) {
            dst_pixels[num_pixels - 1] = src_pixels[num_pixels / 2] >> 4;
        }
        SDL_UnlockSurface(surface);
    }

    return (platform_image_t)surface;
}

static void _sdl_gfx_update_image(platform_image_t image, Rect update_rect, const void *data) {
    if (!image || !data) return;

    SDL_Surface *surface = (SDL_Surface *)image;

    SDL_LockSurface(surface);

    uint8_t* src_pixels = (uint8_t*)data;
    uint8_t* dst_base = (uint8_t*)surface->pixels;

    // Unpack 4-bit data into the 8-bit surface
    for (int y = 0; y < update_rect.h; ++y) {
        uint8_t* dst_row = dst_base + (update_rect.y + y) * surface->pitch + update_rect.x;
        for (int x = 0; x < update_rect.w; ++x) {
            int src_idx = (y * update_rect.w + x);
            uint8_t packed_byte = src_pixels[src_idx / 2];
            if (src_idx % 2 == 0) {
                dst_row[x] = packed_byte >> 4; // High nibble
            } else {
                dst_row[x] = packed_byte & 0x0F; // Low nibble
            }
        }
    }

    SDL_UnlockSurface(surface);
}


static platform_image_t _sdl_gfx_create_render_target(int width, int height, PixelFormat format) {
    return _sdl_gfx_create_image(width, height, format, NULL);
}

static void _sdl_gfx_destroy_image(platform_image_t image) {
    if (image) {
        SDL_FreeSurface((SDL_Surface *)image);
    }
}

// --- 调色板控制 ---
static platform_palette_t _sdl_gfx_create_palette(const uint16_t *colors) {
    if (!colors) return NULL;

    SDL_Palette *palette = SDL_AllocPalette(16);
    if (!palette) {
        SDL_Log("Failed to allocate SDL_Palette: %s", SDL_GetError());
        return NULL;
    }

    SDL_Color sdl_colors[16];
    for (int i = 0; i < 16; ++i) {
        sdl_colors[i] = _get_sdl_color_from_rgb565(colors[i]);
    }

    SDL_SetPaletteColors(palette, sdl_colors, 0, 16);

    return (platform_palette_t)palette;
}

static void _sdl_gfx_destroy_palette(platform_palette_t palette) {
    if (palette) {
        SDL_FreePalette((SDL_Palette *)palette);
    }
}

static void _sdl_gfx_set_palette(platform_palette_t palette) {
    if (!palette || !g_current_render_target) return;

    SDL_SetSurfacePalette((SDL_Surface *)g_current_render_target, (SDL_Palette *)palette);
}

// --- 渲染状态控制 ---
static void _sdl_gfx_set_render_target(platform_image_t image) {
    if (image) {
        g_current_render_target = (SDL_Surface *)image;
    } else {
        g_current_render_target = g_screen_surface;
    }
}

static void _sdl_gfx_set_clip_rect(Rect rect) {
    if (!g_current_render_target) return;
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_SetClipRect(g_current_render_target, &sdl_rect);
}

// --- 绘图操作 ---
static void _sdl_gfx_clear(uint8_t color_idx) {
    if (!g_current_render_target) return;
    SDL_FillRect(g_current_render_target, NULL, color_idx);
}

static void _sdl_gfx_present(void) {
    if (g_window) {
        SDL_UpdateWindowSurface(g_window);
    }
}

static void _sdl_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip) {
    if (!image || !g_current_render_target) return;

    SDL_Surface *src_surface = (SDL_Surface *)image;

    if (flip != imageUnflipped) {
        SDL_Log("Warning: Image flipping is not supported in this software rendering backend.");
    }

    SDL_Rect sdl_src_rect;
    SDL_Rect *sdl_src_rect_ptr = NULL;
    if (src_rect) {
        sdl_src_rect.x = src_rect->x;
        sdl_src_rect.y = src_rect->y;
        sdl_src_rect.w = src_rect->w;
        sdl_src_rect.h = src_rect->h;
        sdl_src_rect_ptr = &sdl_src_rect;
    }

    SDL_Rect sdl_dst_rect = {x, y, 0, 0}; // w and h are ignored by SDL_BlitSurface
    SDL_BlitSurface(src_surface, sdl_src_rect_ptr, g_current_render_target, &sdl_dst_rect);
}

static void _sdl_gfx_draw_rect(Rect rect, uint8_t color_idx) {
    if (!g_current_render_target) return;

    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_FillRect(g_current_render_target, &sdl_rect, color_idx);
}

// 显示信息
static int _sdl_display_get_width() {
    return 0;
}

static int _sdl_display_get_height() {
    return 0;
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

    g_screen_surface = SDL_GetWindowSurface(g_window);
    if (!g_screen_surface) {
        SDL_Log("Screen_surface could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    g_current_render_target = g_screen_surface;

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
                              .gfx_create_palette       = _sdl_gfx_create_palette,
                              .gfx_destroy_palette      = _sdl_gfx_destroy_palette,
                              .gfx_set_palette          = _sdl_gfx_set_palette,
                              .gfx_set_render_target    = _sdl_gfx_set_render_target,
                              .gfx_set_clip_rect        = _sdl_gfx_set_clip_rect,
                              .gfx_clear                = _sdl_gfx_clear,
                              .gfx_present              = _sdl_gfx_present,
                              .gfx_draw_image           = _sdl_gfx_draw_image,
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
    SDL_DestroyWindow(g_window);
    g_window   = NULL;

    SDL_Quit();
}
