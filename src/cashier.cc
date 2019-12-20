#include "cashier.h"

#include "customer_group.h"

unsigned int Cashier::cashier_global_id_ = 0;


Cashier::Cashier() : cashier_id_(++cashier_global_id_) {}
unsigned int Cashier::GetCashierID() const { return cashier_id_; }
void Cashier::ProvideServiceTo(CustomerGroup * customer_group) {
	service_to = customer_group;
}
void Cashier::CompleteService() {
	service_to = nullptr;
}
bool Cashier::IsAvailable() const {
	return service_to == nullptr;
}

