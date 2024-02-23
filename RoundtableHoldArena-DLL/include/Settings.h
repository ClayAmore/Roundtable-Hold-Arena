#pragma once
#include "inih/INIReader.h"
#include <cstdint>

class Settings {
public:
    struct ModPreferences {
        uint32_t matchDuration;
		bool remainAfterMatch;
    };

    static ModPreferences modPreferences;

	static bool ParseIniFile(const INIReader& reader);
};

inline bool Settings::ParseIniFile(const INIReader& reader) {
	if (reader.ParseError() < 0) {
		printf("Can't load 'RoundTableHoldArena.ini'\n");
		return false;
	}

	modPreferences.matchDuration = reader.GetUnsigned("Match Settings", "MatchDuration", 300);
	modPreferences.remainAfterMatch = reader.GetUnsigned("Match Settings", "RemainAfterMatch", 1);

	return true;
}