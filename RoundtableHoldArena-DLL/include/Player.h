#pragma once
#ifndef _PLAYER_
#define _PLAYER_

#include <cstdint>
#include "Global.h"
#include "Pointer.h"
#include "vector3/Vector3.hpp"


class Player {
public:

	enum StartingClass {
		Uknown = -1,
		Vagabond,
		Warrior,
		Hero,
		Bandit,
		Astrologer,
		Prophet,
		Confessor,
		Samurai,
		Prisoner,
		Wretch
	};

	struct CharacterFlags {
		uint8_t noDead : 1;
		uint8_t noDamage : 1;
		uint8_t noFpConsumption : 1;
		uint8_t noStaminaConsumption : 1;
	};

	struct CharacterFlags2 {
		uint8_t unk : 1;
		uint8_t unk2 : 1;
		uint8_t unk3 : 1;
		uint8_t noHit : 1;
		uint8_t noAttack : 1;
		uint8_t noMove : 1;
	};

	struct Attributes {
		uint32_t Vigor;
		uint32_t Mind;
		uint32_t Endurance;
		uint32_t Strength;
		uint32_t Dexterity;
		uint32_t Intelligence;
		uint32_t Faith;
		uint32_t Arcane;
	};

	// Dasaavs playerIns struct
	struct PlayerIns {
		void** vft;
		int handle_chr;
		int handle_map_id;
		void* chrset;
		void* unk_ride_struct_0x90;
		short unk_flag00;
		char pad00[6];
		void* ChrRes;
		int unk_map_id;
		int unk_map_id_ex;
		int map_id;
		int map_id_ex;
		int chrset_cleanup_flag;
		char pad01[4];
		void* unk_GXSg_WorldChrMan_1E550;
		void* ChrModelIns;
		void* PlayerCtrl;
		int think_param_id;
		int npc_id00;
		int chr_type;
		int team_type;
		int net_chrset_sync_map_id;
		int net_chrset_sync_id;
		char pad02[8];
		float chunk_xzyw[4];
		float global_xzyw[4];
		float unk_ride_xzyw[4];
		float dT_chr_update;
		int render_dist_enum;
		int unk_render_enum;
		int render_visibility_enum;
		void* CSTargetVelocityRecorder;
		short reload_CSTargetVelocityRecorder;
		char pad03[6];
		float lock_on_tgt_xzyw[4];
		char unk_flag_ChrActPntSlot;
		char pad04[3];
		float chr_time;
		float unk_chr_time_mul;
		char pad05[4];
		PlayerIns* this_ChrIns;
		int unk_inlined_struct[10];
		float zoom_viewmtx[16];
		int unk_group00[3];
		char pad06[4];
		int unk00;
		void* SpecialEffect;
		long long unk01;
		int npc_id01;
		char pad07[4];
		void** ChrIns_modules;
	};

