#include "GameEnginePriv.h"

SpriteHandle g_sprites[SPRITE_NUM] = {NULL};

SpriteHandle sprite_newSprite(void) {
    SpriteHandle sprite   = platform.mem_alloc(sizeof(Sprite));
    sprite->image         = NULL;
    sprite->updatefunc    = NULL;
    sprite->collisionfunc = NULL;
    sprite->idx           = 0;
    sprite->div           = 0;
    return sprite;
}

void sprite_freeSprite(SpriteHandle sprite);

void sprite_addSprite(SpriteHandle sprite) {
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i] == NULL) {
            g_sprites[i] = sprite;
            return;
        }
}

void sprite_removeSprite(SpriteHandle sprite) {
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i] == sprite) {
            g_sprites[i] = NULL;
            return;
        }
}

void sprite_setAnimation(SpriteHandle sprite, int begin, int end, int div) {
    sprite->begin = begin;
    sprite->end   = end;
    sprite->div   = div;
}

void sprite_updateAndDrawSprites(void) {
    // 更新行为
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i] && g_sprites[i]->updatefunc)
            g_sprites[i]->updatefunc();
    // 绘制图像
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i]) {
            SpriteHandle s = g_sprites[i];
            if (s->image != NULL) {
                graphics_drawImageTable(s->image, s->x, s->y, s->idx, s->flip);
            }

            if ((s->div != 0) && (g_ticks % s->div == 0)) {
                s->idx++;
                if (s->idx > s->end || s->idx <= s->begin)
                    s->idx = s->begin;
            }
        }
}

void sprite_setImageFlip(SpriteHandle sprite, ImageFlip flip) { sprite->flip = flip; }

ImageFlip sprite_getImageFlip(SpriteHandle sprite) { return sprite->flip; }

void sprite_moveTo(SpriteHandle sprite, int x, int y) {
    sprite->x = x;
    sprite->y = y;
}

void sprite_getPosition(SpriteHandle sprite, int *x, int *y) {
    *x = sprite->x;
    *y = sprite->y;
}

void sprite_setImage(SpriteHandle sprite, ImageHandle image, ImageFlip flip) {
    sprite->image = graphics_tableFromImage(image);
    sprite->flip  = flip;
}

void sprite_setImageTable(SpriteHandle sprite, ImageTableHandle table) { sprite->image = table; }

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
