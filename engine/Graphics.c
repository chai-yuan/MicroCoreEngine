#include "GameEnginePriv.h"

void graphics_clear(Color color) { platform.gfx_clear(color); }

void graphics_display(void) { platform.gfx_present(); }

ImageHandle graphics_loadImage(int width, int height, const uint8_t *data) {
    Image *image = system_malloc(sizeof(Image));
    image->ptr   = platform.gfx_create_image(width, height, data);
    image->rect  = (Rect){0, 0, width, height};
    return image;
}
ImageHandle graphics_newImage(int width, int height, Color bgcolor) {
    Image *image = system_malloc(sizeof(Image));
    image->ptr   = platform.gfx_create_render_target(width, height);
    image->rect  = (Rect){0, 0, width, height};

    if (!image->ptr)
        return NULL;
    platform.gfx_set_render_target(image->ptr);
    platform.gfx_clear(bgcolor);
    platform.gfx_set_render_target(NULL);
    return image;
}

void graphics_freeBitmap(ImageHandle image) { system_free(image); }

void graphics_drawImage(ImageHandle image, int x, int y, ImageFlip flip) {
    platform.gfx_draw_image(image->ptr, &image->rect, x, y, flip);
}

void graphics_pushContext(ImageHandle target) { platform.gfx_set_render_target((platform_image_t)target); }

void graphics_popContext(void) { platform.gfx_set_render_target(NULL); }

void graphics_drawRect(Rect rect, Color color) { platform.gfx_draw_rect(rect, color); }

void graphics_drawLine(Point p1, Point p2, int width, Color color) { platform.gfx_draw_line(p1, p2, width, color); }

ImageTableHandle graphics_newImageTable(int count, int width, int height) {
    ImageTable *table = system_malloc(sizeof(ImageTable));
    table->ptr        = platform.gfx_create_render_target(width, height * count);
    table->count      = count;
    table->w          = width;
    table->h          = height;
    return table;
}

ImageTableHandle graphics_loadImageTable(int count, int width, int height, const uint8_t *data) {
    ImageTable *table = system_malloc(sizeof(ImageTable));
    table->ptr        = platform.gfx_create_image(width, height * count, data);
    table->count      = count;
    table->w          = width;
    table->h          = height;
    return table;
}

ImageHandle graphics_getTableImage(ImageTableHandle table, int idx) {
    Image *image = system_malloc(sizeof(Image));
    image->ptr   = table->ptr;
    image->rect  = (Rect){0, idx * table->h, table->w, table->h};
    return image;
}

void graphics_getImageTableSize(ImageHandle image, int *width, int *height) {
    *width  = image->rect.w;
    *height = image->rect.h;
}

void graphics_drawImageTable(ImageTableHandle table, int x, int y, int idx, ImageFlip flip) {
    Rect r = (Rect){0, idx * table->h, table->w, table->h};
    platform.gfx_draw_image(table->ptr, &r, x, y, flip);
}

void graphics_freeImageTable(ImageTableHandle table);
