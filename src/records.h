#ifndef UTILS_RECORDS_H
#define UTILS_RECORDS_H
#include <vector>
#include "customer_group.h"

enum Generators {
	kIntervalArrivalGenerator = 0,
	kWaiterServiceGenerator,
	kBuffetServiceGenerator,
	kCashierServiceGenerator,
	kPersonsGenerator,
	kEnd
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
class Records {
public:
	Records();
	void PushCustomerRecord(CustomerGroupRecord customer_group_record);
	void PushGeneratorRecord(GeneratorRecord generator_record);
	void PushBuffetRecord(bool is_buffet);

	void ConcludeGenerators();
	static unsigned FindMax (std::vector<unsigned> data);
	static unsigned FindMin (std::vector<unsigned> data);
private:
	static void PrintHistogram (std::vector<unsigned> & data_set);
	std::vector<CustomerGroupRecord> customer_group_records_;
	std::vector<GeneratorRecord> generator_records_;
	std::vector<bool> buffet_type_records_;
};


#endif
