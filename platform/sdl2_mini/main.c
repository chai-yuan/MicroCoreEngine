#include "platform_sdl2.h"
#include <stdio.h>

int main() {
    minimal_api_t sdl_api = platform_sdl2_init(30);
    minimal_register_api(sdl_api);
    game_init();
    platform_sdl2_run_main_loop(game_loop);
    platform_sdl2_shutdown();
    return 0;
}
