#include <iostream>

#include "simulator.h"
#include "variables.h"
#include "random_generators.h"
#include "timer.h"

int main ( ) {

	printf("-----------------------------------------------------------------------\n");
	printf("                   The Chinese Restaurant\n\n");
	printf("                                                      By Supawat Tamsri\n");
	printf("-----------------------------------------------------------------------\n");
	// Set up Questions
	int mode = -1, priority = -1, generate = -1;
	
	while (mode != 1 && mode != 0){
		printf("Select the simulation mode (0 - Non-stop, 1 - Step by Step): ");
		std::cin >> mode;
	}
	while (generate != 1 && generate != 0){
		printf("Would you like to re-generate the random kernels? ( 0 - No, 1 - Yes): ");
		std::cin >> generate;
	}
	while (priority < 1 || priority > 4){
		printf("Select logging priority (1-4): ");
		std::cin >> priority;
	}

	// Variable Initializing Form
	const Variables variables = {
			6,											// number of waiters
			{8, 14, 4},									// number of tables in each persons (2,3,4)
			{0.11f, 0.33f, 0.33f, 0.23f},	// possibility of persons in group (1,2,3,4)
			1500,									// average arrival interval of customer group
			100*100,								// variance arrival interval of customer group
			40,										// time from restaurant queue to the table
			4000,								// average waiter service time
			220,								// average cashier service time
			5,											// number of cashiers
			50,										// number of buffet seats
			.6f,							// probability of buffet customer group
			1900,									// average buffet service time
			200*200									// variance buffet service time
		};
	const unsigned int seed = 99;									// selected set of initial kernel set in kernel of generators
	std::string kernels_generated_file = "./kernels/generated.txt"; // path of generated kernels file.
	// Random Initializing Form
	const RandomInitializerForm random_init_form = {
								generate == 1,														// option for generating kernels or reusing kernels
								seed,																			// selected set of initial kernel set in kernel of generators
								kernels_generated_file,														// path to generated kernels file
								static_cast<int>(variables.average_arrival_interval),		// average arrival interval for initializing arrival generator
								static_cast<int>(variables.variance_arrival_interval),	// variance arrival interval for initializing arrival generator
								static_cast<int>(variables.average_waiter_service_time),	// average waiter service time for initializing waiter generator
								static_cast<int>(variables.average_buffet_time),			// average buffet service time for initializing buffet generator
								static_cast<int>(variables.variance_buffet_time),			// variance buffet service time for initializing buffet generator
								static_cast<int>(variables.average_cashier_service_time),	// average cashier service time for initializing buffet generator
								variables.probability_of_persons_in_group,										// probability of persons in group for initializing persons generator
								static_cast<double>(variables.probability_buffet_customer_group)				// probability of buffet type group for initializing buffet type generator
	};
	// Time Initializing Form
	const TimerForm timer_form = {
		28800,	//	 8 AM
		648000	//	 6 PM
						// this means duration is 10 hours (36000 seconds)
	};

	auto simulator = new Simulator(timer_form, variables, random_init_form); // constructing simulator

	simulator->Init(mode == 1, Log::LogPriority(priority));	// initializing simulation mode and log mode
	
	simulator->Run();

	simulator->Conclude();
	delete simulator;
	system("pause");
	return 0;
}
