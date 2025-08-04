#ifndef GAME_ENGINE_PIRV
#define GAME_ENGINE_PIRV

#include "Debug.h"
#include "GameEngine.h"
#include "PlatformPorting.h"

extern platform_api_t platform;

typedef struct Image {
    platform_image_t ptr;
    int              w, h, count;
    Rect             rect;
} Image;
typedef struct TileMap {
    ImageHandle image;
    uint8_t    *tiles;
    uint8_t    *collision;
    int         x, y, tilesw, tilesh;
} TileMap;
typedef struct Sprite {
    ImageHandle             image;
    ImageFlip               flip;
    int                     x, y;
    int                     begin, end, div, idx;
    uint8_t                 collisionMask;
    Rect                    collisionRect;
    SpriteUpdateFunction    updatefunc;
    SpriteCollisionFunction collisionfunc;
} Sprite;
typedef struct Font {
    ImageHandle image;
    int         char_begin;
} Font;

#define SPRITE_NUM 16
extern SpriteHandle g_sprites[SPRITE_NUM];
#define TILEMAP_NUM 2
extern TileMapHandle g_tilemaps[TILEMAP_NUM];
extern uint32_t      g_ticks;

static inline int max(int a, int b) { return a > b ? a : b; }

#endif
