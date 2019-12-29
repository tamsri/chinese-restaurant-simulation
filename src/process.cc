#include "process.h"

#include "event.h"
#include "log.h"

Process::Process ( ) {
	
}

void Process::Execute (unsigned int current_time) {
	Log::GetLog()->Print("Execute needs to be implemented in the derived class", Log::P4, Log::ERROR);
}

void Process::Activate (unsigned int event_time) {
	Log::GetLog()->Print("Activate needs to be implemented in the derived class", Log::P4, Log::ERROR);
}

Event Process::PopEvent()
{
	Event event;
	return event;
}

