#include <iostream>

#include "simulator.h"
#include "variables.h"

#include "random_generators.h"
#include "timer.h"
#include "records.h"

int main ( ) {
	// Enable to simulate multiple times
	// bool is_multiple_sim = true;
	// Multiple simulations for multiple seeds
	// int seeds[10] = { 17,19,23,31,39,68,71,76,90,95 };
	//for (i = 0; i < 10; ++i) {
	printf("-----------------------------------------------------------------------\n");
	printf("                     The Chinese Restaurant [24/7]\n\n");
	printf("                                                      By Supawat Tamsri\n");
	printf("-----------------------------------------------------------------------\n");
	/*------- Setup Questions --------*/
	auto mode = -1, priority = -1, generate = -1, print_event_list = -1, status = -1;
	auto seed = -1;
	// selected set of initial kernel set in kernel of generators, seeds[i] if multiple.
	long long start_simulation_time = -1; // start simulation time
	long long end_simulation_time   = -1; // end simulation time
	long long start_record_time     = -1; // start record time
	long long end_record_time       = -1; // end record time
	/*------ Time Guide --------*/
	// One hour == 3600 seconds
	// 8 AM = 28800
	// 6 PM = 64800

	/*------------------------------------------------- User Input Phase -------------------------------------------------*/
	while (mode != 1 && mode != 0) {
		printf("Select the simulation mode (0 - Non-stop, 1 - Step by Step): ");
		std::cin >> mode;
	}
	while (generate != 1 && generate != 0) {
		printf("Would you like to re-generate the random kernels? ( 0 - No, 1 - Yes): ");
		std::cin >> generate;
	}
	while (priority < 1 || priority > 4) {
		printf("Select logging priority (1-4): ");
		std::cin >> priority;
	}
	while (status != 1 && status != 0) {
		printf("Would you like to turn status on (0 - No, 1 - Yes): ");
		std::cin >> status;
	}
	while (print_event_list != 1 && print_event_list != 0) {
		printf("Would you like to print the future event list (0 - No, 1 - Yes): ");
		std::cin >> print_event_list;
	}
	while (start_simulation_time < 0) {
		printf("Enter Start Simulation time in seconds (Ex, 8AM = 28800 seconds): ");
		std::cin >> start_simulation_time;
	}
	while (end_simulation_time <= 0 || end_simulation_time <= start_simulation_time) {
		printf("Enter End Simulation time in seconds (Ex, 6PM = 64800 seconds): ");
		std::cin >> end_simulation_time;
		if (end_simulation_time <= start_simulation_time
		)
			printf("<!!> End simulation time must be greater than start simulation time <!!>\n");
	}
	while (start_record_time < 0 || start_record_time < start_simulation_time || start_record_time > end_simulation_time
	) {
		printf("Enter Start Record time in seconds (Ex, 8AM = 28800 seconds): ");
		std::cin >> start_record_time;
		if (start_record_time < start_simulation_time
		)
			printf("<!!> Start Record time must be greater than start simulation time <!!>\n");
		if (start_record_time > end_simulation_time
		)
			printf("<!!> Start Record time must be less than end simulation time <!!>\n");
	}
	while (end_record_time < 0 || end_record_time < start_record_time || end_record_time > end_simulation_time) {
		printf("Enter End Record time in seconds (Ex, 6PM = 64800 seconds): ");
		std::cin >> end_record_time;
		if (end_record_time < start_record_time
		)
			printf("<!!> End Record time must be greater than start record time <!!>\n");
		if (end_record_time > end_simulation_time
		)
			printf("<!!> End Record time must be less than end simulation time <!!>\n");
	}
	while (seed < 0 || seed > 99) {
		printf("Enter Seed for simulation (0 - 99): ");
		std::cin >> seed;
		if (seed < 0 || seed > 99) printf("<!!> Seed must be number from 0 to 99 <!!>\n");
	}
	/*------------------------------------------------- Initializing -------------------------------------------------*/

	// Variable Initializing Form

	const Variables variables = {
			6,                            // number of waiters
			{6, 5, 4},                    // number of tables in each persons (2,3,4)
			{0.11f, 0.33f, 0.33f, 0.23f}, // possibility of persons in group (1,2,3,4)
			600,                          // average arrival interval of customer group
			60 * 60,                      // variance arrival interval of customer group
			1000,                         // time from restaurant queue to the table
			5500,                         // average waiter service time
			250,                          // average cashier service time
			1,                            // number of cashiers
			20,                           // number of buffet seats
			.45f,                         // probability of buffet customer group
			6200,                         // average buffet service time
			500 * 500                     // variance buffet service time
		};

	std::string kernels_generated_file = "./kernels/generated.txt";
	// path of generated kernels file for random generator.
	// Random Initializing Form
	const RandomInitializerForm random_init_form = {
			generate == 1,                   // option for generating kernels or reusing kernels
			static_cast<unsigned int>(seed), // selected set of initial kernel set in kernel of generators
			kernels_generated_file,          // path to generated kernels file
			static_cast<int>(variables.average_arrival_interval),
			// average arrival interval for initializing arrival generator
			static_cast<int>(variables.variance_arrival_interval),
			// variance arrival interval for initializing arrival generator
			static_cast<int>(variables.average_waiter_service_time),
			// average waiter service time for initializing waiter generator
			static_cast<int>(variables.average_buffet_time),
			// average buffet service time for initializing buffet generator
			static_cast<int>(variables.variance_buffet_time),
			// variance buffet service time for initializing buffet generator
			static_cast<int>(variables.average_cashier_service_time),
			// average cashier service time for initializing buffet generator
			variables.probability_of_persons_in_group,
			// probability of persons in group for initializing persons generator
			static_cast<double>(variables.probability_buffet_customer_group)
			// probability of buffet type group for initializing buffet type generator
		};
	// Time Initializing Form
	const TimerForm timer_form = {
			static_cast<unsigned int>(start_simulation_time),
			static_cast<unsigned int>(end_simulation_time)
		};
	// Initialize Recorder
	const RecorderForm recorder_form = {
			static_cast<unsigned int>(seed),
			static_cast<unsigned int>(start_record_time),
			static_cast<unsigned int>(end_record_time)
		};

	/*------------------------------------------------- Initialize Simulation -------------------------------------------------*/
	auto simulator = new Simulator(timer_form, variables, random_init_form, recorder_form);
	simulator->Init(mode == 1, Log::LogPriority(priority), status == 1, print_event_list == 1);
	/*------------------------------------------------- Run Simulation -------------------------------------------------*/
	simulator->Run();
	/*------------------------------------------------- Record Simulation -------------------------------------------------*/
	simulator->Conclude();
	simulator->CleanRestaurant();
	delete simulator;
	//}
	system("pause");
	return 0;
}
