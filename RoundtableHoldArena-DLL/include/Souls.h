#pragma once
#include <cstdint>
#include "Global.h"

class Souls {
public:
    typedef void(__stdcall* AddSoulsFunction)(void* unk, uint32_t souls);
    static void AddSouls(uint32_t quantity);
};

inline void Souls::AddSouls(uint32_t quantity) {
    uint8_t* gameDataManPtr = AccessDeepPtr<uint8_t>(Global::GAME_DATA_MAN.ptr(), 0x8, 0);
    AddSoulsFunction addSoulsFunction = reinterpret_cast<AddSoulsFunction>(Global::ADD_SOULS_FUNCTION.ptr());

    if (gameDataManPtr == nullptr || addSoulsFunction == nullptr) {
        printf("gameDataManPtr or addSoulsFunction were nullptr\n");
        return;
    }

    addSoulsFunction((void*)gameDataManPtr, quantity);
}