#include "GameEnginePriv.h"

int tickCount = 0;

int checkSpriteTilemapCollision(Sprite *sprite) { return 0; }

void spriteMove(Sprite *sprite) {
    sprite->x += sprite->speedx;
    sprite->y += sprite->speedy;
}

void gameTick() {
    tickCount++;

    // 精灵运动
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprite[i])
            spriteMove(g_sprite[i]);

    // 绘制
    for (int i = 0; i < BACKGROUND_NUM; i++)
        if (g_background[i])
            drawBackground(g_background[i]);
    if (g_tileMap)
        drawTileMap(g_tileMap);
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprite[i])
            drawSprite(g_sprite[i]);

    // 显示
    if (g_platform_api.drawScreen)
        g_platform_api.drawScreen();
}

void setFramerate(int fps) {
    if (g_platform_api.setFramerate)
        g_platform_api.setFramerate(fps);
}
