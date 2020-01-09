#ifndef MODEL_WAITER_H
#define MODEL_WAITER_H

class Table;
class Log;

class Waiter final {
public:
	Waiter();
	unsigned int GetWaiterId() const;
	void ProvideServiceTo(Table * table);
	void CompleteService();
	bool IsAvailable() const;
private:
	const unsigned int waiter_id_;
	static unsigned int waiter_global_id_;
	Table * service_to_;
	Log * log_;
};
#endif // MODEL_WAITER_H
