#ifndef MODEL_CASHIER_H
#define MODEL_CASHIER_H

class CustomerGroup;

class Cashier {
public:
	Cashier();
	unsigned int GetCashierID() const;
	void ProvideServiceTo(CustomerGroup * customer_group);
	void CompleteService();
	bool IsAvailable() const;
private:
	const unsigned int cashier_id_;
	static unsigned int cashier_global_id_;
	CustomerGroup * service_to;
};


#endif // !MODEL_CASHIER_H

