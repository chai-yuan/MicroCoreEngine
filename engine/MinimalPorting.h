#ifndef Minimal_PORTING_H
#define Minimal_PORTING_H

#include <stdint.h>

#define VMEM_SIZE 1024 * 1024
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128

typedef struct {
    uint8_t *vmem;
    unsigned int (*get_ticks_ms)(void);
    unsigned int (*get_button_state)(void);
    void (*draw)(uint8_t *data, int x, int y, int w, int h);
} minimal_api_t;

extern minimal_api_t mini_platform;

void minimal_register_api(const minimal_api_t api);

#endif
