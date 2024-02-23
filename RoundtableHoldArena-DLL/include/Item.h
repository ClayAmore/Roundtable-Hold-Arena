#pragma once
#ifndef _ITEM_
#define _ITEM_

#include <cstdint>
#include "ParamEditor.h"
#include "EquipParamGoods.h"
#include "Player.h"

// Banlist
#define CrimsonFlask         1000
#define CrimsonFlaskEmpty    1001
#define CrimsonFlask1        1002
#define CrimsonFlaskEmpty1   1003
#define CrimsonFlask2        1004
#define CrimsonFlaskEmpty2   1005
#define CrimsonFlask3        1006
#define CrimsonFlaskEmpty3   1007
#define CrimsonFlask4        1008
#define CrimsonFlaskEmpty4   1009
#define CrimsonFlask5        1010
#define CrimsonFlaskEmpty5   1011
#define CrimsonFlask6        1012
#define CrimsonFlaskEmpty6   1013
#define CrimsonFlask7        1014
#define CrimsonFlaskEmpty7   1015
#define CrimsonFlask8        1016
#define CrimsonFlaskEmpty8   1017
#define CrimsonFlask9        1018
#define CrimsonFlaskEmpty9   1019
#define CrimsonFlask10       1020
#define CrimsonFlaskEmpty10  1021
#define CrimsonFlask11       1022
#define CrimsonFlaskEmpty11  1023
#define CrimsonFlask12       1024
#define CrimsonFlaskEmpty12  1025
#define FrenzyflameStone     3311
#define RawMeatDumplings     1235
#define SpectralSteedWhistle 0130
#define WarmingStone         3310
#define TheUndyingMark       3320
#define ShadowMeldShard      2160
#define Soap			     2120

namespace GameItems {
	class Item {
	public:
		struct ItemUseStruct {
			Player::PlayerIns* ChrIns;
			int unk01;
			int unk02;
			uint32_t itemId;
			int8_t unk04;
			int8_t unk05;
			int8_t unk06;
			int8_t unk07;
			int refId;
			float unk08;
		};

		struct ItemInfo {
			ItemInfo() {
				itemId = -1;
				itemQuantity = 0;
				itemRelayValue = 0;
				itemAshesOfWar = 0;
			};
			uint32_t itemId;
			uint32_t itemQuantity;
			uint32_t itemRelayValue;
			uint32_t itemAshesOfWar;
		};

		struct ItemGiveStruct {
			uint32_t item_struct_count;
			ItemInfo item_info[10];
		};

		enum ItemType {
			WEAPON = 0,
			ARMOR = 0x10000000,
			TALISMAN = 0x20000000,
			GOODS = 0x40000000,
			ASH_OF_WAR = 0x80000000
		};

		static bool ItemsBanned;
		static std::vector<bool> OriginalNoConsumeValues;

		typedef void ItemUseFunction(ItemUseStruct itemUseStruct);
		typedef void ItemGiveFunction(ItemType itemType, int itemId, int quantity);
		typedef void ItemGiveLuaFunction(uint64_t mapItemManager, ItemGiveStruct* itemInfo, void* itemDetails);
		typedef void RestoreFlasksFunction(void* playerIns);
		static ItemUseFunction* ItemUseFunctionOriginal;
		static ItemGiveLuaFunction* ItemGiveLuaFunctionOriginal;

		static void ToggleBannedItems(bool on);
		static void ChangeIcon(uint32_t itemId, uint32_t iconId);
		static void DirectlyGivePlayerItem(ItemType itemType, int id, int quantity);
		static void RemoveItemFromPlayer(ItemType itemType, int id, uint32_t quantity);
		static void StoreOriginalNoConsumeValues();
		static void ToggleItemsNoConsume(bool on);
		static void RestoreFlasks();
	};

