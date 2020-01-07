#include <iostream>

#include "simulator.h"
#include "variables.h"

int main ( ) {
	const Variables variables = {
			6,
			{8, 14, 4},
			{0.11f, 0.33f, 0.33f, 0.23f},
			1500,
			100*100,
			40,
			4000,
			220,
			5,
			20,
			0.6f,
			1900,
			200*200
		};
	const int kernel_set = 5;
	printf("-----------------------------------------------------------------------\n");
	printf("                   The Chinese Restaurant\n\n");
	printf("                                                      By Supawat Tamsri\n");
	printf("-----------------------------------------------------------------------\n");
	auto simulator = new Simulator(36000, variables, kernel_set);
	int mode, priority;
	do {
		printf("Select the simulation mode ( 1 - Step by Step, 0 - Non-stop ): ");
		std::cin >> mode;
	}
	while (mode != 1 && mode != 0);
	do {
		printf("Select logging priority (1-4): ");
		std::cin >> priority;
	}
	while (priority < 1 || priority > 4);
	simulator->Init(mode == 1, Log::LogPriority(priority));

	simulator->Run();

	delete simulator;
	system("pause");
	return 0;
}
