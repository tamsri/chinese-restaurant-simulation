#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H
#include <string>

class Log {
public:
	enum LogType { ERROR, EVENT, INFO, NONE };

	enum LogPriority { P1 = 1, P2, P3, P4 };

	void Print (const std::string & text, LogPriority priority = P4, LogType type = INFO) const;
	static Log* GetLog ( );
	void SetPriority (LogPriority priority);
protected:
	static Log * log_;
	LogPriority priority_ = P4;
};

#endif
