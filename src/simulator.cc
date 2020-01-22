#include "simulator.h"

#include "event.h"

#include "variables.h"

#include "customer_group.h"
#include "manager.h"
#include "waiter.h"
#include "cashier.h"
#include "table.h"
#include "seat.h"

#include "process.h"

#include "random_generators.h"
#include "kernels.h"
#include "timer.h"
#include "records.h"

Simulator::Simulator (const TimerForm & timer_form,
                      const Variables & variables,
                      const RandomInitializerForm & random_init_form,
                      const RecorderForm & recorder_form): current_time_(timer_form.start_time),
                                                           end_time_(timer_form.end_time),
                                                           is_step_(false),
                                                           is_status_(false) {
	// Initialize Timer
	timer_ = new Timer(timer_form.start_time, timer_form.end_time);
	// Initialize variables in simulator
	chinese_restaurant_                    = new ChineseRestaurant();
	chinese_restaurant_->variables         = new Variables(variables);
	chinese_restaurant_->random_generators = new RandomGenerators();
	chinese_restaurant_->records           = new Records(recorder_form, chinese_restaurant_);
	chinese_restaurant_->clock             = timer_;
	// Initialize random generators
	auto * kernels = new Kernels();
	if (random_init_form.is_generated) kernels->GenerateKernels(random_init_form.generate_file_path);
	kernels->ReadKernels(random_init_form.generate_file_path);
	chinese_restaurant_->random_generators->Initialize(kernels, random_init_form.seed, random_init_form);
	process_ = nullptr;
}

void Simulator::Init (const bool is_step, const Log::LogPriority level, const bool is_status,
                      const bool print_event_list) {
	is_step_   = is_step;
	is_status_ = is_status;
	Log::GetLog()->SetPriority(static_cast<Log::LogPriority>(level));
	PrepareRestaurant();
	// Initialize components in process
	process_ = new Process(print_event_list);
}

void Simulator::PrepareRestaurant ( ) const {
	// Copy Variables.
	auto var = chinese_restaurant_->variables;
	// Prepare the manager.
	chinese_restaurant_->manager = new Manager(chinese_restaurant_);
	// Prepare restaurant tables (2 3 4 persons).
	for (unsigned int i = 0 ; i < 3 ; ++i) {
		for (unsigned int j = 0 ; j < var->number_tables[i] ; ++j) {
			auto table = new Table(i + 2);
			chinese_restaurant_->free_restaurant_tables.push_back(table);
			chinese_restaurant_->tables.insert(table);
		}
	}
	// Prepare buffet seats.
	for (unsigned int i = 0 ; i < var->number_buffet_seats ; i++) {
		auto seat = new Seat();
		chinese_restaurant_->free_buffet_seats.push_back(seat);
		chinese_restaurant_->buffet_seats.insert(seat);
	}
	// Prepare waiters.
	for (unsigned int i = 0 ; i < var->number_waiters ; ++i) {
		auto waiter = new Waiter();
		chinese_restaurant_->free_waiter_queue.push(waiter);
		chinese_restaurant_->waiters.insert(waiter);
	}
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->
		                                                                        free_waiter_queue.size()),
		                                              kFreeWaiterQueue
	                                              });
	// Prepare cashier.
	for (unsigned int i = 0 ; i < var->number_cashiers ; ++i) {
		auto cashier = new Cashier();
		chinese_restaurant_->free_cashiers.push(cashier);
		chinese_restaurant_->cashiers.insert(cashier);
	}
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->
		                                                                        free_cashiers.size()),
		                                              kFreeCashierQueue
	                                              });
}

void Simulator::CleanRestaurant ( ) const {
	// Delete Manager
	delete chinese_restaurant_->manager;
	// Delete Tables
	for (auto & table : chinese_restaurant_->tables) delete table;
	// Delete Buffet Seats
	for (auto & seat : chinese_restaurant_->buffet_seats) delete seat;
	// Delete Waiters
	for (auto & waiter : chinese_restaurant_->waiters) delete waiter;
	// Delete Cashiers
	for (auto & cashier : chinese_restaurant_->cashiers) delete cashier;
	printf("-----------------------------------------------------------------------\n");
	printf("                        CLEANED RESTAURANT\n\n");
	printf("-----------------------------------------------------------------------\n");

}

void Simulator::Run ( ) {
	printf("-----------------------------------------------------------------------\n");
	printf("                        OPENED RESTAURANT\n\n");
	printf("-----------------------------------------------------------------------\n");
	// Creating the first customer group.
	(new CustomerGroup(chinese_restaurant_, process_, current_time_))->Activate(current_time_);
	// Run the simulation by popping the first event.
	while (current_time_ <= end_time_) {
		auto * event = process_->PopEvent();
		current_time_ = event->event_time;
		Log::GetLog()->Print("-----------------------------------------------------------------------\n", Log::P2,
		                     Log::NONE);
		Log::GetLog()->Print("TIME: " + timer_->SecondsToTime(current_time_) + " \n", Log::P2, Log::NONE);
		auto * customer_group = event->customer_group;
		customer_group->Execute(current_time_);
		delete event;
		if (customer_group->IsTerminated()) delete customer_group;
		if (is_status_) Status();
		if (is_step_) system("pause");
	}
	printf("-----------------------------------------------------------------------\n");
	printf("                        CLOSED RESTAURANT\n\n");
	printf("-----------------------------------------------------------------------\n");
}

