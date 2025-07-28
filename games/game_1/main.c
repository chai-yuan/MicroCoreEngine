#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // for rand() and srand()
#include <string.h> // for memset()

// 包含你的引擎头文件
#include "GameEngine.h"

// --- 1. 游戏常量和定义 ---

// 游戏对象池大小
#define MAX_ZOMBIES 15
#define MAX_BULLETS 20
#define MAX_ITEMS 5

// 游戏速度
#define PLAYER_SPEED 2
#define BULLET_SPEED 5

// 碰撞掩码
typedef enum {
    COLLISION_GROUP_WALL   = (1 << 0),
    COLLISION_GROUP_PLAYER = (1 << 1),
    COLLISION_GROUP_ZOMBIE = (1 << 2),
    COLLISION_GROUP_BULLET = (1 << 3),
    COLLISION_GROUP_ITEM   = (1 << 4),
} CollisionGroup;

// 游戏状态
typedef enum { GAME_STATE_PLAYING, GAME_STATE_GAME_OVER } GameState;

// 道具类型
typedef enum { ITEM_TYPE_HEALTH, ITEM_TYPE_AMMO } ItemType;

// --- 2. 游戏全局变量 ---

// 游戏状态
static GameState g_game_state;

// 玩家
static SpriteHandle g_player;
static int          g_player_health;
static int          g_player_ammo;
static int          g_player_hunger; // 暂时作为一个示例，可以在loop中递减
static int          g_score;
static int          g_player_facing_x; // 记录玩家朝向用于发射子弹
static int          g_player_facing_y;

// 摄像机/世界坐标
static int g_camera_x;
static int g_camera_y;

// 对象池
static SpriteHandle g_zombies[MAX_ZOMBIES];
static int          g_zombies_health[MAX_ZOMBIES];

static SpriteHandle g_bullets[MAX_BULLETS];
static int          g_bullet_dx[MAX_BULLETS];
static int          g_bullet_dy[MAX_BULLETS];

static SpriteHandle g_items[MAX_ITEMS];
static ItemType     g_items_type[MAX_ITEMS];

// 资源句柄
static ImageTableHandle g_player_img_table;
static ImageTableHandle g_zombie_img_table;
static ImageHandle      g_bullet_img;
static ImageTableHandle g_items_img_table; // 0: health, 1: ammo
static TileMapHandle    g_tilemap;
static ImageTableHandle g_tileset;

// 计时器
static unsigned int g_zombie_spawn_timer;
static unsigned int g_item_spawn_timer;
static unsigned int g_shoot_cooldown_timer;

// --- 3. 函数前向声明 ---

// 精灵行为回调
static void player_collision_callback(SpriteHandle self, SpriteHandle other, CollisionInfo info);
static void zombie_update_callback(SpriteHandle self);
static void bullet_update_callback(SpriteHandle self);
static void bullet_collision_callback(SpriteHandle self, SpriteHandle other, CollisionInfo info);

// 游戏逻辑函数
static void setup_player(void);
static void setup_object_pools(void);
static void spawn_zombie(void);
static void spawn_bullet(int start_x, int start_y, int dx, int dy);
static void spawn_item(void);
static void handle_input(void);
static void update_camera(void);
static void draw_ui(void);
static void reset_game(void);

// 资源加载 (需要你自己填充资源数据)
static void load_assets(void);
static void create_map(void);

// --- 4. 游戏主入口 ---
void game_init(void) {
    system_setFPS(60);
    system_srand(system_getCurrentTime());

    load_assets();
    create_map();
    setup_player();
    setup_object_pools();

    reset_game();
}

void game_loop(void) {
    graphics_clear(0); // 清屏为黑色

    switch (g_game_state) {
    case GAME_STATE_PLAYING: {
        unsigned int currentTime = system_getCurrentTime();

        // --- 更新 ---
        handle_input();

        // 僵尸和道具生成
        if (currentTime > g_zombie_spawn_timer) {
            spawn_zombie();
            g_zombie_spawn_timer = currentTime + 3000; // 每3秒生成一个
        }
        if (currentTime > g_item_spawn_timer) {
            spawn_item();
            g_item_spawn_timer = currentTime + 10000; // 每10秒生成一个
        }

        // 更新摄像机位置
        update_camera();

        // --- 绘制 ---
        // 设置摄像机偏移
        graphics_setDrawOffset(-g_camera_x, -g_camera_y);

        // 绘制地图
        tilemap_drawAtPoint(g_tilemap, 0, 0);

        // 引擎负责更新和绘制所有已添加的精灵
        // 它会自动调用我们设置的 update 和 collision 回调
        sprite_updateAndDrawSprites();

        // 重置偏移，准备绘制UI
        graphics_setDrawOffset(0, 0);
        draw_ui();

        // 检查游戏结束条件
        if (g_player_health <= 0) {
            g_game_state = GAME_STATE_GAME_OVER;
        }
        break;
    }

    case GAME_STATE_GAME_OVER: {
        // 在这里可以绘制 "Game Over" 文本或图片
        // graphics_drawText("Game Over", ...);
        // graphics_drawText("Press A to Restart", ...);

        // 检查重启游戏的输入
        if (system_getButtonState() & buttonA) {
            reset_game();
        }
        break;
    }
    }

    graphics_display(); // 将后台缓冲区的内容显示到屏幕上
}

