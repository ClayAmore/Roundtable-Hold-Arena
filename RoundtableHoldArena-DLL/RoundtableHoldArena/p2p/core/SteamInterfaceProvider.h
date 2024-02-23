#pragma once
#include <windows.h>
#include "steam/steam_api.h"

namespace P2P {
    class SteamInterfaceProvider {
    public:
        void CreateInterfacesWhenReady();
        ISteamUser* GetUser();
        ISteamMatchmaking* GetSteamMatchMaking();

    private:
        bool _ready = false;
        bool isReadyForInterfaceCreation();
        void EnsureReady();

        ISteamUser* _user = nullptr;
        ISteamMatchmaking* _steamMatchMaking = nullptr;
    };
}
