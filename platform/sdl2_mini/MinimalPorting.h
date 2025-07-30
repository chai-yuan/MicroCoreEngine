#ifndef Minimal_PORTING_H
#define Minimal_PORTING_H

#include "GameEngineDef.h"

#define DISPLAY_WIDTH 200
#define DISPLAY_HEIGHT 200
#define VMEM_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT * 2)

typedef struct {
    uint8_t *vmem;
    void *(*mem_alloc)(size_t size);
    void (*mem_free)(void *ptr);
    unsigned int (*get_ticks_ms)(void);
    unsigned int (*get_button_state)(void);
    void (*present)(void);
} minimal_api_t;

void minimal_register_api(const minimal_api_t api);

void game_init(void);
void game_loop(void);

#endif
