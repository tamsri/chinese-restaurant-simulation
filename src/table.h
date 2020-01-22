#ifndef MODEL_TABLE_H
#define MODEL_TABLE_H

#include <vector>
class CustomerGroup;
class Log;
class Seat;

class Table final {
public:
	explicit Table (unsigned int seat_number);
	unsigned int GetTableId ( ) const;
	unsigned int GetSeatNumber ( ) const;
	bool IsFree ( ) const;

	void OnSit (CustomerGroup * arrival_customer_group);
	void OnLeave ( );
	CustomerGroup * occupied_customer_group_;

private:
	const unsigned int table_id_;
	static unsigned int table_global_id_;
	std::vector<Seat> seats_;
};
#endif // MODEL_TABLE_H
