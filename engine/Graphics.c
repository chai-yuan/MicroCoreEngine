#include "GameEngine.h"
#include "PlatformPorting.h"

void graphics_clear(Color color) { platform.gfx_clear(color); }
void graphics_display(void) { platform.gfx_present(); }

ImageHandle graphics_loadImage(int width, int height, const char *data) {
    return (ImageHandle)platform.gfx_create_image(width, height, data);
}

ImageHandle graphics_newImage(int width, int height, Color bgcolor) {
    platform_image_t image = platform.gfx_create_render_target(width, height);
    if (!image)
        return NULL;
    platform.gfx_set_render_target(image);
    platform.gfx_clear(bgcolor);
    platform.gfx_set_render_target(NULL);
    return (ImageHandle)image;
}

void graphics_drawImage(ImageHandle image, int x, int y, ImageFlip flip) {
    // 为了简化，我们假设总是绘制整个位图
    // 您需要一种方法来获取位图的尺寸，这可以存储在您的ImageHandle结构体中
    // Rect src_rect = { 0, 0, image->width, image->height };
    // platform->gfx_draw_image((platform_image_t)image, &src_rect, x, y, flip);
    platform.gfx_draw_image((platform_image_t)image, NULL, x, y, flip); // 传入NULL的src_rect表示绘制整个图像
}

void graphics_pushContext(ImageHandle target) { platform.gfx_set_render_target((platform_image_t)target); }

void graphics_popContext(void) { platform.gfx_set_render_target(NULL); }

void graphics_drawRect(Rect rect, Color color) { platform.gfx_draw_rect(rect, color); }
void graphics_drawLine(Point p1, Point p2, int width, Color color) { platform.gfx_draw_line(p1, p2, width, color); }

ImageTableHandle graphics_newImageTable(int count, int width, int height) {
    ImageHandle *table = system_malloc(sizeof(ImageHandle) * count);
    // ... 循环创建或加载每个image ...
    return (ImageTableHandle)table;
}
