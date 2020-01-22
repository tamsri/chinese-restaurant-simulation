#ifndef UTILS_GENERATORS_H
#define UTILS_GENERATORS_H
#include <vector>

class UniformGenerator final {
public:
	explicit UniformGenerator (int kernel); // Initialize variables and kernel
	double Rand ( );                        // Random double number between 0 and 1
	double Rand (int start, int end);       // Random double number from start to end

	int GetKernel ( ) const; // Return number of the kernel
private:
	int kernel_;
	double m_;
	int a_;
	int q_;
	int r_;
};

class ExponentialGenerator final {
public:
	explicit ExponentialGenerator (UniformGenerator * uniform_generator, double lambda);
	unsigned int Rand ( ) const;
private:
	double lambda_;
	UniformGenerator * uniform_generator_;
};

class NormalGenerator final {
public:
	explicit NormalGenerator (UniformGenerator * uniform_generator_1,
	                          UniformGenerator * uniform_generator_2,
	                          UniformGenerator * uniform_generator_3,
	                          double average  = 0,
	                          double variance = 1);
	unsigned int Rand ( ) const;
private:
	UniformGenerator * uniform_generator_1_;
	UniformGenerator * uniform_generator_2_;
	UniformGenerator * uniform_generator_3_;
	double average_;
	double variance_;
};

class ProbabilityGenerator final {
public:
	explicit ProbabilityGenerator (UniformGenerator * uniform_generator, std::vector<double> probabilities);
	unsigned int Rand ( ) const;
private:
	UniformGenerator * uniform_generator_;
	std::vector<double> probabilities_;
};

class BoolProbabilityGenerator final {
public:
	explicit BoolProbabilityGenerator (UniformGenerator * uniform_generator, double true_probability);
	bool Rand ( ) const;
private:
	UniformGenerator * uniform_generator_;
	double true_probability_;
};
#endif
