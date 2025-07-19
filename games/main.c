#include "Debug.h"
#include "GameEngine.h"
#include "sprite.img.h"
#include "tiles.img.h"

ImageTableHandle g_test_image   = NULL;
ImageTableHandle g_sprite_image = NULL;
int              g_x            = 0;
int              g_y            = 0;
TileMapHandle    g_tilemap      = NULL;
SpriteHandle     g_sprite       = NULL;

char mapData[10][10] = {{0, 0, 0, 6, 6, 6, 0, 0, 0, 0}, {0, 0, 0, 6, 6, 6, 0, 0, 0, 0}, {0, 0, 0, 6, 6, 6, 0, 0, 0, 0},
                        {0, 0, 0, 6, 6, 6, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 3, 0, 0, 0}, {2, 2, 2, 2, 2, 2, 2, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void game_init(void) {
    g_test_image   = graphics_loadImageTable(12, 16, 16, tiles_data);
    g_sprite_image = graphics_loadImageTable(6, 40, 32, my_sprite_data);
    g_tilemap      = tilemap_newTilemap();
    g_sprite       = sprite_newSprite();
    tilemap_setImageTable(g_tilemap, g_test_image);
    tilemap_setTiles(g_tilemap, (uint8_t *)mapData, 10, 10);

    sprite_setImage(g_sprite, graphics_getTableImage(g_sprite_image, 0), imageUnflipped);
    sprite_addSprite(g_sprite);
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
    tilemap_drawAtPoint(g_tilemap, 32, 0, 0);
    sprite_moveTo(g_sprite, g_x, g_y);
    sprite_updateAndDrawSprites();
    graphics_display();
}
