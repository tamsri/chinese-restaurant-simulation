#include "records.h"
#include "log.h"
#include <map>
#include <fstream>

Records::Records (unsigned int seed): seed_(seed) {
	Log::GetLog()->Print("Records Class is initialized");
}

void Records::PushCustomerRecord (const CustomerGroupRecord customer_group_record) {
	customer_group_records_[customer_group_record.id].push_back(customer_group_record);
}

void Records::PushGeneratorRecord (const GeneratorRecord generator_record) {
	generator_records_.push_back(generator_record);
}

void Records::PushBuffetRecord (bool is_buffet) {
	buffet_type_records_.push_back(is_buffet);
}
unsigned int Records::FindMax(const std::vector<unsigned int> & data) {
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
		printf("%d to %d:\t", lower, higher);
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
double Records::FindVariance(std::vector<unsigned int> & data, double & mean) const {
	double result = 0;
	for(auto one : data) {
		result += pow(mean - one, 2);
	}
	result /= data.size();
	return result;
}

double Records::FindAverage (std::vector<unsigned> & data) {
	double result = 0;
	for (auto one : data)
		result += one;
	result /= data.size();
	return result;
}

void Records::ConcludeGenerators ( ) {
	// write records to file
	const std::string records_file_path = "./records/generators-seed-" + std::to_string(seed_) +".txt";
	std::ofstream record_file(records_file_path);
	if (!record_file.is_open())
		Log::GetLog()->Print("The recorder cannot write the file in " + records_file_path, Log::P1,Log::ERROR);
	
	std::string generators[] = {	"\nInterval Arrival Time Generator",
									"\nWaiter Service Time Generator",
									"\nBuffet Service Time Generator",
									"\nCashier Service Time Generator",
								};
	// check first 4 generators
	for(auto i = 0; i != kPersonsGenerator; ++i) {
		printf("%s ", generators[i].c_str());
		record_file << generators[i] << "\nGenerated values: ";
		std::vector<unsigned int> recorder;
		for (auto record : generator_records_)
			if (static_cast<Generators>(i) == record.generator_type) {
				recorder.push_back(record.value);
				record_file << record.value << " ";
			};
		auto average = FindAverage(recorder);
		const auto variance = FindVariance(recorder, average);
		printf("\naverage generated: %.0f\n", average);
		printf("variance generated: %.0f^2\n", sqrt(variance));
		PrintHistogram(recorder);
		record_file << "\nSummary\n";
		record_file << "The average value of generated data: " << average << std::endl;
		record_file << "The variance value of generated data: " << variance << "(" << sqrt(variance) << "^2)\n\n";
	}
	// check persons in group generators
	printf("\nPersons in Group Generator\n");
	record_file << "Persons in Group Generator\nNumber of Persons in Groups: ";
	std::map<int, int> persons;
	for(auto i = 1; i <= 4; ++i) {
		for(auto record : generator_records_) {
			if(record.generator_type == kPersonsGenerator && record.value == static_cast<unsigned int>(i)) {
				persons[i]++;
				record_file << i << " ";
			}
		}
	}
	const auto total = persons[1] + persons[2] + persons[3] + persons[4];
	record_file << "\nSummary\n";
	for (int i = 1; i <= 4; ++i) {
		printf("%d persons group: %d/%d (%.001f percent)\n" ,i, persons[i], total , persons[i]*100/ static_cast<double>(total));
		record_file << i << " persons group: " << persons[i] << " groups (" << persons[i] * 100 / static_cast<double>(total) << " %)\n";
	}
	record_file << "\n";
	// check buffet type persons
	printf("\nBuffet Type Customer Group Generator\n");
	record_file << "Buffet Type Customer Group Generator \nType of Customer Groups (1 - Buffet, 0 - Restaurant): ";
	std::map<bool, int> buffets;
	for(auto record: buffet_type_records_) {
		buffets[record]++;
		record_file << record << " ";
	}
	printf("%d Buffet Groups and %d Restaurant Groups (%.001f percents buffet customers)\n", buffets[true], buffets[false], buffets[true] *100/ static_cast<double>(buffet_type_records_.size()));
	record_file << "\nSummary\n";
	record_file << buffets[true] << " Buffet Groups and" << buffets[false] << " Restaurant Groups (" << buffets[true] * 100 / static_cast<double>(buffet_type_records_.size()) << " % Buffet Groups)";
	record_file.close();
}

void Records::ConcludeCustomers ( ) {
	const auto records_file_path = "./records/customers-seed-" + std::to_string(seed_) +".txt";
	std::ofstream record_file(records_file_path);
	if (!record_file.is_open())
		Log::GetLog()->Print("The recorder cannot write the file in " + records_file_path, Log::P1, Log::ERROR);

	record_file.close();
}
