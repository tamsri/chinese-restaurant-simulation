#ifndef UTILS_RANDOM_GENERATOR_H
#define UTILS_RANDOM_GENERATOR_H
#include <vector>

class NormalGenerator;
class UniformGenerator;
class ExponentialGenerator;
class ProbabilityGenerator;
class BoolProbabilityGenerator;

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
	// Parameters for Persons in Group Generator (Probability by Uniform Generator)
	std::vector<double> persons_in_group_probability; // persons in group probability
	// Parameters for Persons in Group Generator (Probability by Uniform Generator)
	double buffet_probability;					// buffet probability
};

struct RandomGenerators{
	RandomGenerators();
	void Initialize( Kernels * kernels,	int set_index, const RandomInitializerForm & initializer_form);

	NormalGenerator *		interval_arrival_generator;		 // Time Interval Arrival Generator
	ExponentialGenerator *	waiter_service_generator;	 // Time Waiter Service Generator
	NormalGenerator *		buffet_service_generator;		 // Time Buffet Service Generator
	ExponentialGenerator *	cashier_service_generator;   // Time Cashier Service Generator
	ProbabilityGenerator *		persons_generator;
	BoolProbabilityGenerator *  buffet_generator;
};
#endif
