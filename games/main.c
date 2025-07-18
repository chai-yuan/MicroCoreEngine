#include "Debug.h"
#include "GameEngine.h"

ImageHandle g_test_image = NULL;
int         g_x          = 0;
int         g_y          = 0;

void game_init(void) {
    g_test_image = graphics_newImage(32, 32, (Color){255, 0, 0, 255});
    INFO("Game initialized!\n");
}

void game_loop(void) {
    // --- 输入处理 ---
    unsigned int buttons = system_getButtonState();
    if (buttons & buttonLeft)
        g_x -= 4;
    if (buttons & buttonRight)
        g_x += 4;
    if (buttons & buttonUp)
        g_y -= 4;
    if (buttons & buttonDown)
        g_y += 4;

    // --- 渲染 ---
    graphics_clear((Color){50, 50, 50, 255});
    graphics_drawImage(g_test_image, g_x, g_y, imageUnflipped);
    graphics_display();
}
