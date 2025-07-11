#include "GameEngine.h"
#include <SDL.h>

// 放大倍率
#define SCALE_FACTOR 3
// 最终窗口的实际大小
#define SCREEN_WIDTH (LOGICAL_WIDTH * SCALE_FACTOR)
#define SCREEN_HEIGHT (LOGICAL_HEIGHT * SCALE_FACTOR)

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;

#define INT2RGB(r5g6b5) ((r5g6b5 >> 11) & 0x1f) << 3, ((r5g6b5 >> 5) & 0x3f) << 2, (r5g6b5 & 0x1f) << 3, 255

int sdl_init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "game");
    // 设置放大
    SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    return 0;
}

void sdl_cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_putpixel(int x, int y, unsigned int r5g6b5) {
    SDL_SetRenderDrawColor(renderer, INT2RGB(r5g6b5));
    SDL_RenderDrawPoint(renderer, x, y);
}

void sdl_drawscreen() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            sdl_cleanup();
            exit(0);
        }
    }
    SDL_RenderPresent(renderer);
}

int sdl_gettimer() { return SDL_GetTicks(); }

int sdl_getkey() {
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    int          key              = 0;

    if (currentKeyStates[SDL_SCANCODE_W]) {
        key |= KEY_UP;
    }
    if (currentKeyStates[SDL_SCANCODE_S]) {
        key |= KEY_DOWN;
    }
    if (currentKeyStates[SDL_SCANCODE_D]) {
        key |= KEY_RIGHT;
    }
    if (currentKeyStates[SDL_SCANCODE_A]) {
        key |= KEY_LEFT;
    }

    return key;
}
