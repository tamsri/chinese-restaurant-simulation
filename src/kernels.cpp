#include "kernels.h"
#include <cassert>
#include <fstream>
#include "generators.h"
#include <sstream>

Kernels::Kernels(): number_of_sets_(100),
					set_length_(1000000),
                    kernels_loaded_(false) {
	for(auto i = 0; i != COUNT; ++i) {
		kernel_set_.insert(std::pair<GenerateTypes, std::vector<int>>(static_cast<GenerateTypes>(i), std::vector<int>()));
	}
}

int Kernels::GetKernel (const GenerateTypes generate_type, const unsigned int & set) const {
	assert(kernels_loaded_ && "Kernels::GetKernel(): Kernels not loaded from the file!");
	assert(generate_type != COUNT && "Kernels::GetKernel(): Wrong generator type!");
	const auto set_size = kernel_set_.at(generate_type);
	assert(set >= 0 && set < set_size.size() && "Kernels::GetKernel(): set index out of range!");
	return kernel_set_.at(generate_type)[set];
}

void Kernels::GenerateKernels (std::string & output_file_path) const {
	printf("Generating Kernels\n");
	const int kernel = 1;
	std::ofstream output_file(output_file_path);
	auto * uniform_generator = new UniformGenerator(kernel);
	if(output_file.is_open()) {
		// Generate a line of integers for each type of generator
		for(auto generate_type_index = 0; generate_type_index < COUNT; ++generate_type_index) {
			
			printf("Generator Type #%d/%d Set:\n", generate_type_index + 1, COUNT);
			
			for(auto set_index = 0; set_index < number_of_sets_; ++set_index) {
				printf("%d/%d, ", set_index + 1, number_of_sets_);
				for(auto index = 0; index < set_length_; ++index ) {
					uniform_generator->Rand();
				}
				output_file << uniform_generator->GetKernel() << " ";
			}
			printf("\n");
			output_file << "\n";
		}
		output_file.close();
		printf("Successfully generated Kernels\n");
	} else {
		printf("Cannot write the file\n");
	}
	delete uniform_generator;
}

void Kernels::ReadKernels (std::string & input_file_path) {
	printf("Reading kernels from %s\n", input_file_path.c_str());
	std::ifstream file_input(input_file_path);
	
	if (file_input.is_open()) {
		std::string line;
		auto line_index = 0;
		while(getline(file_input, line) && line_index < COUNT ) {
			std::vector<int> kernels;
			// using istringstream allowing split and input number directly
			std::istringstream line_stream(line);
			int number;
			while(line_stream >> number)
				kernels.push_back(number);
			kernel_set_[static_cast<GenerateTypes>(line_index)] = kernels;
			++line_index;
		}
		printf("Successfully read the kernels\n");
		kernels_loaded_ = true;
		file_input.close();
		
	}else {
		assert("Opening Kernels File Error!");
	}
}
