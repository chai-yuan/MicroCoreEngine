#include "GameEnginePriv.h"

unsigned char getKey() {
    if (g_platform_api.getKey)
        return g_platform_api.getKey();
    return 0;
}
