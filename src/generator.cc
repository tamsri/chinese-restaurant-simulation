#include "generator.h"
#include <ctime>
#include <cstdlib>
#include "log.h"

Generator::Generator() {
	srand(static_cast<unsigned int>(time(nullptr)));
	Log::GetLog()->Print("Generator Initialized.", Log::P4);
}

unsigned int Generator::GenerateNormalDistribution(unsigned int average, unsigned int variance) {
	/// TODO implement normal distribution function
	return  rand()%5+1;
}

unsigned int Generator::GenerateExponentialDistribution(unsigned int average) {
	/// TODO implement exponential distribution function
	return rand()%5+1;
}

unsigned int Generator::GenerateByProbability(float probabilities[4]) {
	/// TODO implement probability generator function
	return rand()%3+1;
}
bool Generator::GenerateBoolByProbability(float probability) {
	return static_cast<unsigned int>(rand() % 100) < probability * 100;
}