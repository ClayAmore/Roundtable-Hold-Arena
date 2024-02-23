#pragma once
#include "Pointer.h"
#include "SigScan.h"

using namespace BS2;

#define ON true
#define OFF false

#define DEATH_PLANE_Y -50.0f

#define CAMERA_DEFAULT_Y 1.42f

#define MAX_PLAYERS 6
#define RESET_POISE  1652
#define FREECAM_TOGGLE 9999993
#define NODEAD_TOGGLE_EFFECT 9999995
#define NODEAD_EFFECT 9999996
#define SEAMLESS_HOST 700000400

// SPeffects
#define SPEFFECT_SEAMLESS_TEAM_MODE 9999800
#define SPEFFECT_SEAMLESS_ENEMIES_MODE 9999801
#define SPEFFECT_SEAMLESS_WRONG_MODE 9999802
#define SPEFFECT_SEAMLESS_TEAM_BLUE 9999905
#define SPEFFECT_SEAMLESS_TEAM_RED 9999906
#define SPEFFECT_JUMPING_UP 145
#define SPEFFECT_JUMPING_DOWN 98
#define SPEFFECT_CROUCH 8001
#define SPEFFECT_RUPTURED_TEAR 511075
#define SPEFFECT_NODEAD 9999996
#define SPEFFECT_KABOOM 9999997

// Arena Fight
#define SPEFFECT_TEAMFIGHT 9999930
#define SPEFFECT_BRAWL 9999931
#define SPEFFECT_UNDERGROUND_ARENA 9999917
#define SPEFFECT_SQUARE_ARENA 9999918
#define SPEFFECT_PORTAL_ARENA 9999919
#define SPEFFECT_BUSH_ARENA 9999920
#define SPEFFECT_INITIATE_ARENA_FIGHT 9999900
#define SPEFFECT_SPAWN_TELEPORT 9999901
#define SPEFFECT_RANDOM_TELEPORT 9999902
#define SPEFFECT_NOTHING 9999903
#define SPEFFECT_ARENA_MATCH_END 9999904
#define SPEFFECT_TEAM_HEAL_BIG 9999908
#define SPEFFECT_TEAM_HEAL_SMALL 9999909
#define SPEFFECT_INSIDE_BUSH_ROOM 9999910
#define SPEFFECT_ARENA_MATCH_VICTORY 9999911
#define SPEFFECT_ARENA_MATCH_DEFEAT 9999912
#define SPEFFECT_ARENA_MATCH_STALEMATE 9999913
#define SPEFFECT_ARENA_RAT_POSION 4005
#define SPEFFECT_ARENA_RAT_ROT 4007

// Icons
#define ITEM_ICON_NO_DEAD_ON 3205
#define ITEM_ICON_NO_DEAD_OFF 3082

// Items
#define ITEM_RUIN_FRAGMENT 1760
#define ITEM_NO_DEAD 3320
#define ITEM_SOAP 2120
#define ITEM_SHADOW_MELD 2160
#define ITEM_TEAM_BLUE_DEATH 2130
#define ITEM_TEAM_RED_DEATH 1240
#define ITEM_DEVOURING_GAZE 8979

// Animations
#define ANIMATION_IDLE 0
#define ANIMATION_SPAWN 60501
#define ANIMATION_DEATH_FADE 60260
#define ANIMATION_FADEOUT 60260
#define ANIMATION_SHADOW_MELD 60060
#define ANIMATION_DUEL_RESPAWN 63090 //60602
#define ANIMATION_TEAM_FIGHT_DEATH 60455

// EventFlags
#define EVENT_FLAG_NEW_GAME_OPENED_CHAPEL_DOOR 10018540 


