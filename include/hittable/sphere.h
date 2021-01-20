#ifndef SPHEREH
#define SPHEREH

#include "hittable.h"

void get_sphere_uv(const vec3& p, float& u, float& v);

inline vec3 random_to_sphere(float radius, float distance_squared) {
	float r1 = random_double();
	float r2 = random_double();
	float z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);
	float phi = 2*M_PI*r1;
	float x = cos(phi)*sqrt(1-z*z);
	float y = sin(phi)*sqrt(1-z*z);
	return vec3(x, y, z);
}

class sphere: public hittable {
	public:
		sphere() {}
		sphere(vec3 cen, float r, material *m)
			: center(cen), radius(r), mat_ptr(m) {};

		// Keep same prototype as the actual virtual function
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;
		virtual float pdf_value(const vec3& o, const vec3& v) const;
		virtual vec3 random(const vec3& o) const;

		vec3 center;
		float radius;
		material *mat_ptr;
};

// Using namespace to point to the "hit" function in the namespace "sphere"
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 co = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(co, r.direction()); // 2b
	float c = dot(co, co) - radius*radius;
	float discriminant = b*b - a*c; // b was even, so canceled them

	// When there are 2 intersections (not touch but intersect)
	if (discriminant > 0) {
		// Check first intersection (negative solution is closer to camera than positive one)
		float temp = (-b - sqrt(discriminant)) / a;
		if (t_min < temp && temp < t_max) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius; // normalized
			rec.mat_ptr = mat_ptr;
			get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
			return true;
		}

		// Check second intersection
		temp = (-b + sqrt(discriminant)) / a;
		if (t_min < temp && temp < t_max) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius; // normalized
			rec.mat_ptr = mat_ptr;
			get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
			return true;
		}
	}

	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const {
	box = aabb(
			center - vec3(radius, radius, radius),
			center + vec3(radius, radius, radius)
		  );
	return true;
}

void get_sphere_uv(const vec3& p, float& u, float& v) {
	float phi = atan2(p.z(), p.x());
	float theta = asin(p.y());
	u = 1-(phi + M_PI) / (2*M_PI);
	v = (theta + M_PI/2) / M_PI;
}

float sphere::pdf_value(const vec3& o, const vec3& v) const {
	hit_record rec;
	if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
		float cos_theta_max = sqrt(1 - radius*radius/(center-o).squared_length());
		float solid_angle = 2*M_PI*(1-cos_theta_max);
		return 1 / solid_angle;
	}
	else return 0;
}

vec3 sphere::random(const vec3& o) const {
	vec3 direction = center - o;
	float distance_squared = direction.squared_length();
	onb uvw;
	uvw.build_from_w(direction);
	return uvw.local(random_to_sphere(radius, distance_squared));
}

#endif
