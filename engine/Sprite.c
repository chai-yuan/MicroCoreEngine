#include "GameEnginePriv.h"

SpriteHandle g_sprites[SPRITE_NUM] = {NULL};

SpriteHandle sprite_newSprite(void) {
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i] == NULL) {
            SpriteHandle sprite   = platform.mem_alloc(sizeof(Sprite));
            sprite->image         = NULL;
            sprite->updatefunc    = NULL;
            sprite->collisionfunc = NULL;
            sprite->idx           = 0;
            sprite->div           = 0;

            g_sprites[i] = sprite;
            return sprite;
        }
    return NULL;
}

void sprite_freeSprite(SpriteHandle sprite) {
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i] == sprite) {
            g_sprites[i] = NULL;
            // platform.mem_free(sprite);
            return;
        }
}

void sprite_setAnimation(SpriteHandle sprite, int begin, int end, int div) {
    sprite->begin = begin;
    sprite->end   = end;
    sprite->div   = div;
}

void sprite_setImageFlip(SpriteHandle sprite, ImageFlip flip) { sprite->flip = flip; }

ImageFlip sprite_getImageFlip(SpriteHandle sprite) { return sprite->flip; }

void sprite_setPosition(SpriteHandle sprite, int x, int y) {
    sprite->x = x;
    sprite->y = y;
}

void sprite_getPosition(SpriteHandle sprite, int *x, int *y) {
    *x = sprite->x;
    *y = sprite->y;
}

void sprite_setImage(SpriteHandle sprite, ImageHandle image) { sprite->image = image; }

void sprite_setZIndex(SpriteHandle sprite, int8_t zIndex);

void sprite_setVisible(SpriteHandle sprite, int flag);

void sprite_setUpdateFunction(SpriteHandle sprite, SpriteUpdateFunction func) { sprite->updatefunc = func; }

void sprite_setCollideRect(SpriteHandle sprite, Rect collideRect) { sprite->collisionRect = collideRect; }

Rect sprite_getCollideRect(SpriteHandle sprite) { return sprite->collisionRect; }

uint8_t sprite_getCollideMask(SpriteHandle sprite) { return sprite->collisionMask; }

void sprite_setCollideMask(SpriteHandle sprite, uint8_t mask) { sprite->collisionMask = mask; }

void sprite_setCollisionResponseFunction(SpriteHandle sprite, SpriteCollisionFunction func) {
    sprite->collisionfunc = func;
}
