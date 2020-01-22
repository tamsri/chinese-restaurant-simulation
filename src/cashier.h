#ifndef MODEL_CASHIER_H
#define MODEL_CASHIER_H

class CustomerGroup;

class Cashier final {
public:
	Cashier ( );
	unsigned int GetCashierId ( ) const;
	void ProvideServiceTo (CustomerGroup * customer_group);
	void CompleteService ( );
	bool IsAvailable ( ) const;
private:
	const unsigned int cashier_id_;
	static unsigned int cashier_global_id_;
	CustomerGroup * service_to_;
};

#endif // !MODEL_CASHIER_H
