#include "GameEnginePriv.h"

void graphics_clear(Color color) { platform.gfx_clear(color); }

ImageHandle graphics_newImage(int count, int width, int height, PixelFormat format, const uint8_t *data) {
    Image *image = system_malloc(sizeof(Image));
    image->ptr   = platform.gfx_create_image(width, height * count, format, data);
    image->w     = width;
    image->h     = height;
    image->count = count;
    image->rect  = (Rect){0, 0, width, height};
    return image;
}

void graphics_freeImage(ImageHandle image) { system_free(image); }

void graphics_drawImage(ImageHandle image, int x, int y, int idx, ImageFlip flip) {
    Rect r = (Rect){0, idx * image->h, image->w, image->h};
    platform.gfx_draw_image(image->ptr, &r, x, y, flip);
}

void graphics_pushContext(ImageHandle target) { platform.gfx_set_render_target((platform_image_t)target->ptr); }

void graphics_popContext(void) { platform.gfx_set_render_target(0); }

void graphics_getImageTableSize(ImageHandle image, int *width, int *height) {
    *width  = image->rect.w;
    *height = image->rect.h;
}

void graphics_drawRect(Rect rect, Color color) { platform.gfx_draw_rect(rect, color); }
