#pragma once
#include <cstdint>

class Damage {
public:
    struct StaggerModule {
        uint8_t undefined[0x10];
        float stagger;
        float staggerMax;
        uint8_t undefined2[0x4];
        float resetTimer;
    };

    struct ChrModuleBag {
        uint8_t undefined[0x40];
        StaggerModule* staggerModule;
    };

    struct ChrIns {
        uint8_t undefined[0x8];
        unsigned long long handle;
        uint8_t undefined2[0x180];
        ChrModuleBag* chrModulelBag;
        uint8_t undefined3[0x508];
        unsigned long long targetHandle;
    };

    struct ChrModuleBase {
        uint8_t undefined[0x8];
        ChrIns* owningChrIns;
    };

    struct ChrDamageModule {
        char unk[0x8];
        ChrIns* playerIns;
    };

    struct DamageStruct {
        char unk[0x228];
        int damage;
    };
    typedef void DamageFunction(ChrDamageModule* damageModule, ChrIns* chrIns, DamageStruct* damageStruct, unsigned long long param_4, char param_5);
    static DamageFunction* DamageFunctionOriginal;
};
