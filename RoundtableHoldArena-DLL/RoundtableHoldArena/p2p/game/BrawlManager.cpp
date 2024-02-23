#include "BrawlManager.h"

#define BRAWL_TIMER "BRAWL_IMER"
#define BRAWL_GREATEST_COMBATANT_DELAY "BRAWL_GREATEST_COMBATANT_DELAY"

namespace P2P {
	void BrawlManager::PrepareMap(){
	}

	void BrawlManager::SetArena(Arena arena) {
		if (arena == Arena::Underground) SPeffect::AddSpeffect(SPEFFECT_UNDERGROUND_ARENA);
		else if (arena == Arena::Square) SPeffect::AddSpeffect(SPEFFECT_SQUARE_ARENA);
		else if (arena == Arena::Portals) SPeffect::AddSpeffect(SPEFFECT_PORTAL_ARENA);
		else if (arena == Arena::BushArena) SPeffect::AddSpeffect(SPEFFECT_BUSH_ARENA);
	}

	bool BrawlManager::StartMatch(uint32_t duration) {
		if (!Timer::HasTimer(BRAWL_TIMER)) {
			SPeffect::AddSpeffect(SPEFFECT_BRAWL);
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
			Timer::StartTimer(BRAWL_TIMER, 3.0);
		}
		if (Timer::RemainingTime(BRAWL_TIMER) > 0.0) return false;
		Player::ForceAnimationPlayback(ANIMATION_SPAWN);
		Menu::DisplayBanner(Menu::Commence);
		Timer::EraseTimer(BRAWL_TIMER);
		Timer::StartTimer(MATCH_TIMER_KEY, duration);
		return true;
	}

	void BrawlManager::OnKillUpdate() {
		SPeffect::AddSpeffect(SPEFFECT_MID_HEAL);
	}

	bool BrawlManager::EndMatch(Result result) {
		if (!Timer::HasTimer(BRAWL_TIMER)) {
			Timer::StartTimer(BRAWL_TIMER, 5.0);
			if (result == VICTORY) SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_VICTORY);
			else if (result == DEFEAT) SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_DEFEAT);
			else SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_STALEMATE);
			SPeffect::AddSpeffect(SPEFFECT_ARENA_MATCH_END);
			if (!Timer::HasTimer(GREATEST_COMBATANT_TIMER_KEY))Timer::StartTimer(GREATEST_COMBATANT_TIMER_KEY, 7.0);
		}
		if (Timer::RemainingTime(BRAWL_TIMER) > 0.0) return false;
		Timer::EraseTimer(BRAWL_TIMER);
		Item::ToggleBannedItems(ON);
		Death::ToggleDeathEventTriggerListener(ON);
		Death::ToggleDeathFallCameraListener(ON);
		Death::ToggleDeathMapHitListener(ON);
		Camera::ResetY();
		return true;
	}

	void BrawlManager::CancelMatch() {
		Item::ToggleBannedItems(ON);
		Death::ToggleDeathEventTriggerListener(ON);
		Death::ToggleDeathFallCameraListener(ON);
		Death::ToggleDeathMapHitListener(ON);
		Camera::ResetY();
	}
}