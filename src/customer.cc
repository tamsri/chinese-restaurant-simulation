#include "customer.h"
#include <cassert>

unsigned int Customer::customer_global_id_ = 0;

Customer::Customer() : customer_id_(++customer_global_id_) { };

unsigned int Customer::GetPersonId() const {
	
	return customer_id_;
};