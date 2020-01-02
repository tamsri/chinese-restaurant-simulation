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

/*------------------- Static Initialization ------------------------------*/

unsigned int CustomerGroup::customer_group_global_id_ = 0;

/*------------------- Constructor & Destructor ------------------------------*/
CustomerGroup::CustomerGroup(ChineseRestaurant * chinese_restaurant, Process * process) :
																		chinese_restaurant_(chinese_restaurant),
																		customer_group_id_(++customer_group_global_id_),
																		cashier_(nullptr),
																		served_by_(nullptr),
																		table_(nullptr),
																		process_(process),
																		state_(kArriveState),
																		terminated_(false),
																		log_(Log::GetLog()){
	// generate the customer group
	auto * generator = chinese_restaurant_->random_generator;
	auto * variables = chinese_restaurant_->variables;
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
		customer_members_.push_back(c);
	}
	log_->Print("Customer Group ("+ std::to_string(PersonsInGroup()) +")#" + std::to_string(GetCustomerGroupID()) + " is created");
}

CustomerGroup::~CustomerGroup() {
	while (!customer_members_.empty()) {
		const auto current_customer = customer_members_.back();
		customer_members_.pop_back();
		//log_->Print("Customer #"+ std::to_string(current_customer->GetPersonID())+ " in Group #" + std::to_string(customer_group_id_) + "deleted!\n");
		delete current_customer;
	}
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " is deleted");
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

unsigned int CustomerGroup::GetCustomerGroupID() const
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
		log_->Print("Customer Group has already occupied the table #"+std::to_string(table_->GetTableID()), Log::P4, Log::ERROR);
		return;
	}
	table_ = table;
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "is assigned to table #" + std::to_string(table_->GetTableID()));
}
void CustomerGroup::SitOnTable() {
	table_->OnSit(this);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "sits on the table #");
}
/*------------------- Actions for Tables (for customer) ------------------------------*/
void CustomerGroup::LeaveTable() {
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " leave the table #" + std::to_string(table_->GetTableID()));
	if (table_ == nullptr) {
		log_->Print("Cannot leave the table because the customer group does not have table", Log::P1, Log::ERROR);
		return;
	}
	// push the table back to free tables
	chinese_restaurant_->free_restaurant_tables.push_back(table_);
	// delete table pointer from class
	table_ = nullptr;
}

/*------------------- Actions for Waiters ------------------------------*/
void CustomerGroup::AssignWaiter() {
	if (chinese_restaurant_->free_waiter_queue.front() == nullptr) {
		log_->Print("Cannot assign the waiter because there's no free waiter", Log::P1, Log::ERROR);
		return;
	}
	served_by_ = chinese_restaurant_->free_waiter_queue.front();
	chinese_restaurant_->free_waiter_queue.pop();
}
void CustomerGroup::ActivateWaiter() const {
	if (served_by_ == nullptr) {
		log_->Print("No waiter provide service to the customer group to activate", Log::P1, Log::ERROR);
		return;
	}
	served_by_->ProvideServiceTo(table_);
}
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
/*---------------------- Verbs for Cashier ------------------- */
void CustomerGroup::AssignCashier() {
	if (chinese_restaurant_->free_cashiers.empty()) {
		log_->Print("No free waiters to assign", Log::P1, Log::ERROR);
		return;
	}
	cashier_ = chinese_restaurant_->free_cashiers.front();
	chinese_restaurant_->free_cashiers.pop();
	if(!chinese_restaurant_->check_out_queue.empty()) chinese_restaurant_->check_out_queue.pop();
}
void CustomerGroup::ActivateCashier ( ) {
	if (cashier_ == nullptr) {
		log_->Print("No cashier assigned to activate", Log::P1, Log::ERROR);
		return;
	}
	cashier_->ProvideServiceTo(this);
}
void CustomerGroup::LeaveCashier ( ) {
	if (cashier_ == nullptr) {
		log_->Print("No cashier assigned to leave", Log::P1, Log::ERROR);
		return;
	}
	cashier_->CompleteService();
	chinese_restaurant_->free_cashiers.push(cashier_);
	cashier_ = nullptr;
}


void CustomerGroup::LeaveWaiter() {
	if (served_by_ == nullptr) {
		log_->Print("No waiter assigned to leave", Log::P1, Log::ERROR);
		return;
	}
	// the waiter complete the service
	served_by_->CompleteService();
	// the waiter goes back to free waiter queue
	chinese_restaurant_->free_waiter_queue.push(served_by_);
	// delete waiter pointer from class
	served_by_ = nullptr;
}

void CustomerGroup::AssignBuffetSeats ( ) {
	if (!buffet_seats_.empty()) {
		log_->Print("The customer group has already assigned the buffet seats", Log::P1, Log::ERROR);
		return;
	}
	// Bring each person in the group to buffet seats.
	for (unsigned int i = 0; i < PersonsInGroup(); i++) {
		buffet_seats_.push_back(chinese_restaurant_->free_buffet_seats.back());
		chinese_restaurant_->free_buffet_seats.pop_back();
	}
	// leave the buffet queue
	chinese_restaurant_->buffet_queue.pop();
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
		case kLeaveServiceState:
			active = CustomerGroupLeavesService(current_time);
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
// Action when the customer arrives to restaurant
void CustomerGroup::CustomerGroupArrives (const unsigned int current_time) {
	// Customer enter to the type of service they would like
	if(is_buffet_customer_) {
		chinese_restaurant_->buffet_queue.push(this);
		AssignState(kBuffetQueueState);
	}else {
		chinese_restaurant_->restaurant_queue.push_back(this);
		AssignState(kRestaurantQueueState);
	}
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the restaurant", Log::P2, Log::EVENT);
}

// Create the next arriving customer group
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
	//next_customer_group->Activate(next_arrival_customer_time);
	next_customer_group->Activate(next_arrival_customer_time);
	log_->Print("Customer Group #" + std::to_string(next_customer_group->GetCustomerGroupID()) + " will enter the restaurant at " + std::to_string(next_arrival_customer_time), Log::P4, Log::EVENT);
}

