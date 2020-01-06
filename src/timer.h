#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H
#include <string>

class Timer final {
public:
	explicit Timer(unsigned start_time);

	std::string SecondsToTime(unsigned int seconds) const;
	static std::string SecondsToMinutes(unsigned int seconds);
private:
	unsigned int start_time_;
	std::string ConvertTime(unsigned int seconds) const;
};


#endif
