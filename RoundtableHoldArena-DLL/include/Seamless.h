#pragma once
#include <cstdint>
#include "Global.h"
#include "Pointer.h"
#include "Item.h"

using namespace BS2;
using namespace GameItems;

#define SEAMLESS_MODULE "elden_ring_seamless_coop.dll"

class Seamless {
public:
    enum PVPMode : uint8_t {
        FRIENDLY = 0,
        ENEMIES = 1,
        TEAM = 2,
        NEUTRAL = 3,
    };
    enum Team : uint8_t {
        FREE_FOR_ALL = 0,
        BLUE = 1,
        RED = 2
    };

    enum SeamlessItemRef {
        TINY_GREAT_POT_REF = -2,
        EFFIGY_OF_MALENIA_REF = -3,
        SEPERATION_MIST_REF = -5,
        JUDICATORS_RULEBOOK_REF = -6,
        RUNE_DECANTER_REF = -7
    };

    enum SeamlessItemID {
        TINY_GREAT_POT_ID = 0,
        EFFIGY_OF_MALENIA_ID = 10,
        SEPERATION_MIST_ID = 11,
        JUDICATORS_RULEBOOK_ID = 20,
        RUNE_DECANTER_ID = 21
    };

    struct SeamlessInfo {
        PVPMode mode;
        Team team;
    };

    struct StructForForcingSeamlessItemUse {
        void* ChrIns;
        int unk01 = -1;
        int unk02 = -1;
        int unk03 = 11;
        int8_t unk04 = 2;
        int8_t unk05 = -1;
        int8_t unk06 = -1;
        int8_t unk07 = -1;
        int refId = -5;
        float unk08 = 1.0f;
    };
    typedef void ForceSeamlessItemUseFunction(StructForForcingSeamlessItemUse* inRcx);
    static Seamless::SeamlessInfo GetSeamlessInfo();
    static void ForceUseSeamlessItem(SeamlessItemRef seamlessItem);
    static bool IsRunningSeamless();
};

inline bool Seamless::IsRunningSeamless() {
    uintptr_t seamlessBase = (uintptr_t)GetModuleHandle(SEAMLESS_MODULE);
    return seamlessBase != 0;
}


inline Seamless::SeamlessInfo Seamless::GetSeamlessInfo() {
    uintptr_t seamlessBase = (uintptr_t)GetModuleHandle(SEAMLESS_MODULE);
    uint8_t* mode = AccessDeepPtr<uint8_t>(seamlessBase + 0x2067A8, 0x8, 0x3C);
    uint8_t* team = AccessDeepPtr<uint8_t>(seamlessBase + 0x2067A8, 0x8, 0x40);
    Seamless::PVPMode pvpModeEnumValue = static_cast<PVPMode>(*mode);
    Seamless::Team teamEnumValue = static_cast<Team>(*team);
    if (mode == nullptr || team == nullptr) {
        printf("RoundtableHoldArena::Something went wrong. Couldn't get seamlessInfo!\n");
        return { FRIENDLY, FREE_FOR_ALL };
    }
    return { pvpModeEnumValue, teamEnumValue };
}

inline void Seamless::ForceUseSeamlessItem(SeamlessItemRef seamlessItem) {
    uint8_t* chr_ins = AccessDeepPtr<uint8_t>(Global::WORLD_CHR_MAN.ptr(), 0x1E508, 0);
    uint8_t* seamlesItemUseHookPtr = AccessDeepPtr<uint8_t>(Global::ITEM_USE.ptr());
    ForceSeamlessItemUseFunction* seamless_item_use_f = (ForceSeamlessItemUseFunction*)seamlesItemUseHookPtr;
    StructForForcingSeamlessItemUse itemUseStruct{};
    itemUseStruct.ChrIns = chr_ins;
    itemUseStruct.refId = seamlessItem;
    seamless_item_use_f(&itemUseStruct);
}
