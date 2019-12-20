#include "customer_group.h"

#include <string>

#include "chinese_restaurant.h"
#include "variables.h"
#include "generator.h"

#include "cashier.h"
#include "customer.h"
#include "waiter.h"

#include "log.h"
#include "table.h"
#include "seat.h"
#include "event.h"
#include "process.h"
#include "manager.h"
#include <map>

unsigned int CustomerGroup::customer_group_global_id_ = 0;

CustomerGroup::CustomerGroup(ChineseRestaurant * chinese_restaurant) :  table_(nullptr),
																		served_by_(nullptr),
																		state_(ArriveState),
																		chinese_restaurant_(chinese_restaurant),
																		customer_group_id_(++customer_group_global_id_),
																		terminated_(false){
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
		Log::GetLog()->Print(" Customer #" + std::to_string(c->GetPersonID())+ "is being created in Group # " + std::to_string(customer_group_id_) + ".", Log::P4);
		customer_members_.push_back(c);
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "is created", Log::P4);
}

CustomerGroup::~CustomerGroup() {
	while (!customer_members_.empty()) {
		const auto current_customer = customer_members_.back();
		Log::GetLog()->Print("Customer #"+ std::to_string(current_customer->GetPersonID())+ " in Group #" + std::to_string(customer_group_id_) + "deleted!\n", Log::P4);
		delete current_customer;
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + "is deleted", Log::P4);
}

unsigned int CustomerGroup::PersonsInGroup ( ) const {
	return static_cast<unsigned int>(customer_members_.size());
}

bool CustomerGroup::IsBuffetCustomer() const {
	return is_buffet_customer_;
}

unsigned int CustomerGroup::GetCustomerGroupID() const
{
	return customer_group_id_;
}

bool CustomerGroup::IsTerminated() const {
	return terminated_;
}

void CustomerGroup::Execute(unsigned int current_time) {

	bool active = true;

	while (active) {
		switch (state_) {
		case ArriveState:
			CustomerGroupArrives(current_time);
			CreateNextCustomerGroup(current_time);
			break;
		case RestaurantQueueState:
			active = CustomerGroupWaitsInRestaurantQueue(current_time);
			break;
		case BuffetQueueState:
			active = CustomerGroupWaitsInBuffetQueue(current_time);
			break;
		case RestaurantWaiterState:
			active = CustomerGroupArrivesToTable(current_time);
			break;
		case RestaurantServiceState:
			active = CustomerGroupInRestaurantService(current_time);
			break;
		case BuffetServiceState:
			active = false;
			break;
		case CheckoutQueueState:
			active = CustomerGroupInCheckoutQueue(current_time);
			break;
		case CheckoutServiceState:
			chinese_restaurant_->check_out_queue.pop();
			break;
		case CompletedState:
			active = false;
			terminated_ = true;
			break;
		}
	}
}
bool CustomerGroup::CustomerGroupArrives (unsigned current_time) {
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the restaurant", Log::P3, Log::EVENT);
	if(is_buffet_customer_) {
		chinese_restaurant_->buffet_queue.push(this);
		state_ = BuffetQueueState;
	}else {
		chinese_restaurant_->restaurant_queue.push_back(this);
		state_ = RestaurantQueueState;
	}
}

bool CustomerGroup::CreateNextCustomerGroup(const unsigned int current_time) const {
	// Generate the interval time
	const auto average_arrival = chinese_restaurant_->variables->average_arrival_interval;
	const auto variance_arrival = chinese_restaurant_->variables->variance_arrival_interval;
	const auto interval_time = chinese_restaurant_->random_generator->GenerateNormalDistribution(average_arrival, variance_arrival);
	// Get next arrival to the restaurant
	const auto next_arrival_customer_time = current_time + interval_time;
	auto next_customer_group = new CustomerGroup(chinese_restaurant_);
	next_customer_group->Activate(next_arrival_customer_time);
	Log::GetLog()->Print("Customer Group #" + std::to_string(next_customer_group->GetCustomerGroupID()) + "will enter the restaurant at " + std::to_string(next_arrival_customer_time), Log::P4, Log::EVENT);
}

bool CustomerGroup::CustomerGroupWaitsInRestaurantQueue (const unsigned current_time) {
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the restaurant queue", Log::P3, Log::EVENT);
	// Let the manager manages
	chinese_restaurant_->manager->Manages(this, current_time);
	return false;
}

bool CustomerGroup::CustomerGroupWaitsInBuffetQueue (const unsigned current_time) {
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " entered the buffet queue", Log::P3, Log::EVENT);
	std::queue<Seat *> free_seats;
	// Count empty seats
	for(auto seat : chinese_restaurant_->buffet_seats)
		if(seat->IsEmpty()) free_seats.push(seat);
	// Are Seats enough for the first group
	if (PersonsInGroup() > free_seats.size()) return false;
	// Bring each person in the group to buffet seats.
	for (auto member: customer_members_) {
		free_seats.front()->OnSit(member);
		free_seats.pop();
	}
	state_ = BuffetServiceState;
	return true;
}

bool CustomerGroup::CustomerGroupArrivesToTable (const unsigned current_time) {
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter to restaurant table", Log::P3, Log::EVENT);
	// Customer group sit on the table
	table_->OnSit(this);
	if (chinese_restaurant_->free_waiter_queue.empty()) return false;
	served_by_ = chinese_restaurant_->free_waiter_queue.front();
	served_by_->ProvideServiceTo(table_);
	chinese_restaurant_->free_waiter_queue.pop();
	state_ = RestaurantServiceState;
	return true;
}

bool CustomerGroup::CustomerGroupInRestaurantService (const unsigned current_time) {
	
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " is served by ", Log::P3, Log::EVENT);
	Activate(current_time + service_time_);
	state_ = CheckoutQueueState;
	return false;
}

bool CustomerGroup::CustomerGroupInBuffetService(const unsigned current_time) {
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter to buffet seats", Log::P3, Log::EVENT);
	auto gen = this->chinese_restaurant_->random_generator;
	const auto var = this->chinese_restaurant_->variables;
	Activate(current_time + service_time_);
	state_ = CheckoutQueueState;
	return false;
}

bool CustomerGroup::CustomerGroupInCheckoutQueue (const unsigned current_time) {
	Log::GetLog()->Print("Customer Group #" + std::to_string(GetCustomerGroupID()) + " enter checkout queue", Log::P3, Log::EVENT);
	std::vector<Cashier *> free_cashier;
	for (auto cashier : chinese_restaurant_->cashiers)
		if (cashier->IsAvailable()) free_cashier.push_back(cashier);
	
	state_ = CheckoutServiceState;
	return false;
}

bool CustomerGroup::CustomerGroupInCashier (const unsigned current_time) {
	auto gen = chinese_restaurant_->random_generator;
	const auto var = chinese_restaurant_->variables;
	Activate(current_time + gen->GenerateExponentialDistribution(var->average_cashier_service_time));
	state_ = CompletedState;
	return false;
}
