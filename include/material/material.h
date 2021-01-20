#ifndef MATERIALH
#define MATERIALH

#include "../random.h"
//#include "../vec3.h"
#include "../pdf/cosine_pdf.h"

struct scatter_record {
	ray specular_ray;
	bool is_specular;
	vec3 attenuation;
	pdf *pdf_ptr;
};

class material {
	public:
		virtual bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const {
			return false;
		}

		virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
			return 0;
		}

		virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const {
			return vec3(0,0,0);
		}
};

vec3 random_in_unit_sphere() {
	vec3 p;

	// Keep getting a random point on a unit sphere until we get one
	do {
		// map from [0, 1) to [-1, 1) where -vec3(1,1,1) is offset
		p = 2.0*vec3(random_double(), random_double(), random_double()) - vec3(1,1,1);
	} while (p.squared_length() >= 1.0);

	return p;
}

// Get a point on the unit sphere (not in but on)
vec3 random_on_unit_sphere() {
	vec3 p;

	do {
		p = 2.0*vec3(random_double(), random_double(), random_double()) - vec3(1,1,1);
	} while (dot(p,p) >= 1.0);
	
	return unit_vector(p);
}

vec3 reflect(const vec3& v, const vec3& n) {
	// R+L = 2*dot(L, N)*N
	// R = -L + 2*dot(L, N)*N where V = -L
	// R = V - 2*dot(V, N)*N
	return v - 2*dot(v,n)*n;
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	// x component of unit(v):
	// 		dot(unit(n), N)^2 + x^2 = 1^2
	//		x^2 = 1 - dot(unit(n), N)^2
	//		x = sqrt( 1 - dot(unit(n), N)^2 )
	//		=> sin(theta) = sqrt( 1 - dot(unit(n), N)^2 )
	//
	// Snell's law: ni*sin(theta) = nt*sin(theta')
	//		sin(theta') = ni/nt * sqrt( 1 - dot(unit(n), N)^2 )
	//		x' = ni/nt * sqrt( 1 - dot(unit(n),N)^2 )
	//
	// y' component of refracted unit vector
	//		x'^2 + y'^2 = 1
	//		y'^2 = 1 - x'^2
	//		y' = sqrt( 1 - x'^2 )
	//		=> discriminant is inside of this sqrt: 1 - x'^2
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
	if (discriminant > 0) {
		// isn't it below?
		// refracted = ni_over_nt*sqrt(1-dt*dt) - n*sqrt(discriminant);
		refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
		return true;
	}
	else return false;
}

// Approximation of reflection-refraction by Christophe Schlick
float schlick(float cosine, float ref_idx) {
	float r0 = (1-ref_idx) / (1+ref_idx);
	r0 = r0*r0;
	return r0 + (1-r0)*pow((1 - cosine),5);
}

#endif
