#include "GameEnginePriv.h"

TileMapHandle g_tilemaps[TILEMAP_NUM] = {NULL};

TileMapHandle tilemap_newTilemap(int tilesWide, int tilesHigh) {
    for (int i = 0; i < TILEMAP_NUM; i++) {
        if (g_tilemaps[i] == NULL) {
            TileMap *tile   = platform.mem_alloc(sizeof(TileMap));
            tile->x         = 0;
            tile->y         = 0;
            tile->tilesw    = tilesWide;
            tile->tilesh    = tilesHigh;
            tile->collision = NULL;

            g_tilemaps[i] = tile;
            return g_tilemaps[i];
        }
    }
    return NULL;
}
void tilemap_freeTilemap(TileMapHandle tilemap) {
    for (int i = 0; i < TILEMAP_NUM; i++)
        if (g_tilemaps[i] == tilemap) {
            platform.mem_free(tilemap);
            g_tilemaps[i] = NULL;
            return;
        }
}

void tilemap_setImage(TileMapHandle tilemap, ImageHandle table) { tilemap->image = table; }

ImageHandle tilemap_getImageTable(TileMapHandle tilemap) { return tilemap->image; }

void tilemap_setSize(TileMapHandle tilemap, int tilesWide, int tilesHigh) {
    tilemap->tilesw = tilesWide;
    tilemap->tilesh = tilesHigh;
}

void tilemap_getSize(TileMapHandle tilemap, int *tilesWide, int *tilesHigh) {
    *tilesWide = tilemap->tilesw;
    *tilesHigh = tilemap->tilesh;
}

void tilemap_getPixelSize(TileMapHandle tilemap, uint32_t *width, uint32_t *height);

void tilemap_setTiles(TileMapHandle tilemap, uint8_t *tiles) { tilemap->tiles = tiles; }

void tilemap_setCollision(TileMapHandle tilemap, uint8_t *collision) { tilemap->collision = collision; }

void tilemap_setTileAtPosition(TileMapHandle tilemap, int tilex, int tiley, uint8_t idx) {
    tilemap->tiles[tilex + tiley * tilemap->tilesw] = idx;
}

int tilemap_getTileAtPosition(TileMapHandle tilemap, int tilex, int tiley) {
    return tilemap->tiles[tilex + tiley * tilemap->tilesw];
}

void tilemap_setPosition(TileMapHandle m, int x, int y) {
    m->x = x;
    m->y = y;
}
