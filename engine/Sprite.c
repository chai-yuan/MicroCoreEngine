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
    int imageSize = (sprite->imageh * sprite->imagew) / 2;
    int imageAddr = sprite->address + sprite->imageidx * imageSize;

    drawImage(imageAddr, sprite->x, sprite->y, sprite->imagew, sprite->imageh);
}
