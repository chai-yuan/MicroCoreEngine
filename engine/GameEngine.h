#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "GameEngineDef.h"

void game_init(void);
void game_loop(void);

void        *system_malloc(size_t size);
void         system_free(void *ptr);
void         system_setFPS(int fps);
unsigned int system_getButtonState(void);
unsigned int system_getCurrentTime(void);

int display_getHeight(void);
int display_getWidth(void);

void             graphics_clear(Color color);
void             graphics_display(void);
void             graphics_pushContext(ImageHandle target);
void             graphics_popContext(void);
void             graphics_setDrawMode(ImageDrawMode mode);
void             graphics_drawImage(ImageHandle image, int x, int y, ImageFlip flip);
void             graphics_setDrawOffset(int dx, int dy);
void             graphics_setClipRect(Rect rect);
void             graphics_drawLine(Point p1, Point p2, int width, Color color);
void             graphics_drawRect(Rect rect, Color color);
ImageHandle      graphics_loadImage(int width, int height, const char *date);
ImageHandle      graphics_newImage(int width, int height, Color bgcolor);
ImageTableHandle graphics_newImageTable(int count, int width, int height);
ImageTableHandle graphics_loadImageTable(int count, int width, int height, const char *date);
ImageHandle      graphics_getTableImage(ImageTableHandle table, int idx);
void             graphics_freeImageTable(ImageTableHandle table);

// int        graphics_drawText(const void *text, size_t len, PDStringEncoding encoding, int x, int y);
// void       graphics_setFont(LCDFont *font);

#endif // RETRO_ENGINE_H
