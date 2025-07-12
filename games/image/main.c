#include "GameEngine.h"

unsigned char w[] = {0xff, 0xff, 0xff, 0xfb, 0xfc, 0xcf, 0xcc, 0xcb, 0xff, 0xcc, 0xcc, 0xcb, 0xfc, 0xcc, 0xbc, 0xbb,
                     0xfc, 0xbc, 0xcc, 0xcb, 0xfc, 0xcc, 0xcb, 0xcb, 0xfc, 0xcc, 0xbc, 0xcb, 0xff, 0xbb, 0xbb, 0xbb};

unsigned char maze[16][16] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1}, {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

unsigned char character[] = {0x77, 0xbb, 0xc0, 0x2,  0x7,  0xcc, 0xcc, 0x2,  0xc,  0xc1, 0x71, 0x8,  0xaa,
                             0x7a, 0x7a, 0x8,  0xcc, 0xba, 0xaa, 0xb7, 0x7b, 0xba, 0xaa, 0x8,  0x6,  0x66,
                             0xa6, 0x0,  0xc,  0x0,  0xc,  0x0,  0x77, 0xbb, 0xc0, 0x20, 0x7,  0xcc, 0xcc,
                             0x20, 0xc,  0xc1, 0x71, 0x80, 0xaa, 0x7a, 0x7a, 0x80, 0xac, 0xba, 0xaa, 0x70,
                             0x7,  0xba, 0xaa, 0x80, 0x6,  0x66, 0xa6, 0x0,  0x0,  0xc0, 0xc0, 0x0};

void game_main() {
    loadImage(character, 0, sizeof(character));
    loadImage(w, 1000, sizeof(w));

    Sprite sprite = {
        .address  = 0,
        .imageh   = 8,
        .imagew   = 8,
        .x        = 8,
        .y        = 8,
        .imageidx = 1,
    };

    TileMap tilemap = {
        .address = 1000, .mapw = 16, .maph = 16, .map = maze, .imageh = 8, .imagew = 8, .drawx = 0, .drawy = 0};

    loadTileMap(&tilemap);
    loadSprite(&sprite);

    Timer timer;
    timer.time = 50000;
    loadTimer(&timer);
    while (timer.time) {
        gameTick();
    }
}
