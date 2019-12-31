#ifndef SIMULATOR_FUTURE_EVENT_LIST_H
#define SIMULATOR_FUTURE_EVENT_LIST_H
#include <queue>
struct Event;

class FutureEventList {
public:
	FutureEventList();
	~FutureEventList();
	void insert (Event * event);
	Event * pop();
private:
	Event * first_event_;
};

#endif