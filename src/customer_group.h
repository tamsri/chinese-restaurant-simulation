#ifndef MODEL_CUSTOMER_GROUP_H
#define MODEL_CUSTOMER_GROUP_H

#include <vector>
#include "process.h"
struct ChineseRestaurant;
class Process;
class Customer;
class Log;
class Table;
class Waiter;

class CustomerGroup : public Process {

public:
	CustomerGroup(ChineseRestaurant * chinese_restaurant);
	~CustomerGroup();

	
	bool IsTerminated ( ) const;
	unsigned int GetCustomerGroupID() const;

	void Execute(unsigned int current_time) override;
	// To get data in class	
	unsigned int PersonsInGroup() const;
	bool IsBuffetCustomer() const;

	// To edit data in class
	void EnterTable(Table * table);
	void LeaveTable(Table * table);
	
	enum State {
		ArriveState,
		
		BuffetQueueState,
		BuffetServiceState,

		RestaurantQueueState,
		RestaurantWaiterState,
		RestaurantServiceState,
		
		CheckoutQueueState,
		CheckoutServiceState,
		CompletedState
	};

private:
	ChineseRestaurant * chinese_restaurant_;

	std::vector<Customer *> customer_members_;
	Waiter * served_by_;
	Table * table_;
	State state_;
	
	static unsigned int customer_group_global_id_;
	unsigned int customer_group_id_;
	bool terminated_;
	unsigned int service_time_;
	bool is_buffet_customer_;


	// methods for executing the process
	bool CustomerGroupWaitsInRestaurantQueue (const unsigned int current_time);
	bool CustomerGroupWaitsInBuffetQueue(const  unsigned int current_time);
	bool CustomerGroupInCheckoutQueue(const unsigned int current_time);
	
	bool CustomerGroupArrives ( const unsigned current_time);
	bool CreateNextCustomerGroup(const unsigned int current_time) const;
	bool CustomerGroupArrivesToTable(const unsigned int current_time);
	bool CustomerGroupInRestaurantService(const unsigned int current_time);
	bool CustomerGroupInBuffetService(const unsigned int current_time);
	bool CustomerGroupInCashier(const unsigned int current_time);

};

#endif // MODEL_CUSTOMER_GROUP_H