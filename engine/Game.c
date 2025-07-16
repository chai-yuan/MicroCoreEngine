#include "GameEnginePriv.h"

int g_tickCount = 0;

int checkSpriteTilemapCollision(const Sprite *s, const TileMap *t) {
    int tileSizeW = t->image->w;
    int tileSizeH = t->image->h;

    int hitbox_x = s->x + s->left;
    int hitbox_y = s->y + s->top;
    int hitbox_x1 = s->x + s->right;
    int hitbox_y1 = s->y+s->bottom;

    // 计算碰撞盒覆盖的瓦片网格范围
    int startCol = hitbox_x / tileSizeW;
    int endCol   = (hitbox_x1 - 1) / tileSizeW;
    int startRow = hitbox_y / tileSizeH;
    int endRow   = (hitbox_y1 - 1) / tileSizeH;

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            if (col < 0 || col >= t->mapw || row < 0 || row >= t->maph) {
                return 1; // 与地图外区域碰撞也算碰撞
            }

            // 获取瓦片在一维数组中的索引
            int tileIndex = row * t->mapw + col;

            if (t->collision[tileIndex] & s->collision) {
                return 1; // 发现碰撞！
            }
        }
    }

    return 0;
}

void moveSprite(Sprite *s, TileMap *t) {
    s->speedy += s->accelerationy;
    s->speedx += s->accelerationx;
    if (s->speedx == 0 && s->speedy == 0) {
        return; // 无需移动
    }

    s->y += s->speedy;
    if (checkSpriteTilemapCollision(s, t)) {
        s->y -= s->speedy;
        s->speedy = 0;
    }
    s->x += s->speedx;
    if (checkSpriteTilemapCollision(s, t)) {
        s->x -= s->speedx;
        s->speedx = 0;
    }
}

void gameTick() {
    g_tickCount++;

    // 精灵运动
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprite[i])
            moveSprite(g_sprite[i], g_tileMap);

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
