#include "customer_group.h"

#include <string>

#include "chinese_restaurant.h"
#include "variables.h"
#include "generators.h"
#include "random_generators.h"

#include "waiter.h"
#include "cashier.h"
#include "customer.h"
#include "manager.h"

#include "records.h"
#include "log.h"
#include "table.h"
#include "seat.h"

#include "event.h"
#include "timer.h"

/*------------------- Static Initialization ------------------------------*/

unsigned int CustomerGroup::customer_group_global_id_ = 0;

/*------------------- Constructor & Destructor ------------------------------*/
CustomerGroup::CustomerGroup(ChineseRestaurant * chinese_restaurant, Process * process, const unsigned int current_time) :
																		chinese_restaurant_(chinese_restaurant),
																		customer_group_id_(++customer_group_global_id_),
																		cashier_(nullptr),
																		served_by_(nullptr),
																		table_(nullptr),
																		process_(process),
																		state_(kArriveState),
																		terminated_(false),
																		log_(Log::GetLog()),
																		is_called_(false){
	// generate the customer group
	auto * variables = chinese_restaurant_->variables;
	
	// Generate the amount of persons
	const auto persons_in_group = chinese_restaurant_->random_generators->persons_generator->Rand();
	chinese_restaurant_->records->PushGeneratorRecord({ persons_in_group, current_time, kPersonsGenerator });
	// Generate the service time of the group
	is_buffet_customer_ = chinese_restaurant_->random_generators->buffet_generator->Rand();
	chinese_restaurant_->records->PushBuffetRecord(is_buffet_customer_);
	
	if (is_buffet_customer_) {
		service_time_ = chinese_restaurant_->random_generators->buffet_service_generator->Rand();
		chinese_restaurant_->records->PushGeneratorRecord({ service_time_, current_time, kBuffetServiceGenerator });
	}
	else {
		service_time_ = chinese_restaurant_->random_generators->waiter_service_generator->Rand();
		chinese_restaurant_->records->PushGeneratorRecord({ service_time_, current_time, kWaiterServiceGenerator });
	}
	
	// Generate the cashier time of the group
	cashier_time_ = chinese_restaurant_->random_generators->cashier_service_generator->Rand();
	chinese_restaurant_->records->PushGeneratorRecord({ cashier_time_, current_time, kCashierServiceGenerator });
	// Generate persons in the group
	for (unsigned int i = 0; i < persons_in_group; ++i) {
		auto c = new Customer();
		customer_members_.push_back(c);
	}
	const std::string type = is_buffet_customer_ ? "buffet" : "restaurant";
	log_->Print("Customer Group ("+ std::to_string(PersonsInGroup()) +" persons, " + type +")#" + 
	std::to_string(GetCustomerGroupId()) + " is created. (service time: "
		+ chinese_restaurant_->clock->SecondsToMinutes(service_time_) 
		+", cashier time: " + chinese_restaurant_->clock->SecondsToMinutes(cashier_time_) + ")");
}

CustomerGroup::~CustomerGroup() {
	while (!customer_members_.empty()) {
		const auto current_customer = customer_members_.back();
		customer_members_.pop_back();
		delete current_customer;
	}
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " is deleted");
}
/*------------------- Class Variables' Modification & Presentation ------------------------------*/
unsigned int CustomerGroup::PersonsInGroup ( ) const {
	return static_cast<unsigned int>(customer_members_.size());
}

bool CustomerGroup::IsBuffetCustomer() const {
	return is_buffet_customer_;
}

Customer * CustomerGroup::GetCustomerMember (const unsigned int position) {
	return customer_members_[position];
}

unsigned int CustomerGroup::GetCustomerGroupId() const
{
	return customer_group_id_;
}

bool CustomerGroup::IsTerminated() const {
	return terminated_;
}
/*------------------- Actions of Process  ------------------------------*/
void CustomerGroup::AssignState(const State state)
{
	state_ = state;
}

/*------------------- Actions for Tables (for manager) ------------------------------*/
void CustomerGroup::AssignTable(Table * table)
{
	if (table_ != nullptr) {
		log_->Print("Customer Group has already occupied the table #"+std::to_string(table_->GetTableId()), Log::P4, Log::ERROR);
		return;
	}
	table_ = table;
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " is assigned to table #" + std::to_string(table_->GetTableId()));
}

void CustomerGroup::CallManager (const unsigned int current_time) const {
	chinese_restaurant_->manager->Manages(current_time);
}

void CustomerGroup::SitOnTable() {
	table_->OnSit(this);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " sits on the table");
}

