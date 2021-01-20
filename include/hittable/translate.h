#ifndef TRANSLATEH
#define TRANSLATEH

#include "hittable.h"

class translate : public hittable {
	public:
		translate(hittable *p, const vec3& displacement) : ptr(p), offset(displacement) {}

		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;

		hittable *ptr;
		vec3 offset;
};

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	// Move ray in opposite direction instead of moving the object
	ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (ptr->hit(moved_r, t_min, t_max, rec)) {
		// Also offset the hit point
		rec.p += offset;
		return true;
	}
	else return false;
}

bool translate::bounding_box(float t0, float t1, aabb& box) const {
	if (ptr->bounding_box(t0, t1, box)) {
		box = aabb(box.min() + offset, box.max() + offset);
		return true;
	}
	else return false;
}

#endif
