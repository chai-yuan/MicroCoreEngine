#include "GameEnginePriv.h"

void gameTick() {
    timerTick();
    g_platform_api.drawScreen();
}