// --- 5. 游戏逻辑实现 ---

void reset_game(void) {
    // 重置玩家状态
    g_player_health   = 100;
    g_player_ammo     = 20;
    g_player_hunger   = 100;
    g_score           = 0;
    g_player_facing_x = 0;
    g_player_facing_y = 1; // 默认朝下

    // 将玩家移动到地图中心
    uint32_t map_w, map_h;
    tilemap_getPixelSize(g_tilemap, &map_w, &map_h);
    sprite_moveTo(g_player, map_w / 2, map_h / 2);

    // 隐藏所有池中对象
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (g_zombies[i])
            sprite_removeSprite(g_zombies[i]);
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (g_bullets[i])
            sprite_removeSprite(g_bullets[i]);
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (g_items[i])
            sprite_removeSprite(g_items[i]);
    }

    // 重置计时器
    unsigned int currentTime = system_getCurrentTime();
    g_zombie_spawn_timer     = currentTime + 3000;
    g_item_spawn_timer       = currentTime + 10000;
    g_shoot_cooldown_timer   = 0;

    g_game_state = GAME_STATE_PLAYING;
}

void handle_input(void) {
    unsigned int buttons = system_getButtonState();
    int          dx = 0, dy = 0;

    if (buttons & buttonUp)
        dy = -PLAYER_SPEED;
    if (buttons & buttonDown)
        dy = PLAYER_SPEED;
    if (buttons & buttonLeft)
        dx = -PLAYER_SPEED;
    if (buttons & buttonRight)
        dx = PLAYER_SPEED;

    // 更新玩家朝向（用于射击）
    if (dx != 0 || dy != 0) {
        // 为了简单，不处理对角线情况，只记录最后移动方向
        if (dx != 0) {
            g_player_facing_x = dx > 0 ? 1 : -1;
            g_player_facing_y = 0;
        }
        if (dy != 0) {
            g_player_facing_x = 0;
            g_player_facing_y = dy > 0 ? 1 : -1;
        }
    }

    // 播放行走动画 TODO
    if (dy > 0)
        sprite_setAnimation(g_player, 0, 3, 8);
    else if (dy < 0)
        sprite_setAnimation(g_player, 4, 7, 8);
    else if (dx < 0)
        sprite_setAnimation(g_player, 8, 11, 8);
    else if (dx > 0)
        sprite_setAnimation(g_player, 12, 15, 8);

    // 应用移动，`sprite_moveWithCollisions` 会处理与墙和其它精灵的碰撞
    sprite_moveWithCollisions(g_player, dx, dy);

    // 处理射击
    unsigned int currentTime = system_getCurrentTime();
    if ((buttons & buttonA) && (g_player_ammo > 0) && (currentTime > g_shoot_cooldown_timer)) {
        int px, py;
        sprite_getPosition(g_player, &px, &py);
        spawn_bullet(px, py, g_player_facing_x, g_player_facing_y);
        g_player_ammo--;
        g_shoot_cooldown_timer = currentTime + 250; // 250ms 冷却
    }
}

void update_camera(void) {
    int px, py, pw, ph;
    int screen_w = display_getWidth();
    int screen_h = display_getHeight();

    sprite_getPosition(g_player, &px, &py);
    sprite_getImageSize(sprite_getImage(g_player), &pw, &ph); // 假设有 sprite_getImage

    // 将摄像机中心对准玩家中心
    g_camera_x = px + pw / 2 - screen_w / 2;
    g_camera_y = py + ph / 2 - screen_h / 2;

    // 限制摄像机不出地图边界
    uint32_t map_w, map_h;
    tilemap_getPixelSize(g_tilemap, &map_w, &map_h);
    if (g_camera_x < 0)
        g_camera_x = 0;
    if (g_camera_y < 0)
        g_camera_y = 0;
    if (g_camera_x > map_w - screen_w)
        g_camera_x = map_w - screen_w;
    if (g_camera_y > map_h - screen_h)
        g_camera_y = map_h - screen_h;
}

