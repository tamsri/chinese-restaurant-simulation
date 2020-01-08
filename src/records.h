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
struct CustomerGroupRecord{
	unsigned int id;
	unsigned int record_time;
	CustomerGroup::State record_state;
};
struct GeneratorRecord {
	unsigned int value;
	Generators generator_type;
};
class Records final{
public:
	explicit Records(unsigned int seed);
	void PushCustomerRecord(CustomerGroupRecord customer_group_record);
	void PushGeneratorRecord(GeneratorRecord generator_record);
	void PushBuffetRecord(bool is_buffet);

	void ConcludeGenerators();
	void ConcludeCustomers ( );
	static unsigned FindMax (const std::vector<unsigned> & data);
	static unsigned FindMin (std::vector<unsigned> data);
private:
	unsigned int seed_;
	static void PrintHistogram (std::vector<unsigned> & data_set);
	double FindVariance (std::vector<unsigned int> & data, double & mean) const;
	static double FindAverage(std::vector<unsigned int> & data);
	std::map<int, std::vector<CustomerGroupRecord>> customer_group_records_;
	std::vector<GeneratorRecord> generator_records_;
	std::vector<bool> buffet_type_records_;
};


#endif
