#ifndef UTILITY_GENERATOR_H
#define UTILITY_GENERATOR_H

class Generator {

public:
	Generator();
	unsigned int GenerateNormalDistribution(unsigned int average, unsigned int variance);
	unsigned int GenerateExponentialDistribution(unsigned int average);
	unsigned int GenerateByProbability(float probabilities[]);
	bool GenerateBoolByProbability(float probability);
};

#endif // !UTILITY_GENERATOR_H_