	template <typename T> static T* GetPlayerIns(int index);
	static void SetFlagGroup1(bool noDead, bool noDamage, bool noFpConsumption, bool noStaminaConsumption, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetFlagGroup2(bool noHit, bool noAttack, bool noMove, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static CharacterFlags* GetFlagGroup1(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetNoDead(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetNoDamage(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetNoGoodsConsume(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetNoUpdate(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetNoAttack(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static float FallTimer(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static uint32_t CurrentAnimation(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static float CurrentAnimationLengthPlayed(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void ForceAnimationPlayback(uint32_t animationId, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void Teleport(Vector3 coords, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static bool IsDying(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void ForceDeath(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void HealToFull(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetNoGravity(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static bool GetFadeOut(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetFadeOut(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetNoMove(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void ResetGhostFadeOut(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetMapHit(bool on, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static int GetHealth(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static double GetHealthPercentage(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void SetHealth(double percentage, uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static void ResetPoise(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static std::wstring GetPlayerName(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static std::string GetPlayerNameAsString(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static uint32_t GetPlayerNetChrSyncID(int playerIndex = 0);
	static Vector3 GetCurrentPos(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static bool ToggleNoDead(uint8_t* playerIns = GetPlayerIns<uint8_t>(0));
	static StartingClass GetStartingClass();
	static void LevelUp(uint32_t level, Attributes attributes);
};

template <typename T> T* Player::GetPlayerIns(int index) {
	T* playerIns = AccessDeepPtr<T>(Global::WORLD_CHR_MAN.ptr(), 0x10EF8, 0x10 * index);
	return playerIns;
}

inline void Player::SetFlagGroup1(bool noDead, bool noDamage, bool noFpConsumption, bool noStaminaConsumption, uint8_t* playerIns) {
	CharacterFlags* characterFlags = AccessDeepPtr<CharacterFlags>(playerIns, 0x190, 0, 0x19B);
	if (characterFlags == nullptr) {
		printf("Something went wrong. Couldn't access characterflag group 1 (nodead, nodamage, noFpConsumption, noStaminaConsumption)\n");
		return;
	}
	*characterFlags = { noDead, noDamage, noFpConsumption, noStaminaConsumption };
}

inline void Player::SetFlagGroup2(bool noHit, bool noAttack, bool noMove, uint8_t* playerIns) {
	CharacterFlags2* characterFlags2 = AccessDeepPtr<CharacterFlags2>(playerIns, 0x530);
	if (characterFlags2 == nullptr) {
		printf("Something went wrong. Couldn't access characterflag group 2 (noHit, noAttack, noMove)\n");
		return;
	}
	characterFlags2->noHit = noHit;
	characterFlags2->noAttack = noAttack;
	characterFlags2->noMove = noMove;
}

inline Player::CharacterFlags* Player::GetFlagGroup1(uint8_t* playerIns) {
	CharacterFlags* characterFlags = AccessDeepPtr<CharacterFlags>(playerIns, 0x190, 0, 0x19B);
	if (characterFlags == nullptr) return nullptr;
	return characterFlags;
}

inline void Player::SetNoDead(bool on, uint8_t* playerIns) {
	CharacterFlags* characterFlags = AccessDeepPtr<CharacterFlags>(playerIns, 0x190, 0, 0x19B);
	characterFlags->noDead = on;
}

inline void Player::SetNoDamage(bool on, uint8_t* playerIns) {
	CharacterFlags* characterFlags = AccessDeepPtr<CharacterFlags>(playerIns, 0x190, 0, 0x19B);
	characterFlags->noDamage = on;
}

inline void Player::SetNoGoodsConsume(bool on, uint8_t* playerIns) {
	bool* noGoodsConsume = AccessDeepPtr<bool>(playerIns, 0x532);
	*noGoodsConsume = on;
}

inline void Player::SetNoUpdate(bool on, uint8_t* playerIns) {
	bool* noUpdate = AccessDeepPtr<bool>(playerIns, 0x531);
	*noUpdate = on;
}

inline void Player::SetNoMove(bool on, uint8_t* playerIns) {
	Player::SetFlagGroup2(false, false, on, playerIns);
}

inline uint32_t Player::CurrentAnimation(uint8_t* playerIns) {
	uint32_t* currentAnimation = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0x80, 0x90);
	if (currentAnimation == nullptr) return -1;
	return *currentAnimation;
}

inline float Player::CurrentAnimationLengthPlayed(uint8_t* playerIns) {
	float* animationLengthPlayed = AccessDeepPtr<float>(playerIns, 0x190, 0x18, 0x24);
	if (animationLengthPlayed == nullptr) return 0.0f;
	return *animationLengthPlayed;
}

inline float Player::FallTimer(uint8_t* playerIns) {
	float* fallTimer = AccessDeepPtr<float>(playerIns, 0x190, 0x70, 0x18);
	if (fallTimer == nullptr) return -1;
	return *fallTimer;
}

inline void Player::ForceAnimationPlayback(uint32_t animationId, uint8_t* playerIns) {
	uint32_t* idleAnimation = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0x58, 0x18);
	if (idleAnimation != nullptr) *idleAnimation = animationId;
}

inline void Player::Teleport(Vector3 coords, uint8_t* playerIns) {
	Vector3* absPos = AccessDeepPtr<Vector3>(playerIns, 0x6C0);
	Vector3* localPos = AccessDeepPtr<Vector3>(playerIns, 0x190, 0x68, 0x70);

	// Trying this becasue there are instances where a player gets
	// spawned tiny bit inside the ground and gets stuck.
	coords.y += 1.0;

	localPos->x = localPos->x + coords.x - absPos->x;
	localPos->y = localPos->y + coords.y - absPos->y;
	localPos->z = localPos->z + coords.z - absPos->z;
}


inline bool Player::IsDying(uint8_t* playerIns) {
	uint32_t* health = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x138);
	if (health == nullptr) return false;

	if (*health < 1) {
		return true;
	}
	return false;
}

inline void Player::ForceDeath(uint8_t* playerIns) {
	uint32_t* health = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x138);
	if (health == nullptr) return;
	*health = 0;
}

inline void Player::HealToFull(uint8_t* playerIns) {
	uint32_t* health = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x138);
	uint32_t* maxHealth = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x13C);

	if (health == nullptr || maxHealth == nullptr) return;
	*health = *maxHealth;
}

inline void Player::SetNoGravity(bool on, uint8_t* playerIns) {
	uint8_t* noGravity = AccessDeepPtr<uint8_t>(playerIns, 0x190, 0x68, 0x1D3);
	if (noGravity == nullptr) return;
	*noGravity = on;
}

inline bool Player::GetFadeOut(uint8_t* playerIns) {
	uint8_t* fadeOut = AccessDeepPtr<uint8_t>(playerIns, 0x20);
	if (fadeOut == nullptr) return false;
	return *fadeOut == 1;
}

inline void Player::SetFadeOut(bool on, uint8_t* playerIns) {
	uint8_t* fadeOut = AccessDeepPtr<uint8_t>(playerIns, 0x20);
	if (fadeOut == nullptr) return;
	*fadeOut = on;
}

inline void Player::ResetGhostFadeOut(uint8_t* playerIns) {
	float* ghostFadeOut = AccessDeepPtr<float>(playerIns, 0x23C);
	if (ghostFadeOut == nullptr) return;
	*ghostFadeOut = 1.0f;
}

inline void Player::SetMapHit(bool on, uint8_t* playerIns) {
	uint32_t* mapHit = AccessDeepPtr<uint32_t>(playerIns, 0x58, 0xF0);
	if (mapHit == nullptr) return;
	*mapHit = on;
}

inline int Player::GetHealth(uint8_t* playerIns) {
	uint32_t* health = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x138);
	if (health == nullptr) return -1;
	return *health;
}

inline double Player::GetHealthPercentage(uint8_t* playerIns) {
	uint32_t* healthPtr = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x138);
	uint32_t* maxHealthPtr = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x13C);
	if (healthPtr == nullptr || maxHealthPtr == nullptr) return -1;
	double health = static_cast<double>(*healthPtr);
	double maxHealth = static_cast<double>(*maxHealthPtr);
	double healthPercentage = (health / maxHealth) * 100.0;
	return healthPercentage;
}


inline void Player::SetHealth(double percentage, uint8_t* playerIns) {
	uint32_t* health = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x138);
	uint32_t* maxHealth = AccessDeepPtr<uint32_t>(playerIns, 0x190, 0, 0x13C);
	if (health == nullptr || maxHealth == nullptr) return;
	double amountToHeal = *maxHealth * (percentage / 100.0);
	*health = amountToHeal;
}

inline void Player::ResetPoise(uint8_t* playerIns) {
	float* poiseRecoveryTimer = AccessDeepPtr<float>(playerIns, 0x190, 0x48, 0x20);
	if (poiseRecoveryTimer == nullptr) {
		printf("Couldn't get poiseRecoveryTimer!\n");
		return;
	}
	*poiseRecoveryTimer = 0.01;
}

inline void Player::SetNoAttack(bool on, uint8_t* playerIns) {
	SetFlagGroup2(false, on, false, playerIns);
}

inline std::wstring Player::GetPlayerName(uint8_t* playerIns) {
	const WCHAR* playerNamePtr = AccessDeepPtr<WCHAR>(playerIns, 0x580, 0x9C);
	if (playerNamePtr == nullptr) {
		printf("Couldn't get Player Name!\n");
		return L"";
	}
	return std::wstring(playerNamePtr, 32);
}

inline std::string Player::GetPlayerNameAsString(uint8_t* playerIns) {
	// Get the player name as a wide string
	std::wstring playerName = Player::GetPlayerName(playerIns);

	// Convert the wide string to a string
	std::string playerNameStr(playerName.begin(), playerName.end());

	return playerNameStr;
}

inline uint32_t Player::GetPlayerNetChrSyncID(int playerIndex) {
	Player::PlayerIns* playerIns = (Player::PlayerIns*)*Player::GetPlayerIns<uintptr_t>(playerIndex);
	return playerIns->net_chrset_sync_id;
}

inline Vector3 Player::GetCurrentPos(uint8_t* playerIns) {
	Vector3* absPos = AccessDeepPtr<Vector3>(playerIns, 0x6C0);

	if (absPos == nullptr) {
		printf("Something went wrong. Couldn't get player position!\n");
		return Vector3(0, 0, 0);
	}

	return *absPos;
}

inline bool Player::ToggleNoDead(uint8_t* playerIns) {
	CharacterFlags* characterFlags = AccessDeepPtr<CharacterFlags>(playerIns, 0x190, 0, 0x19B);
	if (characterFlags == nullptr) {
		printf("Something went wrong! Couldn't get characterFlags.\n");
		return false;
	}
	bool on = !characterFlags->noDead;
	characterFlags->noDead = on;
	SetNoGoodsConsume(on);
	return on;
}

inline Player::StartingClass Player::GetStartingClass() {
	StartingClass* startingClass = AccessDeepPtr<StartingClass>(Global::GAME_DATA_MAN.ptr(), 0x08, 0xBF);

	if (startingClass == nullptr ) {
		printf("Something went wrong. Couldn't get the startingClass.");
		return Uknown;
	}

	return *startingClass;
}

inline void Player::LevelUp(uint32_t level, Attributes attributes) {
	uint32_t* l = AccessDeepPtr<uint32_t>(Global::GAME_DATA_MAN.ptr(), 0x08, 0x68);
	Attributes* a = AccessDeepPtr<Attributes>(Global::GAME_DATA_MAN.ptr(), 0x08, 0x3C);

	if (l == nullptr || a == nullptr) {
		printf("Something went wrong. Couldn't get the level or attributes.");
		return;
	}

	*l = level;
	*a = attributes;
}

#endif