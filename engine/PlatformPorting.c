#include "PlatformPorting.h"

platform_api_t g_platform_api;

void platform_register_api(const platform_api_t api){
    g_platform_api = api;
}
