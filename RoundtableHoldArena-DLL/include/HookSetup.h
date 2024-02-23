#pragma once
#include <stdio.h>
#include "minhook/include/MinHook.h"

class HookSetup {
public:
	static bool Init();
	static bool CreateHook(void* function_ptr, void* detour_function, void** detour_function_original);
	static bool EnableHooks();
};

inline bool HookSetup::Init() {
    MH_STATUS initialize_status = MH_Initialize();

    if (initialize_status != MH_OK) {
        //printf("Hit hook MH_Initialize() failed\n");
        return false;
    }

    return true;
}

inline bool HookSetup::CreateHook(void* function_ptr, void* detour_function, void** detour_function_original) {
    MH_STATUS create_hook_status = MH_CreateHook(function_ptr, detour_function, detour_function_original);

    if (create_hook_status != MH_OK) {
        //printf("create_hook_status failed\n");
        return false;
    }

    return true;
}

inline bool HookSetup::EnableHooks() {
    MH_STATUS enable_hooks_status = MH_EnableHook(MH_ALL_HOOKS);

    if (enable_hooks_status != MH_OK) {
        //printf("enable_hooks_status failed\n");
        return false;
    }

    return true;
}