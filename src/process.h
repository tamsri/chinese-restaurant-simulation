#ifndef SIMULATOR_PROCESS_H
#define SIMULATOR_PROCESS_H

class FutureEventList;
struct Event;

class Process {
public:
	Process();
	~Process();
	void InsertEvent(Event * event) const;
	Event * PopEvent() const;
private:
	FutureEventList * future_event_list_;
};

#endif