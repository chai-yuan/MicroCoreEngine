#include "GameEnginePriv.h"

#define SPRITE_NUM 8
SpriteHandle g_sprites[SPRITE_NUM] = {NULL};

SpriteHandle sprite_newSprite(void) {
    SpriteHandle sprite = platform.mem_alloc(sizeof(Sprite));
    sprite->image       = NULL;
    sprite->updatefunc  = NULL;
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
                graphics_drawImage(s->image, s->x, s->y, s->flip);
            } else {
            }
        }
}

void sprite_moveTo(SpriteHandle sprite, int x, int y) {
    sprite->x = x;
    sprite->y = y;
}

void sprite_getPosition(SpriteHandle sprite, int *x, int *y) {
    *x = sprite->x;
    *y = sprite->y;
}

void sprite_setImage(SpriteHandle sprite, ImageHandle image, ImageFlip flip) {
    sprite->image = image;
    sprite->flip  = flip;
}

void sprite_setZIndex(SpriteHandle sprite, int8_t zIndex);

void sprite_setVisible(SpriteHandle sprite, int flag);

void sprite_setUpdateFunction(SpriteHandle sprite, SpriteUpdateFunction func) { sprite->updatefunc = func; }
