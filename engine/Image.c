#include "GameEnginePriv.h"

unsigned char images[IMAGES_SIZE];

void loadImage(unsigned char *image, int address, int len) {
    for (int i = 0; i < len; i++)
        images[address + i] = image[i];
}

void drawImage(int address, int x, int y, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j += 2) {
            g_platform_api.drawPixel(x + j + 1, y + i, g_palette->palette[images[address] & 0xf]);
            g_platform_api.drawPixel(x + j, y + i, g_palette->palette[images[address] >> 4]);
            address++;
        }
    }
}
