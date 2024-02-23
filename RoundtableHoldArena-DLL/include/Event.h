#pragma once
#include <cstdint>

class Event {
public:
	typedef void SetEventFlagFunction(uint64_t eventMan, uint32_t* eventFlagId, bool state);
	typedef bool IsEventFlagFunction(uint64_t eventMan, uint32_t* eventFlagId);
	static SetEventFlagFunction* SetEventFlagOriginal;
};
