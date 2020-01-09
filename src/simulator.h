#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H

#include "log.h"
#include "chinese_restaurant.h"
#include "process.h"

struct Variables;
class Log;
class Timer;
struct RandomInitializerForm;
struct TimerForm;
class Simulator final {
public:
	Simulator(const TimerForm & timer_form, const Variables & variables, const RandomInitializerForm & random_init_form);
	void Init(bool is_step, Log::LogPriority level);
	void Run();
	void Conclude () const;
private:
	unsigned int current_time_;
	const unsigned int duration_;
	bool is_step_;
	
	void PrepareRestaurant ( ) const;
	void CleanRestaurant() const;
	void Status ( ) const;
	Process * process_;
	ChineseRestaurant * chinese_restaurant_;

	Timer * timer_;
};
#endif
