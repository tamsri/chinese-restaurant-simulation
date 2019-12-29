#ifndef SIMULATOR_EVENT_H_
#define SIMULATOR_EVENT_H_


class Process;

struct Event {
	Event(Process * process, const unsigned int time):event_time(time), process(process){};

	unsigned int event_time;
	Process * process;

};
struct EventComparator
{
	bool operator() (const Event * first, const Event * second) const;
};

#endif
