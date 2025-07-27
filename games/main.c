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
uint8_t mapData[] = {195, 196, 197, 195, 196, 197, 195, 196, 197, 195, 196, 197, 195, 196, 197, 198, 219, 220, 221, 219,
                     220, 221, 219, 220, 221, 219, 220, 221, 219, 220, 221, 198, 243, 244, 245, 243, 244, 245, 243, 244,
                     245, 243, 244, 245, 243, 244, 245, 198, 195, 196, 197, 195, 196, 197, 195, 196, 197, 195, 196, 197,
                     195, 196, 197, 198, 219, 220, 221, 219, 220, 221, 219, 220, 221, 219, 220, 221, 219, 220, 221, 198,
                     243, 244, 245, 243, 244, 245, 243, 244, 245, 243, 244, 245, 243, 244, 245, 198, 222, 222, 222, 222,
                     222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198, 222, 222, 222, 222, 222, 222, 222, 222,
                     222, 222, 222, 222, 222, 222, 222, 198, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222,
                     222, 222, 222, 198, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198,
                     222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 198, 242, 242, 242, 242,
                     242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 246, 58,  58,  58,  58,  58,  58,  58,  58,
                     58,  58,  58,  58,  58,  58,  58,  58,  50,  50,  50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
                     50,  50,  50,  50,  6,   6,   6,   5,   6,   6,   6,   6,   29,  6,   6,   6,   6,   29,  6,   6,
                     6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   5,   6,   6,   6,   6,   6};

void sprite_collidefunc(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    if (info.normal.x != 0)
        *info.dx = 0;
    if (info.normal.y != 0)
        *info.dy = 0;
}

void game_init(void) {
    g_test_image   = graphics_loadImageTable(408, 16, 16, pixelFormatPaletteRLE, tiles_data);
    g_sprite_image = graphics_loadImageTable(48, 12, 17, pixelFormatPalette, my_sprite_data);
    g_tilemap      = tilemap_newTilemap();
    g_sprite       = sprite_newSprite();
    tilemap_setImageTable(g_tilemap, g_test_image);
    tilemap_setTiles(g_tilemap, (uint8_t *)mapData, 16, 16);
    tilemap_addTilemap(g_tilemap);

    sprite_setImageTable(g_sprite, g_sprite_image);
    sprite_setCollideRect(g_sprite, (Rect){0, 0, 40, 32});
    sprite_setCollisionResponseFunction(g_sprite, sprite_collidefunc);
    sprite_addSprite(g_sprite);
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
    graphics_clear(0x2233);
    tilemap_drawAtPoint(g_tilemap, 0, 0, 0);
    sprite_updateAndDrawSprites();
    graphics_display();
}
