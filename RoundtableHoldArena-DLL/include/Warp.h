#pragma once
#include <cstdint>
#include "Global.h"

class Warp {
public:
    enum Grace {
        RoundTableHold = 0x00A96AE6
    };
    static bool IsWarping;
    typedef void (_stdcall* WarpFunction)(void* CSLuaEventProxy, void* CSLuaEventScriptImitation, uint32_t warpLocation);
    static WarpFunction WarpFunctionOriginal;
    static void WarpTo(Grace grace);
};

inline void Warp::WarpTo(Grace grace) {
	uint8_t* CSLuaEventProxy = AccessDeepPtr<uint8_t>(Global::CS_LUA_EVENT_MANAGER.ptr(), 0x18);
	uint64_t* CSLuaEventScriptImitation = AccessDeepPtr<uint64_t>(Global::CS_LUA_EVENT_MANAGER.ptr(), 0x8);
	uint32_t warpLocation = grace - 0x3E8;

	if (CSLuaEventProxy == nullptr || CSLuaEventScriptImitation == nullptr) return;

	WarpFunction luaWarp = reinterpret_cast<WarpFunction>(Global::LUA_WARP.ptr());

	if (luaWarp != nullptr) {
		IsWarping = true;
		luaWarp((void*)CSLuaEventProxy, (void*)CSLuaEventProxy, warpLocation);
	}
}
