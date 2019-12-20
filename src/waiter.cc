#include "waiter.h"
#include "log.h"
#include "table.h"
#include <string>

unsigned int Waiter::waiter_global_id_ = 0;

Waiter::Waiter():waiter_id_(++waiter_global_id_) {
	log_ = Log::GetLog();
	log_->Print("Waiter #" + std::to_string(GetWaiterID()) +" is created.", Log::P4);
}

Waiter::~Waiter() {
	log_->Print("Waiter #" + std::to_string(GetWaiterID()) + " is destroyed.", Log::P4);
}

unsigned int Waiter::GetWaiterID() const {
	return waiter_id_;
}

void Waiter::ProvideServiceTo(Table * table) {
	log_->Print("Waiter #" + std::to_string(GetWaiterID()) + "started service on Table #" + std::to_string(service_to_->GetTableID()), Log::P4);
	service_to_ = table;
}

void Waiter::CompleteService() {
	log_->Print("Waiter #" + std::to_string(GetWaiterID()) + "finished service on Table #" + std::to_string(service_to_->GetTableID()), Log::P4);
	service_to_ = nullptr;
}

bool Waiter::IsAvailable() const {
	if (service_to_ != nullptr) return false;
	return true;
}