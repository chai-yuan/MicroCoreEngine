#include "GameEnginePriv.h"

Image loadImage(const unsigned char *image, ImageType type, int imagew, int imageh, int count) {
    static int image_pointer = 0;

    Image ret = {
        .w       = imagew,
        .h       = imageh,
        .count   = count,
        .type    = type,
        .address = image_pointer,
        .idx     = 0,
    };

    int len = imagew * imageh * count;
    len     = (type == IMG_Palette) ? len / 2 : (type == IMG_RGB565) ? len * 2 : len;
    g_platform_api.loadImage(image, image_pointer, len);
    image_pointer += len;

    return ret;
}

void drawImage(Image *image, int x, int y) {
    if (((x + image->w + g_screenx) < 0) || ((x + g_screenx) > LOGICAL_WIDTH) || ((y + image->h + g_screeny) < 0) ||
        ((y + g_screeny) > LOGICAL_HEIGHT))
        return;

    if (g_platform_api.drawImage)
        g_platform_api.drawImage(image, x + g_screenx, y + g_screeny);
}
