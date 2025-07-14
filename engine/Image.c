#include "GameEnginePriv.h"

void loadImage(const unsigned char *image, int address, int len) {
    if (g_platform_api.loadImage)
        g_platform_api.loadImage(image, address, len);
}

void drawImage(int address, int x, int y, int w, int h) {
    if (g_platform_api.drawImage)
        g_platform_api.drawImage(address, x + g_screenx, y + g_screeny, w, h);
}
