#ifndef MODEL_CHINESE_RESTAURANT_H
#define MODEL_CHINESE_RESTAURANT_H

#include <queue>

class CustomerGroup;
class Manager;
class Waiter;
class Cashier;
class Table;
class Seat;
class Generator;
struct Variables;

struct ChineseRestaurant {

	Generator * random_generator;
	Variables * variables;

	Manager * manager;
	std::queue<Waiter * > free_waiters;
	std::queue<Cashier * > free_cashiers;
	std::vector<Table * > restaurant_tables;
	std::vector<Seat * > buffet_seats;

	std::queue<Waiter* > free_waiter_queue;
	std::deque<CustomerGroup * > restaurant_queue;
	std::queue<CustomerGroup * > buffet_queue;
	std::queue<CustomerGroup * > check_out_queue;

};

#endif // MODEL_CHINESE_RESTAURANT_H
