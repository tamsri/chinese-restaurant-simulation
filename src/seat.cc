#include "seat.h"

#include <string>
#include "log.h"
#include "customer.h"


unsigned int Seat::seat_global_id_ = 0;

Seat::Seat() : seat_id_(++seat_global_id_) {
	log_ = Log::GetLog();
	log_->Print("Seat #" + std::to_string(GetSeatID()) + " is created.", Log::P4);
};

Seat::~Seat() {
	occupied_customer_ = nullptr;
	log_->Print("Seat #" + std::to_string(GetSeatID()) + " is deleted.", Log::P4);
}
Customer * Seat::WhoSit() const {
	return occupied_customer_;
}

void Seat::OnSit(Customer * customer) {
	occupied_customer_ = customer;
	log_->Print("Customer #" + std::to_string(customer->GetPersonId()) + " occupied the seat #" + std::to_string(GetSeatID()) + ".", Log::P4);
}

void Seat::OnLeave() {
	occupied_customer_ = nullptr;
	log_->Print("Customer #" + std::to_string(occupied_customer_->GetPersonId()) + " left the seat #" + std::to_string(GetSeatID()) + ".", Log::P4);
}
bool Seat::IsEmpty() const {
	return occupied_customer_ == nullptr;
}

unsigned Seat::GetSeatID() const {
	return seat_id_;
}
