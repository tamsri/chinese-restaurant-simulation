#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

class CustomerGroup;
struct ChineseRestaurant;

class Manager final {
public:
	explicit Manager(ChineseRestaurant * chinese_restaurant);
	void Manages (unsigned int current_time) const;
private:
	ChineseRestaurant * chinese_restaurant_;
};


#endif // !MANAGER_MODEL_H