#ifndef SIMULATOR_PROCESS_H_
#define SIMULATOR_PROCESS_H_

#include <set>
#include "event.h"

class Process {
public:
	Process();
	~Process();

	virtual void Execute(unsigned int current_time);
	
	void Activate(unsigned int event_time);
	unsigned int Time();

	Event * PopEvent();
protected:
	static std::multiset<Event*, EventComparator> timeline_;
};

#endif
