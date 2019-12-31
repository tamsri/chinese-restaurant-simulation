#include "table.h"

#include "string"
#include "log.h"
#include "seat.h"
#include "customer_group.h"


unsigned int Table::table_global_ = 0;

Table::Table(const unsigned int seat_number) : table_id_(++table_global_) {
	log_ = Log::GetLog();
	for (unsigned int i = 0; i < seat_number; ++i) {
		Seat * s = new Seat();

		seats_.push_back(s);
	}
	auto text = "Restaurant Table " + std::to_string(seat_number) + " persons #" + std::to_string(table_id_) + " is created.";
	log_->Print(text, Log::P4);
}

Table::~Table() {
	for (auto seat : seats_) {
		auto text = "Seat #" + std::to_string(seat->GetSeatID()) + " is deleted from Table #" + std::to_string(table_id_)+ ".";
		log_->Print(text, Log::P4);
		delete seat;
	}
	auto text = "Restaurant Table #" + std::to_string(table_id_) + " is deleted.";
	log_->Print(text, Log::P4);
}
void Table::OnSit(CustomerGroup * arrival_customer_group) {
	occupied_customer_group_ = arrival_customer_group;
	for (unsigned int i = 0; i < GetSeatNumber(); ++i) {
		seats_[i]->OnSit(occupied_customer_group_->GetCustomerMember(i));
	}
	log_->Print("Customer Group #" + std::to_string(occupied_customer_group_->GetCustomerGroupID()) + "occupied the Restaurant Table #" + std::to_string(GetTableID()) + ".", Log::P3);
}
void Table::OnLeave() {
	for (auto seat : seats_) {
		seat->OnLeave();
	}
	log_->Print("Customer Group #" + std::to_string(occupied_customer_group_->GetCustomerGroupID()) + "left the Restaurant Table #" + std::to_string(GetTableID()) + ".", Log::P3);
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
