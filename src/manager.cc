#include "manager.h"

#include <string>
#include "log.h"
#include "chinese_restaurant.h"
#include "customer_group.h"
#include "variables.h"
#include "table.h"
Manager::Manager(ChineseRestaurant * chinese_restaurant) :chinese_restaurant_(chinese_restaurant) {
	Log::GetLog()->Print("Manager is created", Log::P4);
}

void Manager::Manages (CustomerGroup * this_customer, unsigned int current_time) {
	std::vector<Table *> free_table;
	// The manager look up for free table
	for (auto table : chinese_restaurant_->restaurant_tables)
		if (table->IsFree()) free_table.push_back(table);
	// If there's no free table, return 0
	if(free_table.empty()) return;
	// If there is free table
	for(auto customer_group: chinese_restaurant_->restaurant_queue) {
		for(auto table : free_table) {
			if (table->GetSeatNumber() >= customer_group->PersonsInGroup()) {
				customer_group->table_ = table;
				customer_group->Activate(current_time + chinese_restaurant_->variables->time_queue_to_table);
			}
		}
	}
}

