#ifndef MODEL_CHINESE_RESTAURANT_H
#define MODEL_CHINESE_RESTAURANT_H

#include <queue>
#include <set>

class CustomerGroup;
class Manager;
class Waiter;
class Cashier;
class Table;
class Seat;
class Generator;
struct Variables;

struct ChineseRestaurant {

	Generator * random_generator{};						// pointer to the random generator
	Variables * variables{};							// pointer to variables of the restaurant

	Manager * manager{};								// pointer to the manager
	
	std::vector<Table *> free_restaurant_tables;		// free tables in restaurant
	std::vector<Seat * > free_buffet_seats;				// free buffet seats in restaurant
	
	std::queue<Waiter* > free_waiter_queue;				// free waiters in restaurant
	std::queue<Cashier * > free_cashiers;				// free cashiers in restaurant

	std::queue<CustomerGroup *> wait_waiter_queue;		// queue of customer group waiting for a waiter
	std::vector<CustomerGroup * > restaurant_queue;		// queue of customer group waiting for a table
	std::queue<CustomerGroup * > buffet_queue;			// queue of customer group waiting for buffet seats
	std::queue<CustomerGroup * > check_out_queue;		// queue of customer group waiting for a cashier

	std::set<Waiter *> waiters;							// waiters in restaurant
	std::set<Cashier *> cashiers;						// cashiers in restaurant
	std::set<Table *> tables;							// tables in restaurant
	std::set<Seat *> buffet_seats;						// buffet seats in restaurant
	std::set<CustomerGroup *> customers;				// customers in restaurant
};

#endif // MODEL_CHINESE_RESTAURANT_H
