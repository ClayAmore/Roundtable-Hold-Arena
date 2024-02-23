#include "RoundTableHoldArena.h"


#define HOOK_RUNTIME_EXCEPTION throw std::runtime_error("Something is wrong. Couldn't hook one of the functions needed for the mod!");

uint32_t RoundTableHoldArena::LastPlayerToHitMe = 0;
bool RoundTableHoldArena::warp_occured = false;
uint64_t RoundTableHoldArena::mySteamId = 0;
int Bullet::BulletHandle = 0;
uint8_t Bullet::BulletManOutput[0x300];
Bullet::BulletManAob Bullet::bulletManAob;
bool Warp::IsWarping = false;
bool Camera::IsFreeCamOn = false;
Mode RoundTableHoldArena::CurrentMode = Mode::None;
Arena RoundTableHoldArena::CurrentArena = Arena::None;
bool RoundTableHoldArena::InitiateArenaMatch = false;
bool RoundTableHoldArena::InMatch = false;
Settings::ModPreferences Settings::modPreferences;
std::unordered_map<std::string, std::pair<TimePoint, double>> Timer::Timers;
bool Death::IsAutoReviveOn = false;
std::vector<bool> Item::OriginalNoConsumeValues;
bool Item::ItemsBanned = false;
std::vector<std::wstring> File::ModFiles;

Damage::DamageFunction* Damage::DamageFunctionOriginal = nullptr;
void RoundTableHoldArena::DamageHook(Damage::ChrDamageModule* damageModule, Damage::ChrIns* chrIns, Damage::DamageStruct* damageStruct, uint64_t param_4, uint8_t param_5) {
	if(!InMatch) return Damage::DamageFunctionOriginal(damageModule, chrIns, damageStruct, param_4, param_5);

	uint32_t netChrSyndId = Player::GetPlayerNetChrSyncID();
	Player::PlayerIns* hitterPlayerIns = (Player::PlayerIns*)chrIns;
	Player::PlayerIns* victimPlayerIns = (Player::PlayerIns*)damageModule->playerIns;

	// Validate that no pointer is a nullptr
	if (hitterPlayerIns == nullptr || victimPlayerIns == nullptr) return;


	// Validate that the playerHandle is of a player and not and npc or trap.
	bool isValidPlayerId = (hitterPlayerIns->net_chrset_sync_id % 0x2D000 == hitterPlayerIns->net_chrset_sync_id - 0x2D000);
	bool isMeGettingHit = (netChrSyndId == victimPlayerIns->net_chrset_sync_id);

	// Check if hitter is square rat
	if (!isValidPlayerId) {
		bool isRat = (hitterPlayerIns->net_chrset_sync_id - (hitterPlayerIns->net_chrset_sync_id % 1000) == 8000);
		if (isRat) {
			SPeffect::AddSpeffect(SPEFFECT_ARENA_RAT_POSION);
			SPeffect::AddSpeffect(SPEFFECT_ARENA_RAT_ROT);
			return;
		}
	}

	// Update last person to hit me
	if (isMeGettingHit && isValidPlayerId) {
		LastPlayerToHitMe = hitterPlayerIns->net_chrset_sync_id;
	}

	Damage::DamageFunctionOriginal(damageModule, chrIns, damageStruct, param_4, param_5);
}

Warp::WarpFunction Warp::WarpFunctionOriginal = nullptr;
void RoundTableHoldArena::WarpHook(void* CSLuaEventProxy, void* CSLuaEventScriptImitation, uint32_t warpLocation) {
	Warp::WarpFunctionOriginal(CSLuaEventProxy, CSLuaEventScriptImitation, warpLocation);
}

