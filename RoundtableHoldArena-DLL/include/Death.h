#pragma once
#include <cstdint>
#include "Global.h"
#include "Pointer.h"

#define DEATH_TIMER "DEATH_TIMER"

using namespace BS2;

class Death {
public:
	struct Bytes{
		uint8_t unk1;
		uint8_t unk2;
		uint8_t unk3;
		uint8_t toggle;
	};

	static bool IsAutoReviveOn;

	static void ToggleDeathEventTriggerListener(bool on);
	static void ToggleDeathFallCameraListener(bool on);
	static void ToggleDeathMapHitListener(bool on);
	static void ToggleAutoRevive();
};

inline void Death::ToggleDeathEventTriggerListener(bool on) {
	Bytes* deathEventTriggerSetter = AccessDeepPtr<Bytes>(Global::DEATH_TRIGGER_EVENT_SETTER.ptr());
	if (deathEventTriggerSetter == nullptr) return;
	deathEventTriggerSetter->toggle = on;
}

inline void Death::ToggleDeathFallCameraListener(bool on) {
	uint8_t* fallCameraSetter = AccessDeepPtr<uint8_t>(Global::FALL_CAMERA_SETTER.ptr());
	if (fallCameraSetter == nullptr) return;
	*fallCameraSetter = on;
}

inline void Death::ToggleDeathMapHitListener(bool on) {
	uint8_t* mapHitDisabledSetter = AccessDeepPtr<uint8_t>(Global::DEATH_DISABLE_MAP_HIT_SETTER.ptr());
	if (mapHitDisabledSetter == nullptr) return;
	*mapHitDisabledSetter = on;
}

inline void Death::ToggleAutoRevive() {
	IsAutoReviveOn = !IsAutoReviveOn;
	ToggleDeathEventTriggerListener(!IsAutoReviveOn);
	ToggleDeathMapHitListener(!IsAutoReviveOn);
}