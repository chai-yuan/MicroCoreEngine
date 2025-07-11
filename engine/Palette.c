#include "GameEngine.h"

Palette define_palette = {
    .palette = {0x0, 0xef18, 0xb9c9, 0x7db6, 0x49ea, 0x6d2d, 0x2a0d, 0xddcb, 0xb46d, 0x42cb, 0xbb09, 0x31a6, 0x73ae,
                0x8d4c, 0x3df9, 0xbdd7},
    .currentColorIndex = 0,
};

Palette *g_palette = &define_palette;

void loadPalette(Palette *palette) { g_palette = palette; }
