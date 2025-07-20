#include "GameEnginePriv.h"

TileMapHandle g_tilemaps[TILEMAP_NUM] = {NULL};

TileMapHandle tilemap_newTilemap(void) {
    TileMap *tile = platform.mem_alloc(sizeof(TileMap));
    return tile;
}
void tilemap_freeTilemap(TileMapHandle tilemap) { platform.mem_free(tilemap); }

void tilemap_addTilemap(TileMapHandle tilemap) {
    for (int i = 0; i < TILEMAP_NUM; i++)
        if (g_tilemaps[i] == NULL) {
            g_tilemaps[i] = tilemap;
            return;
        }
}

void tilemap_removeTilemap(TileMapHandle tilemap) {
    for (int i = 0; i < TILEMAP_NUM; i++)
        if (g_tilemaps[i] == tilemap) {
            g_tilemaps[i] = NULL;
            return;
        }
}

void tilemap_setImageTable(TileMapHandle tilemap, ImageTableHandle table) { tilemap->table = table; }

ImageTableHandle tilemap_getImageTable(TileMapHandle tilemap) { return tilemap->table; }

void tilemap_setSize(TileMapHandle tilemap, int tilesWide, int tilesHigh) {
    tilemap->tilesw = tilesWide;
    tilemap->tilesh = tilesHigh;
}

void tilemap_getSize(TileMapHandle tilemap, int *tilesWide, int *tilesHigh) {
    *tilesWide = tilemap->tilesw;
    *tilesHigh = tilemap->tilesh;
}

void tilemap_getPixelSize(TileMapHandle tilemap, uint32_t *width, uint32_t *height);

void tilemap_setTiles(TileMapHandle tilemap, uint8_t *tiles, int tilesWide, int tilesHigh) {
    tilemap->tiles  = tiles;
    tilemap->tilesw = tilesWide;
    tilemap->tilesh = tilesHigh;
}

void tilemap_setTileAtPosition(TileMapHandle tilemap, int tilex, int tiley, uint8_t idx) {
    tilemap->tiles[tilex + tiley * tilemap->tilesw] = idx;
}

int tilemap_getTileAtPosition(TileMapHandle tilemap, int tilex, int tiley) {
    return tilemap->tiles[tilex + tiley * tilemap->tilesw];
}

void tilemap_drawAtPoint(TileMapHandle m, int x, int y, ImageDrawMode mode) {
    int drawx, drawy = y;

    for (int r = 0; r < m->tilesh; r++) {
        drawx = x;
        for (int c = 0; c < m->tilesw; c++) {
            int idx = m->tiles[c + r * m->tilesh];

            if (idx)
                graphics_drawImageTable(m->table, drawx, drawy, idx - 1, imageUnflipped);

            drawx += m->table->w;
        }
        drawy += m->table->h;
    }
}