Item::ItemUseFunction* Item::ItemUseFunctionOriginal = nullptr;
void RoundTableHoldArena::ItemUseHook(Item::ItemUseStruct itemUseStruct) {

	if (itemUseStruct.itemId == ITEM_NO_DEAD) {
		if (itemUseStruct.ChrIns->net_chrset_sync_id == Player::GetPlayerNetChrSyncID()) {
			Death::ToggleAutoRevive();
			Item::ToggleItemsNoConsume(Death::IsAutoReviveOn);
		}
		return;
	}
	else if (itemUseStruct.itemId == ITEM_SOAP) {
		if (itemUseStruct.ChrIns->net_chrset_sync_id == Player::GetPlayerNetChrSyncID()) {
			Item::RestoreFlasks();
			Player::ResetPoise();
			SPeffect::LawOfRegression();
			SPeffect::AddSpeffect(SPEFFECT_GRACE_HEAL);
			SPeffect::RemoveSpEffect(SPEFFECT_DEATH_BLIGHT);
		}
		return;
	}
	else if (itemUseStruct.itemId == ITEM_SHADOW_MELD) {
		if (itemUseStruct.ChrIns->net_chrset_sync_id == Player::GetPlayerNetChrSyncID()) 
			Player::ForceAnimationPlayback(ANIMATION_SHADOW_MELD);
		return;
	}
	else if (itemUseStruct.itemId == ITEM_DEVOURING_GAZE) {
		if (itemUseStruct.ChrIns->net_chrset_sync_id == Player::GetPlayerNetChrSyncID())
			Camera::ToggleFreeCam();
		return;
	}

	Item::ItemUseFunctionOriginal(itemUseStruct);
}

Event::SetEventFlagFunction* Event::SetEventFlagOriginal = nullptr;
void RoundTableHoldArena::SetEventFlagHook(uint64_t eventMan, uint32_t* eventFlagId, bool state) {
	// For new game. Warps player to Roundtable when they open Chapel door
	if (*eventFlagId == EVENT_FLAG_NEW_GAME_OPENED_CHAPEL_DOOR && state) {
		Warp::WarpTo(Warp::RoundTableHold);
	}
	Event::SetEventFlagOriginal(eventMan, eventFlagId, state);
}

SPeffect::AddSpeffectFunction* SPeffect::AddSpEffectFunctionOriginal = nullptr;
void RoundTableHoldArena::AddSPEffectHook(uint8_t* playerIns, uint32_t speffectId, int param3) {
	if (speffectId == SPEFFECT_NOATTACK) return;
	else if (speffectId == SPEFFECT_TEAMFIGHT) {
		CurrentMode = Mode::TeamFight;
	}
	else if (speffectId == SPEFFECT_BRAWL) {
		CurrentMode = Mode::Brawl;
	}
	else if(speffectId == SPEFFECT_UNDERGROUND_ARENA) {
		CurrentArena = Arena::Underground;
	}
	else if(speffectId == SPEFFECT_SQUARE_ARENA) {
		CurrentArena = Arena::Square;
	}
	else if(speffectId == SPEFFECT_PORTAL_ARENA) {
		CurrentArena = Arena::Portals;
	}
	else if (speffectId == SPEFFECT_BUSH_ARENA) {
		CurrentArena = Arena::BushArena;
	}
	else if (speffectId == SPEFFECT_INITIATE_ARENA_FIGHT) {
		InitiateArenaMatch = true;
	}
	SPeffect::AddSpEffectFunctionOriginal(playerIns, speffectId, param3);
}


