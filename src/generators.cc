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

double ExponentialGenerator::Rand ( ) const {
	const auto k = uniform_generator_->Rand();
	return -(1.0 / lambda_) * log(k);
}

NormalGenerator::NormalGenerator(	UniformGenerator * uniform_generator_1, 
									UniformGenerator * uniform_generator_2,
									const double average, const double variance):
										uniform_generator_1_(uniform_generator_1),
										uniform_generator_2_(uniform_generator_2),
										average_(average),
										variance_(variance)
{
}
double NormalGenerator::Rand() const {
	auto generated_1 = uniform_generator_1_->Rand();
	auto generated_2 = uniform_generator_2_->Rand();
	auto x = -log(generated_1);
	while (generated_2 > pow(x-1, 2)) {
		
	}

	return x;
}