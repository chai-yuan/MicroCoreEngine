#ifndef GAME_ENGINE_PRIV_H
#define GAME_ENGINE_PRIV_H

#include "GameEngine.h"
#include "PlatformPorting.h"

#define IMAGES_SIZE 1024 * 32
#define TIMER_NUM 4

extern Palette      *g_palette;
extern unsigned char images[IMAGES_SIZE];

void timerTick();

#endif // RETRO_ENGINE_H
