#include "TeamFightManager.h"

#define TEAMFIGHT_TIMER "TEAMFIGHT_TIMER"


namespace P2P {
	bool TeamFightManager::CheckIfTeamPicked() {
		Seamless::SeamlessInfo seamlessInfo = Seamless::GetSeamlessInfo();
		return seamlessInfo.team != Seamless::FREE_FOR_ALL;
	}

	void TeamFightManager::PickingTeam() {
		Player::SetNoDamage(ON);
	}

	void TeamFightManager::SetArena(Arena arena) {
		if (arena == Arena::Underground) SPeffect::AddSpeffect(SPEFFECT_UNDERGROUND_ARENA);
		else if (arena == Arena::Square) SPeffect::AddSpeffect(SPEFFECT_SQUARE_ARENA);
		else if (arena == Arena::Portals) SPeffect::AddSpeffect(SPEFFECT_PORTAL_ARENA);
		else if (arena == Arena::BushArena) SPeffect::AddSpeffect(SPEFFECT_BUSH_ARENA);
	}

	bool TeamFightManager::StartMatch(uint32_t duration) {
		if (!Timer::HasTimer(TEAMFIGHT_TIMER)) {
			SPeffect::AddSpeffect(SPEFFECT_TEAMFIGHT);
			Item::ToggleBannedItems(OFF);
			Player::SetNoDamage(OFF);
			Player::SetNoDead(OFF);
			Player::SetNoGoodsConsume(OFF);
			Death::ToggleDeathEventTriggerListener(OFF);
			Death::ToggleDeathFallCameraListener(OFF);
			Death::ToggleDeathMapHitListener(OFF);
			Player::HealToFull();
			SPeffect::LawOfRegression();
			SPeffect::AddSpeffect(SPEFFECT_SOAP);
			SPeffect::AddSpeffect(SPEFFECT_GRACE_HEAL);
			SPeffect::RemoveSpEffect(SPEFFECT_DEATH_BLIGHT);
			SPeffect::RemoveSpEffect(SPEFFECT_NODEAD);
			Item::ToggleItemsNoConsume(OFF);
			Player::ResetPoise();
			SPeffect::AddSpeffect(SPEFFECT_SPAWN_TELEPORT);
			Timer::StartTimer(TEAMFIGHT_TIMER, 3.0);
		}
		if (Timer::RemainingTime(TEAMFIGHT_TIMER) > 0.0) return false;
		Player::ForceAnimationPlayback(ANIMATION_SPAWN);
		Menu::DisplayBanner(Menu::Commence);
		Timer::EraseTimer(TEAMFIGHT_TIMER);
		Timer::StartTimer(MATCH_TIMER_KEY, duration);
		return true;
	}

	void TeamFightManager::OnKillUpdate() {
		SPeffect::AddSpeffect(SPEFFECT_MID_HEAL);
	}

	void TeamFightManager::OnKillUpdate(Seamless::Team selfTeam, Seamless::Team victimTeam) {
		bool isSelfBlueTeam = selfTeam == Seamless::BLUE;
		bool isSelfRedTeam = selfTeam == Seamless::RED;
		bool isVictimBlueTeam = victimTeam == Seamless::BLUE;
		bool isVictimRedTeam = victimTeam == Seamless::RED;

		if (isSelfBlueTeam && isVictimRedTeam) SPeffect::AddSpeffect(SPEFFECT_SMALL_HEAL);
		else if (isSelfRedTeam && isVictimBlueTeam) SPeffect::AddSpeffect(SPEFFECT_SMALL_HEAL);
	}

	bool TeamFightManager::EndMatch(Result result) {
		if (!Timer::HasTimer(TEAMFIGHT_TIMER)) {
			Timer::StartTimer(TEAMFIGHT_TIMER, 5.0);
			if (result == VICTORY) SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_VICTORY);
			else if (result == DEFEAT) SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_DEFEAT);
			else SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_STALEMATE);
			if (!Timer::HasTimer(GREATEST_COMBATANT_TIMER_KEY))Timer::StartTimer(GREATEST_COMBATANT_TIMER_KEY, 7.0);
			SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_END);
		}
		if (Timer::RemainingTime(TEAMFIGHT_TIMER) > 0.0) return false;
		Timer::EraseTimer(TEAMFIGHT_TIMER);
		Item::ToggleBannedItems(ON);
		Death::ToggleDeathEventTriggerListener(ON);
		Death::ToggleDeathFallCameraListener(ON);
		Death::ToggleDeathMapHitListener(ON);
		Camera::ResetY();
		return true;
	}

	void TeamFightManager::CancelMatch() {
		Player::SetNoDamage(OFF);
		Item::ToggleBannedItems(ON);
		Death::ToggleDeathEventTriggerListener(ON);
		Death::ToggleDeathFallCameraListener(ON);
		Death::ToggleDeathMapHitListener(ON);
		Camera::ResetY();
	}
}