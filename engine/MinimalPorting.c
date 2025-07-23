#include "MinimalPorting.h"
#include "Debug.h"
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

    unsigned char bk_color[2] = {0};
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int i2 = 0; i2 < DISPLAY_WIDTH; i2++) {
            mini_platform.draw(bk_color, i2, i, 1, 1, imageUnflipped);
        }
    }
}

static void wrapper_gfx_present(void) { mini_platform.present(); }

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

    mini_platform.draw(img->pixels + (r.y * img->width + r.x) * 2, x, y, r.w, r.h, flip);
}

static void wrapper_gfx_draw_rect(Rect rect, Color color) { WARN("wrapper_gfx_draw_rect not implemented"); }

static int wrapper_display_get_width(void) { return DISPLAY_WIDTH; }

static int wrapper_display_get_height(void) { return DISPLAY_HEIGHT; }
