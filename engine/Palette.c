#include "PlatformPorting.h"

void loadPalette(int *palette) {
    if (g_platform_api.loadPalette)
        g_platform_api.loadPalette(palette);
}
