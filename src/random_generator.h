#ifndef UTILS_RANDOM_GENERATOR_H
#define UTILS_RANDOM_GENERATOR_H

class NormalGenerator;
class UniformGenerator;
class ExponentialGenerator;
class Kernels;

struct RandomInitializerForm {

	// Parameters For Interval Arrival Time generator (Normal Generator)
	int average_interval_arrival_time;			// average interval arrival time of customer groups
	int variance_interval_arrival_time;			// variance interval arrival time of customer groups

	// Parameters for Waiter Service Time Generator (Exponential Generator) 
	int average_waiter_service_time;			// average waiter service time of customer groups

	// Parameters for Buffet Service Time Generator (Normal Generator)			
	int average_buffet_service_time;			// average buffet service time of customer groups
	int variance_buffet_service_time;			// variance buffet service time of customer groups

	// Parameters for Cashier Service Time Generator (Exponential Generator)
	int average_cashier_service_time;
};

struct RandomGenerator final{
	RandomGenerator();
	void Initialize( Kernels * kernels,	int set_index, RandomInitializerForm initializer_form);

	NormalGenerator *		interval_arrival_generator;		 // Time Interval Arrival Generator
	ExponentialGenerator *	waiter_service_generator;	 // Time Waiter Service Generator
	NormalGenerator *		buffet_service_generator;		 // Time Buffet Service Generator
	ExponentialGenerator *	cashier_service_generator;   // Time Cashier Service Generator
};
#endif