/*------------------- Actions for Waiters ------------------------------*/
void CustomerGroup::SitOnBuffetSeats() {
	if (buffet_seats_.empty()) {
		log_->Print("No buffet seats assigned to the customer group to set", Log::P1, Log::ERROR);
		return;
	}
	for (unsigned int i = 0; i < PersonsInGroup(); i++) {
		buffet_seats_[i]->OnSit(customer_members_[i]);
	}
}
void CustomerGroup::LeaveBuffetSeats()
{
	if (buffet_seats_.empty()) {
		log_->Print("No buffet seats assigned to the customer group to leave", Log::P1, Log::ERROR);
		return;
	}
	for (auto * seat : buffet_seats_) {
		seat->OnLeave();
		chinese_restaurant_->free_buffet_seats.push_back(seat);
	}
	buffet_seats_.clear();
}

/*---------------------- Verbs for Process ------------------- */
void CustomerGroup::Activate(const unsigned int current_time)
{
	process_->InsertEvent(new Event(this, current_time));
}
void CustomerGroup::Execute(const unsigned int current_time) {
	auto active = true;
	while (active) {
		switch (state_) {
		case kArriveState:
			CustomerGroupArrives(current_time);
			CreateNextCustomerGroup(current_time);
			break;
		case kRestaurantQueueState:
			active = CustomerGroupWaitsInRestaurantQueue(current_time);
			break;
		case kBuffetQueueState:
			active = CustomerGroupWaitsInBuffetQueue(current_time);
			break;
		case kRestaurantArriveTableSate:
			active = CustomerGroupArrivesToTable(current_time);
			break;
		case kRestaurantWaiterState:
			active = CustomerGroupWaitsTheWaiter(current_time);
			break;
		case kRestaurantServiceState:
			active = CustomerGroupInRestaurantService(current_time);
			break;
		case kBuffetServiceState:
			active = CustomerGroupInBuffetService(current_time);
			break;
		case kLeaveServiceState:
			active = CustomerGroupLeavesService(current_time);
			break;
		case kCheckoutQueueState:
			active = CustomerGroupInCheckoutQueue(current_time);
			break;
		case kCheckoutServiceState:
			active = CustomerGroupInCashier(current_time);
			break;
		case kCompletedState:
			active = CustomerGroupComplete(current_time);
			terminated_ = true;
			break;
		}
	}
}

void CustomerGroup::Call ( ) {
	is_called_ = true;
}

void CustomerGroup::UnCall()
{
	is_called_ = false;
}

bool CustomerGroup::IsCalled ( ) const {
	return is_called_;
}

// Action when the customer arrives to restaurant
void CustomerGroup::CustomerGroupArrives (const unsigned int current_time) {
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " entered the restaurant", Log::P2, Log::EVENT);
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(),current_time, kArriveState });
	// Customer enter to the type of service they would like
	if(is_buffet_customer_) {
		chinese_restaurant_->records->PushQueueRecord({ current_time,static_cast<unsigned int>(chinese_restaurant_->buffet_queue.size()), kBuffetQueue });
		chinese_restaurant_->buffet_queue.push(this);
		chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(),current_time, kBuffetQueueState });
		AssignState(kBuffetQueueState);
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " entered the buffet queue", Log::P3, Log::EVENT);
	}
	else {
		chinese_restaurant_->records->PushQueueRecord({current_time, static_cast<unsigned int>(chinese_restaurant_->restaurant_queue.size()), kTableQueue});
		chinese_restaurant_->restaurant_queue.push_back(this);
		chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(), current_time, kRestaurantQueueState });
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " entered the restaurant queue", Log::P3, Log::EVENT);
		AssignState(kRestaurantQueueState);
	}
}

// Create the next arriving customer group
void CustomerGroup::CreateNextCustomerGroup(const unsigned int current_time) const {
	const auto var = chinese_restaurant_->variables;
	// Generate the interval time
	const auto interval_time = chinese_restaurant_->random_generators->interval_arrival_generator->Rand();
	chinese_restaurant_->records->PushGeneratorRecord({ interval_time, current_time, kIntervalArrivalGenerator });
	// Get next arrival to the restaurant
	const auto next_arrival_customer_time = current_time + interval_time;
	(new CustomerGroup(chinese_restaurant_, process_, current_time))->Activate(next_arrival_customer_time);
	//log_->Print("Next customer Group  will enter the restaurant at " + std::to_string(next_arrival_customer_time), Log::P4, Log::EVENT);
}

// Action when the customer group arrives to restaurant queue
bool CustomerGroup::CustomerGroupWaitsInRestaurantQueue (const unsigned int current_time) {
	UnCall();
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " calls the manager for table", Log::P3, Log::EVENT);
	// Let the manager manages if the manager is free
	if(chinese_restaurant_->manager->IsFree())
		CallManager(current_time);
	return false;
}

