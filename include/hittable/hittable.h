#ifndef HITTABLEH
#define HITTABLEH

#include "../ray.h"
#include "../aabb.h"
#include "../random.h"
#include "../onb.h"

class material;

struct hit_record {
	float t;
	vec3 p;
	vec3 normal;
	material *mat_ptr;
	float u, v; // image texture map
};

class hittable {
	public:
		// virtual function with "= 0" is pure abstruct function
		// this has to be implemented and cannot be instanciated
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
		virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
		virtual float pdf_value(const vec3& o, const vec3& v) const { return 0.0; } // dummy
		virtual vec3 random(const vec3& o) const { return vec3(1, 0, 0); }          // dummy
};

#endif
