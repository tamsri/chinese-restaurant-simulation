#include "records.h"
#include "log.h"
#include <map>
#include <fstream>
#include "timer.h"

Records::Records(const RecorderForm recorder_form) :	seed_(recorder_form.selected_seed),
														start_time_(recorder_form.start_time),
														end_time_(recorder_form.end_time){
	Log::GetLog()->Print("Records Class is initialized");
}

void Records::PushCustomerRecord (const CustomerGroupRecord customer_group_record) {
	customer_group_records_[customer_group_record.id].push_back(customer_group_record);
}

void Records::PushGeneratorRecord (const GeneratorRecord generator_record) {
	generator_records_.push_back(generator_record);
}

void Records::PushBuffetRecord (const bool is_buffet) {
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

long double Records::FindVariance(std::vector<unsigned int> & data, long double & mean) const {
	long double result = 0;
	for(auto one : data) {
		result += pow(mean - one, 2);
	}
	result /= data.size();
	return result;
}

long double Records::FindAverage (std::vector<unsigned> & data) {
	long double result = 0;
	for (auto one : data)
		result += one;
	result /= data.size();
	return result;
}

void Records::ConcludeGenerators ( ) {
	printf("---------------------------------------------------------------------------------\n");
	printf("                              Generators Reports\n");
	printf("---------------------------------------------------------------------------------\n");
	// write records to file
	const auto records_file_path = "./records/generators-seed-" + std::to_string(seed_) +".txt";
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
		printf("\naverage generated: %.0Lf\n", average);
		printf("variance generated: %.0Lf^2\n", sqrt(variance));
		//PrintHistogram(recorder);
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
	record_file << buffets[true] << " Buffet Groups and " << buffets[false] << " Restaurant Groups (" << buffets[true] * 100 / static_cast<double>(buffet_type_records_.size()) << " % Buffet Groups)";
	record_file.close();
}

void Records::ConcludeCustomers ( ) {
	std::string activity_list[] = {
		"arrives to restaurant",
		"enters buffet queue",
		"starts buffet service",
		"enters restaurant queue",
		"assigned to table",
		"waits for a waiter",
		"starts restaurant service",
		"leaves the service",
		"enters the checkout queue",
		"starts checkout service",
		"finishes checkout & leaves"
	};
	const auto records_file_path = "./records/customers-seed-" + std::to_string(seed_) +".txt";
	std::ofstream record_file(records_file_path);
	if (!record_file.is_open())
		Log::GetLog()->Print("The recorder cannot write the file in " + records_file_path, Log::P1, Log::ERROR);
	record_file << "\t\t\tCustomer Groups - Seed: " << std::to_string(seed_) <<"\n";
	record_file << "----------------------------------------------------------------------------------\n";
	record_file << "State numbers\t|\tActivity Times(seconds)\t|\tActions\n";
	record_file << "----------------------------------------------------------------------------------\n";
	for(const auto & customer : customer_group_records_) {
		record_file << "Customer Group #" << customer.first << ":\n";
		for(auto activity : customer.second) {
			record_file << "\t"<< activity.record_state << "\t|\t\t" << activity.record_time <<"\t\t|\t" << activity_list[activity.record_state]<< std::endl;
		}
	}
	record_file.close();

	long double average_table_wait = 0;
	long double average_waiter_wait = 0;
	long double average_buffet_wait = 0;  
	long double average_checkout_wait = 0;
	unsigned int restaurant_group = 0;
	unsigned int wait_waiter_group = 0;
	unsigned int buffet_group = 0;
	unsigned int checkout_group = 0;
	// finding average table wait
	for(const auto & customer: customer_group_records_) {
		auto enter_queue_start = start_time_;
		auto wait_waiter_start = start_time_;
		auto wait_checkout_start = start_time_;
		auto wait_waiter_end = end_time_;
		auto enter_queue_end = end_time_;
		auto wait_checkout_end = end_time_;
		auto is_buffet = false;
		auto is_restaurant = false;
		auto is_checkout = false;
		auto is_waiter = false;
		// Scanning activities of a customer
		for(auto activity : customer.second) {			
			switch (activity.record_state) {
			case CustomerGroup::kBuffetQueueState:
				enter_queue_start = activity.record_time;
				break;
			case CustomerGroup::kRestaurantQueueState:
				enter_queue_start = activity.record_time;
				break;
			case CustomerGroup::kBuffetServiceState:
				is_buffet = true;
				enter_queue_end = activity.record_time;
				break;
			case CustomerGroup::kRestaurantArriveTableSate:
				is_restaurant = true;
				enter_queue_end = activity.record_time;
				break;
			case CustomerGroup::kRestaurantWaiterState:
				wait_waiter_start = activity.record_time;
				break;
			case CustomerGroup::kRestaurantServiceState:
				wait_waiter_end = activity.record_time;
				is_waiter = true;
				break;
			case CustomerGroup::kCheckoutQueueState:
				wait_checkout_start = activity.record_time;
				break;
			case CustomerGroup::kCheckoutServiceState:
				wait_checkout_end = activity.record_time;
				is_checkout = true;
			default:
				break;
			}
		}
		if(is_buffet) {
			average_buffet_wait += enter_queue_end - enter_queue_start;
			buffet_group++;
		}
		if(is_restaurant){
			average_table_wait += enter_queue_end - enter_queue_start;
			restaurant_group++;
		}
		if (is_waiter) {
			average_waiter_wait += wait_waiter_end - wait_waiter_start;
			wait_waiter_group++;
		}
		if(is_checkout) {
			average_checkout_wait += wait_checkout_end - wait_checkout_start;
			checkout_group++;
		}
	}
	average_table_wait /= restaurant_group;
	average_buffet_wait /= buffet_group;
	average_waiter_wait /= wait_waiter_group;
	average_checkout_wait /= checkout_group;
	printf("---------------------------------------------------------------------------------\n");
	printf("                             Simulations Reports\n");
	printf("---------------------------------------------------------------------------------\n");
	printf("Average Restaurant Queue Waiting Time:\t %.2Lf seconds \t\t(%s)\n", average_table_wait, Timer::SecondsToMany(static_cast<unsigned int>(average_table_wait)).c_str());
	printf("Average Buffet Seats Waiting Time:\t %.02Lf seconds \t\t(%s)\n", average_buffet_wait, Timer::SecondsToMany(static_cast<unsigned int >(average_buffet_wait)).c_str());
	printf("Average Waiter Waiting Time on Table:\t %.02Lf seconds \t\t(%s)\n", average_waiter_wait, Timer::SecondsToMany(static_cast<unsigned int>(average_waiter_wait)).c_str());
	printf("Average Checkout Queue Waiting Time:\t %.02Lf seconds \t\t(%s)\n", average_checkout_wait, Timer::SecondsToMany(static_cast<unsigned int>(average_checkout_wait)).c_str());
}
