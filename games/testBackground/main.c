#include "GameEngine.h"
#include "bg_test.h"

void game_main() {
    loadImage(bg_test_data, 0, sizeof(bg_test_data));

    Background bg = {
        .x       = 0,
        .y       = 0,
        .address = 0,
        .imageh  = bg_test_height,
        .imagew  = bg_test_width,
    };

    loadBackground(&bg);
    setFramerate(25);
    while (1) {
        gameTick();
    }
}