File::tCreateFileW File::tCreateFileWOriginal = nullptr;
HANDLE RoundTableHoldArena::FileWriteHook(
	LPCWSTR lpFileName, 
	DWORD dwDesiredAccess, 
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
	DWORD dwCreationDisposition, 
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile) {

	if (lpFileName != nullptr) {
		std::wstring filePath = std::wstring(lpFileName);
		File::ReplaceFilePath(filePath, File::ModFiles);
		lpFileName = filePath.c_str();
	}

	return File::tCreateFileWOriginal(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

File::VirtualToArchivePathFunction* File::VirtualToArchivePathOriginal = nullptr;
void* RoundTableHoldArena::VirtualToArchivePath(File::DLString_EldenRing* path, UINT64 p2, UINT64 p3, UINT64 p4, UINT64 p5, UINT64 p6) {
	auto res = File::VirtualToArchivePathOriginal(path, p2, p3, p4, p5, p6);

	if (path != nullptr) {
		if (File::FilePathContainsNames(path->str(), File::ModFiles)) {
			File::ProcessArchivePath(path->str(), path->length);
		}
	}

	return res;
}


void RoundTableHoldArena::Setup()
{	
	// Seamless Check ============
	if (!Seamless::IsRunningSeamless()) {
		MessageBox(NULL, "Vanilla Elden Ring detected running Roundtable Hold Arena Mod.", "Error: Mod Conflict!", MB_ICONERROR | MB_OK);
		throw std::runtime_error("Roundtable Hold Arena Mod attached to Vanilla game! The process will be stopped.");
	}

	// Settings ==================
	INIReader reader(dllPath + "\\RoundtableHoldArenaSettings.ini");
	Settings::ParseIniFile(reader);

	// Modfiles ==================
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring dllPathWstr = converter.from_bytes(dllPath);
	File::LoadModFilePaths(dllPathWstr);

	// Overlay ===================
	InitFramework(device, spriteBatch, window, dllPath);
	windowWidth = ofWindowWidth;
	windowHeight = ofWindowHeight;

	overlay = CreateBox(424, 340, OF::Right, OF::Bottom, { 0, 0, 100, 140 });
	timer = CreateBox(overlay, 424, 40, OF::Left, OF::Top, { 0, 40, 0, 0 });
	timerBar = CreateBox(timer, 350, 32, OF::Left, OF::Top, { 10, 5, 0, 0 });
	timerBorder = CreateBox(timer, 444, 60, OF::Left, OF::Top, { -6, -8, 0, 0 });
	greatestCombatnat = CreateBox(600, 300, OF::HCenter, OF::VCenter, { 0, 0, 0, 0 });
	font = LoadFont(dllPath + "\\hook_fonts\\font.spritefont");
	brawlTexture = LoadTexture(dllPath + "\\hook_textures\\OverlayBrawl.png");
	teamFightTexture = LoadTexture(dllPath + "\\hook_textures\\OverlayTeamFight.png");
	timerTexture = LoadTexture(dllPath + "\\hook_textures\\timer.png");
	timerBarTexture = LoadTexture(dllPath + "\\hook_textures\\timerBar.png");
	SetFont(font);

	// ===== NEW P2P =========
	_steam_interface_provider = new SteamInterfaceProvider();
	_steam_interface_provider->CreateInterfacesWhenReady();
	_p2pLobby = new P2PLobby(_steam_interface_provider->GetUser(), _steam_interface_provider->GetSteamMatchMaking());


	// ======== HOOK ==============
	if (!HookSetup::Init()) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::ITEM_USE.ptr(), (void*)ItemUseHook, (void**)&Item::ItemUseFunctionOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::HIT_FUNCTION.ptr(), (void*)DamageHook, (void**)&Damage::DamageFunctionOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::LUA_WARP.ptr(), (void*)WarpHook, (void**)&Warp::WarpFunctionOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::SET_EVENT_FLAG.ptr(), (void*)SetEventFlagHook, (void**)&Event::SetEventFlagOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::ADD_SPEFFECT_FUNCTION.ptr(), (void*)AddSPEffectHook, (void**)&SPeffect::AddSpEffectFunctionOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)File::CreateFileWPtr(), RoundTableHoldArena::FileWriteHook, (LPVOID*)&File::tCreateFileWOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)File::VirtualToArchivePtr(), (void*)VirtualToArchivePath, (void**)&File::VirtualToArchivePathOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::EnableHooks()) HOOK_RUNTIME_EXCEPTION;
	// ============================

}

