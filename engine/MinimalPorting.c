#include "MinimalPorting.h"
#include "PlatformPorting.h"

minimal_api_t mini_platform;

static uint8_t *vmem_next_free = NULL;

typedef struct {
    int      width;
    int      height;
    uint8_t *pixels;
} InternalImage;

static InternalImage *current_render_target = NULL;
static Rect           current_clip_rect;

static void            *wrapper_mem_alloc(size_t size);
static void             wrapper_mem_free(void *ptr);
static platform_image_t wrapper_gfx_create_image(int width, int height, const void *data);
static void             wrapper_gfx_destroy_image(platform_image_t image);
static platform_image_t wrapper_gfx_create_render_target(int width, int height);
static void             wrapper_gfx_set_render_target(platform_image_t image);
static void             wrapper_gfx_set_clip_rect(Rect rect);
static void             wrapper_gfx_clear(Color color);
static void             wrapper_gfx_present(void);
static void wrapper_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip);
static void wrapper_gfx_draw_rect(Rect rect, Color color);
static int  wrapper_display_get_width(void);
static int  wrapper_display_get_height(void);

void minimal_register_api(const minimal_api_t api) {
    mini_platform = api;

    vmem_next_free = mini_platform.vmem;

    current_clip_rect.x = 0;
    current_clip_rect.y = 0;
    current_clip_rect.w = DISPLAY_WIDTH;
    current_clip_rect.h = DISPLAY_HEIGHT;

    platform_api_t wrapper_api = {// Memory
                                  .mem_alloc = wrapper_mem_alloc,
                                  .mem_free  = wrapper_mem_free,
                                  // Time & Input (Direct mapping)
                                  .get_ticks_ms     = mini_platform.get_ticks_ms,
                                  .get_button_state = mini_platform.get_button_state,
                                  // Graphics Resource Management
                                  .gfx_create_image         = wrapper_gfx_create_image,
                                  .gfx_update_image         = NULL, // Not implemented for simplicity
                                  .gfx_create_render_target = wrapper_gfx_create_render_target,
                                  .gfx_destroy_image        = wrapper_gfx_destroy_image,
                                  // Rendering State
                                  .gfx_set_render_target = wrapper_gfx_set_render_target,
                                  .gfx_set_clip_rect     = wrapper_gfx_set_clip_rect,
                                  // Drawing
                                  .gfx_clear      = wrapper_gfx_clear,
                                  .gfx_present    = wrapper_gfx_present,
                                  .gfx_draw_image = wrapper_gfx_draw_image,
                                  .gfx_draw_line  = NULL, // Not implemented for simplicity
                                  .gfx_draw_rect  = wrapper_gfx_draw_rect,
                                  // Display Info
                                  .display_get_width  = wrapper_display_get_width,
                                  .display_get_height = wrapper_display_get_height};

    platform_register_api(wrapper_api);
}

static void *wrapper_mem_alloc(size_t size) {
    size = (size + 3) & ~3;
    if (vmem_next_free + size > mini_platform.vmem + VMEM_SIZE) {
        return NULL; // Out of memory
    }
    void *ptr = vmem_next_free;
    vmem_next_free += size;
    return ptr;
}

static void wrapper_mem_free(void *ptr) {
    // No-op
}

static platform_image_t wrapper_gfx_create_image(int width, int height, const void *data) {
    InternalImage *img = (InternalImage *)wrapper_mem_alloc(sizeof(InternalImage));
    if (!img)
        return NULL;

    int size    = width * height * 2;
    img->width  = width;
    img->height = height;
    img->pixels = (uint8_t *)wrapper_mem_alloc(size);
    if (!img->pixels)
        return NULL; // Not enough space for pixel data

    if (data) {
        for (int i = 0; i < size; i++)
            img->pixels[i] = ((const uint8_t *)data)[i];
    }

    return (platform_image_t)img;
}

static platform_image_t wrapper_gfx_create_render_target(int width, int height) {
    int size = width * height * 2;
    return wrapper_gfx_create_image(width, height, wrapper_mem_alloc(size));
}

static void wrapper_gfx_destroy_image(platform_image_t image) {
    // No-op
}

