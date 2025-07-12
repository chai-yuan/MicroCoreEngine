#include "GameEnginePriv.h"

struct Timer *g_timer[TIMER_NUM] = {0};

void loadTimer(Timer *timer) {
    for (int i = 0; i < TIMER_NUM; i++)
        if (g_timer[i] == 0) {
            g_timer[i] = timer;
            return;
        }
}

void removeTimer(Timer *timer) {
    for (int i = 0; i < TIMER_NUM; i++)
        if (g_timer[i] == timer) {
            g_timer[i] = 0;
            return;
        }
}

int  lastCheckTime = 0;
void timerTick() {
    int wait      = g_platform_api.getTime() - lastCheckTime;
    lastCheckTime = wait + lastCheckTime;

    for (int i = 0; i < TIMER_NUM; i++) {
        if (g_timer[i]) {
            g_timer[i]->time -= wait;

            if (g_timer[i]->time < 0) {
                g_timer[i]->time = 0;
            }
        }
    }
}
