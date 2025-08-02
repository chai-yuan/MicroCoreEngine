#include "GameEnginePriv.h"

static inline int checkSpriteTilemapCollision(const SpriteHandle s, const TileMapHandle t) {
    if (t->collision == NULL)
        return 0;

    int tileSizeW = t->image->w;
    int tileSizeH = t->image->h;

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

int checkSpritesCollision(const SpriteHandle s, const SpriteHandle s2) {
    if ((s->collisionMask & s2->collisionMask) == 0)
        return 0;

    int s_left   = s->x + s->collisionRect.x;
    int s_right  = s_left + s->collisionRect.w;
    int s_top    = s->y + s->collisionRect.y;
    int s_bottom = s_top + s->collisionRect.h;

    int s2_left   = s2->x + s2->collisionRect.x;
    int s2_right  = s2_left + s2->collisionRect.w;
    int s2_top    = s2->y + s2->collisionRect.y;
    int s2_bottom = s2_top + s2->collisionRect.h;

    if (s_right <= s2_left || s_left >= s2_right || s_bottom <= s2_top || s_top >= s2_bottom) {
        return 0; // 不碰撞
    }

    return 1; // 碰撞
}

void sprite_moveWithCollisions(SpriteHandle s, int dx, int dy) {
    if (s->collisionMask == 0 || s->collisionfunc == NULL) {
        s->x += dx;
        s->y += dy;
        return;
    }

    while (dx || dy) {
        int moveX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
        int moveY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

        s->x += moveX;
        dx -= moveX;
        for (int i = 0; i < TILEMAP_NUM; i++)
            if (g_tilemaps[i] && checkSpriteTilemapCollision(s, g_tilemaps[i])) {
                s->x -= moveX;
                break;
            }

        s->y += moveY;
        dy -= moveY;
        for (int i = 0; i < TILEMAP_NUM; i++)
            if (g_tilemaps[i] && checkSpriteTilemapCollision(s, g_tilemaps[i])) {
                s->y -= moveY;
                break;
            }

        for (int i = 0; i < SPRITE_NUM; i++)
            if (g_sprites[i] && (g_sprites[i] != s) && checkSpritesCollision(s, g_sprites[i])) {
                s->collisionfunc(s, g_sprites[i], (CollisionInfo){});
            }
    }
}
