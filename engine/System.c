#include "GameEnginePriv.h"

void        *system_malloc(size_t size) { return platform.mem_alloc(size); }
void         system_free(void *ptr) { platform.mem_free(ptr); }
unsigned int system_getButtonState(void) { return platform.get_button_state(); }
unsigned int system_getCurrentTime(void) { return platform.get_ticks_ms(); }
