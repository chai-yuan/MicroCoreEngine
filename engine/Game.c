#include "GameEnginePriv.h"

void tilemap_draw(TileMapHandle m) {
    int drawx, drawy = m->y;

    for (int r = 0; r < m->tilesh; r++) {
        drawx = m->x;
        for (int c = 0; c < m->tilesw; c++) {
            int idx = m->tiles[c + r * m->tilesw];

            if (idx) {
                graphics_drawImage(m->image, drawx, drawy, idx - 1, imageUnflipped);
            }

            drawx += m->image->w;
        }
        drawy += m->image->h;
    }
}

void sprite_draw(SpriteHandle s) {
    if (s->image != NULL) {
        graphics_drawImage(s->image, s->x, s->y, s->idx, s->flip);
    }

    if ((s->div != 0) && (g_ticks % s->div == 0)) {
        s->idx++;
        if (s->idx > s->end || s->idx <= s->begin)
            s->idx = s->begin;
    }
}

void game_tick(void) {
    game_loop();

    for (int i = 0; i < TILEMAP_NUM; i++) {
        if (g_tilemaps[i] != NULL)
            tilemap_draw(g_tilemaps[i]);
    }

    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i] && g_sprites[i]->updatefunc)
            g_sprites[i]->updatefunc(g_sprites[i]);

    for (int i = 0; i < SPRITE_NUM; i++) {
        if (g_sprites[i] != NULL)
            sprite_draw(g_sprites[i]);
    }

#ifdef ENABLE_DEBUG_COL
    for (int i = 0; i < SPRITE_NUM; i++)
        if (g_sprites[i]) {
            Rect r = {g_sprites[i]->x, g_sprites[i]->y, g_sprites[i]->collisionRect.w, g_sprites[i]->collisionRect.h};
            graphics_drawRect(r, (Color){100, 255, 0, 255});
        }

#endif
    g_ticks++;
    platform.gfx_present();
}