void RoundTableHoldArena::Render()
{
	uint8_t* playerIns = AccessDeepPtr<uint8_t>(Global::WORLD_CHR_MAN.ptr(), 0x10EF8, 0);
	uint32_t* sessionState = AccessDeepPtr<uint32_t>(Global::SESSION_MAN.ptr(), 0xC);
	uint8_t* playerDeadNum = AccessDeepPtr<uint8_t>(Global::CS_PLAYLOG.ptr(), 0xE8);
	uint8_t* soloParamRepository = Global::SOLO_PARAM_REPOSITORY.ptr();


	if (playerIns == nullptr || sessionState == nullptr || playerDeadNum == nullptr || soloParamRepository == nullptr) {
		isInsideBushRoom = false;
		isJumping = false;
		return;
	}
	
	// Used for setting up things like removing material req from weapon upgrades or changing items.
	if(!oneTimeSetup) OneTimeSetup();

	// Checks if free is activated then it listens for crouch cancel out of freecam.
	if (Camera::IsFreeCamOn) CheckForCamEnd();

	// Checks if nodead is on or not and sets the icon appropriately
	CheckForAutoRevive();

	// Revives player if they died while autorevive is on
	HandleDuelDeath(playerIns);

	// Checks if host has initated arena match
	CheckForArenaMatchInitiate();

	// Check if dying to gravity in Square arena
	HandleFallCamera(playerIns);

	// Checks and remove VFX from 'Assassins Gambit' and 'Unseen Form' if player is in bush room 
	CheckIfInBushRoom();

	// Checks and updates current seamelss team
	CheckSeamlessTeamAndMode();

	// Checks and removes rot if you died
	RemoveRot(playerDeadNum);


	if (*sessionState != 0) {

		if (_p2pLobby->IsHost()) {
			CheckForMatchEnd();
		}
		_p2pLobby->HandleMessageQueue();

		InMatch = _p2pLobby->LocalLobbyData.mode != Mode::None;

		// Handles arena match death and respawn
		if(InMatch) HandleArenaMatchDeath(playerIns);
		else if (_p2pLobby->MyLobbyMemberData.state != State::Nothing) {
			_p2pLobby->Reset();
		}
	}
	else if (Item::ItemsBanned) {
		Item::ToggleBannedItems(ON);
	}
	
	//Stops the code unless you are in the a match.
	if (!InMatch) return;

	DisplayScoreOverlay();
	DisplayTimerOverlay();
	DisplayGreatestCombatantOverlay();
	CheckForWindowResize();
}

void RoundTableHoldArena::OneTimeSetup() {
	ParamEditor<EquipMtrlSetParam> pEditor = ParamEditor<EquipMtrlSetParam>();
	ParamHeader* header = pEditor.GetParamHeader();
	EquipMtrlSetParam* pParam = pEditor.GetParamDataPtr();
	for (int i = 0; i < header->RowCount; i++) {
		pParam[i].materialId01 = -1;
		pParam[i].itemNum01 = -1;
	}
	// Store the values in order to restore item no consume state after deactivating undying mark.
	Item::StoreOriginalNoConsumeValues();

	oneTimeSetup = true;
}

void RoundTableHoldArena::CheckForAutoRevive() {
	static bool isAutoReviveOn;

	if (isAutoReviveOn == Death::IsAutoReviveOn) return;

	isAutoReviveOn = Death::IsAutoReviveOn;
	if (isAutoReviveOn) {
		SPeffect::AddSpeffect(SPEFFECT_NODEAD);
		Item::ChangeIcon(ITEM_NO_DEAD, ITEM_ICON_NO_DEAD_ON);
	}
	else {
		SPeffect::RemoveSpEffect(SPEFFECT_NODEAD);
		Item::ChangeIcon(ITEM_NO_DEAD, ITEM_ICON_NO_DEAD_OFF);
	}
}

