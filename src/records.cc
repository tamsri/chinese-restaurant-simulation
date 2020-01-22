#include "records.h"
#include "log.h"
#include <map>
#include <fstream>
#include "timer.h"
#include "chinese_restaurant.h"

Records::Records (const RecorderForm recorder_form,
                  ChineseRestaurant * chinese_restaurant) : seed_(recorder_form.selected_seed),
                                                            start_time_(recorder_form.start_time),
                                                            end_time_(recorder_form.end_time),
                                                            number_groups_(0),
                                                            chinese_restaurant_(chinese_restaurant) {
	// Initialize queue records
	for (auto i = 0 ; i != kQueueEnd ; ++i) {
		queue_records_[static_cast<Queues>(i)].first  = start_time_;
		queue_records_[static_cast<Queues>(i)].second = 0;
	}
	Log::GetLog()->Print("Records Class is initialized");
}

void Records::PushCustomerRecord (const CustomerGroupRecord customer_group_record) {
	if (customer_group_record.record_time < start_time_) return;
	if (customer_group_record.record_time > end_time_) return;
	customer_group_records_[customer_group_record.id].push_back(customer_group_record);
}

void Records::PushGeneratorRecord (const GeneratorRecord generator_record) {
	if (generator_record.record_time < start_time_) return;
	if (generator_record.record_time > end_time_) return;
	generator_records_.push_back(generator_record);
}

void Records::PushBuffetRecord (const bool is_buffet) { buffet_type_records_.push_back(is_buffet); }

void Records::PushQueueRecord (const QueueRecord queue_record) {
	if (queue_record.record_time < start_time_) return;
	if (queue_record.record_time > end_time_) return;
	const auto last_check = queue_records_[queue_record.queue_type].first;
	queue_records_[queue_record.queue_type].second += (queue_record.record_time - last_check) * queue_record.queue_size;
	queue_records_[queue_record.queue_type].first = queue_record.record_time;
}

unsigned int Records::FindMax (const std::vector<unsigned int> & data) {
	unsigned int max = 0;
	for (auto one : data) if (one > max) max = one;
	return max;
}

unsigned int Records::FindMin (std::vector<unsigned int> data) {
	auto min = data[0];
	for (auto one : data) if (one < min) min = one;
	return min;
}

void Records::PrintHistogram (std::vector<unsigned int> & data_set) {
	std::map<unsigned int, int> histogram;
	const auto step  = data_set.size() < 10 ? static_cast<int>(data_set.size()) : 10;
	const auto max   = static_cast<int>(FindMax(data_set));
	const auto min   = static_cast<int>(FindMin(data_set));
	const auto width = (max - min) / step;
	auto lower       = min;
	auto higher      = lower + width;
	for (auto i = 0 ; i < step ; ++i) {
		printf("%d to %d:\t", lower, higher);
		for (auto data : data_set) {
			if (data > static_cast<unsigned int>(lower) && data <= static_cast<unsigned int>(higher)) {
				histogram[i] += 1;
			}
		}
		for (auto j = 0 ; j < histogram[i] ; ++j) printf("|");
		printf("\n");
		lower = higher + 1;
		higher += width;
	}
}

long double Records::FindVariance (std::vector<unsigned int> & data, long double & mean) const {
	long double result = 0;
	for (auto one : data) { result += pow(mean - one, 2); }
	result /= data.size();
	return result;
}

long double Records::FindAverage (std::vector<unsigned> & data) {
	long double result = 0;
	for (auto one : data) result += one;
	result /= data.size();
	return result;
}

