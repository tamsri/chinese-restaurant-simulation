#include "timer.h"
#include "log.h"

Timer::Timer (unsigned start_time) : start_time_(start_time) {
	Log::GetLog()->Print("Restaurant Opens at " + ConvertTime(start_time) + " O'Clock");
}
std::string Timer::ConvertTime(const unsigned int seconds) const {
	std::string result;
	const auto hour = static_cast<int>(seconds) / 3600;
	result += std::to_string(hour);
	result += ":";
	const auto minute = (static_cast<int>(seconds) - hour * 3600) / 60;
	result += std::to_string(minute);
	result += ":";
	const auto second = static_cast<int>(seconds) % 60;
	result += std::to_string(second);
	return result;
}
std::string Timer::SecondsToMinutes (const unsigned int seconds) {
	std::string result;
	const auto minute = seconds / 60;
	result += std::to_string(minute);
	result += " minutes ";
	const auto second = static_cast<int>(seconds) % 60;
	result += std::to_string(second) + " seconds ";
	result += "("+ std::to_string(seconds) + " seconds)";
	return result;
}
std::string Timer::SecondsToTime(unsigned int seconds) const {
	std::string answer;
	answer += ConvertTime(seconds+start_time_);
	answer += " O'Clock (" + std::to_string(seconds) + " seconds)";
	return answer;
}


