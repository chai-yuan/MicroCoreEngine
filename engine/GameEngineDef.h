#ifndef GAME_ENDGINE_DEF
#define GAME_ENDGINE_DEF

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef uint16_t Color;       // RGB565
typedef uint8_t  Palette_idx; // RGB565 (0-15)
typedef struct {
    int x, y, w, h;
} Rect;
typedef struct {
    int x, y;
} Point;
typedef struct {
    int x, y;
} Vector;
typedef struct {
    int   *dx, *dy;
    Vector normal;
} CollisionInfo;
typedef enum { pixelFormatPalette, pixelFormatPaletteRLE, pixelFormatMonochrome } PixelFormat;
typedef enum { drawNormal, drawAlpha, drawAdd, drawMultiply } ImageDrawMode;
typedef enum { imageUnflipped, imageFlippedX, imageFlippedY, imageFlippedXY } ImageFlip;
typedef enum { buttonLeft = 1, buttonRight = 2, buttonUp = 4, buttonDown = 8, buttonA = 16, buttonB = 32 } Button;
typedef struct Image      *ImageHandle;
typedef struct ImageTable *ImageTableHandle;
typedef struct TileMap    *TileMapHandle;
typedef struct Sprite     *SpriteHandle;

typedef void (*SpriteUpdateFunction)(void);
typedef void (*SpriteCollisionFunction)(SpriteHandle self, SpriteHandle other, CollisionInfo info);

#endif
