#include "GameEnginePriv.h"
#include <SDL.h>

// 放大倍率
#define SCALE_FACTOR 3
// 最终窗口的实际大小
#define SCREEN_WIDTH (LOGICAL_WIDTH * SCALE_FACTOR)
#define SCREEN_HEIGHT (LOGICAL_HEIGHT * SCALE_FACTOR)

// 游戏数据
unsigned char images[IMAGES_SIZE];
int           palette[16] = {0x0,    0xef18, 0xb9c9, 0x7db6, 0x49ea, 0x6d2d, 0x2a0d, 0xddcb,
                             0xb46d, 0x42cb, 0xbb09, 0x31a6, 0x73ae, 0x8d4c, 0x3df9, 0xbdd7};

SDL_Window   *window     = NULL;
SDL_Renderer *renderer   = NULL;
Uint32        g_waittime = 0;

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

void sdl_loadPalette(int *p) {
    for (int i = 0; i < 16; i++)
        palette[i] = p[i];
}

void sdl_drawscreen() {
    static Uint32 last_time = 0;
    if (last_time == 0) {
        last_time = SDL_GetTicks();
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            sdl_cleanup();
            exit(0);
        }
    }

    SDL_RenderPresent(renderer);
    // 清空用于下一次绘制
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Uint32 current_time = SDL_GetTicks();
    Uint32 frame_time   = current_time - last_time;

    if (frame_time < g_waittime) {
        SDL_Delay(g_waittime - frame_time);
    }
    last_time = SDL_GetTicks();
}

int sdl_gettimer() { return SDL_GetTicks(); }

void sdl_setfps(int fps) { g_waittime = 1000 / fps; }

unsigned char sdl_getkey() {
    const Uint8  *currentKeyStates = SDL_GetKeyboardState(NULL);
    unsigned char key              = 0;

    if (currentKeyStates[SDL_SCANCODE_UP]) {
        key |= KEY_UP;
    }
    if (currentKeyStates[SDL_SCANCODE_DOWN]) {
        key |= KEY_DOWN;
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
        key |= KEY_RIGHT;
    }
    if (currentKeyStates[SDL_SCANCODE_LEFT]) {
        key |= KEY_LEFT;
    }

    return key;
}

void sdl_loadImage(const unsigned char *image, int address, int len) {
    if (address + len > IMAGES_SIZE){
        SDL_Log("Error: Image data out of bounds. Address: %d, Size: %d", address, len);
        return;
    }

    for (int i = 0; i < len; i++)
        images[address + i] = image[i];
}

void sdl_drawImage16bit(int address, int x, int y, int w, int h) {
    if (address + w * h * 2 > IMAGES_SIZE) {
        SDL_Log("Error: Image data out of bounds. Address: %d, Size: %d", address, w * h * 2);
        return;
    }

    Uint16 *pixel_data = (Uint16 *)(images + address);

    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            int index = row * w + col;

            Uint16 color16 = pixel_data[index];

            if (color16 == 0) {
                continue;
            }

            Uint8 r = ((color16 & 0xF800) >> 11) << 3;
            Uint8 g = ((color16 & 0x07E0) >> 5) << 2;
            Uint8 b = (color16 & 0x001F) << 3;

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawPoint(renderer, x + col, y + row);
        }
    }
}

void sdl_drawImage(Image *image, int x, int y) {
    if (image->type == IMG_RGB565)
        return;

    Uint32 size       = (image->w * image->h) / 2;
    Uint8 *pixel_data = (Uint8 *)(images + image->address + image->idx * size);

    for (int row = 0; row < image->h; row++) {
        for (int col = 0; col < image->w; col += 2) {
            int index = (row * image->w + col) / 2;

            Uint16 color16 = palette[pixel_data[index] & 0xf];
            if (color16 != 0) {
                Uint8 r = ((color16 & 0xF800) >> 11) << 3;
                Uint8 g = ((color16 & 0x07E0) >> 5) << 2;
                Uint8 b = (color16 & 0x001F) << 3;

                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x + col, y + row);
            }

            color16 = palette[pixel_data[index] >> 4];
            if (color16 != 0) {
                Uint8 r = ((color16 & 0xF800) >> 11) << 3;
                Uint8 g = ((color16 & 0x07E0) >> 5) << 2;
                Uint8 b = (color16 & 0x001F) << 3;

                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x + col + 1, y + row);
            }
        }
    }
}
