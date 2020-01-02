#ifndef MODEL_TABLE_H
#define MODEL_TABLE_H

#include <vector>
class CustomerGroup;
class Log;
class Seat;

class Table {
public:
	Table(const unsigned int seat_number);
	~Table();

	unsigned int GetTableID() const;
	unsigned int GetSeatNumber() const;
	bool IsFree ( ) const;

	void OnSit (CustomerGroup * arrival_customer_group);
	void OnLeave();
	CustomerGroup * occupied_customer_group_;

private:
	const unsigned int table_id_;
	static unsigned int table_global_;
	std::vector<Seat * > seats_;
};
#endif // MODEL_TABLE_H
