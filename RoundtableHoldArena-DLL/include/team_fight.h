#pragma once
#include <cstdint>
#include <vector>
#include <mutex>
#include <map>
#include "pointer.h"
#include "Item.h"
#include "SPEeffect.h"
#include "Damage.h"

class TeamFight {
public:
	bool team_fight_main(uint8_t* s_players[6]);
	int blue_kills = 0;
	int red_kills = 0;
	static void individuals_kills_counter(Damage::ChrDamageModule* damageModule, Damage::ChrIns* chrIns, Damage::DamageStruct* damageStruct, unsigned long long param_4, char param_5);
private:
	void check_for_fight_initiating(uint8_t* self, uint32_t* seamless_pvp_mode, uint32_t* seamless_pvp_team, int self_status, int host_status, int INITIATE_ID, int PICKING_TEAM_ID, int READY_TO_START_ID);
};
