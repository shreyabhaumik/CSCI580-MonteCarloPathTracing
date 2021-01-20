#ifndef RANDOMOLDH
#define RANDOMOLDH

#include <cstdlib> // rand

inline double random_old_double() {
	return rand() / (RAND_MAX + 1.0);
}

#endif