void RoundTableHoldArena::CheckForCamEnd() {
	if (SPeffect::HasSpeffect(SPEFFECT_CROUCH)) {
		Camera::TurnOffFreeCam();
	}
}

void RoundTableHoldArena::CheckForArenaMatchInitiate() {
	if (CurrentMode == Mode::None || CurrentArena == Arena::None) return;
	_p2pLobby->LocalLobbyData.mode = CurrentMode;
	_p2pLobby->LocalLobbyData.arena = CurrentArena;
	_p2pLobby->LocalLobbyData.state = CurrentMode == Mode::TeamFight ? State::TeamFightInitiate : State::BrawlInitiate;
	_p2pLobby->UpdateLobbyData();
	CurrentMode = Mode::None;
	CurrentArena == Arena::None;
}

void RoundTableHoldArena::CheckIfInBushRoom() {
	bool inBushRoom = SPeffect::HasSpeffect(SPEFFECT_INSIDE_BUSH_ROOM);
	if (inBushRoom ^ isInsideBushRoom) {
		ParamEditor<SpEffectVfxParam> pEditor = ParamEditor<SpEffectVfxParam>();
		ParamHeader* paramHeader = pEditor.GetParamHeader();
		SpEffectVfxParam* paramData = pEditor.GetParamDataPtr();
		std::vector<uint32_t> paramRowIds = pEditor.GetRowIds();

		if (inBushRoom) {
			for (int i = 0; i < paramHeader->RowCount; i++) {
				if (paramRowIds[i] == 8526) {
					paramData[i].finishSfxId = -1;
				}
				if (paramRowIds[i] == 1467000) {
					paramData[i].finishSfxId = -1;
				}
			}
		}
		else {
			for (int i = 0; i < paramHeader->RowCount; i++) {
				if (paramRowIds[i] == 8526) {
					paramData[i].finishSfxId = 523412;
				}
				if (paramRowIds[i] == 1467000) {
					paramData[i].finishSfxId = 523412;
				}
			}
		}

		isInsideBushRoom = inBushRoom;
	}
}

void RoundTableHoldArena::CheckSeamlessTeamAndMode() {
	Seamless::SeamlessInfo seamlessInfo = Seamless::GetSeamlessInfo();
	if (seamlessInfo.mode == Seamless::ENEMIES) SPeffect::AddSpeffect(SPEFFECT_SEAMLESS_ENEMIES_MODE);
	else if (seamlessInfo.mode == Seamless::TEAM) SPeffect::AddSpeffect(SPEFFECT_SEAMLESS_TEAM_MODE);
	else SPeffect::AddSpeffect(SPEFFECT_SEAMLESS_WRONG_MODE);


	if (seamlessInfo.team == Seamless::BLUE) SPeffect::AddSpeffect(SPEFFECT_SEAMLESS_TEAM_BLUE);
	else if (seamlessInfo.team == Seamless::RED) SPeffect::AddSpeffect(SPEFFECT_SEAMLESS_TEAM_RED);

}

void RoundTableHoldArena::RemoveRot(uint8_t* playerDeadNum) {
	if (playerDeadNum != nullptr && *playerDeadNum != deathCount) {
		Item::RemoveItemFromPlayer(Item::GOODS, 12, 999);
		Item::RemoveItemFromPlayer(Item::GOODS, 13, 999);
		Item::RemoveItemFromPlayer(Item::GOODS, 14, 999);
		Item::RemoveItemFromPlayer(Item::GOODS, 15, 999);
		Item::RemoveItemFromPlayer(Item::GOODS, 16, 999);
		deathCount = *playerDeadNum;
	}
}

