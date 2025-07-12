#include "GameEngine.h"

unsigned char w[] = {0xff, 0xff, 0xff, 0xfb, 0xfc, 0xcf, 0xcc, 0xcb, 0xff, 0xcc, 0xcc, 0xcb, 0xfc, 0xcc, 0xbc, 0xbb,
                     0xfc, 0xbc, 0xcc, 0xcb, 0xfc, 0xcc, 0xcb, 0xcb, 0xfc, 0xcc, 0xbc, 0xcb, 0xff, 0xbb, 0xbb, 0xbb};
unsigned char f[] = {0xcc, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xcc, 0xcc, 0xfc, 0xfc, 0xcc, 0xfc,
                     0xcf, 0xcc, 0xcc, 0xcf, 0xcf, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcf, 0xcc, 0xcc, 0xff};

unsigned char maze[16][16] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}, {1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}, {1, 1, 1, 1, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}, {1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}, {1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1}, {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
    {1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

unsigned char character[] = {0x00, 0x22, 0x20, 0x00, 0x02, 0x22, 0x22, 0x00, 0x21, 0x12, 0x11, 0x20, 0x21, 0x62,
                             0x16, 0x20, 0x22, 0x22, 0x22, 0x20, 0x22, 0x22, 0x22, 0x20, 0x20, 0x20, 0x20, 0x20};

void game_main() {
    loadImage(character, 0, sizeof(character));
    loadImage(w, 1000, sizeof(w));
    loadImage(f, 1032, sizeof(f));

    Sprite sprite = {
        .address  = 0,
        .imageh   = 7,
        .imagew   = 8,
        .left     = 0,
        .right    = 7,
        .top      = 0,
        .bottom   = 7,
        .x        = 8,
        .y        = 8,
        .imageidx = 0,
    };

    TileMap tilemap = {.address = 1000, .mapw = 16, .maph = 16, .map = maze, .imageh = 8, .imagew = 8, .x = 0, .y = 0};

    loadTileMap(&tilemap);
    loadSprite(&sprite);

    setFramerate(25);
    while (1) {
        unsigned char key = getKey();

        if (key & KEY_RIGHT)
            sprite.speedx = 2;
        else if (key & KEY_LEFT)
            sprite.speedx = -2;
        else
            sprite.speedx = 0;

        if (key & KEY_UP)
            sprite.speedy = -2;
        else if (key & KEY_DOWN)
            sprite.speedy = 2;
        else
            sprite.speedy = 0;

        gameTick();
    }
}
