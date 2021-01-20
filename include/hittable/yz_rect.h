#ifndef YZRECTH
#define YZRECTH

#include "hittable.h"

class yz_rect: public hittable {
	public:
		yz_rect() {}
		yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, material *mat)
			: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
		virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const {
			box =  aabb(vec3(k-0.0001, y0, z0), vec3(k+0.0001, y1, z1));
			return true;
		}
		material  *mp;
		float y0, y1, z0, z1, k;
};

bool yz_rect::hit(const ray& r, float t0, float t1, hit_record& rec) const {
	float t = (k-r.origin().x()) / r.direction().x();

	// Out of frustum
	if (t < t0 || t > t1) return false;

	float y = r.origin().y() + t*r.direction().y();
	float z = r.origin().z() + t*r.direction().z();

	// Out of frustum
	if (y < y0 || y > y1 || z < z0 || z > z1) return false;

	rec.u = (y-y0)/(y1-y0);
	rec.v = (z-z0)/(z1-z0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_parameter(t);
	rec.normal = vec3(1, 0, 0);

	return true;
}

#endif
