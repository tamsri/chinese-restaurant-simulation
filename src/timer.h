#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H
#include <string>

struct TimerForm {
	unsigned int start_time;
	unsigned int end_time;
};
struct TimerFormat {
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
};
class Timer final {
public:
	explicit Timer(unsigned start_time);

	std::string SecondsToTime(unsigned int seconds) const;
	static std::string SecondsToMany (unsigned seconds);
	std::string SecondsToMinutes(unsigned int seconds) const;
	std::string ConvertTime(unsigned int seconds) const;
private:
	unsigned int start_time_;
};


#endif
