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
	
	std::vector<Table *> free_restaurant_tables;	 // free tables in restaurant
	std::vector<Seat * > free_buffet_seats;			 // free buffet seats in restaurant
	
	std::queue<Waiter* > free_waiter_queue;			 // free waiters in restaurant
	std::queue<Cashier * > free_cashiers;			 // free cashiers in restaurant

	std::queue<CustomerGroup *> wait_waiter_queue;
	std::vector<CustomerGroup * > restaurant_queue;
	std::queue<CustomerGroup * > buffet_queue;
	std::queue<CustomerGroup * > check_out_queue;

};

#endif // MODEL_CHINESE_RESTAURANT_H