namespace Global
{
    const SigScannedPtr<uint8_t> SOLO_PARAM_REPOSITORY(Signature("48 8B 0D ?? ?? ?? ?? 48 85 C9 0F 84 ?? ?? ?? ?? 45 33 C0 BA 90", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> EVENT_MAN(Signature("48 83 3D ?? ?? ?? ?? 00 48 8B F9 0F 84 ?? ?? ?? ?? 48", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> WORLD_CHR_MAN(Signature("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 0F 48 39 88", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> FIELD_AREA(Signature("48 8B 3D ?? ?? ?? ?? 49 8B D8 48 8B F2 4C 8B F1 48 85 FF", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> GAME_DATA_MAN(Signature("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 05 48 8B 40 58 C3 C3", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> CS_LUA_EVENT_MANAGER(Signature("48 83 3D ?? ?? ?? ?? 00 48 8B F9 0F 84 ?? ?? ?? ?? 48", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> CS_PLAYLOG(Signature("48 8B 0D ?? ?? ?? ?? B2 02 E8 ?? ?? ?? ?? 48  8B CB E8 ?? ?? ?? ?? 44", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> SESSION_MAN(Signature("4C 8B 05 ?? ?? ?? ?? 48 8B D9 33 C9 0F 29 74 24 ?? 0F 29 7C 24 ?? B2 01", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> MAP_ITEM_MAN(Signature("48 8B 0D ?? ?? ?? ?? C7 44 24 50 FF FF FF FF C7 45 A0 FF FF FF FF 48 85 C9 75 2E", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> BULLET_MAN(Signature("48 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 44 24 ?? 48  89  44  24 ?? 48 89 7C 24 ?? C7 44 24 ?? ?? ?? ?? ?? 48", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> CS_MENU_MAN_IMP(Signature("48 8B 0D ?? ?? ?? ?? 83 79 ?? 00 0F 85 ?? ?? ?? ?? 49 8B 87 ?? ?? ?? ?? 48 8B 88 ?? ?? ?? ?? E8", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> ITEM_USE(Signature("40 53 48 83 EC 40 8B 51 18 48 8B D9 48 8D 4C 24 20 48 C7 44 24 20 00 00", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> RESTORE_FLASKS(Signature("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 48 8B 01 48 8B F9 32 DB", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> SET_EVENT_FLAG(Signature("?? ?? ?? ?? ?? 48 89 74 24 18 57 48 83 EC 30 48 8B DA 41 0F B6 F8 8B 12 48 8B F1 85 D2 0F 84 ?? ?? ?? ?? 45 84 C0", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> REMOVE_SPEFFECT_FUNCTION(Signature("48 83 EC 28 8B C2 48 8B 51 08 48 85 D2 ?? ?? 90", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> ADD_SPEFFECT_FUNCTION(Signature("48 8B C4 48 89 58 08 48 89 70 10 57 48 81 EC ?? ?? ?? ?? 0F 28 05 ?? ?? ?? ?? 48 8B F1 0F 28 0D ?? ?? ?? ?? 48 8D 48 88", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> ADD_SOULS_FUNCTION(Signature("44 8B 49 ?? 45 33 DB 44 89 5C 24", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> LUA_WARP(Signature("C3 ?? ?? ?? ?? ?? ?? 57 48 83 EC ?? 48 8B FA 44", 0x2, Signature::Type::Address));
    const SigScannedPtr<uint8_t> BULLET_SPAWN_FUNCTION(Signature("40 53 55 56 57 48 81 EC 98 07 00 00 48 C7 44 24 ?? ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> VIRTUAL_TO_ARCHIVE_PATH(Signature("E8 ?? ?? ?? ?? 48 83 7B 20 08 48 8D 4B 08 72 03 48 8B 09 4C 8B 4B 18 41 B8 05 00 00 00 4D 3B C8", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> HIT_FUNCTION(Signature("4C 8B DC 55 53 56 57 41 56 41 57 49 8D 6B 88 48 81 EC 48 01 00 00", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> DISPLAY_BANNER_FUNCTION(Signature("40 53 48 83 EC 30 48 8D 4C 24 20 C7 44 24 20 FF FF FF FF 8B DA 48 C7 44 24 28 00", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> ADD_ITEM_FUNCTION(Signature("40 56 57 41 56 48 83 ec 50 48 c7 44 24 30 fe ff ff ff 48 89 5c 24 70 48 89 6c 24 78", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> CAMREA_UPDATE_FUNCTION(Signature("4C 8B DC 55 53 41 56 41 57 49 8D AB 28 FF FF FF 48 81 EC B8", 0x217, Signature::Type::Address));

    // Differnet autorevive 
    const SigScannedPtr<uint8_t> DEATH_TRIGGER_EVENT_SETTER(Signature("83 49 24 01 48 ?? ?? ?? ?? ?? 03 48 85 ?? 75 ?? 48 ?? ?? ?? ?? ?? 03", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> FALL_CAMERA_SETTER(Signature("01 C0 E2 02 08 91 ?? 01 00 00 C3 49 89 06 48 8D", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> DEATH_DISABLE_MAP_HIT_SETTER(Signature("01 C3 83 A1 F0 00 00 00", 0, Signature::Type::Address));
}   


