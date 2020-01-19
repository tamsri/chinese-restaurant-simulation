#include "timer.h"
#include "log.h"

Timer::Timer (unsigned start_time) : start_time_(start_time) {
	Log::GetLog()->Print("Restaurant will open at " + ConvertTime(start_time_) + " O'Clock");
}
std::string Timer::ConvertTime(unsigned int time) const {
	std::string result;
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
std::string Timer::SecondsToMany(unsigned int seconds){
	std::string result;
	if(seconds > 86400) {
		const unsigned int day = static_cast<int>(seconds) / 86400;
		seconds = seconds - day * 86400;
		result += std::to_string(day) + " days ";
	}
	if(seconds > 3600) {
		const unsigned int hour = static_cast<int>(seconds) / 3600;
		seconds = seconds - hour * 3600;
		result += std::to_string(hour) + " hours ";
	}
	if (seconds > 60) {
		const unsigned int minute = static_cast<int>(seconds) / 60;
		seconds = seconds - minute * 60;
		result += std::to_string(minute) + " minutes ";
	}
	result += std::to_string(seconds) + " seconds";
	return result;
}

