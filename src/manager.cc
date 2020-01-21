#include "manager.h"

#include <string>
#include "log.h"
#include "records.h"

#include "chinese_restaurant.h"
#include "customer_group.h"
#include "variables.h"
#include "table.h"
#include "timer.h"

Manager::Manager(ChineseRestaurant * chinese_restaurant) :	chinese_restaurant_(chinese_restaurant),
															is_free_(true){
	Log::GetLog()->Print("Manager is created");
}

void Manager::Manages (const unsigned int current_time) {
	// If there's no free table, return 0
	if(chinese_restaurant_->free_restaurant_tables.empty()) return;
	// If there is free table
	for (unsigned int i_customer_group = 0; i_customer_group < chinese_restaurant_->restaurant_queue.size(); i_customer_group++) {
		auto customer_group = chinese_restaurant_->restaurant_queue[i_customer_group];
		for (unsigned int i_table = 0; i_table < chinese_restaurant_->free_restaurant_tables.size(); i_table++) {
			auto * table = chinese_restaurant_->free_restaurant_tables[i_table];
			if (table->GetSeatNumber() >= customer_group->PersonsInGroup() && !customer_group->IsCalled()) {
				// Set the manager not free to leading another group
				MakeBusy();
				// Assign the table for the customer group
				customer_group->AssignTable(table);
				// Call customer group to avoid recalling by another group (Fixed bug)
				customer_group->Call();
				// Assign state for the leaded group
				customer_group->AssignState(CustomerGroup::kRestaurantArriveTableSate);
				// Push the record of the customer status
				chinese_restaurant_->records->PushCustomerRecord({ customer_group->GetCustomerGroupId(), current_time, CustomerGroup::kRestaurantArriveTableSate });
				// Generate the arrival time for the group
				const unsigned int arrive_to_table_time = current_time + chinese_restaurant_->variables->time_queue_to_table;
				customer_group->Activate(arrive_to_table_time);
				Log::GetLog()->Print("Manager assigned table #" + std::to_string(table->GetTableId()) 
										+ " to customer group #" + std::to_string(customer_group->GetCustomerGroupId())
										+ " will arrive to table at "+ chinese_restaurant_->clock->ConvertTime(arrive_to_table_time));
				// Push the record of the table waiting queue
				chinese_restaurant_->records->PushQueueRecord({ current_time,static_cast<unsigned int>(chinese_restaurant_->restaurant_queue.size()),kTableQueue });
				// Remove the selected group from the restaurant table queue
				chinese_restaurant_->restaurant_queue.erase(chinese_restaurant_->restaurant_queue.begin() + i_customer_group);
				// Remove the selcted table from the free tables queue
				chinese_restaurant_->free_restaurant_tables.erase(chinese_restaurant_->free_restaurant_tables.begin() + i_table);
				return;
			}
		}
	}
}

bool Manager::IsFree ( ) const {
	return is_free_;
}

void Manager::MakeFree ( ) {
	is_free_ = true;
}

void Manager::MakeBusy ( ) {
	is_free_ = false;
}

