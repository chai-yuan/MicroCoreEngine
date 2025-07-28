#include "GameEnginePriv.h"

uint32_t g_ticks = 0;

void *system_malloc(size_t size) {
    void *ptr = platform.mem_alloc(size);
    if (ptr == NULL) {
        WARN("system_malloc error");
    }

    return ptr;
}
void         system_free(void *ptr) { platform.mem_free(ptr); }
unsigned int system_getButtonState(void) { return platform.get_button_state(); }
unsigned int system_getCurrentTime(void) { return platform.get_ticks_ms(); }

static unsigned int g_seed;

void system_srand(unsigned int seed) { g_seed = seed; }
int  system_rand(void) {
    g_seed = g_seed * 1103515245 + 12345;
    return (g_seed >> 16) & 0x7FFF;
}
