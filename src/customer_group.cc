#include "customer_group.h"

#include <string>

#include "chinese_restaurant.h"
#include "variables.h"
#include "generator.h"

#include "waiter.h"
#include "cashier.h"
#include "customer.h"
#include "manager.h"


#include "log.h"
#include "table.h"
#include "seat.h"

#include "event.h"
#include "future_event_list.h"

/*------------------- Static Initialization ------------------------------*/

unsigned int CustomerGroup::customer_group_global_id_ = 0;

/*------------------- Constructor & Destructor ------------------------------*/
CustomerGroup::CustomerGroup(ChineseRestaurant * chinese_restaurant, Process * process) :
																		chinese_restaurant_(chinese_restaurant),
																		customer_group_id_(++customer_group_global_id_),
																		process_(process){
	// generate the customer group
	auto generator = chinese_restaurant_->random_generator;
	auto variables = chinese_restaurant_->variables;
	// Generate the amount of persons
	const auto persons_in_group = generator->GenerateByProbability(variables->probability_persons_in_customer_group);
	// Generate the service time of the group
	is_buffet_customer_ = generator->GenerateBoolByProbability(variables->probability_buffet_customer_group);
	if (is_buffet_customer_) {
		service_time_ = generator->GenerateNormalDistribution(variables->average_buffet_time, variables->variance_buffet_time);
	}
	else {
		service_time_ = generator->GenerateExponentialDistribution(variables->average_waiter_service_time);
	}
	// Generate persons in the group
	for (unsigned int i = 0; i < persons_in_group; ++i) {
		auto c = new Customer();
		// Log::GetLog()->Print(" Customer #" + std::to_string(c->GetPersonID())+ "is being created in Group # " + std::to_string(customer_group_id_) + ".");
		customer_members_.push_back(c);
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "is created");
}

CustomerGroup::~CustomerGroup() {
	while (!customer_members_.empty()) {
		const auto current_customer = customer_members_.back();
		customer_members_.pop_back();
		//Log::GetLog()->Print("Customer #"+ std::to_string(current_customer->GetPersonID())+ " in Group #" + std::to_string(customer_group_id_) + "deleted!\n");
		delete current_customer;
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "is deleted");
}
/*------------------- Class Variables' Modification & Presentation ------------------------------*/
unsigned int CustomerGroup::PersonsInGroup ( ) const {
	return static_cast<unsigned int>(customer_members_.size());
}

bool CustomerGroup::IsBuffetCustomer() const {
	return is_buffet_customer_;
}

Customer * CustomerGroup::GetCustomerMember (unsigned int position) {
	return customer_members_[position];
}

unsigned int CustomerGroup::GetCustomerGroupID() const
{
	return customer_group_id_;
}

bool CustomerGroup::IsTerminated() const {
	return terminated_;
}
/*------------------- Verbs of Class  ------------------------------*/
void CustomerGroup::AssignState(const State state)
{
	state_ = state;
}
void CustomerGroup::AssignTable(Table * table)
{
	table_ = table;
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "is assigned to table #" + std::to_string(table_->GetTableID()));
}
void CustomerGroup::SitOnTable() {
	table_->OnSit(this);
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "sit on the table #");
}
void CustomerGroup::SitOnBuffetSeats() {
	for (unsigned int i = 0; i < PersonsInGroup(); i++) {
		buffet_seats_[i]->OnSit(customer_members_[i]);
	}
}
void CustomerGroup::LeaveBuffetSeats()
{
	for (auto seat : buffet_seats_) {
		seat->OnLeave();
	}
	buffet_seats_.clear();
}

