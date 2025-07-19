#ifndef GAME_ENGINE_PIRV
#define GAME_ENGINE_PIRV

#include "GameEngine.h"
#include "PlatformPorting.h"

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
    ImageHandle          image;
    ImageFlip            flip;
    int                  x, y;
    SpriteUpdateFunction updatefunc;
} Sprite;

#endif
