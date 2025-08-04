#include "GameEnginePriv.h"

FontHandle font_newFont(ImageHandle image, int char_begin) {
    FontHandle ret  = system_malloc(sizeof(Font));
    ret->image      = image;
    ret->char_begin = char_begin;
    return ret;
}

void font_free(FontHandle font);

void font_drawText(FontHandle font, const char *text, int x, int y) {
    for (int i = 0; text[i]; i++) {
        graphics_drawImage(font->image, x + i * font->image->w, y, text[i] - font->char_begin, 0);
    }
}

void font_measureText(FontHandle font, const char *text, int *width, int *height);
