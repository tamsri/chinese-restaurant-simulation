#ifndef MODEL_CUSTOMER_H
#define MODEL_CUSTOMER_H

class Customer {
private:
	const unsigned int customer_id_;
	static unsigned int customer_global_id_;
public:
	Customer();
	unsigned int GetPersonID() const;
};

#endif // MODEL_CUSTOMER_H
