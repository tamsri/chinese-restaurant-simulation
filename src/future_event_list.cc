#include "future_event_list.h"

#include <cassert>
#include "log.h"
#include "event.h"
#include  "customer_group.h"

FutureEventList::FutureEventList (const bool print_event_list) : first_event_(nullptr), print_event_list_(print_event_list) {
	Log::GetLog()->Print("Future Event List is created");
}

void FutureEventList::insert (Event * event) {
	// if the event list is empty
	if (first_event_ == nullptr) first_event_ = event;
		// if the event list is not empty
	else if (event->event_time < first_event_->event_time) {
		event->next_event = first_event_;
		first_event_      = event;
	}
	else if (event->event_time == first_event_->event_time) {
		event->next_event        = first_event_->next_event;
		first_event_->next_event = event;
	}
	else {
		auto * current_event = first_event_;
		while (current_event->next_event != nullptr) {
			if (event->event_time <= current_event->next_event->event_time || event->event_time <= current_event->
			    event_time)
				break;
			current_event = current_event->next_event;
		}
		if (current_event->next_event != nullptr) event->next_event = current_event->next_event;
		current_event->next_event = event;
	}

	// Event Lists checker
	if (!print_event_list_) return;
	auto * check_event = first_event_;
	while (check_event != nullptr) {
		printf("%d(#%d) ", check_event->event_time, check_event->customer_group->GetCustomerGroupId());
		check_event = check_event->next_event;
	}
	printf("\n");
}

Event* FutureEventList::pop ( ) {
	if (first_event_ == nullptr)
		assert("No future event to pop");
	auto * pop_event = first_event_;
	first_event_     = pop_event->next_event;
	return pop_event;
}
