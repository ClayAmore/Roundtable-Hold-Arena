#pragma once
#include <cstdint>
#include <cstring>
#include <sstream>
#include <nlohmann.json/json.hpp>
#include "Seamless.h"

using json = nlohmann::json;

#define MAX_MESSAGE_SIZE 2048
#define MAX_PLAYERS 127

#define LOBBY_DATA_KEY "roundTableHoldArena"

namespace P2P {

    enum class Mode : uint8_t {
        None = 0,
        TeamFight = 1,
        Brawl = 2,
    };
    enum class State : uint8_t {
        // NOTHING
        Nothing = 0,

        // TEAM FIGHT
        TeamFightInitiate = 1,
        TeamFightPickingTeam = 2,
        TeamFightReadyToStart = 3,
        TeamFightOngoing = 4,
        TeamFightEndMatch = 5,

        // BRAWL
        BrawlInitiate = 6,
        BrawlReadyToStart = 7,
        BrawlOngoing = 8,
        BrawlEndMatch = 9,
    };

    enum class Arena : uint8_t {
        None,
        Underground,
        Square,
        Portals,
        BushArena
    };

    enum P2PLobbyMsgType : char {
        UpdateKills = 1
    };

    struct P2PLobbyMsgHeader {
        P2PLobbyMsgType type;
        uint32_t sender;
    };

    struct P2PLobbyUpdateKillsMsg {
        P2PLobbyMsgHeader header;
        uint32_t killerId;
        Seamless::Team team;
    };

    struct P2PLobbyData {
        Mode mode = Mode::None;
        State state = State::Nothing;
        Arena arena = Arena::None;
        uint16_t blueTeamScore = 0;
        uint16_t redTeamScore = 0;
        uint16_t mostKills = 0;
        uint32_t matchDuration = 0;
        bool remainAfterMatch = 0;
        char greatestCombatant[32];

        std::string serialize() const {
            json j;
            j["mode"] = static_cast<uint8_t>(mode);
            j["state"] = static_cast<uint8_t>(state);
            j["arena"] = static_cast<uint8_t>(arena);
            j["blueTeamScore"] = blueTeamScore;
            j["redTeamScore"] = redTeamScore;
            j["mostKills"] = mostKills;
            j["matchDuration"] = matchDuration;
            j["remainAfterMatch"] = remainAfterMatch;
            j["greatestCombatant"] = greatestCombatant;
            return j.dump();
        }

        static P2PLobbyData deserialize(const std::string& str) {
            json j = json::parse(str);
            P2PLobbyData data;
            data.mode = static_cast<Mode>(j["mode"].get<uint8_t>());
            data.state = static_cast<State>(j["state"].get<uint8_t>());
            data.arena = static_cast<Arena>(j["arena"].get<uint8_t>());
            data.blueTeamScore = j["blueTeamScore"].get<uint16_t>();
            data.redTeamScore = j["redTeamScore"].get<uint16_t>();
            data.mostKills = j["mostKills"].get<uint16_t>();
            data.matchDuration = j["matchDuration"].get<uint32_t>();
            data.remainAfterMatch = j["remainAfterMatch"].get<bool>();
            std::string greatestCombatant = j["greatestCombatant"].get<std::string>();
            std::strncpy(data.greatestCombatant, greatestCombatant.c_str(), sizeof(data.greatestCombatant) - 1);
            data.greatestCombatant[sizeof(data.greatestCombatant) - 1] = '\0';
            return data;
        }
    };

    struct P2PLobbyMemberData {
        State state = State::Nothing;
        Seamless::Team team = Seamless::FREE_FOR_ALL;
        uint16_t kills = 0;
        uint16_t deaths = 0;
        char charName[32] = "";

        std::string serialize() const {
            json j;
            j["state"] = static_cast<uint8_t>(state);
            j["team"] = static_cast<uint8_t>(team);
            j["charName"] = charName;
            j["kills"] = kills;
            j["deaths"] = deaths;
            return j.dump();
        }

        static P2PLobbyMemberData deserialize(const std::string& str) {
            json j = json::parse(str);
            P2PLobbyMemberData data;
            data.state = static_cast<State>(j["state"].get<uint8_t>());
            data.team = static_cast<Seamless::Team>(j["team"].get<uint8_t>());
            data.kills = j["kills"].get<uint16_t>();
            data.deaths = j["deaths"].get<uint16_t>();
            std::string charName = j["charName"].get<std::string>();
            std::strncpy(data.charName, charName.c_str(), sizeof(data.charName) - 1);
            data.charName[sizeof(data.charName) - 1] = '\0';
            return data;
        }
    };
}