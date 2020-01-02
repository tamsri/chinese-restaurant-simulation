#include "manager.h"

#include <string>
#include "log.h"
#include "chinese_restaurant.h"
#include "customer_group.h"
#include "variables.h"
#include "table.h"


Manager::Manager(ChineseRestaurant * chinese_restaurant) :chinese_restaurant_(chinese_restaurant) {
	Log::GetLog()->Print("Manager is created");
}

Manager::~Manager ( ) {
	Log::GetLog()->Print("Manager is deleted");
}

void Manager::Manages (unsigned int current_time) const {
	// If there's no free table, return 0
	if(chinese_restaurant_->free_restaurant_tables.empty()) return;
	// If there is free table
	for (unsigned int i_customer_group = 0; i_customer_group < chinese_restaurant_->restaurant_queue.size(); i_customer_group++) {
		auto customer_group = chinese_restaurant_->restaurant_queue[i_customer_group];
		for (unsigned int i_table = 0; i_table < chinese_restaurant_->free_restaurant_tables.size(); i_table++) {
			auto * table = chinese_restaurant_->free_restaurant_tables[i_table];
			if (table->GetSeatNumber() >= customer_group->PersonsInGroup()) {
				//const unsigned int arrive_to_table_time = current_time + chinese_restaurant_->variables->time_queue_to_table;
				const unsigned int arrive_to_table_time = current_time + chinese_restaurant_->variables->time_queue_to_table;
				customer_group->AssignTable(table);
				customer_group->AssignState(CustomerGroup::kRestaurantWaiterState);
				customer_group->Activate(arrive_to_table_time);
				Log::GetLog()->Print("Manager assigned table #" + std::to_string(table->GetTableID()) + " to customer group #" + std::to_string(customer_group->GetCustomerGroupID())+ " will arrive to table at "+ std::to_string(arrive_to_table_time));
				chinese_restaurant_->restaurant_queue.erase(chinese_restaurant_->restaurant_queue.begin() + i_customer_group);
				chinese_restaurant_->free_restaurant_tables.erase(chinese_restaurant_->free_restaurant_tables.begin() + i_table);
				return;
			}
		}
	}
}

