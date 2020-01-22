#ifndef SIMULATOR_PROCESS_H
#define SIMULATOR_PROCESS_H

class FutureEventList;
struct Event;

class Process final {
public:
	explicit Process (bool print_event_list);
	void InsertEvent (Event * event) const;
	Event* PopEvent ( ) const;
private:
	FutureEventList * future_event_list_;
};

#endif
