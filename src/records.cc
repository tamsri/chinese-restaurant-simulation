#include "records.h"
#include "log.h"
#include <map>

Records::Records ( ) {
	Log::GetLog()->Print("Records Class is initialized");
}

void Records::PushCustomerRecord (const CustomerGroupRecord customer_group_record) {
	customer_group_records_.push_back(customer_group_record);
}

void Records::PushGeneratorRecord (const GeneratorRecord generator_record) {
	generator_records_.push_back(generator_record);
}

void Records::PushBuffetRecord (bool is_buffet) {
	buffet_type_records_.push_back(is_buffet);
}
unsigned int Records::FindMax(std::vector<unsigned int> data) {
	unsigned int max = 0;
	for (auto one : data)
		if (one > max) max = one;
	return max;
}
unsigned int Records::FindMin(std::vector<unsigned int> data) {
	auto min = data[0];
	for (auto one : data)
		if (one < min) min = one;
	return min;
}
void Records::PrintHistogram(std::vector<unsigned int> & data_set) {
	std::map<unsigned int, int> histogram;
	const auto step = (data_set.size() < 10) ? static_cast<int>(data_set.size()) : 10;
	const auto max = static_cast<int>(FindMax(data_set));
	const auto min = static_cast<int>(FindMin(data_set));
	const auto width = (max - min)/step;
	auto lower = min;	
	auto higher = lower + width;
	for(auto i = 0; i < step; ++i) {
		printf("%d to %d:\t\t", lower, higher);
		for(auto data : data_set) {
			if(data > static_cast<unsigned int>(lower) && data <= static_cast<unsigned int>(higher)) {
				histogram[i] += 1;
			}
		}
		for (auto j = 0; j < histogram[i]; ++j) printf("|");
		printf("\n");
		lower = higher+1;
		higher += width;
	}
}
void Records::ConcludeGenerators ( ) {
	std::string generators[] = {	"Interval Arrival Time Generator",
									"Waiter Service Time Generator",
									"Buffet Service Time Generator",
									"Cashier Service Time Generator",
								};
	// check first 4 generators
	for(auto i = 0; i != kPersonsGenerator; ++i) {
		printf("%s ", generators[i].c_str());
		std::vector<unsigned int> recorder;
		double average = 0;
		for (auto record : generator_records_)
			if (static_cast<Generators>(i) == record.generator_type) {
				average += record.value;
				recorder.push_back(record.value);
			};
		printf("(average generated: %.0f)\n", static_cast<double>(average / recorder.size()));
		PrintHistogram(recorder);
		//for(auto one : recorder) {
		//	printf("%d ", one);
		//}
		//printf("\n");
	}
	// check persons in group generators
	printf("Persons in Group Generator\n");
	std::map<int, int> persons;
	for(auto i = 1; i <= 4; ++i) {
		for(auto record : generator_records_) {
			if(record.generator_type == kPersonsGenerator && record.value == static_cast<unsigned int>(i)) {
				persons[i]++;
			}
		}
	}
	const auto total = persons[1] + persons[2] + persons[3] + persons[4];
	for (int i = 1; i <= 4; ++i)
		printf("%d persons: %d/%d (%.001f percent)\n" ,i, persons[i], total , persons[i]*100/ static_cast<double>(total));
	// check buffet type persons
	printf("Buffet Type Customer Group Generator:\n");
	std::map<bool, int> buffets;
	// printf("Buffet List: ");
	for(auto record: buffet_type_records_) {
		buffets[record]++;
		//if (record) printf("1 ");
		//else printf("0 ");
	}
	printf("%d Buffet Groups and %d Restaurant Group (%.001f percent)\n", buffets[true], buffets[false], buffets[true] *100/ static_cast<double>(buffet_type_records_.size()));
}
