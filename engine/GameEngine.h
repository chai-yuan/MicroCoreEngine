#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

// 游戏的逻辑分辨率
#define LOGICAL_WIDTH 128
#define LOGICAL_HEIGHT 128

/**
 * @brief 定义了键盘按键代码。
 * @note 这些值是位掩码，可以组合使用。
 */
typedef enum KeyCode {
    KEY_UP     = 1,  // 上
    KEY_DOWN   = 2,  // 下
    KEY_LEFT   = 4,  // 左
    KEY_RIGHT  = 8,  // 右
    KEY_A      = 16, // A 键
    KEY_B      = 32, // B 键
    KEY_SELECT = 64, // Select 键
    KEY_START  = 128 // Start 键
} KeyCode;

typedef struct Timer {
    int time;
} Timer;

typedef struct TileMap {
    int            x, y;
    int            address;
    int            imagew, imageh;
    int            mapw, maph;
    unsigned char *map;
    unsigned char *collision;
} TileMap;

typedef struct Sprite {
    int x, y;
    int speedx, speedy;
    int accelerationx, accelerationy;

    unsigned char collision;
    int           left, right, top, bottom;

    int address;
    int imagew, imageh;
    int imageidx;
} Sprite;

// --- 游戏逻辑 ---
void game_main();
void gameTick();
void setFramerate(int fps);
// --- 屏幕操作 ---
void screenRoll(int x, int y);
// --- 图像操作 ---
void loadImage(unsigned char *image, int address, int len);
void drawImage(int address, int x, int y, int w, int h);
// --- 调色板与颜色 ---
void loadPalette(int *palette);
// --- 输入操作 ---
unsigned char getKey();
// --- Sprite 操作 ---
void loadSprite(Sprite *sprite);
void removeSprite(Sprite *sprite);
// --- 几何图形绘制 ---
// --- 字符串与文本 ---
// --- 瓦片地图 ---
void loadTileMap(TileMap *tileMap);
void removeTileMap(TileMap *tileMap);
// --- 计时器 ---
void loadTimer(Timer *timer);
void removeTimer(Timer *timer);
// --- 数学函数 ---
// --- 内存管理 ---

#endif // RETRO_ENGINE_H