// Action when the customer group enter the buffet queue
bool CustomerGroup::CustomerGroupWaitsInBuffetQueue (const unsigned int current_time) {
	UnCall();
	if (chinese_restaurant_->buffet_queue.front() != this) return false;
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " look for " + std::to_string(PersonsInGroup())  + " buffet seats", Log::P3, Log::EVENT);
	// Are Seats enough for the first group
	if (chinese_restaurant_->free_buffet_seats.size() < PersonsInGroup()) return false;
	// Bring each person in the group to buffet seats.
	for (unsigned int i = 0; i < PersonsInGroup(); i++) {
		buffet_seats_.push_back(chinese_restaurant_->free_buffet_seats.back());
		chinese_restaurant_->free_buffet_seats.pop_back();
	}
	// leave the buffet queue
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->buffet_queue.size()), kBuffetQueue });
	chinese_restaurant_->buffet_queue.pop();
	/// Additionally activate if next waiting group may have enough seats
	if (!chinese_restaurant_->buffet_queue.empty() && 
		chinese_restaurant_->buffet_queue.front()->PersonsInGroup() <= chinese_restaurant_->free_buffet_seats.size() &&
		!chinese_restaurant_->buffet_queue.front()->IsCalled()) {
		chinese_restaurant_->buffet_queue.front()->Activate(current_time);
		chinese_restaurant_->buffet_queue.front()->Call();
	}
		
	// Assign the group to buffet service state
	AssignState(kBuffetServiceState);
	return true;
}

// Action when the customer group arrives to the table and wait for waiter
bool CustomerGroup::CustomerGroupArrivesToTable (const unsigned int current_time) {
	// UnCall() the selected customer group to avoid multiple signaling bug
	UnCall();
	// Set the manager free from leading to table
	chinese_restaurant_->manager->MakeFree();
	// If a group  waits for the manager, the manager goes back to lead them to table.
	if (!chinese_restaurant_->restaurant_queue.empty())
		CallManager(current_time);
	// Customer group sit on the table
	SitOnTable();
	// The group waits for a free waiter
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->wait_waiter_queue.size()),kWaitWaiterQueue });
	chinese_restaurant_->wait_waiter_queue.push(this);
	// Assign the group to wait for the waiter
	AssignState(kRestaurantWaiterState);
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(), current_time, kRestaurantWaiterState });
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " enters to restaurant table", Log::P3, Log::EVENT);
	return true;
}

// Action when the customer sat on table and call for a waiter
bool CustomerGroup::CustomerGroupWaitsTheWaiter(const unsigned int current_time) {
	UnCall();
	if (chinese_restaurant_->wait_waiter_queue.front() != this) return false;
	// Check if there is any available waiter
	if (chinese_restaurant_->free_waiter_queue.empty()) {
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + "  waits for a free waiter ", Log::P3, Log::EVENT);
		return false;
	}
	// Assign the front free waiter to the customer group, and pop out from free waiters
	served_by_ = chinese_restaurant_->free_waiter_queue.front();
	// pop free waiter out from queue
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->free_waiter_queue.size()), kFreeWaiterQueue });
	chinese_restaurant_->free_waiter_queue.pop();
	// pop group out from waiting a waiter	
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->wait_waiter_queue.size()),kWaitWaiterQueue });
	chinese_restaurant_->wait_waiter_queue.pop();

	// Assign the group to the service state
	AssignState(kRestaurantServiceState);
	return true;
}
// Action when the waiter arrives to the table and start the service
bool CustomerGroup::CustomerGroupInRestaurantService (const unsigned int current_time) {
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(), current_time, kRestaurantServiceState });
	// The waiter started to provide service to customer
	served_by_->ProvideServiceTo(table_);
	// Start the service when waiter has already assigned to the table
	Activate(current_time + service_time_);
	AssignState(kLeaveServiceState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " is served by the waiter #" + std::to_string(served_by_->GetWaiterId()), Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupInBuffetService(const unsigned int current_time) {
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(),current_time,kBuffetServiceState });
	// Sit on the buffet seats
	SitOnBuffetSeats();
	// Start the service when everyone sat on the seats
	Activate(current_time + service_time_);
	AssignState(kLeaveServiceState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " enters to buffet seats", Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupLeavesService (const unsigned int current_time) {
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(), current_time, kLeaveServiceState });
	/* ------------------------ Activate the objects in queues ------------------------ */
	
	// For restaurant service group, the waiter goes back to free_waiter_queue and leave table
	if (!IsBuffetCustomer()) {
		// If a table is waiting for a waiter, activate them for looking for a waiter
		if (!chinese_restaurant_->wait_waiter_queue.empty() && !chinese_restaurant_->wait_waiter_queue.front()->IsCalled()) {
			chinese_restaurant_->wait_waiter_queue.front()->Activate(current_time);
			chinese_restaurant_->wait_waiter_queue.front()->Call();
		}
		// If a group is waiting for a table, activate them to get managed by manager
		if (!chinese_restaurant_->restaurant_queue.empty() && !chinese_restaurant_->free_restaurant_tables.empty()) {
			CallManager(current_time);
		}
		/*------ Free Waiter ------*/
			// the waiter complete the service
		served_by_->CompleteService();
			// the waiter goes back to free waiter queue
		chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->free_waiter_queue.size()), kFreeWaiterQueue });
		chinese_restaurant_->free_waiter_queue.push(served_by_);
			// delete waiter pointer from class
		served_by_ = nullptr;
		/*------ Free Table ------*/
			// push the table back to free tables
		chinese_restaurant_->free_restaurant_tables.push_back(table_);
			// delete table pointer from class
		table_ = nullptr;
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " leaves the restaurant table", Log::P3, Log::EVENT);
	}
	
	// For buffet service, customer members leave the buffet seats
	else {
		// If there's group waiting in the queue and buffet seat
		if (!chinese_restaurant_->buffet_queue.empty() && 
			chinese_restaurant_->buffet_queue.front()->PersonsInGroup() <= chinese_restaurant_->buffet_queue.size() &&
			!chinese_restaurant_->buffet_queue.front()->IsCalled()) {
			chinese_restaurant_->buffet_queue.front()->Activate(current_time);
			chinese_restaurant_->buffet_queue.front()->Call();
		}
		LeaveBuffetSeats();
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " leaves the buffet seats", Log::P3, Log::EVENT);
	}
	// Customer Group enter the checkout queue
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->check_out_queue.size()), kCheckoutQueue });
	chinese_restaurant_->check_out_queue.push(this);
	
	AssignState(kCheckoutQueueState);
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(), current_time, kCheckoutQueueState });
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " goes to the checkout queue", Log::P3, Log::EVENT);
	return true;
}

