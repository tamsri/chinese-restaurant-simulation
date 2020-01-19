#ifndef UTILS_RECORDS_H
#define UTILS_RECORDS_H
#include <vector>
#include "customer_group.h"
#include <map>

enum Generators {
	kIntervalArrivalGenerator = 0,
	kWaiterServiceGenerator,
	kBuffetServiceGenerator,
	kCashierServiceGenerator,
	kPersonsGenerator,
	kBuffetTypeGenerator,
	kEnd
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
	Generators generator_type;
};
class Records final {
public:
	explicit Records(RecorderForm recorder_form);
	void PushCustomerRecord(CustomerGroupRecord customer_group_record);
	void PushGeneratorRecord(GeneratorRecord generator_record);
	void PushBuffetRecord(bool is_buffet);
	
	void ConcludeGenerators();
	void ConcludeCustomers ( );
	static unsigned FindMax (const std::vector<unsigned> & data);
	static unsigned FindMin (std::vector<unsigned> data);
private:
	unsigned int seed_;
	unsigned int start_time_;
	unsigned int end_time_;
	static void PrintHistogram (std::vector<unsigned> & data_set);
	long double FindVariance (std::vector<unsigned> & data, long double & mean) const;
	static long double FindAverage (std::vector<unsigned int> & data);
	std::map<int, std::vector<CustomerGroupRecord>> customer_group_records_;
	std::vector<GeneratorRecord> generator_records_;
	std::vector<bool> buffet_type_records_;
};


#endif