static void wrapper_gfx_set_render_target(platform_image_t image) { current_render_target = (InternalImage *)image; }

static void wrapper_gfx_set_clip_rect(Rect rect) { current_clip_rect = rect; }

static void wrapper_gfx_clear(Color color) {
    Rect clear_rect = {0, 0, 0, 0};
    if (current_render_target) {
        clear_rect.w = current_render_target->width;
        clear_rect.h = current_render_target->height;
    } else {
        clear_rect.w = DISPLAY_WIDTH;
        clear_rect.h = DISPLAY_HEIGHT;
    }
    wrapper_gfx_draw_rect(clear_rect, color);
}

static void wrapper_gfx_present(void) {
    // No-op
}

static void wrapper_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip) {
    InternalImage *img = (InternalImage *)image;
    if (!img)
        return;

    Rect r;
    if (src_rect) {
        r = *src_rect;
    } else {
        r.x = 0;
        r.y = 0;
        r.w = img->width;
        r.h = img->height;
    }

    if (flip == imageUnflipped && !current_render_target) {
        mini_platform.draw(img->pixels + r.y * img->width + r.x, x, y, r.w, r.h);
        return;
    }

    /*for (int sy = 0; sy < r.h; ++sy) {*/
    /*    for (int sx = 0; sx < r.w; ++sx) {*/
    /*        int dest_x = x + sx;*/
    /*        int dest_y = y + sy;*/
    /**/
    /*        // Check against clip rect*/
    /*        if (dest_x < current_clip_rect.x || dest_x >= current_clip_rect.x + current_clip_rect.w ||*/
    /*            dest_y < current_clip_rect.y || dest_y >= current_clip_rect.y + current_clip_rect.h) {*/
    /*            continue;*/
    /*        }*/
    /**/
    /*        int read_sx = (flip & imageFlippedX) ? (r.w - 1 - sx) : sx;*/
    /*        int read_sy = (flip & imageFlippedY) ? (r.h - 1 - sy) : sy;*/
    /**/
    /*        Color c = img->pixels[(r.y + read_sy) * img->width + (r.x + read_sx)];*/
    /**/
    /*        if (current_render_target) {*/
    /*            current_render_target->pixels[dest_y * current_render_target->width + dest_x] = c;*/
    /*        } else {*/
    /*            // Drawing 1x1 pixel using the minimal API's draw function is inefficient*/
    /*            // but fulfills the contract.*/
    /*            mini_platform.draw(&c, dest_x, dest_y, 1, 1);*/
    /*        }*/
    /*    }*/
    /*}*/
}

static void wrapper_gfx_draw_rect(Rect rect, Color color) {
    /*// For simplicity, we create a temporary buffer for one row and draw it repeatedly.*/
    /*// A more advanced version might allocate the whole rectangle if memory allows.*/
    /*uint8_t *row_buffer = (uint8_t *)wrapper_mem_alloc(rect.w);*/
    /*if (!row_buffer)*/
    /*    return; // Not enough memory for even one row*/
    /**/
    /*memset(row_buffer, color, rect.w);*/
    /**/
    /*for (int i = 0; i < rect.h; ++i) {*/
    /*    int y = rect.y + i;*/
    /*    // Clipping is implicitly handled by the draw call for the screen,*/
    /*    // but needs to be manual for render targets.*/
    /*    if (current_render_target) {*/
    /*        // Manual clipping for render target*/
    /*        if (y >= 0 && y < current_render_target->height) {*/
    /*            int x_start = rect.x < 0 ? 0 : rect.x;*/
    /*            int len     = rect.w;*/
    /*            if (x_start + len > current_render_target->width) {*/
    /*                len = current_render_target->width - x_start;*/
    /*            }*/
    /*            if (len > 0) {*/
    /*                memcpy(current_render_target->pixels + y * current_render_target->width + x_start, row_buffer,
     * len);*/
    /*            }*/
    /*        }*/
    /*    } else {*/
    /*        mini_platform.draw(row_buffer, rect.x, y, rect.w, 1);*/
    /*    }*/
    /*}*/
}

static int wrapper_display_get_width(void) { return DISPLAY_WIDTH; }

static int wrapper_display_get_height(void) { return DISPLAY_HEIGHT; }
