#pragma once
#include "Player.h"
#include "Seamless.h"
#include "Death.h"
#include "Timer.h"
#include "SpEffect.h"
#include "Settings.h"
#include "Warp.h"
#include "Menu.h"
#include "Camera.h"
#include "data/P2PLobbyData.h"

namespace P2P {
    class TeamFightManager {
    public:
        enum Result {
            VICTORY,
            DEFEAT,
            STALEMATE
        };
        bool CheckIfTeamPicked();
        void PickingTeam();
        void SetArena(Arena arena);
        bool StartMatch(uint32_t duration);
        void OnKillUpdate();
        void OnKillUpdate(Seamless::Team selfTeam, Seamless::Team victimTeam);
        bool EndMatch(Result result);
        void CancelMatch();
    };
}