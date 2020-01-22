#include "event.h"

#include "customer_group.h"

Event::Event (CustomerGroup * customer_group, const unsigned time) : customer_group(customer_group)
                                                                   , event_time(time)
                                                                   , next_event(nullptr) {
	//Log::GetLog()->Print(
	//	"Event created for the customer group #" + std::to_string(customer_group->GetCustomerGroupID()) + " at " + std::to_string(event_time),
	//	Log::P2, Log::EVENT);
}

bool EventComparator::operator() (const Event * first, const Event * second) const {
	return first->event_time < second->event_time;
}
