#ifndef SIMULATOR_FUTURE_EVENT_LIST_H
#define SIMULATOR_FUTURE_EVENT_LIST_H
struct Event;

class FutureEventList final {
public:
	FutureEventList();
	~FutureEventList();
	void insert (Event * event);
	Event * pop();
private:
	Event * first_event_;
};

#endif