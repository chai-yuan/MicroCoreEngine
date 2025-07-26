#ifndef GAME_ENGINE_PIRV
#define GAME_ENGINE_PIRV

#include "Debug.h"
#include "GameEngine.h"
#include "PlatformPorting.h"

extern platform_api_t platform;

typedef struct Image {
    platform_image_t ptr;
    Rect             rect;
} Image;
typedef struct ImageTable {
    platform_image_t ptr;
    int              w, h, count;
} ImageTable;
typedef struct TileMap {
    ImageTableHandle table;
    uint8_t         *tiles;
    int              tilesw, tilesh;
} TileMap;
typedef struct Sprite {
    ImageHandle             image;
    ImageFlip               flip;
    int                     x, y;
    uint32_t                mask;
    Rect                    collisionRect;
    SpriteUpdateFunction    updatefunc;
    SpriteCollisionFunction collisionfunc;
} Sprite;

#define SPRITE_NUM 8
extern SpriteHandle g_sprites[SPRITE_NUM];
#define TILEMAP_NUM 4
extern TileMapHandle g_tilemaps[TILEMAP_NUM];

static inline int max(int a, int b) { return a > b ? a : b; }

#endif
