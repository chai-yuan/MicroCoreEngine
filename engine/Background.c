#include "GameEnginePriv.h"

struct Background *g_background[BACKGROUND_NUM] = {0};

void loadBackground(Background *background) {
    for (int i = 0; i < BACKGROUND_NUM; i++)
        if (g_background[i] == 0) {
            g_background[i] = background;
            return;
        }
}

void removeBackground(Background *background) {
    for (int i = 0; i < BACKGROUND_NUM; i++)
        if (g_background[i] == background) {
            g_background[i] = 0;
            return;
        }
}

void drawBackground(Background *background) { drawImage(background->image, background->x, background->y); }
