#include "GameEngine.h"

int g_screenx = 0, g_screeny = 0;

void screenRoll(int x, int y) {
    g_screenx += x;
    g_screeny += y;
}
