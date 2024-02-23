#pragma once
#include "Player.h"
#include "Death.h"
#include "Timer.h"
#include "SpEffect.h"
#include "Seamless.h"
#include "Settings.h"
#include "Menu.h"
#include "Warp.h"
#include "Camera.h"
#include "data/P2PLobbyData.h"

namespace P2P {
    class BrawlManager {
    public: 
        enum Result {
            VICTORY,
            DEFEAT,
            STALEMATE
        };
        void PrepareMap();
        void SetArena(Arena arena);
        bool StartMatch(uint32_t duration);
        void OnKillUpdate();
        bool EndMatch(Result);
        void CancelMatch();
    };
}   