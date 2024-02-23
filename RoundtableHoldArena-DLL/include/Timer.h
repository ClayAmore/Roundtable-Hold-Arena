#pragma once
#include <cstdint>
#include <unordered_map>

#define MATCH_TIMER_KEY "MATCH_TIMER_KEY"
#define GREATEST_COMBATANT_TIMER_KEY "GREATEST_COMBATANT_TIMER_KEY"

#define Now std::chrono::system_clock::now
#define SystemClock std::chrono::system_clock
#define TimePoint std::chrono::system_clock::time_point
#define Duration std::chrono::duration


class Timer {
public:
	static void StartTimer(std::string key, uint32_t duration);
	static uint64_t TimeSinceEpoch(std::string key);
	static double RemainingTimeFromEpoch(double epoch, double duration);
	static double ElapsedTime(std::string key);
	static double RemainingTime(std::string key);
	static double RemainingTime(int64_t sinceEpoch, double duration);
	static bool TimerEnded(std::string key);
	static bool HasTimer(std::string key);
	static void EraseTimer(std::string key);
private:
	static std::unordered_map<std::string, std::pair<TimePoint, double>> Timers;
};

inline void Timer::StartTimer(std::string key, uint32_t duration) {
	Timers[key].first = Now();
	Timers[key].second = duration;
}

inline uint64_t Timer::TimeSinceEpoch(std::string key) {
	// Retrieve the time_point associated with the key
	TimePoint timePoint = Timers[key].first;

	// Get the duration since the epoch
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());

	// Convert the duration to milliseconds
	uint64_t milliseconds = duration.count();

	return milliseconds;
}

inline double Timer::RemainingTimeFromEpoch(double startEpoch, double duration) {

	double nowEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(Now().time_since_epoch()).count();
	double elapsed_seconds = (nowEpoch - startEpoch)/1000;
	double remaining_seconds = duration - elapsed_seconds;
	return remaining_seconds;
}

inline double Timer::ElapsedTime(std::string key) {
	Duration<double> elapsed_seconds = Now() - Timers[key].first;
	return elapsed_seconds.count();
}

inline double Timer::RemainingTime(std::string key) {
	Duration<double> elapsed_seconds = Now() - Timers[key].first;
	return Timers[key].second - elapsed_seconds.count();
}

inline double Timer::RemainingTime(int64_t sinceEpoch, double duration) {
	// Get the current time since the epoch
	TimePoint currentTime = std::chrono::system_clock::now();
	int64_t currentMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();

	// Calculate the remaining time
	int64_t elapsedTime = currentMilliseconds - sinceEpoch;
	double remainingTime = duration - static_cast<double>(elapsedTime) / 1000.0;

	return remainingTime;
}

inline bool Timer::HasTimer(std::string key) {
	return Timers.find(key) != Timers.end();
}

inline bool Timer::TimerEnded(std::string key) {
	bool timerEnded = Timer::RemainingTime(key) < 1;
	return timerEnded;
}

inline void Timer::EraseTimer(std::string key) {
	Timers.erase(key);
}
