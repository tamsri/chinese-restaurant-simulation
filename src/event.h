#ifndef SIMULATOR_EVENT_H_
#define SIMULATOR_EVENT_H_


class CustomerGroup;

struct Event {
	Event(CustomerGroup * customer_group, const unsigned int time);
	CustomerGroup * customer_group;		// the customer group who does the event
	unsigned int event_time;			// the start time of event
	Event * next_event;
};
/// TODO: DELETE SOON
struct EventComparator
{
	bool operator() (const Event * first, const Event * second) const;
};

#endif