void RoundTableHoldArena::HandleArenaMatchDeath(uint8_t* playerIns) {
	// Define player states
	static enum PlayerState {
		Alive,
		Dying,
		BannerShowing,
		Respawning
	} currentState = Alive;

	// State-based logic
	switch (currentState) {
	case Alive:
		// Check if the player is dying
		if (Player::IsDying(playerIns) || SPeffect::HasSpeffect(SPEFFECT_KABOOM)) {
			SendScoreUpdateRequest();
			currentState = Dying;
			Timer::StartTimer(DEATH_TIMER, 10.0);
		}
		break;

	case Dying:
		// Check remaining time
		if (Timer::RemainingTime(DEATH_TIMER) > 5.0) return;
		// Display invader vanquished(Edited to YOU DIED) banner and switch to banner showing state
		Menu::DisplayBanner(Menu::InvaderVanquished);
		currentState = BannerShowing;
		break;

	case BannerShowing:
		// Calculate elapsed time since banner showing started
		if (Timer::RemainingTime(DEATH_TIMER) > 0.0) return;
		// Switch to respawning state
		currentState = Respawning;
		break;

	case Respawning:
		// Perform actions for respawning
		Item::RestoreFlasks();
		Player::HealToFull();
		Player::ResetPoise();
		Camera::SetY(CAMERA_DEFAULT_Y);
		SPeffect::AddSpeffect(SPEFFECT_RANDOM_TELEPORT);
		Player::ResetGhostFadeOut();
		Player::ForceAnimationPlayback(ANIMATION_FADEOUT);
		SPeffect::LawOfRegression();
		SPeffect::AddSpeffect(SPEFFECT_GRACE_HEAL);
		SPeffect::AddSpeffect(SPEFFECT_SOAP);
		SPeffect::RemoveSpEffect(SPEFFECT_DEATH_BLIGHT);
		SPeffect::RemoveSpEffect(SPEFFECT_KABOOM);
		Player::ForceAnimationPlayback(ANIMATION_SPAWN, playerIns);
		Camera::ResetY();
		Timer::EraseTimer(DEATH_TIMER);
		currentState = Alive;
		break;
	}
}

void RoundTableHoldArena::HandleFallCamera(uint8_t* playerIns) {
	bool isSquareArean = SPeffect::HasSpeffect(SPEFFECT_SQUARE_ARENA);
	bool isOngoingMatch = 
		_p2pLobby->MyLobbyMemberData.state == State::BrawlOngoing || 
		_p2pLobby->MyLobbyMemberData.state == State::TeamFightOngoing;
	bool isEndingMatch = _p2pLobby->MyLobbyMemberData.state == State::BrawlEndMatch ||
		_p2pLobby->MyLobbyMemberData.state == State::TeamFightEndMatch;

	if (isEndingMatch) return Camera::ResetY();

	if (!isOngoingMatch || !isSquareArean) return;
	Vector3 playerPos = Player::GetCurrentPos(playerIns);

	if (playerPos.y > DEATH_PLANE_Y) return;
	Player::SetHealth(0);
	Camera::SetY(CAMERA_DEFAULT_Y + (std::abs(playerPos.y) + DEATH_PLANE_Y));
}

void RoundTableHoldArena::HandleDuelDeath(uint8_t* playerIns) {
	// Only works if player has autorevive on and is not in an active match
	if (!Death::IsAutoReviveOn || InMatch) return;

	// Define player states
	static enum PlayerState {
		Alive,
		Dying,
		Respawning
	} currentState = Alive;

	// State-based logic
	switch (currentState) {
	case Alive:
		// Check if the player is dying
		if (Player::IsDying(playerIns)) {
			currentState = Dying;
			Timer::StartTimer(DEATH_TIMER, 1.0);
		}
		break;

	case Dying:
		// Calculate elapsed time since banner showing started
		if (Timer::RemainingTime(DEATH_TIMER) > 0.0) return;
		// Switch to respawning state
		currentState = Respawning;
		break;

	case Respawning:
		// Perform actions for respawning
		Item::RestoreFlasks();
		Player::HealToFull();
		Player::ResetPoise();
		Player::ResetGhostFadeOut();
		SPeffect::LawOfRegression();
		SPeffect::AddSpeffect(SPEFFECT_SOAP);
		SPeffect::AddSpeffect(SPEFFECT_GRACE_HEAL);
		SPeffect::RemoveSpEffect(SPEFFECT_DEATH_BLIGHT);
		Player::ForceAnimationPlayback(ANIMATION_DUEL_RESPAWN, playerIns);
		Timer::EraseTimer(DEATH_TIMER);
		currentState = Alive;
		break;
	}
}

