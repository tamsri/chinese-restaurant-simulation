#ifndef UTILS_GENERATORS_H
#define UTILS_GENERATORS_H
class UniformGenerator final {
public:
	explicit UniformGenerator(int kernel);	// Initialize variables and kernel
	double Rand();							// Random double number between 0 and 1
	double Rand(int start, int end);		// Random double number from start to end

	int GetKernel() const;					// Return number of the kernel
private:
	int kernel_;
	double m_;
	int a_;
	int q_;
	int r_;
};

class ExponentialGenerator final {
public:
	explicit ExponentialGenerator(UniformGenerator * uniform_generator, double lambda);
	double Rand() const;
private:
	double lambda_;
	UniformGenerator * uniform_generator_;
};

class NormalGenerator final {
public:
	explicit NormalGenerator(	UniformGenerator * uniform_generator_1, 
								UniformGenerator * uniform_generator_2,
								double average,
								double variance);
	double Rand() const;
private:
	UniformGenerator * uniform_generator_1_;
	UniformGenerator * uniform_generator_2_;
	double average_;
	double variance_;
};
#endif 