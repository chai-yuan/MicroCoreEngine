#include "GameEnginePriv.h"

void loadImage(const unsigned char *image, int address, int len) {
    if (g_platform_api.loadImage)
        g_platform_api.loadImage(image, address, len);
}

void drawImage(int address, ImageType type, int x, int y, int w, int h) {
    if (((x + w + g_screenx) < 0) || ((x + g_screenx) > LOGICAL_WIDTH) || ((y + h + g_screeny) < 0) ||
        ((y + g_screeny) > LOGICAL_HEIGHT))
        return;

    if (type == IMG_Palette && g_platform_api.drawImagePalette)
        g_platform_api.drawImagePalette(address, x + g_screenx, y + g_screeny, w, h);
    if (type == IMG_RGB565 && g_platform_api.drawImageRGB565)
        g_platform_api.drawImageRGB565(address, x + g_screenx, y + g_screeny, w, h);
}