void RoundTableHoldArena::CheckForMatchEnd() {
	// Check if brawl or team fight is ongoing
	bool isBrawlOngoing = _p2pLobby->MyLobbyMemberData.state == State::BrawlOngoing;
	bool isTeamFightOngoing = _p2pLobby->MyLobbyMemberData.state == State::TeamFightOngoing;

	// Check if brawl or team fight is ending
	bool isBrawlEnding = _p2pLobby->LocalLobbyData.state == State::BrawlEndMatch;
	bool isTeamFightEnding = _p2pLobby->LocalLobbyData.state == State::TeamFightEndMatch;

	// If neither brawl nor team fight is ongoing or it's already end match, return
	if ((!isBrawlOngoing && !isTeamFightOngoing) || isBrawlEnding || isTeamFightEnding)
		return;

	// If match timer doesn't exist or still has remaining time, return
	if (Timer::RemainingTime(MATCH_TIMER_KEY) > 0.0)
		return;

	// End the match
	if (_p2pLobby->LocalLobbyData.mode == Mode::TeamFight) {
		_p2pLobby->LocalLobbyData.state = State::TeamFightEndMatch;
	}
	else if (_p2pLobby->LocalLobbyData.mode == Mode::Brawl) {
		_p2pLobby->LocalLobbyData.state = State::BrawlEndMatch;
	}

	// Erase the timer and update the lobby data
	Timer::EraseTimer(MATCH_TIMER_KEY);
	_p2pLobby->UpdateLobbyData();
}

void RoundTableHoldArena::DisplayScoreOverlay() {
	std::stringstream score;
	int16_t blueTeamScore = _p2pLobby->LocalLobbyData.blueTeamScore;
	int16_t redTeamScore = _p2pLobby->LocalLobbyData.redTeamScore;
	int16_t mostKills = _p2pLobby->LocalLobbyData.mostKills;
	int16_t ownKills = _p2pLobby->MyLobbyMemberData.kills;

	if (_p2pLobby->LocalLobbyData.mode == Mode::TeamFight &&
		_p2pLobby->MyLobbyMemberData.state == State::TeamFightOngoing) {
		score << std::to_string(blueTeamScore) << std::setfill(' ') << std::setw(19) << std::to_string(redTeamScore);
		DrawBox(overlay, teamFightTexture, scale);
		DrawText(overlay, score.str(), scale, 221, 221, 221, 255, 0.0f, OF::Right, OF::Bottom, { 0, 0, 10, 10 });
	}
	else if (_p2pLobby->MyLobbyMemberData.state == State::BrawlOngoing) {
		score << std::to_string(mostKills) << std::setfill(' ') << std::setw(19) << std::to_string(ownKills);
		DrawBox(overlay, brawlTexture, scale);
		DrawText(overlay, score.str(), scale, 221, 221, 221, 255, 0.0f, OF::Right, OF::Bottom, { 0, 0, 10, 10 });
	}
}

