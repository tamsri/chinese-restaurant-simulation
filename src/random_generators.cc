#include "random_generators.h"
#include "generators.h"
#include "kernels.h"
RandomGenerators::RandomGenerators ( ): interval_arrival_generator(nullptr),
                                        waiter_service_generator(nullptr),
                                        buffet_service_generator(nullptr),
                                        cashier_service_generator(nullptr),
										persons_generator(nullptr),
                                        buffet_generator(nullptr) { }

void RandomGenerators::Initialize (		Kernels * kernels,
										const int set_index,
										const RandomInitializerForm & initializer_form) {
	
	interval_arrival_generator = new NormalGenerator(	new UniformGenerator(kernels->GetKernel(Kernels::TIA1, set_index)),
														new UniformGenerator(kernels->GetKernel(Kernels::TIA2, set_index)),
														initializer_form.average_interval_arrival_time,
														initializer_form.variance_interval_arrival_time);
	
	waiter_service_generator = new ExponentialGenerator(new UniformGenerator(kernels->GetKernel(Kernels::TWS, set_index)),
													(1/static_cast<double>(initializer_form.average_waiter_service_time)));
	
	buffet_service_generator = new NormalGenerator(new UniformGenerator(kernels->GetKernel(Kernels::TBS1, set_index)),
													new UniformGenerator(kernels->GetKernel(Kernels::TBS2, set_index)),
														initializer_form.average_buffet_service_time, 
														initializer_form.variance_buffet_service_time);
	
	cashier_service_generator = new ExponentialGenerator(new UniformGenerator(kernels->GetKernel(Kernels::TCS, set_index)),
														1/static_cast<double>(initializer_form.average_cashier_service_time));
	persons_generator = new ProbabilityGenerator(new UniformGenerator(kernels->GetKernel(Kernels::TPG, set_index)),
												initializer_form.persons_in_group_probability);

	buffet_generator = new BoolProbabilityGenerator(new UniformGenerator(kernels->GetKernel(Kernels::TPB, set_index)),
													initializer_form.buffet_probability);
}
