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

int main() {
    sdl_init();
    platform_api_t api = {
        .drawScreen = sdl_drawscreen,
        .drawPixel  = sdl_putpixel,
        .getTime    = sdl_gettimer,
        .getKey     = sdl_getkey,

        .setFramerate = sdl_setfps,
    };
    platform_register_api(api);

    game_main();

    printf("game quit\n");
    sdl_cleanup();
    return 0;
}
