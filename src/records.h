#ifndef UTILS_RECORDS_H
#define UTILS_RECORDS_H
#include <vector>
#include "customer_group.h"
#include <map>

struct ChineseRestaurant;
enum Generators {
	kIntervalArrivalGenerator = 0,
	kWaiterServiceGenerator,
	kBuffetServiceGenerator,
	kCashierServiceGenerator,
	kPersonsGenerator,
	kBuffetTypeGenerator,
	kGeneratorEnd
};
enum Queues {
	kTableQueue = 0,
	kBuffetQueue,
	kFreeWaiterQueue,
	kWaitWaiterQueue,
	kCheckoutQueue,
	kFreeCashierQueue,
	kQueueEnd
};
struct RecorderForm {
	unsigned int selected_seed;
	unsigned int start_time;
	unsigned int end_time;
};
struct CustomerGroupRecord{
	unsigned int id;
	unsigned int record_time;
	CustomerGroup::State record_state;
};
struct GeneratorRecord {
	unsigned int value;
	unsigned int record_time;
	Generators generator_type;
};
struct QueueRecord {
	unsigned int record_time;
	unsigned int queue_size;
	Queues queue_type;
};
class Records final {
public:
	explicit Records(RecorderForm recorder_form, ChineseRestaurant * chinese_restaurant);
	// Functions for pushing records
	void PushCustomerRecord(CustomerGroupRecord customer_group_record);
	void PushGeneratorRecord(GeneratorRecord generator_record);
	void PushBuffetRecord(bool is_buffet);
	void PushQueueRecord(QueueRecord queue_record);
	// Conclusions
	void ConcludeGenerators();
	void ConcludeCustomers ();
	void ConcludeResult();
	// Statistic Utilities
	static unsigned FindMax (const std::vector<unsigned> & data);
	static unsigned FindMin (std::vector<unsigned> data);
private:
	unsigned int seed_;
	unsigned int start_time_;
	unsigned int end_time_;
	ChineseRestaurant * chinese_restaurant_;
	static void PrintHistogram (std::vector<unsigned> & data_set);
	long double FindVariance (std::vector<unsigned> & data, long double & mean) const;
	static long double FindAverage (std::vector<unsigned int> & data);
	std::map<int, std::vector<CustomerGroupRecord>> customer_group_records_;
	std::vector<GeneratorRecord> generator_records_;
	std::vector<bool> buffet_type_records_;
	std::map <Queues, std::pair<unsigned int, long double>> queue_records_; // first of pair is last check time, second of pair is average queue length
};


#endif
