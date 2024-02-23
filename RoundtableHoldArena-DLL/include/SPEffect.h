#pragma once
#ifndef _SPEFFECT_
#define _SPEFFECT_

#include <cstdint>
#include "Global.h"
#include "Pointer.h"
#include "Player.h"

using namespace BS2;

#define SPEFFECT_SEAMLESS_SMOKE 700000402
#define SPEFFECT_DEATH_BLIGHT 70
#define SPEFFECT_NOATTACK 9621
#define SPEFFECT_SOAP 502120
#define SPEFFECT_GRACE_HEAL 100
#define SPEFFECT_LAW_OF_REGRESSION 1673000
#define SPEFFECT_SMALL_HEAL 1565
#define SPEFFECT_MID_HEAL 1636
#define SPEFFECT_BIG_HEAL 20

class SPeffect {
public:
    struct SpEffectListNode {
        void* specialEffectParamEntryPtr;
        uint32_t speffectId;
        uint8_t unk02[0x24];
        SpEffectListNode* next;
        SpEffectListNode* prev;
        float duration;
        float interval;
        float totalDuration;
        uint8_t unk04[0x3C];
    };
    typedef void AddSpeffectFunction(uint8_t* playerIns, uint32_t speffectId, int param3);
    typedef void RemoveSpeffectFunction(void* playerIns, uint32_t speffectId);
    static AddSpeffectFunction* AddSpEffectFunctionOriginal;

    static void AddSpeffect(uint32_t spEffectId, uint8_t playerIndex = 0);
    static void RemoveSpEffect(uint32_t spEffectId, uint8_t playerIndex = 0);
    static void RemoveSpEffect(std::initializer_list<uint32_t> speffects, uint8_t playerIndex = 0);
    static bool HasSpeffect(uint32_t speffectId, uint8_t playerIndex = 0);
    static void LawOfRegression();
};


inline void SPeffect::AddSpeffect(uint32_t spEffectId, uint8_t playerIndex) {
    uint8_t* playerIns = AccessDeepPtr<uint8_t>(Global::WORLD_CHR_MAN.ptr(), 0x1E508, 0);
    AddSpeffectFunction* addSpeffectFunction = (AddSpeffectFunction*)(Global::ADD_SPEFFECT_FUNCTION.ptr());
    if (playerIns == nullptr || addSpeffectFunction == nullptr) return;
    addSpeffectFunction(playerIns, spEffectId, 0);
}

inline void SPeffect::RemoveSpEffect(uint32_t spEffectId, uint8_t playerIndex) {
    uint8_t* playerSpecialEffects = AccessDeepPtr<uint8_t>(Global::WORLD_CHR_MAN.ptr(), 0x1E508, 0x178, 0);
    RemoveSpeffectFunction* removeSpeffectFunction = (RemoveSpeffectFunction*)(Global::REMOVE_SPEFFECT_FUNCTION.ptr());
    if (removeSpeffectFunction == nullptr) {
        return;
    }
    removeSpeffectFunction((void*)playerSpecialEffects, spEffectId);
}

inline void SPeffect::RemoveSpEffect(std::initializer_list<uint32_t> speffects, uint8_t playerIndex) {
    uint8_t* playerSpecialEffects = AccessDeepPtr<uint8_t>(Global::WORLD_CHR_MAN.ptr(), 0x1E508, 0x178, 0);
    RemoveSpeffectFunction* removeSpeffectFunction = (RemoveSpeffectFunction*)(Global::REMOVE_SPEFFECT_FUNCTION.ptr());
    if (removeSpeffectFunction == nullptr) return;
    for (uint32_t spEffectId : speffects) {
        removeSpeffectFunction(playerSpecialEffects, spEffectId);
    }
}

inline bool SPeffect::HasSpeffect(uint32_t spEffectId, uint8_t playerIndex) {
    // Get the player instance pointer
    uint8_t* playerIns = Player::GetPlayerIns<uint8_t>(playerIndex);

    if (playerIns == nullptr) return false;

    // Access the SpEffectListNode pointer from the player instance
    SpEffectListNode** node_ptr = AccessDeepPtr<SpEffectListNode*>(playerIns, 0x178, 8);

    if (node_ptr == nullptr) {
        // If the node pointer is null, return false
        return false;
    }

    // Get the first node in the linked list
    SpEffectListNode* node = *node_ptr;

    while (node != nullptr) {
        // Check if the current node's spEffectId matches the target spEffectId
        if (node->speffectId == spEffectId) {
            // If a match is found, return true
            return true;
        }

        // Move to the next node in the linked list
        node = node->next;
    }

    // If no match is found, return false
    return false;
}

inline void SPeffect::LawOfRegression() {
    for (int i = 0; i < 35; i++) SPeffect::AddSpeffect(SPEFFECT_LAW_OF_REGRESSION + i);
}

#endif