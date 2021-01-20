#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "../include/random.h"
#include "../include/vec3.h"

inline float pdf  (float x) { return 0.5 * x; } // p(x) = x/2
inline float pdf_2(float x) { return 0.5; }     // p(x) = 1/2
inline float pdf_3(float x) { return 3*x*x/8; } // p(x) = 3/8 * x^2
inline float pdf_4(const vec3& p) { return 1 / (4*M_PI); }

inline vec3 random_cosine_direction() {
	float r1 = random_double();
	float r2 = random_double();
	float z = sqrt(1-r2);
	float phi = 2*M_PI*r1;
	float x = cos(phi)*sqrt(r2);
	float y = sin(phi)*sqrt(r2);
	return vec3(x, y, z);
}

void random_limited_iteration();
void random_forever_iteration();
void stratified();
void monte_carlo();
void nonuniform_sample();
void uniform_sample();
void nonuniform_sample_2();
void sphere_example();

void random_direction_on_hemisphere();
void random_direction_on_hemisphere_rand_in_cosine();

vec3 random_on_unit_sphere_test();


int main() {
	// 1) Example of estimating PI
	//random_limited_iteration();
	//random_forever_iteration();
	//stratified();
	
	// 2) Example of I = SUM[x^2] from 0 to 2 (Ans is I = 2.66667)
	//monte_carlo();         // Returns I = 2.66587
	//nonuniform_sample();   // Returns I = 2.66726
	//uniform_sample();      // Returns I = 2.66587
	//nonuniform_sample_2(); // Returns I = 2.66667!

	//sphere_example();

	//random_direction_on_hemisphere();
	random_direction_on_hemisphere_rand_in_cosine();
}


void random_limited_iteration() {
	int N = 1000;
	int inside_circle = 0;
	for (int i = 0; i < N; i++) {
		float x = 2*random_double() - 1;
		float y = 2*random_double() - 1;
		if(x*x + y*y < 1)
			inside_circle++;
	}
	// (area of circle) / (area of square) = PI * r^2 / (2r)^2 = PI / 4
	// (# of points inside circle) / (# samples) = PI / 4
	std::cout << "Estimate of Pi = " << 4*float(inside_circle) / N << "\n";
}


void random_forever_iteration() {
	int inside_circle = 0;
	int runs = 0;
	while (true) {
		runs++;
		float x = 2*random_double() - 1;
		float y = 2*random_double() - 1;
		if(x*x + y*y < 1)
			inside_circle++;

		if(runs% 100000 == 0)
			std::cout << "Estimate of Pi = " << 4*float(inside_circle) / runs << "\n";
	}
	// Law of Diminishing Returns = each sample helps less than the last one
	// -> The last affects a lot to the result
	// We can weaken this sideeffecct with stratifying
}


/* Stratifying/Jittering
 * Instead of taking random samples, take a grid and take one sample within each
 *
 * Problem (Curse of Dimensionality)
 * Converges with a better asymptotic rate in lower dimension
 * but this feature decreases with the dimension of the problem
 */
void stratified() {
	int inside_circle = 0;
	int inside_circle_stratified = 0;
	int sqrt_N = 10000;
	for (int i = 0; i < sqrt_N; i++) {
		for (int j = 0; j < sqrt_N; j++) {
			float x = 2*random_double() - 1;
			float y = 2*random_double() - 1;
			if (x*x + y*y < 1)
				inside_circle++;
			x = 2*((i + random_double()) / sqrt_N) - 1;
			y = 2*((j + random_double()) / sqrt_N) - 1;
			if (x*x + y*y < 1)
				inside_circle_stratified++;
		}
	}
	std::cout << "Regular	Estimate of Pi = " <<
		  4*float(inside_circle) / (sqrt_N*sqrt_N) << "\n";
	std::cout << "Stratified Estimate of Pi = " <<
		  4*float(inside_circle_stratified) / (sqrt_N*sqrt_N) << "\n";
}

//////////////////////////////////////////////////////////////////////////////////

/*
 * Monte Carlo Steps
 * 1) Have an integral of f(x) over some domain [a, b]
 *    -> f(x) is the target we want to estimate about
 * 2) Choose a pdf p(r) that is non zero over [a, b] (can be uniform/non-uniform)
 *    -> The more p follows f, the faster it converges
 * 3) Average by f(x) / p(r) where r is a random number (use pdf as weight)
 */

/*
 * Integration of x^2 from 0 to 2
 * I = area(x^2, 0, 2)
 * I = 2 * average(x^2, 0, 2)
 */
void monte_carlo() {
	int N = 1000000;
	float sum;
	for (int i = 0; i < N; i++) {
		// get random number between 0 and 2
		float x = 2*random_double();
		sum += x*x;
	}
	// Finally average the sum of sampling in [0, 2) * 2
	// multiple of 2 is coming from the range [0, 2) not [0, 1]
	std::cout << "I = " << 2*sum/N << "\n";
	// Returns I = 2.66587
}

/*
 * Non uniform sampling using pdf :)
 */
void nonuniform_sample() {
	int N = 1000000;
	float sum;
	for (int i = 0; i < N; i++) {
		float x = sqrt(4*random_double()); // <- P-1(x)
		// weight is 1/pdf(x)
		sum += x*x / pdf(x);
	}
	std::cout << "I = " << sum/N << "\n";
	// Returns I = 2.66726
}

void uniform_sample() {
	int N = 1000000;
	float sum;
	for (int i = 0; i < N; i++) {
		float x = 2*random_double(); // <- P-1(x)
		sum += x*x / pdf_2(x);
	}
	std::cout << "I = " << sum/N << "\n";
}

// perfect importance sampling is only possible when the answer is known
void nonuniform_sample_2() {
	int N = 1;
	float sum;
	for (int i = 0; i < N; i++) {
		float x = pow(8*random_double(), 1./3.); // <- P-1(x)
		sum += x*x / pdf_3(x);
	}
	std::cout << "I =" << sum/N << "\n";
}

void sphere_example() {
	int N = 1000000;
	float sum;
	for (int i = 0; i < N; i++) {
		vec3 d = random_on_unit_sphere_test();
		float cosine_squared = d.z()*d.z();
		sum += cosine_squared / pdf_4(d);
	}
	std::cout << "I =" << sum/N << "\n";
}

void random_direction_on_hemisphere() {
	int N = 1000000;
	float sum = 0.0;
	for (int i = 0; i < N; i++) {
		float r1 = random_double();
		float r2 = random_double();
		float x = cos(2*M_PI*r1)*2*sqrt(r2*(1-r2));
		float y = sin(2*M_PI*r1)*2*sqrt(r2*(1-r2));
		float z = 1 - r2;
		sum += z*z*z / (1.0/(2.0*M_PI));
	}
	std::cout << "PI/2 = " << M_PI/2 << "\n";
	std::cout << "Estimate = " << sum/N << "\n";
}

void random_direction_on_hemisphere_rand_in_cosine() {
	int N = 1000000;
	float sum = 0.0;
	for (int i = 0; i < N; i++) {
		vec3 v = random_cosine_direction();
		sum += v.z()*v.z()*v.z() / (v.z()/(M_PI));
	}
	std::cout << "PI/2 = " << M_PI/2 << "\n";
	std::cout << "Estimate = " << sum/N << "\n";
}

vec3 random_on_unit_sphere_test() {
	vec3 p;

	do {
		p = 2.0*vec3(random_double(), random_double(), random_double()) - vec3(1,1,1);
	} while (dot(p,p) >= 1.0);
	
	return unit_vector(p);
}





