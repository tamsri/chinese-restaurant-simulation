#include"log.h"

Log * Log::log_ = nullptr;

void Log::Print (const std::string & text, const LogPriority priority, const LogType type) const {
	if (priority > priority_) return;
	
	switch (type) {
	case ERROR:
		printf("ERROR: ");
		break;
	case EVENT:
		printf("EVENT: ");
		break;
	case INFO:
		printf("INFO: ");
	default:
		break;
	}

	printf("%s\n", text.c_str());
}

Log * Log::GetLog ( ) {
	if (log_ == nullptr) log_ = new Log();
	return log_;
}

void Log::SetPriority (LogPriority priority) {
	priority_ = priority;
}
