#include "PlatformPorting.h"
#include "GameEnginePriv.h"

platform_api_t platform;

void platform_register_api(const platform_api_t api) {
    platform = api;

    game_init();
}
