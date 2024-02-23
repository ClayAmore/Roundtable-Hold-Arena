#pragma once
#include <functional>
#include <typeinfo>
#include <codecvt>
#include <queue>
#include "steam/steam_api.h"
#include "data/P2PLobbyData.h"
#include "game/BrawlManager.h"
#include "game/TeamFightManager.h"
#include "Settings.h"

#define LOBBY_DATA_KEY "RoundTableHoldArena"

namespace P2P {

    class P2PLobby {
    public:
        P2PLobby(
            ISteamUser* steamUser,
            ISteamMatchmaking* steamMatchMaking
        );

        CCallResult< P2PLobby, LobbyMatchList_t > m_CallResultLobbyMatchList;
        P2PLobbyData LocalLobbyData;
        P2PLobbyMemberData MyLobbyMemberData;

        // Steam User
        bool IsHost();
        uint64_t GetSelfSteamId();

        // Callback for disconenct
        void Cancel();
        void Reset();

        // Lobby control
        void OpenLobby();
        void CloseLobby();
        void CreateALobby();
        void JoinLobby(uint64_t lobbyId);

        // Lobby requests
        void RequestLobbyList();
        bool RequestLobbyData(CSteamID lobbyId);

        // Getters for local varibales
        uint32_t GetMyChrSetSyncId();

        // Lobby Data
        bool SyncLobbyData();
        void UpdateLobbyData();

        // Chat
        void HandleMessageQueue();
        void BroadcastMessageToLobbyMembers(const char* data, int size);

        // Lobby Member Functions
        void UpdateLobbyMemberData();
    private:
        // Callbacks
        STEAM_CALLBACK(P2PLobby, onLobbyEntered, LobbyEnter_t);
        STEAM_CALLBACK(P2PLobby, onLobbyCreated, LobbyCreated_t);
        STEAM_CALLBACK(P2PLobby, onLobbyDataUpdated, LobbyDataUpdate_t);
        STEAM_CALLBACK(P2PLobby, onLobbyChatMsg, LobbyChatMsg_t);
        void onLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);

        // Steam Interfaces
        ISteamUser* _steamUser;
        ISteamMatchmaking* _steamMatchMaking;

        // Local variables
        CSteamID _lobbyId;
        uint32_t _netChrsetSyncId;
        std::queue<std::vector<char>> messageQueue;

        // LobbyMemberData getter
        void readLobbyMemberData(CSteamID lobbyMemberID, std::function<void(const P2PLobbyMemberData&)> callback);
        void iterateLobbyMembers(std::function<void(const P2PLobbyMemberData&)> callback);

        // Managers
        BrawlManager* _brawlManager;
        TeamFightManager* _teamFightManager;

        // Helper functions
        bool isValidP2PLobbyData(const  P2PLobbyData& lobbyData);
        bool isValidP2PLobbyMemberData(const P2PLobbyMemberData& memberData);
        bool CheckIfEveryoneHasState(State state);
        void resetLocalData();
        void recalculateAndUpdateTeamFightScore();
        void recalculateAndUpdateMostKills(uint64_t memberSteamId);
        
        // LobbyData callback handlers
        void handleLobbyDataUpdate(const char* data);

        // TeamFight LobbyData handle
        void handleTeamFightLobbyDataUpdate();
        void handleTeamFightInitiate();
        void handleTeamFightOngoing();
        void handleTeamFightEndMatch();

        // Brawl LobbyData handle
        void handleBrawlLobbyDataUpdate();
        void handleBrawlInitiate();
        void handleBrawlOngoing();
        void handleBrawlEndMatch();

        // LobbyMemberData callback handlers
        void handleLobbyMemberDataUpdate(uint64_t memberId, const char* data);
        void handleTeamFightLobbyMemberDataUpdate(uint64_t memberId, P2PLobbyMemberData* lobbyData);
        void handleBrawlLobbyDataMemberUpdate(uint64_t memberId, P2PLobbyMemberData* lobbyData);
    };
}