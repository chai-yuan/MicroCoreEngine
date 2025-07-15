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

typedef struct Image {
    int       address;
    ImageType type;
    int       w, h, idx, count;
} Image;

typedef struct TileMap {
    int            x, y;
    int            mapw, maph;
    unsigned char *map;
    unsigned char *collision;

    Image *image;
} TileMap;

typedef struct Sprite {
    int x, y;
    int speedx, speedy;
    int accelerationx, accelerationy;

    unsigned char collision;
    int           left, right, top, bottom;

    Image *image;
    int   framerate;
} Sprite;

typedef struct Background {
    int x, y;

    Image *image;
} Background;

// --- 游戏逻辑 ---
void game_main();
void gameTick();
void setFramerate(int fps);
// --- 屏幕操作 ---
void screenRoll(int x, int y);
// --- 图像操作 ---
Image loadImage(const unsigned char *image, ImageType type, int imagew, int imageh, int count);
void  drawImage(Image *image, int x, int y);
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
// --- 数学函数 ---
// --- 内存管理 ---

#endif // RETRO_ENGINE_H
