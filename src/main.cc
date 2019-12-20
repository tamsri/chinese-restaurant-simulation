#include <iostream>

#include "simulator.h"
#include "variables.h"
int main() {
	const Variables variables = { 6,
	{8,14,4},
	{0.11f, 0.33f,0.33f,0.23f},
	1500,
	100,
	40,
	4000,
	220,
	5,
	20,
	0.6f,
	1900,
	200 };
	
	printf("-----------------------------------------------------------------------\n");
	printf("                   The Chinese Restaurant\n\n");
	printf("                                                      By Supawat Tamsri\n");
	printf("-----------------------------------------------------------------------\n");
	Simulator * simulator = new Simulator(200, variables);
	int mode, priority;
	do {
		printf("Select the simulation mode ( 1 - Step by Step, 0 - Non-stop ): ");
		std::cin >> mode;
	} while (mode != 1 && mode != 0);
	do {
		printf("Select logging priority (1-4): ");
		std::cin >> priority;
	} while (priority < 1 || priority > 4);
	simulator->Init(mode == 1, Log::LogPriority(priority));
	printf("-----------------------------------------------------------------------\n");
	printf("                        OPENED RESTAURANT\n\n");
	printf("-----------------------------------------------------------------------\n");
	simulator->Run();
	printf("-----------------------------------------------------------------------\n");
	printf("                        CLOSED RESTAURANT\n\n");
	printf("-----------------------------------------------------------------------\n");
	delete simulator;
	system("pause");
	return 0;
}
