#include "GameEnginePriv.h"

struct Sprite *g_sprite[SPRITE_NUM] = {0};

void loadSprite(Sprite *sprite) {
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprite[i] == 0) {
            g_sprite[i] = sprite;
            return;
        }
}

void removeSprite(Sprite *sprite) {
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprite[i] == sprite) {
            g_sprite[i] = 0;
            return;
        }
}

void drawSprite(Sprite *sprite) {
    if ((sprite->framerate != 0) && ((g_tickCount % sprite->framerate) == 0)) {
        sprite->image->idx++;
        sprite->image->idx %= sprite->image->count;
    }

    drawImage(sprite->image, sprite->x, sprite->y);
}
