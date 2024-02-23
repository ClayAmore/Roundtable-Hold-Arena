#pragma once
#include <filesystem>
#include "IRenderCallback.h"
#include "OverlayFramework.h"
#include "Global.h"
#include "P2PLobby.h"
#include "core/SteamInterfaceProvider.h"
#include "SpEffectVfxParam.h"
#include "EquipMtrlSetParam.h"
#include "HookSetup.h"
#include "Damage.h"
#include "Warp.h"
#include "bullet.h"
#include "Souls.h"
#include "Item.h"
#include "Player.h"
#include "SPEffect.h"
#include "Camera.h"
#include "VFTHook.h"
#include "Event.h"
#include "Menu.h"
#include "File.h"

using namespace OF;
using namespace P2P;
using namespace GameItems;

class RoundTableHoldArena : public IRenderCallback
{
public:
	void Setup();
	void Render();
	RoundTableHoldArena(char path[2048]) {
		std::string s(path);
		dllPath = s.substr(0, s.find_last_of("\\/"));
	}

	static Mode CurrentMode;
	static Arena CurrentArena;
	static bool InitiateArenaMatch;
	static bool InMatch;
	static uint32_t LastPlayerToHitMe;

	// For file mods
	static HANDLE FileWriteHook(
		LPCWSTR lpFileName, 
		DWORD dwDesiredAccess, 
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
		DWORD dwCreationDisposition, 
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile);
	static void* VirtualToArchivePath(File::DLString_EldenRing* path, UINT64 p2, UINT64 p3, UINT64 p4, UINT64 p5, UINT64 p6);

	static void AddSPEffectHook(uint8_t* playerIns, uint32_t speffectId, int param3);
	static void SetEventFlagHook(uint64_t eventMan, uint32_t* eventFlagId, bool state);
	static void ItemUseHook(Item::ItemUseStruct itemUseStruct);
	static void DamageHook(Damage::ChrDamageModule* damageModule, Damage::ChrIns* chrIns, Damage::DamageStruct* damageStruct, uint64_t param_4, uint8_t param_5);
	static void WarpHook(void* CSLuaEventProxy, void* CSLuaEventScriptImitation, uint32_t warpLocation);
	static bool warp_occured;
	static uint64_t mySteamId;

private:
	std::string dllPath;

	RECT rect;
	OF::Box* overlay = nullptr;
	OF::Box* timer = nullptr;
	OF::Box* timerBar = nullptr;
	OF::Box* timerBorder = nullptr;
	OF::Box* greatestCombatnat = nullptr;
	OF::Box* messageBox = nullptr;
	float scale = 0.0f;
	int windowWidth = 0;
	int windowHeight = 0;
	int brawlTexture = 0;
	int teamFightTexture = 0;
	int messageBoxTexture = 0;
	int timerTexture = 0;
	int timerBarTexture = 0;
	int font = 0;
	int textoffsetX = 0;
	int textoffsetY = 0;
	float pulse = 1.0;
	float time = 1.0;

	// ====== P2P ========
	SteamInterfaceProvider* _steam_interface_provider;
	P2PLobby* _p2pLobby;
	// ===================

	bool oneTimeSetup = false;

	void OneTimeSetup();
	void CheckForAutoRevive();
	void CheckForCamEnd();
	void CheckForArenaMatchInitiate();
	void CheckIfInBushRoom();
	void CheckSeamlessTeamAndMode();
	void HandleArenaMatchDeath(uint8_t* playerIns);
	void HandleFallCamera(uint8_t* playerIns);
	void HandleDuelDeath(uint8_t* playerIns);
	void CheckForMatchEnd();
	void SendScoreUpdateRequest();
	void RemoveRot(uint8_t* playerDeadNum);

	void DisplayScoreOverlay();
	void DisplayTimerOverlay();
	void DisplayGreatestCombatantOverlay();
	void CheckForWindowResize();


	int deathCount = 0;
	bool updateKillSent = false;
	bool is_in_team_fight = false;
	bool isInsideBushRoom = false;
	bool isJumping = false;
	bool needNoMaterialForWeaponUpgrades = false;
	Seamless::PVPMode pvp_mode = Seamless::FRIENDLY;
	RTTIScanner* scanner;

};