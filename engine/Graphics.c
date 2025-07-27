#include "GameEnginePriv.h"

void graphics_clear(Color color) { platform.gfx_clear(color); }

void graphics_display(void) {
#ifdef ENABLE_DEBUG_COL
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i]) {
            Rect r = {g_sprites[i]->x, g_sprites[i]->y, g_sprites[i]->collisionRect.w, g_sprites[i]->collisionRect.h};
            graphics_drawRect(r, (Color){100, 255, 0, 255});
        }

#endif
    platform.gfx_present();
    g_ticks++; // TODO 应该被移动到其他位置
}

ImageHandle graphics_loadImage(int width, int height, PixelFormat format, const uint8_t *data) {
    Image *image = system_malloc(sizeof(Image));
    image->ptr   = platform.gfx_create_image(width, height, format, data);
    image->rect  = (Rect){0, 0, width, height};
    return image;
}

ImageHandle graphics_newImage(int width, int height, PixelFormat format) {
    Image *image = system_malloc(sizeof(Image));
    image->ptr   = platform.gfx_create_image(width, height, format, NULL);
    image->rect  = (Rect){0, 0, width, height};
    return image;
}

void graphics_freeBitmap(ImageHandle image) { system_free(image); }

void graphics_drawImage(ImageHandle image, int x, int y, ImageFlip flip) {
    platform.gfx_draw_image(image->ptr, &image->rect, x, y, flip);
}

void graphics_pushContext(ImageHandle target) { platform.gfx_set_render_target((platform_image_t)target); }

void graphics_popContext(void) { platform.gfx_set_render_target(NULL); }

void graphics_drawRect(Rect rect, Color color) { platform.gfx_draw_rect(rect, color); }

ImageTableHandle graphics_newImageTable(int count, int width, int height, PixelFormat format) {
    ImageTable *table = system_malloc(sizeof(ImageTable));
    table->ptr        = platform.gfx_create_image(width, height * count, format, NULL);
    table->count      = count;
    table->w          = width;
    table->h          = height;
    return table;
}

ImageTableHandle graphics_loadImageTable(int count, int width, int height, PixelFormat format, const uint8_t *data) {
    ImageTable *table = system_malloc(sizeof(ImageTable));
    table->ptr        = platform.gfx_create_image(width, height * count, format, data);
    table->count      = count;
    table->w          = width;
    table->h          = height;
    return table;
}

ImageTableHandle graphics_tableFromImage(ImageHandle image) {
    ImageTable *table = system_malloc(sizeof(ImageTable));
    table->ptr        = image->ptr;
    table->count      = 1;
    table->w          = image->rect.w;
    table->h          = image->rect.h;
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
