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
uint8_t          mapData[]  = {
        195, 196, 197, 195, 196, 197, 195, 196, 197, 195, 196, 197, 195, 196, 197, 198,
        219, 220, 221, 219, 220, 221, 219, 220, 221, 219, 220, 221, 219, 220, 221, 198,
        243, 244, 245, 243, 244, 245, 243, 244, 245, 243, 244, 245, 243, 244, 245, 198,
        195, 196, 197, 195, 196, 197, 195, 196, 197, 195, 196, 197, 195, 196, 197, 198,
        219, 220, 221, 219, 220, 221, 219, 220, 221, 219, 220, 221, 219, 220, 221, 198,
        243, 244, 245, 243, 244, 245, 243, 244, 245, 243, 244, 245, 243, 244, 245, 198,
        222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198,
        222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198,
        222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198,
        222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198,
        222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198,
        242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 246,
        58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
        50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
        6, 6, 6, 5, 6, 6, 6, 6, 29, 6, 6, 6, 6, 29, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6
      };

void sprite_collidefunc(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    if (info.normal.x != 0)
        *info.dx = 0;
    if (info.normal.y != 0)
        *info.dy = 0;
}

void game_init(void) {
    g_test_image   = graphics_loadImageTable(408, 16, 16, pixelFormatPalette, tiles_data);
    g_sprite_image = graphics_loadImageTable(6, 40, 32, pixelFormatPalette, my_sprite_data);
    g_tilemap      = tilemap_newTilemap();
    g_sprite       = sprite_newSprite();
    tilemap_setImageTable(g_tilemap, g_test_image);
    tilemap_setTiles(g_tilemap, (uint8_t *)mapData, 16, 16);
    tilemap_addTilemap(g_tilemap);

    sprite_setImage(g_sprite, graphics_getTableImage(g_sprite_image, 0), imageUnflipped);
    sprite_setCollideRect(g_sprite, (Rect){0, 0, 40, 32});
    sprite_setCollisionResponseFunction(g_sprite, sprite_collidefunc);
    sprite_addSprite(g_sprite);
    sprite_moveTo(g_sprite, 0, 0);
    INFO("Game initialized!\n");
}

void game_loop(void) {
    // --- 输入处理 ---
    unsigned int buttons = system_getButtonState();
    if (buttons & buttonLeft) {
        sprite_moveWithCollisions(g_sprite, -4, 0);
        sprite_setImageFlip(g_sprite, imageFlippedX);
    }
    if (buttons & buttonRight) {
        sprite_moveWithCollisions(g_sprite, 4, 0);
        sprite_setImageFlip(g_sprite, imageUnflipped);
    }
    if (buttons & buttonUp)
        sprite_moveWithCollisions(g_sprite, 0, -4);
    if (buttons & buttonDown)
        sprite_moveWithCollisions(g_sprite, 0, 4);

    // --- 渲染 ---
    graphics_clear(0);
    tilemap_drawAtPoint(g_tilemap, 0, 0, 0);
    sprite_updateAndDrawSprites();
    graphics_display();
}
