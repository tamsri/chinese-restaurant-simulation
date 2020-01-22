#include "cashier.h"

#include "customer_group.h"

unsigned int Cashier::cashier_global_id_ = 0;

Cashier::Cashier ( ) : cashier_id_(++cashier_global_id_),
                       service_to_(nullptr) {}

unsigned int Cashier::GetCashierId ( ) const { return cashier_id_; }

void Cashier::ProvideServiceTo (CustomerGroup * customer_group) { service_to_ = customer_group; }

void Cashier::CompleteService ( ) { service_to_ = nullptr; }

bool Cashier::IsAvailable ( ) const { return service_to_ == nullptr; }
