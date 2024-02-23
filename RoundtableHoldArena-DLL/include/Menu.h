#pragma once
#include <cstdint>
#include "Global.h"

class Menu {
public: 
	enum BannerType {
        YouDied = 5,
        HostVanquished = 7,
        BloodyFingerVanquished = 8,
        LostGraceDiscovered = 0xb,
        Unknown14 = 0xc,
        LegendFelled = 2,
        DemigodFelled = 1,
        GreatEnemyFelled = 3,
        EnemyFelled = 4,
        DutyFulfilled = 9,
        MapFound = 0x11,
        GreatRuneRestored = 0x15,
        GodSlain = 0x16,
        DuelistVanquished = 0x17,
        RecusantVanquished = 0x18,
        InvaderVanquished = 0x19,
        Commence = 0xd,
        Stalemate = 0xf,
        Victory = 0xe,
        Defeat = 0x10,
        Uknown6 = 6,
        Uknown10 = 10,
        Uknown0x12 = 0x12,
        Uknown0x13 = 0x13,
        Uknown0x14 = 0x14,
        Uknown0x1e = 0x1e,
        Uknown0x1f = 0x1f,
        DefeatFadeToBlack = 0x20,
        MatchStart = 0x21,
        MatchEnd = 0x22,
        Slain = 0x23,
        Victory2 = 0x24,
        Draw = 0x25,
        Defeat2 = 0x26,
        Uknown0x27 = 0x27
	};

    typedef void DisplayBannerFunction(void* csMenuMapImp, BannerType bannerType);
    static void DisplayBanner(BannerType bannerType);
};


inline void Menu::DisplayBanner(Menu::BannerType bannerType) {
    DisplayBannerFunction* displayBannerF = (DisplayBannerFunction*)Global::DISPLAY_BANNER_FUNCTION.ptr();
    if (displayBannerF == nullptr) {
        printf("displayBannerF is null\n");
        return;
    }
    void* CSMenuManImp = Global::CS_MENU_MAN_IMP.ptr();
    if (CSMenuManImp == nullptr) {
        printf("CSMenuManImp is null\n");
        return;
    }
    displayBannerF(CSMenuManImp, bannerType);
}