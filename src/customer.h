#ifndef MODEL_CUSTOMER_H
#define MODEL_CUSTOMER_H

class Customer {
public:
	Customer();
	unsigned int GetPersonId() const;
private:
	const unsigned int customer_id_;
	static unsigned int customer_global_id_;
};

#endif // MODEL_CUSTOMER_H
