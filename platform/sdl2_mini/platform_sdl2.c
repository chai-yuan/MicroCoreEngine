#include "platform_sdl2.h"
#include "Debug.h"
#include <SDL2/SDL.h>
#include <stdlib.h> // For malloc/free

// 放大倍率
#define SCALE_FACTOR 2
// 最终窗口的实际大小
#define SCREEN_WIDTH (DISPLAY_WIDTH * SCALE_FACTOR)
#define SCREEN_HEIGHT (DISPLAY_HEIGHT * SCALE_FACTOR)

SDL_Window   *window              = NULL;
SDL_Renderer *renderer            = NULL;
int           g_target_frame_time = 0;
uint8_t       vmem[VMEM_SIZE];

int sdl_init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "game");
    // 设置放大
    SDL_RenderSetLogicalSize(renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    return 0;
}

void sdl_cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

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

static void _sdl_present() {
    Uint16(*vmem_display)[DISPLAY_WIDTH] = (Uint16(*)[DISPLAY_WIDTH])vmem;

    for (int row = 0; row < DISPLAY_HEIGHT; row++) {
        for (int col = 0; col < DISPLAY_WIDTH; col++) {
            Uint16 color16 = vmem_display[row][col];

            if (color16 == 0) {
                continue;
            }

            Uint8 r = ((color16 & 0xF800) >> 11) << 3;
            Uint8 g = ((color16 & 0x07E0) >> 5) << 2;
            Uint8 b = ((color16 & 0x001F)) << 3;

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawPoint(renderer, col, row);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

minimal_api_t platform_sdl2_init(int target_fps) {
    sdl_init();
    g_target_frame_time = 1000 / target_fps;

    minimal_api_t sdl_api = {
        .vmem             = vmem,
        .get_button_state = _sdl_get_button_state,
        .get_ticks_ms     = _sdl_get_ticks_ms,
        .present          = _sdl_present,
    };

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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