void Records::ConcludeGenerators ( ) {
	printf("---------------------------------------------------------------------------------\n");
	printf("                              Generators Reports\n");
	printf("---------------------------------------------------------------------------------\n");
	// write records to file
	const auto records_file_path = "./records/generators-seed-" + std::to_string(seed_) + ".txt";
	std::ofstream record_file(records_file_path);
	if (!record_file.is_open())
		Log::GetLog()->Print("The recorder cannot write the file in " + records_file_path,
		                     Log::P1, Log::ERROR);

	std::string generators[] = {
			"\nInterval Arrival Time Generator",
			"\nWaiter Service Time Generator",
			"\nBuffet Service Time Generator",
			"\nCashier Service Time Generator",
		};
	// check first 4 generators
	for (auto i = 0 ; i != kPersonsGenerator ; ++i) {
		printf("%s ", generators[i].c_str());
		record_file << generators[i] << "\nGenerated values: ";
		std::vector<unsigned int> recorder;
		for (auto record : generator_records_)
			if (static_cast<Generators>(i) == record.generator_type) {
				recorder.push_back(record.value);
				record_file << record.value << " ";
			}
		auto average        = FindAverage(recorder);
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
	for (auto i = 1 ; i <= 4 ; ++i) {
		for (auto record : generator_records_) {
			if (record.generator_type == kPersonsGenerator && record.value == static_cast<unsigned int>(i)) {
				persons[i]++;
				record_file << i << " ";
			}
		}
	}
	const auto total = persons[1] + persons[2] + persons[3] + persons[4];
	record_file << "\nSummary\n";
	for (auto i = 1 ; i <= 4 ; ++i) {
		printf("%d persons group: %d/%d (%.001f percent)\n", i, persons[i], total,
		       persons[i] * 100 / static_cast<double>(total));
		record_file << i << " persons group: " << persons[i] << " groups (" << persons[i] * 100 / static_cast<double>(
			total) << " %)\n";
	}
	record_file << "\n";
	// check buffet type persons
	printf("\nBuffet Type Customer Group Generator\n");
	record_file << "Buffet Type Customer Group Generator \nType of Customer Groups (1 - Buffet, 0 - Restaurant): ";
	std::map<bool, int> buffets;
	for (auto record : buffet_type_records_) {
		buffets[record]++;
		record_file << record << " ";
	}
	printf("%d Buffet Groups and %d Restaurant Groups (%.001f percents buffet customers)\n", buffets[true],
	       buffets[false], buffets[true] * 100 / static_cast<double>(buffet_type_records_.size()));
	record_file << "\nSummary\n";
	record_file << buffets[true] << " Buffet Groups and " << buffets[false] << " Restaurant Groups (" << buffets[true] *
		100 / static_cast<double>(buffet_type_records_.size()) << " % Buffet Groups)";
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
	const auto records_file_path = "./records/customers-seed-" + std::to_string(seed_) + ".txt";
	std::ofstream record_file(records_file_path);
	if (!record_file.is_open())
		Log::GetLog()->Print("The recorder cannot write the file in " + records_file_path,
		                     Log::P1, Log::ERROR);
	record_file << "\t\t\tCustomer Groups - Seed: " << std::to_string(seed_) << "\n";
	record_file << "----------------------------------------------------------------------------------\n";
	record_file << "State numbers\t|\tActivity Times(seconds)\t|\tActions\n";
	record_file << "----------------------------------------------------------------------------------\n";
	for (const auto & customer : customer_group_records_) {
		record_file << "Customer Group #" << customer.first << ":\n";
		for (auto activity : customer.second) {
			record_file << "\t" << activity.record_state << "\t|\t\t" << activity.record_time << "\t\t|\t" <<
				activity_list[activity.record_state] << std::endl;
		}
	}
	record_file.close();
}

void Records::ConcludeResult ( ) {
	long double average_table_wait    = 0;
	long double average_waiter_wait   = 0;
	long double average_buffet_wait   = 0;
	long double average_checkout_wait = 0;
	unsigned int restaurant_group     = 0;
	unsigned int wait_waiter_group    = 0;
	unsigned int buffet_group         = 0;
	unsigned int checkout_group       = 0;
	std::map<unsigned int, unsigned int> wait_table_groups;
	// finding average table wait
	for (const auto & customer : customer_group_records_) {
		auto enter_queue_start   = start_time_;
		auto wait_waiter_start   = start_time_;
		auto wait_checkout_start = start_time_;

		auto wait_waiter_end   = end_time_;
		auto enter_queue_end   = end_time_;
		auto wait_checkout_end = end_time_;

		auto is_buffet     = false;
		auto is_restaurant = false;
		auto is_checkout   = false;
		auto is_waiter     = false;

		// Scanning activities of a customer
		for (auto activity : customer.second) {
			switch (activity.record_state) {
			case CustomerGroup::kBuffetQueueState:
				enter_queue_start = activity.record_time;
				break;
			case CustomerGroup::kRestaurantQueueState:
				enter_queue_start = activity.record_time;
				break;
			case CustomerGroup::kBuffetServiceState:
				is_buffet = true; //Calculate only groups who finish waiting
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
				break;
			default:
				break;
			}
		}
		// Calculate average buffet wait
		if (is_buffet) {
			average_buffet_wait += enter_queue_end - enter_queue_start;
			buffet_group++;
		}
		// calculate average b
		if (is_restaurant) {
			average_table_wait += enter_queue_end - enter_queue_start;
			wait_table_groups[restaurant_group] = enter_queue_end - enter_queue_start;
			restaurant_group++;
		}
		if (is_waiter) {
			average_waiter_wait += wait_waiter_end - wait_waiter_start;
			wait_waiter_group++;
		}
		if (is_checkout) {
			average_checkout_wait += wait_checkout_end - wait_checkout_start;
			checkout_group++;
		}
	}
	number_groups_ = buffet_group + restaurant_group;
	average_table_wait /= restaurant_group;
	average_buffet_wait /= buffet_group;
	average_waiter_wait /= wait_waiter_group;
	average_checkout_wait /= checkout_group;

	// Conclude Queue Records
	for (auto i = 0 ; i != kQueueEnd ; ++i) {
		const auto end_time = queue_records_[static_cast<Queues>(i)].first;
		queue_records_[static_cast<Queues>(i)].second /= end_time - start_time_;
	}
	printf("---------------------------------------------------------------------------------\n");
	printf("                             Simulations Results\n");
	printf("---------------------------------------------------------------------------------\n");
	printf("                                   General Data\n");
	printf("Arrival Customer Group Rate: \t %.2LF groups/hour\n",
	       number_groups_ * 6000 / static_cast<long double>((end_time_ - start_time_)));
	printf("                             Average Waiting Times\n");
	printf("Restaurant Queue Waiting Time:\t %.2Lf seconds \t\t(%s)\n", average_table_wait,
	       Timer::SecondsToMany(static_cast<unsigned int>(average_table_wait)).c_str());
	printf("Buffet Seats Waiting Time:\t %.02Lf seconds \t\t(%s)\n", average_buffet_wait,
	       Timer::SecondsToMany(static_cast<unsigned int>(average_buffet_wait)).c_str());
	printf("Waiter Waiting Time on Table:\t %.02Lf seconds \t\t(%s)\n", average_waiter_wait,
	       Timer::SecondsToMany(static_cast<unsigned int>(average_waiter_wait)).c_str());
	printf("Checkout Queue Waiting Time:\t %.02Lf seconds \t\t(%s)\n", average_checkout_wait,
	       Timer::SecondsToMany(static_cast<unsigned int>(average_checkout_wait)).c_str());
	printf("                             Average Queue Lengths\n");
	std::string queues_intro[] = {
			"Table Queue Length:\t",
			"Buffet Queue Length:\t",
			"Free Waiter Queue Length:",
			"Waiting Waiter Queue Length:",
			"Checkout Queue Length:\t",
			"Cashier Queue Length:\t"
		};
	for (auto i = 0 ; i != kQueueEnd ; ++i) {
		printf("%s\t %.2Lf \t\t\n", queues_intro[i].c_str()
		     , queue_records_[static_cast<Queues>(i)].second);
	}

	// push wanted result into file
	const auto result_file_path = "./result/results-seed-" + std::to_string(seed_) + ".txt";
	std::ofstream result_file(result_file_path);
	if (!result_file.is_open())
		Log::GetLog()->Print("The recorder cannot write the file in " + result_file_path,
		                     Log::P1, Log::ERROR);
	result_file << "The Simulation Result\n";
	result_file << "Seed Number:" << seed_ << " || Total Customer Groups: " << number_groups_;
	result_file << "\nStart Record Time: " << start_time_ << " End Record Time: " << end_time_;
	result_file << "\nthe average waiting time for the table:\t " << average_table_wait;
	result_file << "\nthe average table queue length:\t\t " << queue_records_[kTableQueue].second;
	result_file << "\nthe average waiting time for the waiter: " << average_waiter_wait;
	result_file << "\nthe average cashier queue length:\t" << queue_records_[kCheckoutQueue].second;
	result_file << "\n" << seed_ << "\t" << average_table_wait << "\t" << queue_records_[kTableQueue].second
		<< "\t" << average_waiter_wait << "\t" << queue_records_[kCheckoutQueue].second;
	result_file.close();

	// push waiting table time of each customer group
	const auto wait_record_file_path = "./result/waiting-table-seed-" + std::to_string(seed_) + ".txt";
	std::ofstream wait_record_file(wait_record_file_path);
	if (!wait_record_file.is_open())
		Log::GetLog()->
			Print("The recorder cannot write the file in " + wait_record_file_path, Log::P1, Log::ERROR);
	//wait_record_file << "Waiting Table Customer Groups\n";
	//wait_record_file << "Seed Number:" << seed_ << " || Total Customer Groups: " << number_groups_;
	//wait_record_file << "\nStart Record Time: " << start_time_ << " End Record Time: " << end_time_;
	//wait_record_file << "\nCustomer Group\tWaiting Time for Table\n\n";
	wait_record_file << seed_ << '\n';
	wait_record_file << "Waiting Time\n(seconds)\n";
	for (auto & wait_table_group : wait_table_groups
	)
		wait_record_file /*<< wait_table_group.first << "\t"*/ << wait_table_group.second << "\n";
	wait_record_file.close();

}
