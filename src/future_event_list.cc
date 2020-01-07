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

void FutureEventList::insert (Event * input_event) {
	// if the event list is empty
	if(first_event_ == nullptr) first_event_ = input_event;
	// if the event list is not empty
	else if (input_event->event_time < first_event_->event_time) {
		input_event->next_event = first_event_;
		first_event_ = input_event;
	}
	else if (input_event->event_time == first_event_->event_time) {
		input_event->next_event = first_event_->next_event;
		first_event_->next_event = input_event;
	}
	else {
		Event * current_event = first_event_;
		while(current_event->next_event != nullptr) {
			if (input_event->event_time <= current_event->next_event->event_time || input_event->event_time <= current_event->event_time) break;
			current_event = current_event->next_event;
		}
		if(current_event->next_event != nullptr) input_event->next_event = current_event->next_event;
		current_event->next_event = input_event;
	}

	/*
	// Event Lists checker
	Event * check_event = first_event_;
	while (check_event != nullptr) {
		printf("%d ", check_event->event_time);
		check_event = check_event->next_event;
	}
	printf("\n");*/
}

Event * FutureEventList::pop ( ) {
	if (first_event_ == nullptr) assert("No future event to pop");
	Event * pop_event = first_event_;
	first_event_ = pop_event->next_event;
	return pop_event;
}

