#ifndef XZRECTH
#define XZRECTH

#include "hittable.h"

class xz_rect: public hittable {
	public:
		xz_rect() {}
		xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, material *mat)
			: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

		virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const {
			box =  aabb(vec3(x0,k-0.0001,z0), vec3(x1, k+0.0001, z1));
			return true;
		}
		virtual float  pdf_value(const vec3& o, const vec3& v) const;
		virtual vec3 random(const vec3& o) const;

		material *mp;
		float x0, x1, z0, z1, k;
};

bool xz_rect::hit(const ray& r, float t0, float t1, hit_record& rec) const {
	float t = (k-r.origin().y()) / r.direction().y();

	// Out of frustum
	if (t < t0 || t > t1) return false;

	float x = r.origin().x() + t*r.direction().x();
	float z = r.origin().z() + t*r.direction().z();

	// Out of frustum
	if (x < x0 || x > x1 || z < z0 || z > z1) return false;

	rec.u = (x-x0)/(x1-x0);
	rec.v = (z-z0)/(z1-z0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_parameter(t);
	rec.normal = vec3(0, 1, 0);

	return true;
}

float xz_rect::pdf_value(const vec3& o, const vec3& v) const {
	hit_record rec;
	if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
		float area = (x1-x0)*(z1-z0);
		float distance_squared = rec.t * rec.t * v.squared_length();
		float cosine = fabs(dot(v, rec.normal) / v.length());
		return  distance_squared / (cosine * area);
	}
	else return 0;
}

// Returns a direction from origin to random point in light
vec3 xz_rect::random(const vec3& o) const {
	vec3 random_point = vec3(x0 + random_double()*(x1-x0), k, z0 + random_double()*(z1-z0));
	return random_point - o;
}

#endif
