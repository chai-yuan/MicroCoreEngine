#include "GameEngine.h"
#include "bg_test.img.h"

void game_main() {
    loadImage(my_sprite_data_data, 0, sizeof(my_sprite_data_data));

    Background bg = {
        .x       = 0,
        .y       = 0,
        .address = 0,
        .type    = IMG_RGB565,
        .imageh  = my_sprite_data_height,
        .imagew  = my_sprite_data_width,
    };

    loadBackground(&bg);
    setFramerate(25);
    while (1) {
        gameTick();
    }
}
