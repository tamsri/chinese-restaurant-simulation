#ifndef SIMULATOR_FUTURE_EVENT_LIST_H
#define SIMULATOR_FUTURE_EVENT_LIST_H
struct Event;

class FutureEventList final {
public:
	explicit FutureEventList (bool print_event_list);
	void insert (Event * event);
	Event* pop ( );
private:
	Event * first_event_;
	bool print_event_list_;
};

#endif
