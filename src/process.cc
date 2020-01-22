#include "process.h"

#include "event.h"
#include "log.h"
#include "future_event_list.h"

Process::Process (const bool print_event_list) {
	future_event_list_ = new FutureEventList(print_event_list);
	Log::GetLog()->Print("Process is constructed");
}

void Process::InsertEvent (Event * event) const { future_event_list_->insert(event); }

Event* Process::PopEvent ( ) const {
	auto * event = future_event_list_->pop();
	return event;
}
