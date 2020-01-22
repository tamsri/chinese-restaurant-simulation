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
struct RecorderForm;

class Simulator final {
public:
	Simulator (const TimerForm & timer_form,
	           const Variables & variables,
	           const RandomInitializerForm & random_init_form,
	           const RecorderForm & recorder_form);
	void Init (bool is_step, Log::LogPriority level, bool is_status, bool print_event_list);
	void Run ( );
	void Conclude ( ) const;
	void CleanRestaurant ( ) const;
private:
	unsigned int current_time_;
	const unsigned int end_time_;
	bool is_step_;
	bool is_status_;

	void PrepareRestaurant ( ) const;

	void Status ( ) const;
	Process * process_;
	ChineseRestaurant * chinese_restaurant_;

	Timer * timer_;
};
#endif
