#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "GameEngineDef.h"

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
ImageHandle      graphics_loadImage(int width, int height, const uint8_t *data);
ImageHandle      graphics_newImage(int width, int height, Color bgcolor);
void             graphics_getImageSize(ImageHandle image, int *width, int *height);
void             graphics_freeBitmap(ImageHandle image);
ImageTableHandle graphics_newImageTable(int count, int width, int height);
ImageTableHandle graphics_loadImageTable(int count, int width, int height, const uint8_t *data);
void             graphics_getImageTableSize(ImageHandle image, int *width, int *height);
ImageHandle      graphics_getTableImage(ImageTableHandle table, int idx);
void             graphics_drawImageTable(ImageTableHandle table, int x, int y, int idx, ImageFlip flip);
void             graphics_freeImageTable(ImageTableHandle table);
// int        graphics_drawText(const void *text, size_t len, PDStringEncoding encoding, int x, int y);
// void       graphics_setFont(LCDFont *font);

TileMapHandle    tilemap_newTilemap(void);
void             tilemap_freeTilemap(TileMapHandle tilemap);
void             tilemap_setImageTable(TileMapHandle tilemap, ImageTableHandle table);
ImageTableHandle tilemap_getImageTable(TileMapHandle tilemap);
void             tilemap_setSize(TileMapHandle tilemap, int tilesWide, int tilesHigh);
void             tilemap_getSize(TileMapHandle tilemap, int *tilesWide, int *tilesHigh);
void             tilemap_getPixelSize(TileMapHandle tilemap, uint32_t *width, uint32_t *height);
void             tilemap_setTiles(TileMapHandle tilemap, uint8_t *tiles, int tilesWide, int tilesHigh);
void             tilemap_setTileAtPosition(TileMapHandle tilemap, int tilex, int tiley, uint8_t idx);
int              tilemap_getTileAtPosition(TileMapHandle tilemap, int tilex, int tiley);
void             tilemap_drawAtPoint(TileMapHandle m, int x, int y, ImageDrawMode mode);
void             tilemap_addTilemap(TileMapHandle tilemap);
void             tilemap_removeTilemap(TileMapHandle tilemap);

SpriteHandle sprite_newSprite(void);
void         sprite_freeSprite(SpriteHandle sprite);
void         sprite_addSprite(SpriteHandle sprite);
void         sprite_removeSprite(SpriteHandle sprite);
void         sprite_updateAndDrawSprites(void);
void         sprite_moveTo(SpriteHandle sprite, int x, int y);
void         sprite_getPosition(SpriteHandle sprite, int *x, int *y);
void         sprite_setImage(SpriteHandle sprite, ImageHandle image, ImageFlip flip);
void         sprite_setZIndex(SpriteHandle sprite, int8_t zIndex);
void         sprite_setVisible(SpriteHandle sprite, int flag);
void         sprite_setImageFlip(SpriteHandle sprite, ImageFlip flip);
ImageFlip    sprite_getImageFlip(SpriteHandle sprite);
void         sprite_setUpdateFunction(SpriteHandle sprite, SpriteUpdateFunction func);
void         sprite_setCollideRect(SpriteHandle sprite, Rect collideRect);
Rect         sprite_getCollideRect(SpriteHandle sprite);
uint32_t     sprite_getCollideMask(SpriteHandle sprite);
void         sprite_setCollideMask(SpriteHandle sprite, uint32_t mask);
void         sprite_setCollisionResponseFunction(SpriteHandle sprite, SpriteCollisionFunction func);
void         sprite_moveWithCollisions(SpriteHandle s, int dx, int dy);

#endif // RETRO_ENGINE_H
