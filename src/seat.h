#ifndef MODEL_SEAT_H
#define MODEL_SEAT_H

class Customer;

class Seat final {
public:
	Seat ( );
	unsigned int GetSeatId ( ) const;
	bool IsEmpty ( ) const;
	void OnSit (Customer * customer);
	void OnLeave ( );
private:
	static unsigned int seat_global_id_;
	const unsigned int seat_id_;
	Customer * occupied_customer_;
};

#endif // MODEL_SEAT_H