void draw_ui(void) {
    int screen_w = display_getWidth();
    // 血条
    graphics_drawRect((Rect){10, 10, 102, 12}, 0x1111);
    graphics_drawRect((Rect){11, 11, g_player_health, 10}, g_player_health > 25 ? 0x11e0 : 0xf000);

    // 弹药 (用矩形代替文字)
    // graphics_drawText( ammo_str, ... );
    graphics_drawRect((Rect){10, 25, 20, 10}, 0xfaaa);

    // 分数 (用矩形代替文字)
    // graphics_drawText( score_str, ... );
    graphics_drawRect((Rect){screen_w - 50, 10, 40, 10}, 0x3322);
}

// --- 6. 对象生成和行为 ---

void spawn_zombie(void) {
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        // 查找一个未激活的僵尸
        if (sprite_getUpdateFunction(g_zombies[i]) == NULL) { // 假设可以用这个判断是否激活
            sprite_setUpdateFunction(g_zombies[i], zombie_update_callback);

            // 在屏幕外随机位置生成
            uint32_t map_w, map_h;
            tilemap_getPixelSize(g_tilemap, &map_w, &map_h);
            int spawn_x = rand() % map_w;
            int spawn_y = rand() % map_h;
            sprite_moveTo(g_zombies[i], spawn_x, spawn_y);

            g_zombies_health[i] = 50; // 僵尸血量

            sprite_addSprite(g_zombies[i]); // 添加到引擎进行更新和绘制
            return;                         // 只生成一个
        }
    }
}

void spawn_bullet(int start_x, int start_y, int dx, int dy) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (sprite_getUpdateFunction(g_bullets[i]) == NULL) {
            sprite_setUpdateFunction(g_bullets[i], bullet_update_callback);
            sprite_setCollisionResponseFunction(g_bullets[i], bullet_collision_callback);

            sprite_moveTo(g_bullets[i], start_x, start_y);
            g_bullet_dx[i] = dx * BULLET_SPEED;
            g_bullet_dy[i] = dy * BULLET_SPEED;

            sprite_addSprite(g_bullets[i]);
            return;
        }
    }
}

void spawn_item() {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (sprite_getUpdateFunction(g_items[i]) == NULL) {
            sprite_setUpdateFunction(g_items[i], (SpriteUpdateFunction)1); // 标记为激活，但没有更新逻辑

            uint32_t map_w, map_h;
            tilemap_getPixelSize(g_tilemap, &map_w, &map_h);
            int spawn_x = rand() % map_w;
            int spawn_y = rand() % map_h;
            sprite_moveTo(g_items[i], spawn_x, spawn_y);

            g_items_type[i] = (rand() % 2 == 0) ? ITEM_TYPE_HEALTH : ITEM_TYPE_AMMO;
            sprite_setImage(g_items[i], graphics_getTableImage(g_items_img_table, g_items_type[i]), imageUnflipped);

            sprite_addSprite(g_items[i]);
            return;
        }
    }
}

// --- 7. 回调函数 ---

void zombie_update_callback(SpriteHandle self) {
    int zx, zy, px, py;
    sprite_getPosition(self, &zx, &zy);
    sprite_getPosition(g_player, &px, &py);

    int dx = 0, dy = 0;
    if (px > zx)
        dx = 1;
    else if (px < zx)
        dx = -1;
    if (py > zy)
        dy = 1;
    else if (py < zy)
        dy = -1;

    sprite_moveWithCollisions(self, dx, dy);
}

void bullet_update_callback(SpriteHandle self) {
    int idx = -1;
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (g_bullets[i] == self) {
            idx = i;
            break;
        }
    }
    if (idx == -1)
        return;

    // 移动
    sprite_moveWithCollisions(self, g_bullet_dx[idx], g_bullet_dy[idx]);

    // 简单的生命周期检查 (基于位置)
    int x, y;
    sprite_getPosition(self, &x, &y);
    if (x < g_camera_x - 50 || x > g_camera_x + display_getWidth() + 50 || y < g_camera_y - 50 ||
        y > g_camera_y + display_getHeight() + 50) {

        // 移除子弹
        sprite_setUpdateFunction(self, NULL);
        sprite_removeSprite(self);
    }
}

void player_collision_callback(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    // 检查是否撞到僵尸
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (g_zombies[i] == other) {
            g_player_health -= 1; // 每次碰撞掉1血
            if (g_player_health < 0)
                g_player_health = 0;
            return;
        }
    }

    // 检查是否撞到道具
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (g_items[i] == other) {
            if (g_items_type[i] == ITEM_TYPE_HEALTH) {
                g_player_health += 25;
                if (g_player_health > 100)
                    g_player_health = 100;
            } else if (g_items_type[i] == ITEM_TYPE_AMMO) {
                g_player_ammo += 10;
            }
            // 移除道具
            sprite_setUpdateFunction(other, NULL);
            sprite_removeSprite(other);
            return;
        }
    }
}

