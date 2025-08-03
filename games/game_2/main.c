#include "Debug.h"
#include "GameEngine.h"
#include "sprite.img.h"

ImageHandle g_sprite_image = NULL;

void game_init(void) { g_sprite_image = graphics_newImage(1, 100, 100, pixelFormatMonochrome, my_sprite_data); }

void game_loop(void) { graphics_drawImage(g_sprite_image, 0, 0, 0, 0); }
