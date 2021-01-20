#ifndef HAMMERSLAYH
#define HAMMERSLAYH

//#include <cmath>
//#include <stdlib.h>

class hammersley {
	public:
		hammersley() {}
		//HammerslaySampler() {}
		~hammersley(){}

		double vdc(int n, double base);
		double * get_hammersley(int n, int base, int N);
		//Vec3f sample();
};

double hammersley::vdc(int n, double base) {
	double vdc = 0, denom = 1;
	while(n) {
		vdc += fmod(n, base) / (denom *= base);
		n /= base; // note: conversion from 'double' to 'int'
	}
	return vdc;
}

double * hammersley::get_hammersley(int n, int base, int N) {
	static double  x[2];
	x[0] = vdc(n, base);
	x[1] = (double)n / (double)N;
	return x;
}

/*
Vec3f hammersley::sample(int index, int base, int num_sample) {
	double *p1 = get_hammersley(index, base, num_sample);
	return Vec3f(p1[0], p1[1], 0.0f);
}
*/

#endif
