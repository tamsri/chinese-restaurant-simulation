#include "process.h"

#include "event.h"
#include "log.h"
#include "future_event_list.h"

Process::Process ( ) {
	future_event_list_ = new FutureEventList();
	Log::GetLog()->Print("Process is constructed");
}
Process::~Process ( ) {
	delete future_event_list_;
	Log::GetLog()->Print("Process is constructed");

}

void Process::InsertEvent (Event * event) const {
	future_event_list_->insert(event);
}

Event * Process::PopEvent() const {
	Event * event = future_event_list_->pop();
	return event;
}
