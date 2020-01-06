#ifndef  UTILS_KERNELS_H
#define UTILS_KERNELS_H
#include <string>
#include <vector>
#include <map>

class Kernels final {
public:
	enum GenerateTypes
	{
		TIA1 = 0,   // Time Interval Arrival 1
		TIA2,	    // Time Interval Arrival 2
		TWS,		// Time Waiter Service
		TBS1,		// Time Buffet Service 1
		TBS2,		// Time Buffet Service 2
		TCS,        // Time Cashier Service
		COUNT
	};

	Kernels();

	int GetKernel(GenerateTypes generate_type, int set) const;
	void GenerateKernels(std::string & output_file_path) const;
	void ReadKernels(std::string & input_file_path);
private:

	const int number_of_sets_;
	const int set_length_;

	bool kernels_loaded_;

	std::map<GenerateTypes, std::vector<int> > kernel_set_;
};

#endif // ! UTILS_KERNELS_H
