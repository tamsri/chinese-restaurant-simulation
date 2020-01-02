#include "table.h"

#include "string"
#include "log.h"
#include "seat.h"
#include "customer_group.h"


unsigned int Table::table_global_ = 0;

Table::Table(const unsigned int seat_number) : table_id_(++table_global_) {
	for (unsigned int i = 0; i < seat_number; ++i) {
		Seat * s = new Seat();
		seats_.push_back(s);
	}
	Log::GetLog()->Print("Restaurant Table for " + std::to_string(seat_number) + " persons #" + std::to_string(table_id_) + " is created");
}

Table::~Table() {
	for (auto seat : seats_) {
		delete seat;
	}
	Log::GetLog()->Print("Restaurant Table #" + std::to_string(table_id_) + " is deleted");
}
void Table::OnSit(CustomerGroup * arrival_customer_group) {
	occupied_customer_group_ = arrival_customer_group;
	for (unsigned int i = 0; i < occupied_customer_group_->PersonsInGroup(); ++i) {
		seats_[i]->OnSit(occupied_customer_group_->GetCustomerMember(i));
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(occupied_customer_group_->GetCustomerGroupID()) + "occupied the Restaurant Table #" + std::to_string(GetTableID()), Log::P3);
}
void Table::OnLeave() {
	for (auto seat : seats_) {
		seat->OnLeave();
	}
	Log::GetLog()->Print("Customer Group #" + std::to_string(occupied_customer_group_->GetCustomerGroupID()) + "left the Restaurant Table #" + std::to_string(GetTableID()), Log::P3);
	occupied_customer_group_ = nullptr;
}
unsigned int Table::GetSeatNumber() const {
	return static_cast<unsigned int>(seats_.size());
}
bool Table::IsFree() const {
	return occupied_customer_group_ == nullptr;
}
unsigned int Table::GetTableID() const {
	return table_id_;
}
