#include "GameEnginePriv.h"

void loadImage(unsigned char *image, int address, int len) { g_platform_api.loadImage(image, address, len); }

void drawImage(int address, int x, int y, int w, int h) { g_platform_api.drawImage(address, x, y, w, h); }
