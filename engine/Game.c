#include "GameEnginePriv.h"

int checkSpriteTilemapCollision(Sprite *sprite) {
    if (g_tileMap == 0)
        return 0;

    int sprite_left   = sprite->x + sprite->left;
    int sprite_right  = sprite->x + sprite->right;
    int sprite_top    = sprite->y + sprite->top;
    int sprite_bottom = sprite->y + sprite->bottom;

    int tile_left   = sprite_left / g_tileMap->imagew;
    int tile_right  = sprite_right / g_tileMap->imagew;
    int tile_top    = sprite_top / g_tileMap->imageh;
    int tile_bottom = sprite_bottom / g_tileMap->imageh;

    for (int ty = tile_top; ty <= tile_bottom; ++ty) {
        for (int tx = tile_left; tx <= tile_right; ++tx) {
            if (g_tileMap->collision[tx + ty * g_tileMap->mapw] & sprite->collision) {
                return 1;
            }
        }
    }

    return 0;
}

void spriteMove(Sprite *sprite) {
    sprite->speedx += sprite->accelerationx;
    sprite->x += sprite->speedx;

    if (checkSpriteTilemapCollision(sprite)) {
        // 如果发生了碰撞，将精灵移回碰撞前的位置
        // 然后根据速度方向，将其对齐到瓦片的边缘
        if (sprite->speedx > 0) { // 向右移动导致碰撞
            int tile_right_edge = (sprite->x + sprite->right) / g_tileMap->imagew * g_tileMap->imagew;
            sprite->x           = tile_right_edge - sprite->right - 1; // 对齐到碰撞砖块的左边
        } else if (sprite->speedx < 0) {                               // 向左移动导致碰撞
            int tile_left_edge = ((sprite->x + sprite->left) / g_tileMap->imagew + 1) * g_tileMap->imagew;
            sprite->x          = tile_left_edge; // 对齐到碰撞砖块的右边
        }
        sprite->speedx = 0; // 发生碰撞后，水平速度清零
    }

    sprite->speedy += sprite->accelerationy;
    sprite->y += sprite->speedy;
    if (checkSpriteTilemapCollision(sprite)) {
        if (sprite->speedy > 0) { // 向下移动导致碰撞 (落地)
            int tile_bottom_edge = (sprite->y + sprite->bottom) / g_tileMap->imageh * g_tileMap->imageh;
            sprite->y            = tile_bottom_edge - sprite->imageh - 1; // 对齐到碰撞砖块的上边
        } else if (sprite->speedy < 0) {                                  // 向上移动导致碰撞 (撞到天花板)
            int tile_top_edge = ((sprite->y + sprite->top) / g_tileMap->imageh + 1) * g_tileMap->imageh;
            sprite->y         = tile_top_edge; // 对齐到碰撞砖块的下边
        }
        sprite->speedy = 0; // 发生碰撞后，垂直速度清零
    }
}

void gameTick() {
    timerTick();

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
