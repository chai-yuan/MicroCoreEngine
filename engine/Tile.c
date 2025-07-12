#include "GameEnginePriv.h"

struct TileMap *g_tileMap[TILEMAP_NUM] = {0};

void loadTileMap(TileMap *tileMap) {
    for (int i = 0; i < TILEMAP_NUM; i++)
        if (g_tileMap[i] == 0) {
            g_tileMap[i] = tileMap;
            return;
        }
}

void removeTileMap(TileMap *tileMap) {
    for (int i = 0; i < TILEMAP_NUM; i++)
        if (g_tileMap[i] == tileMap) {
            g_tileMap[i] = 0;
            return;
        }
}

void drawTileMap(TileMap *tileMap) {
    int tileSize = tileMap->imagew * tileMap->imageh;
    int x = tileMap->drawx, y = tileMap->drawy;

    for (int idx0 = 0; idx0 < tileMap->maph; idx0++) {
        x = tileMap->drawx;

        for (int idx1 = 0; idx1 < tileMap->mapw; idx1++) {
            int tile   = tileMap->map[idx0 * tileMap->mapw + idx1];
            int offset = tileSize * (tile - 1);

            if (tile)
                drawImage(tileMap->address + offset, x, y, tileMap->imagew, tileMap->imageh);

            x += tileMap->imagew;
        }

        y += tileMap->imageh;
    }
}
