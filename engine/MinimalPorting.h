#ifndef Minimal_PORTING_H
#define Minimal_PORTING_H

#include "GameEngineDef.h"

#define VMEM_SIZE 1024 * 1024
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128

typedef struct {
    uint8_t *vmem;
    unsigned int (*get_ticks_ms)(void);
    unsigned int (*get_button_state)(void);
    void (*present)(void);
} minimal_api_t;

void minimal_register_api(const minimal_api_t api);

void game_init(void);
void game_loop(void);

#endif