void Simulator::Conclude ( ) const {
	// conclude queue records
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->
		                                                                        restaurant_queue.size()),
		                                              kTableQueue
	                                              });
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->buffet_queue.size()
		                                              ),
		                                              kBuffetQueue
	                                              });
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->
		                                                                        free_waiter_queue.size()),
		                                              kFreeWaiterQueue
	                                              });
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->
		                                                                        wait_waiter_queue.size()),
		                                              kWaitWaiterQueue
	                                              });
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->
		                                                                        check_out_queue.size()),
		                                              kCheckoutQueue
	                                              });
	chinese_restaurant_->records->PushQueueRecord({
		                                              current_time_,
		                                              static_cast<unsigned int>(chinese_restaurant_->
		                                                                        free_cashiers.size()),
		                                              kFreeCashierQueue
	                                              });
	printf("-----------------------------------------------------------------------\n");
	printf("                        RESTAURANT SUMMARY\n\n");
	printf("-----------------------------------------------------------------------\n");
	chinese_restaurant_->records->ConcludeGenerators();
	chinese_restaurant_->records->ConcludeCustomers();
	chinese_restaurant_->records->ConcludeResult();
}

void Simulator::Status ( ) const {
	printf("````````STATUS```````````");

	printf("\n -- Front of Restaurant --");
	// Queue of Restaurant Customer Group
	printf("\nRestaurant Queue (%d groups): ", static_cast<int>(chinese_restaurant_->restaurant_queue.size()));
	if (!chinese_restaurant_->restaurant_queue.empty()) {
		for (auto & customer_group : chinese_restaurant_->restaurant_queue) {
			printf("#%d(%d persons) ", customer_group->GetCustomerGroupId(), customer_group->PersonsInGroup());
		}
	}
	// Queue of Buffet Customer Group
	printf("\nBuffet Queue(%d groups): ", static_cast<int>(chinese_restaurant_->buffet_queue.size()));
	if (!chinese_restaurant_->buffet_queue.empty()) {
		auto temp_buffet_queue = chinese_restaurant_->buffet_queue;
		while (!temp_buffet_queue.empty()) {
			printf("#%d(%d persons) ", temp_buffet_queue.front()->GetCustomerGroupId(),
			       temp_buffet_queue.front()->PersonsInGroup());
			temp_buffet_queue.pop();
		}
	}

	printf("\n -- Inside of Restaurant -- ");
	// Available Waiters in the queue
	printf("\nFree Waiters Queue (%d persons): ", static_cast<int>(chinese_restaurant_->free_waiter_queue.size()));
	if (!chinese_restaurant_->free_waiter_queue.empty()) {
		auto temp_free_waiter_queue = chinese_restaurant_->free_waiter_queue;
		while (!temp_free_waiter_queue.empty()) {
			printf("#%d ", temp_free_waiter_queue.front()->GetWaiterId());
			temp_free_waiter_queue.pop();
		}
	}

	// Available Tables in the restaurant
	printf("\nFree Table (%d tables): ", static_cast<int>(chinese_restaurant_->free_restaurant_tables.size()));
	for (auto & free_restaurant_table : chinese_restaurant_->free_restaurant_tables)
		printf("#%d(%d seats) ",
		       free_restaurant_table->GetTableId(),
		       free_restaurant_table->GetSeatNumber());
	// Available Buffet Seats in the restaurant
	printf("\nFree Buffet Seats (%d seats): ", static_cast<int>(chinese_restaurant_->free_buffet_seats.size()));
	for (auto & free_buffet_seat : chinese_restaurant_->free_buffet_seats
	)
		printf("#%d ", free_buffet_seat->GetSeatId());

	printf("\n -- Checkout point of Restaurant --");
	// Queue of Checkout Customer Group
	printf("\nCheckout Queue (%d groups): ", static_cast<int>(chinese_restaurant_->check_out_queue.size()));
	if (!chinese_restaurant_->check_out_queue.empty()) {
		auto temp_check_out_queue = chinese_restaurant_->check_out_queue;
		while (!temp_check_out_queue.empty()) {
			printf("#%d ", temp_check_out_queue.front()->GetCustomerGroupId());
			temp_check_out_queue.pop();
		}
	}
	// Available Cashiers
	printf("\nFree Cashier Queue (%d persons): ", static_cast<int>(chinese_restaurant_->free_cashiers.size()));
	if (!chinese_restaurant_->free_cashiers.empty()) {
		auto temp_free_cashier = chinese_restaurant_->free_cashiers;
		while (!temp_free_cashier.empty()) {
			printf("#%d ", temp_free_cashier.front()->GetCashierId());
			temp_free_cashier.pop();
		}
	}
	printf("\n");
}
