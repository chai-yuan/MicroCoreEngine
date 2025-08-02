#include "Debug.h"
#include "GameEngine.h"
#include "bullet.img.h"
#include "gun.img.h"
#include "sprite.img.h"
#include "tiles.img.h"
#include "zombieSmall.img.h"

ImageHandle   g_tilemap_image = NULL;
ImageHandle   g_gun_imgae = NULL, g_bullet_image = NULL;
ImageHandle   g_sprite_image = NULL, g_zombie_image = NULL;
TileMapHandle g_tilemap = NULL;
SpriteHandle  g_sprite = NULL, g_sprite_gun = NULL;

uint8_t mapData[] = {1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  10,
                     13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 16, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 10,
                     13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 10, 13, 15, 15, 15, 15, 15, 28, 29, 29, 30, 15, 10,
                     13, 15, 15, 15, 15, 15, 40, 40, 40, 40, 15, 10, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 10,
                     13, 15, 28, 29, 30, 15, 15, 15, 15, 15, 15, 10, 7,  15, 40, 40, 40, 15, 15, 15, 15, 15, 15, 10,
                     13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 10, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 22};

void buttetUpdate(SpriteHandle self) {
    int px, py;
    sprite_getPosition(self, &px, &py);
    sprite_setPosition(self, px + 5, py);
    if (px > 200)
        sprite_freeSprite(self);
}

void playerSpriteUpdate(SpriteHandle self) {
    unsigned int buttons = system_getButtonState();
    if (buttons & buttonUp) {
        sprite_moveWithCollisions(g_sprite, 0, -2);
        sprite_setAnimation(g_sprite, 1 * 6 + 24, 1 * 6 + 29, 5);
    }
    if (buttons & buttonDown) {
        sprite_moveWithCollisions(g_sprite, 0, 2);
        sprite_setAnimation(g_sprite, 1 * 6 + 24, 1 * 6 + 29, 5);
    }
    if ((buttons & 0xf) == 0)
        sprite_setAnimation(g_sprite, 1 * 6, 1 * 6 + 5, 5);

    int px, py;
    sprite_getPosition(self, &px, &py);
    sprite_setPosition(g_sprite_gun, px, py + 8);

    static int lastShot = 0;

    if (buttons & buttonA) {
        sprite_setAnimation(g_sprite_gun, 6, 8, 2);
        if (lastShot < 0) {
            SpriteHandle bullet = sprite_newSprite();
            sprite_setImage(bullet, g_bullet_image);
            sprite_setPosition(bullet, px + 10, py + 9);
            sprite_setUpdateFunction(bullet, buttetUpdate);
            sprite_setCollideMask(bullet, 0x1);
            sprite_setCollideRect(bullet, (Rect){0, 0, 1, 1});
            lastShot = 5;
        }
    } else {
        sprite_setAnimation(g_sprite_gun, 0, 5, 5);
    }
    lastShot--;
}

void zombieUpdate(SpriteHandle self) {
    int px, py;
    sprite_getPosition(self, &px, &py);
    sprite_moveWithCollisions(self, -1, 0);
    if (px < -20)
        sprite_freeSprite(self);
}

void zombieCollide(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    sprite_freeSprite(self);
    sprite_freeSprite(other);
}

void game_init(void) {
    g_tilemap_image = graphics_newImage(48, 16, 16, pixelFormatPaletteRLE, tiles_data);
    g_sprite_image  = graphics_newImage(48, 12, 17, pixelFormatPalette, my_sprite_data);
    g_gun_imgae     = graphics_newImage(24, 16, 10, pixelFormatPalette, gun_data);
    g_bullet_image  = graphics_newImage(1, 6, 3, pixelFormatPalette, bullet_data);
    g_zombie_image  = graphics_newImage(12, 14, 16, pixelFormatPalette, zombieSmall_data);
    g_tilemap       = tilemap_newTilemap(12, 12);
    g_sprite        = sprite_newSprite();
    g_sprite_gun    = sprite_newSprite();

    tilemap_setImage(g_tilemap, g_tilemap_image);
    tilemap_setTiles(g_tilemap, mapData);

    sprite_setImage(g_sprite, g_sprite_image);
    sprite_setPosition(g_sprite, 5, 50);
    sprite_setUpdateFunction(g_sprite, playerSpriteUpdate);

    sprite_setImage(g_sprite_gun, g_gun_imgae);

    INFO("Game initialized!\n");
}

void game_loop(void) {
    static int gameTick = 0;

    if ((gameTick % 50) == 0) {
        SpriteHandle zombie = sprite_newSprite();
        sprite_setImage(zombie, g_zombie_image);
        sprite_setPosition(zombie, 150, system_rand() % 110);
        sprite_setUpdateFunction(zombie, zombieUpdate);
        sprite_setCollisionResponseFunction(zombie, zombieCollide);
        sprite_setAnimation(zombie, 0, 5, 5);
        sprite_setCollideMask(zombie, 0x1);
        sprite_setCollideRect(zombie, (Rect){0, 0, 14, 16});
    }

    graphics_clear(0);
    gameTick++;
}
