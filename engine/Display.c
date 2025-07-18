#include "GameEngine.h"
#include "PlatformPorting.h"

int display_getWidth(void) { return platform.display_get_width(); }
int display_getHeight(void) { return platform.display_get_height(); }