	const uint32_t itemBanList[] = {
		CrimsonFlask,        CrimsonFlaskEmpty,     CrimsonFlask1,        CrimsonFlaskEmpty1,
		CrimsonFlask2,       CrimsonFlaskEmpty2,    CrimsonFlask3,        CrimsonFlaskEmpty3,
		CrimsonFlask4,       CrimsonFlaskEmpty4,    CrimsonFlask5,        CrimsonFlaskEmpty5,
		CrimsonFlask6,       CrimsonFlaskEmpty6,    CrimsonFlask7,        CrimsonFlaskEmpty7,
		CrimsonFlask8,       CrimsonFlaskEmpty8,    CrimsonFlask9,        CrimsonFlaskEmpty9,
		CrimsonFlask10,      CrimsonFlaskEmpty10,   CrimsonFlask11,       CrimsonFlaskEmpty11,
		CrimsonFlask12,      CrimsonFlaskEmpty12,   WarmingStone,          FrenzyflameStone,
		RawMeatDumplings,    SpectralSteedWhistle,  TheUndyingMark,		   ShadowMeldShard,
		Soap
	};											 
		
	inline void Item::ToggleBannedItems(bool on) {
		ParamEditor<EquipParamGoods> pEditor = ParamEditor<EquipParamGoods>();
		ParamHeader* header = pEditor.GetParamHeader();
		EquipParamGoods* pParam = pEditor.GetParamDataPtr();
		std::vector<uint32_t> rowIds = pEditor.GetRowIds();

		for (int i = 0; i < header->RowCount; i++) {
			uint32_t rowId = rowIds[i];
			bool inArray = std::find(std::begin(itemBanList), std::end(itemBanList), rowId) != std::end(itemBanList);

			if (inArray) pParam[i].enable_live = on;
		}

		ItemsBanned = !on;
	}

	inline void Item::ChangeIcon(uint32_t itemId, uint32_t iconId) {
		ParamEditor<EquipParamGoods> pEditor = ParamEditor<EquipParamGoods>();
		ParamHeader* header = pEditor.GetParamHeader();
		EquipParamGoods* pParam = pEditor.GetParamDataPtr();
		std::vector<uint32_t> rowIds = pEditor.GetRowIds();

		for (int i = 0; i < header->RowCount; i++) {
			if (rowIds[i] == itemId) pParam[i].iconId = iconId;
;		}
	}

	inline void Item::DirectlyGivePlayerItem(Item::ItemType itemType, int itemId, int quantity) {
		ItemGiveFunction* addItemFunction = (ItemGiveFunction*)(Global::ADD_ITEM_FUNCTION.ptr());
		if (addItemFunction == nullptr) {
			printf("Failed getting addItemFunction!\n");
			return;
		}
		addItemFunction(itemType, itemId, quantity);
	}

	inline void Item::RemoveItemFromPlayer(ItemType itemType, int itemId, uint32_t quantity) {
		if (quantity > 1) quantity = quantity * -1;
		DirectlyGivePlayerItem(itemType, itemId, quantity);
	}

	inline void Item::StoreOriginalNoConsumeValues() {
		ParamEditor<EquipParamGoods> pEditor = ParamEditor<EquipParamGoods>();
		ParamHeader* header = pEditor.GetParamHeader();
		EquipParamGoods* pParam = pEditor.GetParamDataPtr();
		std::vector<uint32_t> rowIds = pEditor.GetRowIds();

		for (int i = 0; i < header->RowCount; i++) {
			OriginalNoConsumeValues.push_back(pParam[i].isConsume);
		}
	}

	inline void Item::ToggleItemsNoConsume(bool on) {
		ParamEditor<EquipParamGoods> pEditor = ParamEditor<EquipParamGoods>();
		ParamHeader* header = pEditor.GetParamHeader();
		EquipParamGoods* pParam = pEditor.GetParamDataPtr();
		std::vector<uint32_t> rowIds = pEditor.GetRowIds();

		for (int i = 0; i < header->RowCount; i++) {
			pParam[i].isConsume = on ? false : OriginalNoConsumeValues[i];
		}
	}

	inline void Item::RestoreFlasks() {
		uintptr_t* playerIns = Player::GetPlayerIns<uintptr_t>(0);
		RestoreFlasksFunction* restoreFlasksF = (RestoreFlasksFunction*)(Global::RESTORE_FLASKS.ptr());
		printf("Playerins address is: %p\n", playerIns);
		if (restoreFlasksF == nullptr) {
			printf("Failed getting restoreFlasksF!\n");
			return;
		}
		restoreFlasksF((void*)*playerIns);
	}
}
#endif