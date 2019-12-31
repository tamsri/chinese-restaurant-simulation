#include "future_event_list.h"

#include <cassert>
#include "log.h"
#include "event.h"

FutureEventList::FutureEventList ( ) : first_event_(nullptr){
	Log::GetLog()->Print("Future Event List is created");
}

FutureEventList::~FutureEventList ( ) {
	while(first_event_!=nullptr) {
		Event * deleted_event = first_event_;
		first_event_ = first_event_->next_event;
		delete deleted_event;
	}
	Log::GetLog()->Print("Future Event List is deleted");
}

void FutureEventList::insert (Event * event) {
	// If future event list doesn't exist any event
	if (first_event_ == nullptr) {
		first_event_ = event;
		return;
	}
	// Start counting event from first event
	Event * current_event = first_event_;
	while(current_event->next_event != nullptr) {
		if (event->event_time >= current_event->event_time && event->event_time <= current_event->next_event->event_time) break;
		Log::GetLog()->Print("... error ...");
		current_event = current_event->next_event;
	}
	if(current_event->next_event!=nullptr) event->next_event = current_event->next_event;
	current_event->next_event = event;
	
}

Event * FutureEventList::pop ( ) {
	Event * pop_event = first_event_;
	first_event_ = pop_event->next_event;
	return pop_event;
}

