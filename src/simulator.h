#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H

#include "log.h"
#include "chinese_restaurant.h"
#include "process.h"

struct Variables;
class Log;

class Simulator {
public:
	Simulator(unsigned int end_time, Variables variables );
	~Simulator();
	void Init(bool is_step, Log::LogPriority level);
	void PrepareRestaurant ( ) const;
	void Run();
	void Status ( ) const;
private:
	unsigned int current_time_;
	const unsigned int end_time_;
	bool is_step_;
	
	Process * process_;
	ChineseRestaurant * chinese_restaurant_;
};
#endif
