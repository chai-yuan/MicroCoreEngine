#include "Debug.h"
#include "GameEngine.h"
#include "sprite.img.h"
#include "tiles.img.h"

ImageHandle   g_test_image   = NULL;
ImageHandle   g_sprite_image = NULL;
int           g_x            = 0;
int           g_y            = 0;
TileMapHandle g_tilemap      = NULL;
SpriteHandle  g_sprite       = NULL;
uint8_t mapData[] = {1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  10,
                     13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
                     13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
                     13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 13, 15, 15, 15, 15, 6,  15, 15, 15, 15, 15, 16,
                     13, 15, 15, 15, 15, 12, 23, 25, 29, 30, 15, 16, 7,  15, 15, 15, 15, 12, 17, 12, 24, 24, 15, 16,
                     13, 15, 15, 15, 15, 18, 23, 18, 15, 15, 15, 16, 19, 20, 20, 20, 20, 20, 17, 20, 20, 20, 20, 22};

void sprite_collidefunc(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    if (info.normal.x != 0)
        *info.dx = 0;
    if (info.normal.y != 0)
        *info.dy = 0;
}

void game_init(void) {
    g_test_image   = graphics_newImage(48, 16, 16, pixelFormatPaletteRLE, tiles_data);
    g_sprite_image = graphics_newImage(48, 12, 17, pixelFormatPalette, my_sprite_data);
    g_tilemap      = tilemap_newTilemap();
    g_sprite       = sprite_newSprite();
    tilemap_setImage(g_tilemap, g_test_image);
    tilemap_setTiles(g_tilemap, (uint8_t *)mapData, 12, 12);

    sprite_setImage(g_sprite, g_sprite_image);
    sprite_setCollideRect(g_sprite, (Rect){0, 0, 40, 32});
    sprite_setCollisionResponseFunction(g_sprite, sprite_collidefunc);
    sprite_moveTo(g_sprite, 0, 0);
    INFO("Game initialized!\n");
}

uint8_t dir = 0;

void game_loop(void) {
    // --- 输入处理 ---
    unsigned int buttons = system_getButtonState();
    if (buttons & buttonLeft) {
        sprite_moveWithCollisions(g_sprite, -2, 0);
        dir = 2;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons & buttonRight) {
        sprite_moveWithCollisions(g_sprite, 2, 0);
        dir = 1;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons & buttonUp) {
        sprite_moveWithCollisions(g_sprite, 0, -2);
        dir = 3;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons & buttonDown) {
        sprite_moveWithCollisions(g_sprite, 0, 2);
        dir = 0;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons == 0)
        sprite_setAnimation(g_sprite, dir * 6, dir * 6 + 5, 5);

    // --- 渲染 ---
    graphics_clear(0);
}