void CustomerGroup::LeaveTable() {
	if (table_ == nullptr) {
		Log::GetLog()->Print("Cannot leave the table because the customer group does not have table", Log::P4, Log::ERROR);
		return;
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " leave the table #" + std::to_string(table_->GetTableID()));
	table_ = nullptr;
}
void CustomerGroup::Activate(const unsigned int current_time)
{
	process_->InsertEvent(new Event(this, current_time));
}
void CustomerGroup::Execute(const unsigned int current_time) {

	bool active = true;

	while (active) {
		switch (state_) {
		case kArriveState:
			CustomerGroupArrives(current_time);
			CreateNextCustomerGroup(current_time);
			break;
		case kRestaurantQueueState:
			CustomerGroupWaitsInRestaurantQueue(current_time);
			active = false;
			break;
		case kBuffetQueueState:
			active = CustomerGroupWaitsInBuffetQueue(current_time);
			break;
		case kRestaurantWaiterState:
			active = CustomerGroupArrivesToTable(current_time);
			break;
		case kRestaurantServiceState:
			active = CustomerGroupInRestaurantService(current_time);
			break;
		case kBuffetServiceState:
			active = CustomerGroupInBuffetService(current_time);
			break;
		case kCheckoutQueueState:
			active = CustomerGroupInCheckoutQueue(current_time);
			break;
		case kCheckoutServiceState:
			active = CustomerGroupInRestaurantService(current_time);
			break;
		case kCompletedState:
			active = CustomerGroupComplete(current_time);
			terminated_ = true;
			break;
		}
	}
}
void CustomerGroup::CustomerGroupArrives (const unsigned int current_time) {
	if(is_buffet_customer_) {
		chinese_restaurant_->buffet_queue.push(this);
		state_ = kBuffetQueueState;
	}else {
		chinese_restaurant_->restaurant_queue.push_back(this);
		state_ = kRestaurantQueueState;
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the restaurant", Log::P2, Log::EVENT);
}

void CustomerGroup::CreateNextCustomerGroup(const unsigned int current_time) const {
	const auto var = chinese_restaurant_->variables;
	auto gen = chinese_restaurant_->random_generator;
	// Generate the interval time
	const auto average_arrival = var->average_arrival_interval;
	const auto variance_arrival = var->variance_arrival_interval;
	const auto interval_time = gen->GenerateNormalDistribution(average_arrival, variance_arrival);
	// Get next arrival to the restaurant
	const auto next_arrival_customer_time = current_time + interval_time;
	auto next_customer_group = new CustomerGroup(chinese_restaurant_, process_);
	next_customer_group->Activate(next_arrival_customer_time);
	Log::GetLog()->Print("Customer Group #" + std::to_string(next_customer_group->GetCustomerGroupID()) + " will enter the restaurant at " + std::to_string(next_arrival_customer_time), Log::P4, Log::EVENT);
}

void CustomerGroup::CustomerGroupWaitsInRestaurantQueue (const unsigned int current_time) {
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the restaurant queue", Log::P3, Log::EVENT);
	// Let the manager manages
	chinese_restaurant_->manager->Manages(this, current_time);
}

bool CustomerGroup::CustomerGroupWaitsInBuffetQueue (const unsigned int current_time) {
	std::queue<Seat *> free_seats;
	// Count empty seats
	for(auto seat : chinese_restaurant_->buffet_seats)
		if(seat->IsEmpty()) free_seats.push(seat);
	// Are Seats enough for the first group
	if (PersonsInGroup() > free_seats.size()) return false;
	// Bring each person in the group to buffet seats.
	for (auto member: customer_members_) {
		free_seats.front()->OnSit(member);
		buffet_seats_.push_back(free_seats.front());
		free_seats.pop();
	}
	// assign the group to buffet service state
	state_ = kBuffetServiceState;
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the buffet queue", Log::P3, Log::EVENT);
	return true;
}

bool CustomerGroup::CustomerGroupArrivesToTable (const unsigned int current_time) {
	// Customer group sit on the table
	SitOnTable();
	// Check if there is any available waiter
	if (chinese_restaurant_->free_waiter_queue.empty()) return false;
	// Assign the front free waiter to the customer group, and pop out from free waiters
	served_by_ = chinese_restaurant_->free_waiter_queue.front();
	chinese_restaurant_->free_waiter_queue.pop();
	// assign the group to restaurant service state
	state_ = kRestaurantServiceState;
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter to restaurant table", Log::P3, Log::EVENT);
	return true;
}

bool CustomerGroup::CustomerGroupInRestaurantService (const unsigned int current_time) {
	// The waiter started to provide service to customer
	served_by_->ProvideServiceTo(table_);
	// Start the service when waiter has already assigned to the table
	Activate(current_time + service_time_);
	state_ = kCheckoutQueueState;
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " is served by the waiter #" + std::to_string(served_by_->GetWaiterID()), Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupInBuffetService(const unsigned int current_time) {
	// Sit on the buffet seats
	SitOnBuffetSeats();
	// Start the service when everyone sat on the seats
	Activate(current_time + service_time_);
	state_ = kCheckoutQueueState;
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter to buffet seats", Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupInCheckoutQueue (const unsigned int current_time) {
	// For restaurant service, the waiter goes back to free_waiter_queue and leave table
	if (!IsBuffetCustomer()) {
		served_by_->ProvideServiceTo(nullptr);
		if(chinese_restaurant_->free_waiter_queue.empty()) {
			
		}
		chinese_restaurant_->free_waiter_queue.push(served_by_);
		served_by_ = nullptr;
		LeaveTable();
	}
	// For buffet service, customer members leave the buffet seats
	else LeaveBuffetSeats();
	// Check if the cashier is empty
	if (chinese_restaurant_->free_cashiers.empty()) return false;
	// the first available cashier provide the service, and pop out from free cashier
	cashier_ = chinese_restaurant_->free_cashiers.front();
	chinese_restaurant_->free_cashiers.pop();
	// assign the state to the customer group
	state_ = kCheckoutServiceState;
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter the checkout queue", Log::P3, Log::EVENT);
	return true;
}

bool CustomerGroup::CustomerGroupInCashier (const unsigned int current_time) {
	// Generate the checkout time in cashier
	auto gen = chinese_restaurant_->random_generator;
	const auto var = chinese_restaurant_->variables;
	// Activate the customer group to complete
	Activate(current_time + gen->GenerateExponentialDistribution(var->average_cashier_service_time));
	state_ = kCompletedState;
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " arrives to the cashier #" + std::to_string(cashier_->GetCashierID()), Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupComplete (const unsigned int current_time) {
	// Complete the service by cashier
	cashier_->CompleteService();
	// Check if the free cashier is empty, then assign cashier activate the waiting group
	if (chinese_restaurant_->free_cashiers.empty() && !chinese_restaurant_->check_out_queue.empty()) {
		chinese_restaurant_->check_out_queue.front()->Activate(current_time);	
	} 
	chinese_restaurant_->free_cashiers.push(cashier_);
	cashier_ = nullptr;
	
}