bool CustomerGroup::CustomerGroupInCheckoutQueue (const unsigned int current_time) {
	UnCall();
	if (chinese_restaurant_->check_out_queue.front() != this) return false;
	/* ------------------------ Actions in the checkout queue ------------------------ */
	// Check if no free cashier is unavailable, the customer group arrives to checkout queue
	if (chinese_restaurant_->free_cashiers.empty()) {
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " waits for a free cashier in the queue", Log::P3, Log::EVENT);
		return false;
	}
	// the first available cashier provide the service, and pop out from free cashier
	cashier_ = chinese_restaurant_->free_cashiers.front();
	// pop free cashier to the group
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->free_cashiers.size()), kFreeCashierQueue });
	chinese_restaurant_->free_cashiers.pop();
	// pop the group out from the queue
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->check_out_queue.size()), kCheckoutQueue });
	chinese_restaurant_->check_out_queue.pop();
	
	// assign the state to the customer group
	AssignState(kCheckoutServiceState);
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(), current_time, kCheckoutServiceState });
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " arrives to the free cashier", Log::P3, Log::EVENT);
	return true;
}

bool CustomerGroup::CustomerGroupInCashier (const unsigned int current_time) {
	// cashier provide the service to customer group
	cashier_->ProvideServiceTo(this);
	// Activate the customer group to complete
	Activate(current_time+cashier_time_);
	// assign the state to complete
	AssignState(kCompletedState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " starts service with the cashier #" + std::to_string(cashier_->GetCashierId()), Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupComplete (const unsigned int current_time) {
	chinese_restaurant_->records->PushCustomerRecord({ GetCustomerGroupId(), current_time, kCompletedState });
	// Check if the free cashier is empty, then assign cashier activate the waiting group
	/// FIXED: a bug from two+ groups leaving at the same time and active the front group than once
	if (!chinese_restaurant_->check_out_queue.empty() && !chinese_restaurant_->check_out_queue.front()->IsCalled()) {
		chinese_restaurant_->check_out_queue.front()->Activate(current_time);
		chinese_restaurant_->check_out_queue.front()->Call();
	}
	// the customer group leave the cashier
	cashier_->CompleteService();
	chinese_restaurant_->records->PushQueueRecord({ current_time, static_cast<unsigned int>(chinese_restaurant_->free_cashiers.size()), kFreeCashierQueue });
	chinese_restaurant_->free_cashiers.push(cashier_);
	cashier_ = nullptr;
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupId()) + " leaves the restaurant", Log::P2, Log::EVENT);
	return false;
}

