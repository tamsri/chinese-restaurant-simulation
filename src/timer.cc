#include "timer.h"
#include "log.h"

Timer::Timer (unsigned start_time) : start_time_(start_time) {
	Log::GetLog()->Print("Restaurant will open at " + ConvertTime(0) + " O'Clock");
}
std::string Timer::ConvertTime(const unsigned int seconds) const {
	std::string result;
	auto time = start_time_ + seconds;
	if(time>=86400) {
		const unsigned int day = static_cast<int>(time) / 86400;
		time = time - day * 86400;
		result += "Day " + std::to_string(day) + " ";
	}
	const auto hour = static_cast<int>(time) / 3600;
	result += std::to_string(hour);
	result += ":";
	const auto minute = (static_cast<int>(time) - hour * 3600) / 60;
	result += std::to_string(minute);
	result += ":";
	const auto second = static_cast<int>(time) % 60;
	result += std::to_string(second);
	return result;
}
std::string Timer::SecondsToMinutes (const unsigned int seconds) const {
	std::string result;
	const auto minute = seconds / 60;
	result += std::to_string(minute);
	result += " minutes ";
	const auto second = static_cast<int>(seconds) % 60;
	result += std::to_string(second) + " seconds ";
	return result;
}
std::string Timer::SecondsToTime(unsigned int seconds) const {
	std::string answer;
	answer += ConvertTime(seconds);
	answer += " O'Clock (" + std::to_string(seconds) + " seconds)";
	return answer;
}


