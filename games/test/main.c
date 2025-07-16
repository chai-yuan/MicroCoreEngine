#include "GameEngine.h"
#include "bg.img.h"
#include "person.img.h"
#include "person_run.img.h"
#include "map.img.h"

// 地图尺寸: 40 x 12
char mapData[12][40] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0, 10, 9, 0, 7, 7, 11, 0, 1, 1, 0, 0, 0, 8, 8, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 12, 12, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 0, 12, 0, 3, 3, 3, 0, 0, 12, 0, 0, 0 },
    { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3 },
    { 6, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 6, 6, 6, 2, 2, 2, 2, 2, 2, 2, 6, 6, 6, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
    { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 }
};

void game_main() {
    Image person_idle = loadImage(person_data, IMG_Palette, 40, 32, 6);
    Image person_run  = loadImage(person_run_data, IMG_Palette, 40, 32, 8);
    Image map         = loadImage(map_image_data, IMG_RGB565, 16, 16, 12);
    Image background  = loadImage(bg_data, IMG_Palette, bg_width, bg_height, 1);

    Sprite sprite = {
        .image         = &person_idle,
        .left          = 10,
        .right         = 25,
        .top           = 4,
        .bottom        = 32,
        .collision     = 3,
        .x             = 50,
        .y             = 10,
        .accelerationy = 1,
        .framerate     = 4,
    };

    TileMap tilemap = {.image = &map, .mapw = 40, .maph = 12, .map = mapData, .collision = mapData};

    Background bg = {
        .x     = 0,
        .y     = 0,
        .image = &background,
    };

    // loadBackground(&bg);
    loadTileMap(&tilemap);
    loadSprite(&sprite);

    setFramerate(25);
    while (1) {
        unsigned char key = getKey();

        if (key & KEY_RIGHT) {
            sprite.speedx = 2;
            sprite.image  = &person_run;
        } else if ((key & KEY_LEFT)) {
            sprite.speedx = -2;
            sprite.image  = &person_run;
        } else {
            sprite.speedx = 0;
            sprite.image  = &person_idle;
        }

        if (key & KEY_UP)
            sprite.speedy = -5;

        gameTick();

        screenRoll(-sprite.speedx, 0);
    }
}