void bullet_collision_callback(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    // 检查是否击中僵尸
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (g_zombies[i] == other) {
            g_zombies_health[i] -= 25; // 子弹伤害

            if (g_zombies_health[i] <= 0) {
                // 僵尸死亡
                sprite_setUpdateFunction(other, NULL);
                sprite_removeSprite(other);
                g_score += 10;
            }

            // 移除子弹
            sprite_setUpdateFunction(self, NULL);
            sprite_removeSprite(self);
            return;
        }
    }
}

// --- 8. 初始化辅助函数 ---

void setup_player() {
    g_player = sprite_newSprite();
    sprite_setImageTable(g_player, g_player_img_table);
    sprite_setCollideRect(g_player, (Rect){2, 2, 12, 12}); // 假设玩家图片是16x16
    sprite_setCollisionResponseFunction(g_player, player_collision_callback);

    // 玩家会和僵尸、道具碰撞
    sprite_setCollideMask(g_player, COLLISION_GROUP_ZOMBIE | COLLISION_GROUP_ITEM);
    sprite_setZIndex(g_player, 10); // 确保玩家在较高层
    sprite_addSprite(g_player);
}

void setup_object_pools() {
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        g_zombies[i] = sprite_newSprite();
        sprite_setImageTable(g_zombies[i], g_zombie_img_table);
        sprite_setAnimation(g_zombies[i], 0, 3, 10); // 播放僵尸走路动画
        sprite_setCollideRect(g_zombies[i], (Rect){2, 2, 12, 12});
        // 僵尸会和玩家、子弹碰撞
        sprite_setCollideMask(g_zombies[i], COLLISION_GROUP_PLAYER | COLLISION_GROUP_BULLET);
        sprite_setZIndex(g_zombies[i], 5);
        // 先不添加到引擎，等生成时再添加
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        g_bullets[i] = sprite_newSprite();
        sprite_setImage(g_bullets[i], g_bullet_img, imageUnflipped);
        sprite_setCollideRect(g_bullets[i], (Rect){0, 0, 4, 4}); // 假设子弹4x4
        // 子弹只和僵尸碰撞
        sprite_setCollideMask(g_bullets[i], COLLISION_GROUP_ZOMBIE);
        sprite_setZIndex(g_bullets[i], 20); // 子弹在最上层
    }

    for (int i = 0; i < MAX_ITEMS; i++) {
        g_items[i] = sprite_newSprite();
        // 图片在生成时设置
        sprite_setCollideRect(g_items[i], (Rect){0, 0, 16, 16});
        // 道具只和玩家碰撞
        sprite_setCollideMask(g_items[i], COLLISION_GROUP_PLAYER);
        sprite_setZIndex(g_items[i], 1); // 道具在最下层
    }
}

void load_assets(void) {
    // 这里你需要加载你的实际图片数据
    // const uint8_t player_data[] = { ... };
    // g_player_img_table = graphics_loadImageTable(16, 16, 16, PIXEL_FORMAT, player_data);

    // 占位符: 创建空白图像
    g_player_img_table = graphics_newImageTable(16, 16, 16, pixelFormatPaletteRLE);
    g_zombie_img_table = graphics_newImageTable(4, 16, 16, pixelFormatPaletteRLE);
    g_bullet_img       = graphics_newImage(4, 4, pixelFormatPaletteRLE);
    g_items_img_table  = graphics_newImageTable(2, 16, 16, pixelFormatPaletteRLE);
    g_tileset          = graphics_newImageTable(10, 16, 16, pixelFormatPaletteRLE);
}

void create_map(void) {
    g_tilemap = tilemap_newTilemap();
    tilemap_setImageTable(g_tilemap, g_tileset);

    int map_width  = 50; // 50个瓦片宽
    int map_height = 40; // 40个瓦片高
    tilemap_setSize(g_tilemap, map_width, map_height);

    // 创建一个简单的地图数据: 0为空地，1为墙
    uint8_t *map_data = system_malloc(map_width * map_height);
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            if (x == 0 || y == 0 || x == map_width - 1 || y == map_height - 1) {
                map_data[y * map_width + x] = 1; // 边界为墙
            } else {
                map_data[y * map_width + x] = 0; // 中间为空地
            }
        }
    }
    tilemap_setTiles(g_tilemap, map_data, map_width, map_height);
    system_free(map_data);

    // 将地图添加到渲染列表
    tilemap_addTilemap(g_tilemap);
}
