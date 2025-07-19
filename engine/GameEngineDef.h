#ifndef GAME_ENDGINE_DEF
#define GAME_ENDGINE_DEF

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    unsigned char r, g, b, a;
} Color;
typedef struct {
    int x, y, w, h;
} Rect;
typedef struct {
    int x, y;
} Point;
typedef enum { pixelRGBA4444 } PixelFormat;
typedef enum { drawNormal, drawAlpha, drawAdd, drawMultiply } ImageDrawMode;
typedef enum { imageUnflipped, imageFlippedX, imageFlippedY, imageFlippedXY } ImageFlip;
typedef enum { buttonLeft = 1, buttonRight = 2, buttonUp = 4, buttonDown = 8, buttonA = 16, buttonB = 32 } Button;
typedef struct Image      *ImageHandle;
typedef struct ImageTable *ImageTableHandle;
typedef struct TileMap    *TileMapHandle;

#endif
