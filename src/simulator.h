#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H

#include "log.h"
#include "chinese_restaurant.h"
#include "process.h"

struct Variables;
class Log;
class Timer;

class Simulator final {
public:
	Simulator(unsigned int end_time, const Variables & variables, int kernel_set_index);
	void Init(bool is_step, Log::LogPriority level);
	void Run();
private:
	unsigned int current_time_;
	const unsigned int end_time_;
	bool is_step_;
	
	void PrepareRestaurant ( ) const;
	void CleanRestaurant();
	void Status ( ) const;
	Process * process_;
	ChineseRestaurant * chinese_restaurant_;

	Timer * timer_;
};
#endif
