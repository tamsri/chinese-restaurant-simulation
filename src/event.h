#ifndef SIMULATOR_EVENT_H_
#define SIMULATOR_EVENT_H_


class Process;

struct Event {
	unsigned int event_time;
	Process * process;

	explicit Event(Process * process):event_time(0), process(process){};
};
struct EventComparator
{
	bool operator() (const Event * first, const Event * second) const;
};

#endif
