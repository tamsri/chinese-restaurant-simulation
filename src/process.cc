#include "process.h"

#include "log.h"

Process::Process ( ) {
	
}

void Process::Execute (unsigned current_time) {
	Log::GetLog()->Print("Execute needs to be implemented in the derived class", Log::P4, Log::ERROR);
}

void Process::Activate (unsigned event_time) {
	
}