void RoundTableHoldArena::DisplayTimerOverlay() {
	bool isBrawlEnding = _p2pLobby->LocalLobbyData.state == State::BrawlEndMatch;
	bool isTeamFightEnding = _p2pLobby->LocalLobbyData.state == State::TeamFightEndMatch;

	if (isBrawlEnding || isTeamFightEnding) return;

	double remainingTime = Timer::RemainingTime(MATCH_TIMER_KEY);
	if (remainingTime < 0.0f && ((int)remainingTime % 2 == 0)) {
		DrawBox(timer, 0, 0, 0, 255, scale);
		DrawBox(timerBorder, timerTexture, scale);
		return;
	}

	if (remainingTime < 0.0f) return;

	float remainingTimeNormalized = remainingTime / _p2pLobby->LocalLobbyData.matchDuration;
	timerBar->width = timer->width * remainingTimeNormalized;
	DrawBox(timer, 0, 0, 0, 255, scale);
	DrawBox(timerBar, timerBarTexture, scale);
	DrawBox(timerBorder, timerTexture, scale);
}

void RoundTableHoldArena::DisplayGreatestCombatantOverlay() {
	// Check if brawl or team fight is ending
	bool isBrawlEnding = _p2pLobby->LocalLobbyData.state == State::BrawlEndMatch;
	bool isTeamFightEnding = _p2pLobby->LocalLobbyData.state == State::TeamFightEndMatch;
	bool hasTimer = Timer::HasTimer(GREATEST_COMBATANT_TIMER_KEY);
	std::string greatestCombatantsString = _p2pLobby->LocalLobbyData.greatestCombatant;

	// Return early if it's not the ending of a brawl or team fight
	if (!hasTimer && (!isBrawlEnding || !isTeamFightEnding || greatestCombatantsString.size() == 0)) return;

	// Draw the box and text overlay
	DrawBox(greatestCombatnat, 0, 0, 0, 0, scale);
	DrawText(greatestCombatnat, "Greatest combatant: " + greatestCombatantsString, scale * 1.2, 221, 221, 221, 255, 0.0f, OF::HCenter, OF::Bottom, { 0, 20, 0, 0 });

	// Erase the timer if it has expired
	if (Timer::RemainingTime(GREATEST_COMBATANT_TIMER_KEY) < 0.0) {
		Timer::EraseTimer(GREATEST_COMBATANT_TIMER_KEY);
	}
}

void RoundTableHoldArena::CheckForWindowResize() {
	if (GetClientRect(this->window, &rect))
	{
		bool should_reisze = false;
		LONG tempWidth = rect.right - rect.left;
		LONG tempHeight = rect.bottom - rect.top;

		if (windowWidth != tempWidth) {
			windowWidth = tempWidth;
			should_reisze = true;
		}

		if (windowHeight != tempHeight) {
			windowHeight = tempHeight;
			should_reisze = true;
		}

		if (scale == 0.0f) {
			should_reisze = true;
		}

		if (should_reisze) {
			scale = ((float)windowWidth / 4000.0f);
		}
	}
}

void RoundTableHoldArena::SendScoreUpdateRequest() {
	// If nobody killed me, then no need to broadcast a message.
	if (LastPlayerToHitMe == 0) {
		if (_p2pLobby->LocalLobbyData.mode == Mode::Brawl)
			_p2pLobby->MyLobbyMemberData.kills--;

		_p2pLobby->MyLobbyMemberData.deaths++;
		_p2pLobby->UpdateLobbyMemberData();
		return;
	}

	// Broadcast to lobby 
	P2PLobbyUpdateKillsMsg msg{};
	P2PLobbyMsgHeader header{};
	header.type = UpdateKills;
	msg.header = header;
	msg.team = Seamless::GetSeamlessInfo().team;
	msg.killerId = LastPlayerToHitMe;
	char message[sizeof(P2PLobbyUpdateKillsMsg)];
	memcpy(message, &msg, sizeof(P2PLobbyUpdateKillsMsg));
	_p2pLobby->BroadcastMessageToLobbyMembers(message, sizeof(message));

	// Reset the lastPersonToHitMeReference
	LastPlayerToHitMe = 0;

	// Increment my deaths and update Lobby
	_p2pLobby->MyLobbyMemberData.deaths++;
	_p2pLobby->UpdateLobbyMemberData();
}