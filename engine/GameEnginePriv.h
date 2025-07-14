#ifndef GAME_ENGINE_PRIV_H
#define GAME_ENGINE_PRIV_H

#include "GameEngine.h"
#include "PlatformPorting.h"

#define IMAGES_SIZE 1024 * 128
#define TIMER_NUM 4
#define BACKGROUND_NUM 4
#define SPRITE_NUM 4

extern struct TileMap    *g_tileMap;
extern struct Sprite     *g_sprite[SPRITE_NUM];
extern struct Background *g_background[BACKGROUND_NUM];
extern int                g_screenx, g_screeny;

void timerTick();
void drawTileMap(TileMap *tileMap);
void drawSprite(Sprite *sprite);
void drawBackground(Background *background);

#endif // RETRO_ENGINE_H
