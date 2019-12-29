#ifndef SIMULATOR_PROCESS_H_
#define SIMULATOR_PROCESS_H_

#include <set>
#include "event.h"

class Process {
public:
	Process();

	virtual void Execute(unsigned int current_time);
	virtual void Activate(unsigned int event_time);
	Event PopEvent();
protected:
	static std::multiset<Event*, EventComparator> timeline_;
};

#endif
