#include "simulator.h"

#include "event.h"

#include "variables.h"
#include "generator.h"

#include "customer_group.h"
#include "manager.h"
#include "waiter.h"
#include "cashier.h"
#include "table.h"
#include "seat.h"

Simulator::Simulator (const unsigned end_time, const Variables variables):	current_time_(0),
																			end_time_(end_time),
																			is_step_(false),
																			process_(nullptr),
																			log_(nullptr) {
	chinese_restaurant_                   = new ChineseRestaurant();
	chinese_restaurant_->variables        = new Variables(variables);
	chinese_restaurant_->random_generator = new Generator();
}

void Simulator::Init (bool is_step, Log::LogPriority level) {
	is_step_ = is_step;
	process_ = new Process;
	Log::GetLog()->SetPriority(static_cast<Log::LogPriority>(level));
	PrepareRestaurant();
}

void Simulator::PrepareRestaurant() const {
	// Copy Variables.
	auto var = chinese_restaurant_->variables;
	// Prepare the manager.
	chinese_restaurant_->manager = new Manager(chinese_restaurant_);
	// Prepare restaurant tables (2 3 4 persons).
	for (unsigned int i = 0; i < 3;++i) {
		for(unsigned int j = 0; j < var->number_tables[i]; ++j) {
			auto table = new Table(i + 2);
			chinese_restaurant_->restaurant_tables.push_back(table);
		}
	}
	// Prepare buffet seats.
	for (unsigned int i = 0; i < var->number_buffet_seats; i++) {
		auto seat = new Seat();
		chinese_restaurant_->buffet_seats.push_back(seat);
	}
	// Prepare waiters.
	for (unsigned int i = 0; i < var->number_waiters; ++i) {
		auto waiter = new Waiter();
		chinese_restaurant_->free_waiter_queue.push(waiter);
	}
	// Prepare cashier.
	for (unsigned int i = 0; i < var->number_cashiers; ++i) {
		auto cashier = new Cashier();
		chinese_restaurant_->cashiers.push_back(cashier);
	}
}

void Simulator::Run() {
	// Creating the first customer group.
	(new CustomerGroup(chinese_restaurant_))->Activate(current_time_);
	// Run the simulation by popping the first event.
	while (current_time_ <= end_time_) {
		const auto event = process_->PopEvent();
		current_time_ = event->event_time_;
		Log::GetLog()->Print("-----------------------------------------------------------------------\n", Log::P3);
		Log::GetLog()->Print("TIME: " +std::to_string(current_time_) + "\n", Log::P3);
		CustomerGroup * customer_group = event->customer_group_;
		customer_group->Execute(current_time_);
		delete event;
		if (customer_group->IsTerminated()) delete customer_group;
		if(is_step_) {
			Log::GetLog()->Print("Press any key to continue", Log::P1);
			system("pause");
		}
	}
}