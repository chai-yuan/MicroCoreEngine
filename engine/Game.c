#include "GameEnginePriv.h"

void gameTick() {
    timerTick();

    for (int i = 0; i < TILEMAP_NUM; i++)
        if (g_tileMap[i])
            drawTileMap(g_tileMap[i]);
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprite[i])
            drawSprite(g_sprite[i]);

    g_platform_api.drawScreen();
}
