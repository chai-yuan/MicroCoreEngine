#include "GameEnginePriv.h"

struct TileMap *g_tileMap = 0;

void loadTileMap(TileMap *tileMap) { g_tileMap = tileMap; }

void removeTileMap(TileMap *tileMap) {
    if (g_tileMap == tileMap)
        g_tileMap = 0;
}

void drawTileMap(TileMap *tileMap) {
    int tileSize = tileMap->imagew * tileMap->imageh;
    int x = tileMap->x, y = tileMap->y;

    for (int idx0 = 0; idx0 < tileMap->maph; idx0++) {
        x = tileMap->x;

        for (int idx1 = 0; idx1 < tileMap->mapw; idx1++) {
            int tile   = tileMap->map[idx0 * tileMap->mapw + idx1];
            int offset = tileSize * (tile - 1) / 2;

            if (tile)
                drawImage(tileMap->address + offset, tileMap->type, x, y, tileMap->imagew, tileMap->imageh);

            x += tileMap->imagew;
        }

        y += tileMap->imageh;
    }
}
