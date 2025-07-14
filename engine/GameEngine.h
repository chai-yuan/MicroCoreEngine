#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

// 游戏的逻辑分辨率
#define LOGICAL_WIDTH 128
#define LOGICAL_HEIGHT 128

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

typedef enum ImageType {
    IMG_Palette = 0,
    IMG_RGB565  = 1,
} ImageType;

typedef struct Timer {
    int time;
} Timer;

typedef struct TileMap {
    int            x, y;
    int            mapw, maph;
    unsigned char *map;
    unsigned char *collision;

    int       address;
    ImageType type;
    int       imagew, imageh;
} TileMap;

typedef struct Sprite {
    int x, y;
    int speedx, speedy;
    int accelerationx, accelerationy;

    unsigned char collision;
    int           left, right, top, bottom;

    int       address;
    ImageType type;
    int       imagew, imageh;
    int       framerate, frameidx, framelen;
} Sprite;

typedef struct Background {
    int x, y;

    int       address;
    ImageType type;
    int       imagew, imageh;
} Background;

// --- 游戏逻辑 ---
void game_main();
void gameTick();
void setFramerate(int fps);
// --- 屏幕操作 ---
void screenRoll(int x, int y);
// --- 图像操作 ---
void loadImage(const unsigned char *image, int address, int len);
void drawImage(int address, ImageType type, int x, int y, int w, int h);
// --- 调色板与颜色 ---
void loadPalette(int *palette);
// --- 输入操作 ---
unsigned char getKey();
// --- Sprite 操作 ---
void loadSprite(Sprite *sprite);
void removeSprite(Sprite *sprite);
// --- 背景操作 ---
void loadBackground(Background *background);
void removeBackground(Background *background);
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
