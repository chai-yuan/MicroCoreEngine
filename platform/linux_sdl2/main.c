#include "GameEngine.h"
#include "PlatformPorting.h"
#include <stdio.h>

int           sdl_init();
void          sdl_putpixel(int x, int y, unsigned int r5g6b5);
int           sdl_gettimer();
void          sdl_drawscreen();
unsigned char sdl_getkey();
void          sdl_setfps(int fps);
void          sdl_cleanup();
void          sdl_loadImage(const unsigned char *image, int address, int len);
void          sdl_drawImagePalette(int address, int x, int y, int w, int h);
void          sdl_drawImage16bit(int address, int x, int y, int w, int h);
void          sdl_loadPalette(int *p);

int main() {
    sdl_init();
    platform_api_t api = {
        .drawScreen       = sdl_drawscreen,
        .getTime          = sdl_gettimer,
        .getKey           = sdl_getkey,
        .setFramerate     = sdl_setfps,
        .loadImage        = sdl_loadImage,
        .loadPalette      = sdl_loadPalette,
        .drawImagePalette = sdl_drawImagePalette,
        .drawImageRGB565  = sdl_drawImage16bit,
    };
    platform_register_api(api);

    game_main();

    printf("game quit\n");
    sdl_cleanup();
    return 0;
}
