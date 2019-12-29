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
class Seat;
class Cashier;
class CustomerGroup : public Process {

public:
	CustomerGroup(ChineseRestaurant * chinese_restaurant);
	~CustomerGroup();

	enum State {
		kArriveState,
		
		kBuffetQueueState,
		kBuffetServiceState,

		kRestaurantQueueState,
		kRestaurantWaiterState,
		kRestaurantServiceState,
		
		kCheckoutQueueState,
		kCheckoutServiceState,
		kCompletedState
	};
	
	bool IsTerminated ( ) const;
	unsigned int GetCustomerGroupID() const;

	void Execute(unsigned int current_time) override;
	/*----------- variables in a customer group ----------------*/
	unsigned int PersonsInGroup() const;
	bool IsBuffetCustomer() const;

	// To be accessibly by managers
	void AssignTable(Table * table);
	void AssignState(const State state);
	void Activate(unsigned event_time) override;

private:
	/*----------- variables in a customer group ----------------*/
	ChineseRestaurant * chinese_restaurant_;
	static unsigned int customer_group_global_id_;
	unsigned int customer_group_id_;
	unsigned int service_time_;

	bool is_buffet_customer_;
	/*----------- variables for a restaurant customer group ----------------*/
	std::vector<Customer *> customer_members_;
	Cashier * cashier_{};
	/*----------- variables for a restaurant customer group ----------------*/
	Waiter * served_by_;
	Table * table_;
	/*----------- variables for a buffet customer group ----------------*/
	std::vector<Seat *> buffet_seats_;

	/*----------- variables for the process ----------------*/
	State state_;
	bool terminated_;
	/*------------------- Verbs of Class  ------------------------------*/
	// verbs for restaurant customer groups
	void SitOnTable();
	void LeaveTable();
	// verbs for buffet customer groups
	void SitOnBuffetSeats();
	void LeaveBuffetSeats();
	/*----------- methods for executing the process ----------------*/
	void CustomerGroupWaitsInRestaurantQueue (const unsigned int current_time);
	bool CustomerGroupWaitsInBuffetQueue(const  unsigned int current_time);
	bool CustomerGroupInCheckoutQueue(const unsigned int current_time);
	
	void CustomerGroupArrives ( const unsigned int current_time);
	void CreateNextCustomerGroup(const unsigned int current_time) const;
	bool CustomerGroupArrivesToTable(const unsigned int current_time);
	bool CustomerGroupInRestaurantService(const unsigned int current_time);
	bool CustomerGroupInBuffetService(const unsigned int current_time);
	bool CustomerGroupInCashier(const unsigned int current_time);


};
#endif // MODEL_CUSTOMER_GROUP_H
