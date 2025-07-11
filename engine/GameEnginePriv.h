#ifndef GAME_ENGINE_PRIV_H
#define GAME_ENGINE_PRIV_H

#include "GameEngine.h"
#include "PlatformPorting.h"

#define IMAGES_SIZE 1024 * 32
#define TIMER_NUM 4
#define TILEMAP_NUM 2

extern Palette        *g_palette;
extern unsigned char   images[IMAGES_SIZE];
extern struct TileMap *g_tileMap[TILEMAP_NUM];

void timerTick();
void drawTileMap(TileMap *tileMap);

#endif // RETRO_ENGINE_H
