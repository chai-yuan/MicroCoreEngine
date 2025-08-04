#include "Debug.h"
#include "GameEngine.h"
#include "font.img.h"
#include "sprite.img.h"

ImageHandle g_sprite_image = NULL;
FontHandle  g_font         = NULL;

void game_init(void) {
    g_sprite_image = graphics_newImage(95, 8, 16, pixelFormatMonochrome, font_data);
    g_font         = font_newFont(g_sprite_image, 32);
}

void game_loop(void) { font_drawText(g_font, "Hello world!", 0, 0); }
