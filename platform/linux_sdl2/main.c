#include "platform_sdl2.h"
#include <stdio.h>

void game_init(void);
void game_loop(void);

int main(int argc, char *argv[]) {
    if (platform_sdl2_init("Game Engine On SDL2", 320, 240, 30) != 0) {
        fprintf(stderr, "Failed to initialize platform!\n");
        return -1;
    }

    game_init();

    platform_sdl2_run_main_loop(game_loop);

    platform_sdl2_shutdown();
    return 0;
}
