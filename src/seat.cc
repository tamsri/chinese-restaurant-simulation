#include "seat.h"

#include <string>
#include "log.h"
#include "customer.h"


unsigned int Seat::seat_global_id_ = 0;

Seat::Seat() : seat_id_(++seat_global_id_) {
	Log::GetLog()->Print("Seat #" + std::to_string(GetSeatId()) + " is created.", Log::P4);
};

Seat::~Seat() {
	occupied_customer_ = nullptr;
	Log::GetLog()->Print("Seat #" + std::to_string(GetSeatId()) + " is deleted.", Log::P4);
}

void Seat::OnSit(Customer * customer) {
	occupied_customer_ = customer;
}

void Seat::OnLeave() {
	occupied_customer_ = nullptr;
}
bool Seat::IsEmpty() const {
	return occupied_customer_ == nullptr;
}

unsigned Seat::GetSeatId() const {
	return seat_id_;
}
