#include "GameEnginePriv.h"

static inline int checkSpriteTilemapCollision(const SpriteHandle s, const TileMapHandle t) {
    if (t->collision == NULL)
        return 0;

    int tileSizeW = t->table->w;
    int tileSizeH = t->table->h;

    int hitbox_x  = s->x + s->collisionRect.x;
    int hitbox_y  = s->y + s->collisionRect.y;
    int hitbox_x1 = s->x + s->collisionRect.w;
    int hitbox_y1 = s->y + s->collisionRect.h;

    // 计算碰撞盒覆盖的瓦片网格范围
    int startCol = max(hitbox_x / tileSizeW, 0);
    int endCol   = (hitbox_x1 - 1) / tileSizeW;
    int startRow = max(hitbox_y / tileSizeH, 0);
    int endRow   = (hitbox_y1 - 1) / tileSizeH;

    for (int row = startRow; (row <= endRow) && (row < t->tilesh); ++row) {
        for (int col = startCol; (col <= endCol) && (col < t->tilesw); ++col) {
            // 获取瓦片在一维数组中的索引
            int tileIndex = row * t->tilesw + col;
            if (t->collision[tileIndex] & s->collisionMask) {
                return 1;
            }
        }
    }
    return 0;
}

void sprite_moveWithCollisions(SpriteHandle s, int dx, int dy) {
    while (dx || dy) {
        int moveX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
        int moveY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
        s->x += moveX;
        dx -= moveX;
        if (s->collisionfunc && checkSpriteTilemapCollision(s, g_tilemaps[0])) {
            s->x -= moveX;
            s->collisionfunc(s, NULL, (CollisionInfo){&dx, &dy, {-moveX, 0}});
        }
        s->y += moveY;
        dy -= moveY;
        if (s->collisionfunc && checkSpriteTilemapCollision(s, g_tilemaps[0])) {
            s->y -= moveY;
            s->collisionfunc(s, NULL, (CollisionInfo){&dx, &dy, {0, -moveY}});
        }
    }
}