// Action when the customer group arrives to restaurant queue
bool CustomerGroup::CustomerGroupWaitsInRestaurantQueue (const unsigned int current_time) const {
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the restaurant queue", Log::P3, Log::EVENT);
	// Let the manager manages
	chinese_restaurant_->manager->Manages(current_time);
	return false;
}

// Action when the customer group enter the buffet queue
bool CustomerGroup::CustomerGroupWaitsInBuffetQueue (const unsigned int current_time) {
	// Are Seats enough for the first group
	if (PersonsInGroup() > chinese_restaurant_->free_buffet_seats.size()) return false;
	AssignBuffetSeats();
	// assign the group to buffet service state
	AssignState(kBuffetServiceState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the buffet queue", Log::P3, Log::EVENT);
	return true;
}

// Action when the customer group arrives to the table and wait for waiter
bool CustomerGroup::CustomerGroupArrivesToTable (const unsigned int current_time) {
	// Customer group sit on the table
	SitOnTable();
	// Check if there is any available waiter
	if (chinese_restaurant_->free_waiter_queue.empty()) {
		chinese_restaurant_->wait_waiter_queue.push(this);
		return false;
	}
	// Assign the front free waiter to the customer group, and pop out from free waiters
	AssignWaiter();
	// assign the group to restaurant service state
	AssignState(kRestaurantServiceState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter to restaurant table", Log::P3, Log::EVENT);
	return true;
}
// Action when the waiter arrives to the table and start the service
bool CustomerGroup::CustomerGroupInRestaurantService (const unsigned int current_time) {
	// The waiter started to provide service to customer
	ActivateWaiter();
	// Start the service when waiter has already assigned to the table
	Activate(current_time + service_time_);
	AssignState(kCheckoutQueueState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " is served by the waiter #" + std::to_string(served_by_->GetWaiterID()), Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupInBuffetService(const unsigned int current_time) {
	// Sit on the buffet seats
	SitOnBuffetSeats();
	// Start the service when everyone sat on the seats
	Activate(current_time + service_time_);
	AssignState(kLeaveServiceState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter to buffet seats", Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupLeavesService (const unsigned int current_time) {
	/* ------------------------ Activate the objects in queues ------------------------ */
// For restaurant service group, the waiter goes back to free_waiter_queue and leave table
	if (!IsBuffetCustomer()) {
		// If a table is waiting for a waiter, activate them for looking for a waiter
		if (!chinese_restaurant_->wait_waiter_queue.empty() && chinese_restaurant_->free_waiter_queue.empty()) {
			//chinese_restaurant_->wait_waiter_queue.front()->Activate(current_time);
		}
		// If a group is waiting for a table, activate them for getting managed by manager
		else if (!chinese_restaurant_->restaurant_queue.empty() && chinese_restaurant_->free_restaurant_tables.empty()) {
			chinese_restaurant_->restaurant_queue.front()->Activate(current_time);
		}
		/*------ Free Waiter ------*/
		LeaveWaiter();
		/*------ Free Table ------*/
		// the customer group set free from table
		LeaveTable();
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " leaves the restaurant table", Log::P3, Log::EVENT);
	}
	// For buffet service, customer members leave the buffet seats
	else {
		// If 
		if (!chinese_restaurant_->buffet_queue.empty() && chinese_restaurant_->free_buffet_seats.empty()) {
			chinese_restaurant_->buffet_queue.front()->Activate(current_time);
		}
		LeaveBuffetSeats();
		log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " leaves the buffet seats", Log::P3, Log::EVENT);
	}
	AssignState(kCheckoutQueueState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enters the checkout queue", Log::P3, Log::EVENT);
	return true;
}

bool CustomerGroup::CustomerGroupInCheckoutQueue (const unsigned int current_time) {
	/* ------------------------ Actions in the checkout queue ------------------------ */
	// Check if no free cashier is unavailable, the customer group arrives to checkout queue
	if (chinese_restaurant_->free_cashiers.empty()) {
		chinese_restaurant_->check_out_queue.push(this);
		return false;
	}
	// the first available cashier provide the service, and pop out from free cashier
	AssignCashier();
	// assign the state to the customer group
	AssignState(kCheckoutServiceState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " arrives to the free cashier", Log::P3, Log::EVENT);
	return true;
}

bool CustomerGroup::CustomerGroupInCashier (const unsigned int current_time) {
	// cashier provide the service to customer group
	ActivateCashier();
	// Generate the checkout time in cashier
	auto gen = chinese_restaurant_->random_generator;
	const auto var = chinese_restaurant_->variables;
	// Activate the customer group to complete
	Activate(current_time + gen->GenerateExponentialDistribution(var->average_cashier_service_time) + 100);
	// assign the state to complete
	AssignState(kCompletedState);
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " starts service with the cashier #" + std::to_string(cashier_->GetCashierID()), Log::P3, Log::EVENT);
	return false;
}

bool CustomerGroup::CustomerGroupComplete (const unsigned int current_time) {
	// Check if the free cashier is empty, then assign cashier activate the waiting group
	if (!chinese_restaurant_->check_out_queue.empty() && chinese_restaurant_->free_cashiers.empty()) {
		chinese_restaurant_->check_out_queue.front()->Activate(current_time);	
	}
	// the customer group leave the cashier
	LeaveCashier();
	log_->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " leaves the restaurant", Log::P2, Log::EVENT);
	return false;
}

