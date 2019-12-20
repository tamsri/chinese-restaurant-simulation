#include "event.h"

bool  EventComparator::operator()  (const Event * first, const Event * second) const {
	return first->event_time < second->event_time;
}
