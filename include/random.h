#ifndef RANDOMH
#define RANDOMH

#include <functional>
#include <random>

// Modern random generator
inline double random_double() {
	// Space for the static variable is allocated only once (reuse space!)
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	static std::function<double()> rand_generator = std::bind(distribution, generator);
	return rand_generator(); // returns value in [0, 1)
}

#endif
