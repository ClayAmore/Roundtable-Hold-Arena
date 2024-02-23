#include "SteamInterfaceProvider.h"

namespace P2P {
    void SteamInterfaceProvider::CreateInterfacesWhenReady() {

        while (!SteamAPI_Init()) {
            Sleep(1);
        }

        _user = SteamUser();
        _steamMatchMaking = SteamMatchmaking();
        _ready = true;
    }

    bool SteamInterfaceProvider::isReadyForInterfaceCreation() {
        return SteamAPI_GetHSteamUser() && SteamAPI_GetHSteamPipe();
    }

    void SteamInterfaceProvider::EnsureReady() {
        if (!_ready) {
            printf("SteamInterfaceProvider is not ready\n");
        }
    }

    ISteamUser* SteamInterfaceProvider::GetUser() {
        EnsureReady();
        return _user;
    }

    ISteamMatchmaking* SteamInterfaceProvider::GetSteamMatchMaking() {
        EnsureReady();
        return _steamMatchMaking;
    }
}
