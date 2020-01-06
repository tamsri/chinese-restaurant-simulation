#ifndef MODEL_VARIABLES_H
#define MODEL_VARIABLES_H

struct Variables {
	const unsigned int number_waiters;								// Number of Waiters
	const unsigned int number_tables[3];							// Number of Restaurant Tables
	std::vector<double> probability_of_persons_in_group;						// Probability of CustomerGroup's Number (1 2 3 4)
	const unsigned int average_arrival_interval;						// Average Time Interval between Two consecutive groups
	const unsigned int variance_arrival_interval;						// Variance Time Interval between Two consecutive groups
	const unsigned int time_queue_to_table;								// Time from Queue to Restaurant Table
	const unsigned int average_waiter_service_time;						// Average Waiter Service Time
	const unsigned int average_cashier_service_time;					// Average Cashier Service Time
	const unsigned int number_cashiers;								// Number of Cashiers
	const unsigned int number_buffet_seats;							// Number of Buffet Seat
	const double probability_buffet_customer_group;					// Probability of Buffet CustomerGroup
	const unsigned int average_buffet_time;							// Average Buffet Time
	const unsigned int variance_buffet_time;						// Variance Buffet Time
};
#endif // !MODEL_VARIABLES_H