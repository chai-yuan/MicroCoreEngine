#include "GameEngine.h"

/**
 * @brief 分配指定大小的内存。
 * @param size 要分配的字节数。
 * @return 指向已分配内存的指针，如果失败则返回 NULL。
 */
void* memAlloc(int size);

/**
 * @brief 释放之前分配的内存。
 * @param ptr 指向要释放的内存的指针。
 */
void memFree(void* ptr);

/**
 * @brief 拷贝内存区域。
 * @param dest 目标内存地址。
 * @param src 源内存地址。
 * @param size 要拷贝的字节数。
 */
void memCopy(void* dest, const void* src, int size);
