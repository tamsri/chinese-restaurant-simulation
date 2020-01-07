#include "generators.h"
#include <cmath>

UniformGenerator::UniformGenerator (const int kernel) : kernel_(kernel) {
	m_ = 2147483647.0;
	a_ = 16807;
	q_ = 127773;
	r_ = 2836;
}

double UniformGenerator::Rand () {
	const int h = kernel_ / q_;
	kernel_ = a_ * (kernel_ - q_ * h) - r_ * h;
	if (kernel_ < 0)
		kernel_ = kernel_ + static_cast < int > (m_);
	return kernel_ / m_;
}

double UniformGenerator::Rand (const int start, const int end) {
	return Rand()*(end - start) + start;
}

int UniformGenerator::GetKernel ( ) const {
	return kernel_;
}

ExponentialGenerator::ExponentialGenerator (UniformGenerator * uniform_generator,const double lambda):
																	lambda_(lambda),
																	uniform_generator_(uniform_generator) {
}

unsigned int ExponentialGenerator::Rand ( ) const {
	// Inverse Transformation Method
	const auto k = uniform_generator_->Rand();
	return static_cast<unsigned int>(-1/( lambda_) * log(k));
}

NormalGenerator::NormalGenerator(	UniformGenerator * uniform_generator_1, 
									UniformGenerator * uniform_generator_2,
									UniformGenerator * uniform_generator_3,
									const double average, const double variance):
										uniform_generator_1_(uniform_generator_1),
										uniform_generator_2_(uniform_generator_2),
										uniform_generator_3_(uniform_generator_3),
										average_(average),
										variance_(variance)
{
}
unsigned int NormalGenerator::Rand() const {
	// Rejection Method
	auto generated_1 = uniform_generator_1_->Rand();
	auto generated_2 = uniform_generator_2_->Rand();
	auto x = -log(generated_1);
	while (generated_2 > exp(-0.5*pow(x-1, 2))  ) {
		generated_1 = uniform_generator_1_->Rand();
		generated_2 = uniform_generator_2_->Rand();
		x = -log(generated_1);
	}
	const auto generated_3 = uniform_generator_3_->Rand();
	const auto sign = (generated_3 <= 0.5f) ? 1 : -1;
	return static_cast<unsigned int>(sqrt(variance_)*x*sign+average_);
}


ProbabilityGenerator::ProbabilityGenerator (UniformGenerator * uniform_generator, std::vector<double> probabilities): uniform_generator_(uniform_generator),
																													  probabilities_(std::move(probabilities)) {
}

unsigned int ProbabilityGenerator::Rand() const
{
	unsigned int answer = 1;
	double keeper = 0;
	const auto random_number = uniform_generator_->Rand();
	for (auto probability : probabilities_) {
		keeper += probability;
		if (random_number <= keeper) break;
		answer++;
	}
	return answer;
}

BoolProbabilityGenerator::BoolProbabilityGenerator (UniformGenerator * uniform_generator, const double true_probability):	uniform_generator_(uniform_generator),
																															true_probability_(true_probability) {
	
}
bool BoolProbabilityGenerator::Rand ( ) const {
	return uniform_generator_->Rand() <= true_probability_;
}
