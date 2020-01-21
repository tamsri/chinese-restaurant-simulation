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

class CustomerGroup final {
public:
	CustomerGroup(ChineseRestaurant * chinese_restaurant, Process * process, unsigned int current_time);
	~CustomerGroup();
	enum State {
			// states for 
		kArriveState,							// state when the customer group arrives to the restaurant
			// states for buffet customer group
		kBuffetQueueState,						// state when the customer group arrives the the buffet queue
		kBuffetServiceState,					// state when the customer group start the service after arriving to the seats
			// states for restaurant customer group
		kRestaurantQueueState,					// state when the customer group arrives to the restaurant queue
		kRestaurantArriveTableSate,
		kRestaurantWaiterState,					// state when the customer group arrives to the table and wait for the waiter
		kRestaurantServiceState,				// state when the customer group start the service after a waiter arrives
			// states for checkout
		kLeaveServiceState,
		kCheckoutQueueState,					// state when the customer group finish the service and arrive to checkout queue
		kCheckoutServiceState,					// state when the customer group start the checkout service
		kCompletedState							// state when the customer group finish leave the restaurant
	};
	
	bool IsTerminated ( ) const;					// get a signal if the customer group is to be terminated
	unsigned int GetCustomerGroupId() const;		// get the id of the customer group
	/*----------- variables in a customer group ----------------*/
	unsigned int PersonsInGroup() const;			// get the number of persons in the customer group
	bool IsBuffetCustomer() const;					// get the type of the customer group (buffet or restaurant)
	Customer * GetCustomerMember(unsigned int position); // get the customer member according to position
	
	// To be accessible by managers
	void AssignTable(Table * table);				// assign the table to the customer group for manager
	void AssignState(const State state);			// assign the state to the customer group for manager
	void Activate(unsigned int current_time);		// activate the customer group for manager
	// To be accessible by simulator
	void Execute(unsigned int current_time);		// execute the process inside the customer group
	// verbs to prevent contemporary bugs
	void Call();
	void UnCall();
	bool IsCalled() const;
private:
	/*----------- variables in a customer group ----------------*/
	
	ChineseRestaurant * chinese_restaurant_;		// the pointer attribute to the chinese restaurant
	static unsigned int customer_group_global_id_;  // the global id for counting customer group
	unsigned int customer_group_id_;				// the id of the customer group
	unsigned int service_time_;						// the initially generated service time of the customer group
	unsigned int cashier_time_;						// the initially generated cashier time of the customer group
	bool is_buffet_customer_;						// indicator type of the customer group
	bool is_called_;
	/*----------- variables for a restaurant customer group ----------------*/
	
	std::vector<Customer *> customer_members_;		// customer members in the customer group
	Cashier * cashier_;								// the cashier that the customer group occupy
	
	/*----------- variables for a restaurant customer group ----------------*/
	
	Waiter * served_by_;							// the waiter that served the customer group
	Table * table_;									// the table that customer group occupy
	
	/*----------- variables for a buffet customer group ----------------*/
	
	std::vector<Seat *> buffet_seats_; // the vector of buffet seats that customer members occupy

	/*----------- variables for the process ----------------*/
	Process * process_;								// the reference of the process
	State state_;									// the state of the customer group
	bool terminated_;								// the indicator for terminating customer
	Log * log_;
	/*------------------- Verbs of Class  ------------------------------*/
	
	
	// verbs for restaurant customer groups
	void CallManager(const unsigned int current_time) const;	// the action for customer group to call the manager to provide the table
	void SitOnTable();											// the action for customer group sitting on the table
	// verbs for buffet customer groups
	void SitOnBuffetSeats();	// the action for customer group sitting on buffet seats
	void LeaveBuffetSeats();	// the action for customer group leaving the buffet seats
	/*----------- methods for executing the process ----------------*/

	// executes for customer group on queues
	void CustomerGroupArrives ( const unsigned int current_time); // execute arriving customer group to enter their type service
	void CreateNextCustomerGroup(const unsigned int current_time) const; // execute generating the next customer group

	bool CustomerGroupWaitsTheWaiter (const unsigned int current_time);
	bool CustomerGroupWaitsInRestaurantQueue (const unsigned int current_time); // execute customer group waiting in restaurant queue
	bool CustomerGroupWaitsInBuffetQueue(const  unsigned int current_time); // execute customer group waiting in buffet queue
	bool CustomerGroupInCheckoutQueue(const unsigned int current_time); // execute customer group waiting in checkout queue
	
	
	bool CustomerGroupArrivesToTable(const unsigned int current_time); // execute customer group arriving to table by manager and waiting for the waiter
	bool CustomerGroupInRestaurantService(const unsigned int current_time); // execute customer group starting restaurant service after the waiter arrives to the table
	bool CustomerGroupInBuffetService(const unsigned int current_time); // execute customer group starting buffer service after arrive to the seats
	bool CustomerGroupLeavesService(const unsigned int current_time);
	bool CustomerGroupInCashier(const unsigned int current_time); // execute customer group 
	bool CustomerGroupComplete (const unsigned int current_time); // the customer group complete


};
#endif // MODEL_CUSTOMER_GROUP_H
