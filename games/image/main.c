#include "GameEngine.h"

unsigned char w[] = {0xff, 0xff, 0xff, 0xfb, 0xfc, 0xcf, 0xcc, 0xcb, 0xff, 0xcc, 0xcc, 0xcb, 0xfc, 0xcc, 0xbc, 0xbb,
                     0xfc, 0xbc, 0xcc, 0xcb, 0xfc, 0xcc, 0xcb, 0xcb, 0xfc, 0xcc, 0xbc, 0xcb, 0xff, 0xbb, 0xbb, 0xbb};
unsigned char f[] = {0x88, 0x88, 0x57, 0x75, 0x88, 0x58, 0xd8, 0x85, 0x78, 0x88, 0xd7, 0x88, 0x78, 0x88, 0x8d, 0xd8,
                     0x78, 0x88, 0x57, 0x58, 0x75, 0x8d, 0x7d, 0x58, 0xdd, 0x8d, 0xdd, 0x88, 0x88, 0x88, 0x88, 0xdd};
;

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
        .address   = 0,
        .imageh    = 7,
        .imagew    = 8,
        .left      = 0,
        .right     = 6,
        .top       = 0,
        .bottom    = 7,
        .collision = 1,
        .x         = 8,
        .y         = 8,
        .imageidx  = 0,
    };

    TileMap tilemap = {.address   = 1000,
                       .mapw      = 16,
                       .maph      = 16,
                       .map       = (unsigned char *)maze,
                       .collision = (unsigned char *)maze,
                       .imageh    = 8,
                       .imagew    = 8,
                       .x         = 0,
                       .y         = 0};

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
