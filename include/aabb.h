#ifndef AABBH
#define AABBH

inline float ffmin(float a, float b) { return a < b ? a : b; } // if (a < b) is true, return a. Otherwise return b
inline float ffmax(float a, float b) { return a > b ? a : b; }

// Axis-align vounding box
class aabb {
	public:
		aabb() {}
		aabb(const vec3& a, const vec3& b) { _min = a; _max = b; }

		vec3 min() const { return _min; }
		vec3 max() const { return _max; }
		bool hit(const ray& r, float tmin, float tmax) const;

		// bounding box range
		vec3 _min;
		vec3 _max;
};

inline bool aabb::hit(const ray& r, float tmin, float tmax) const {
	for (int a = 0; a < 3; a++) {
		float invD = 1.0f / r.direction()[a];
		float t0 = (min()[a] - r.origin()[a]) * invD; // tx0 = (x0-Ax)/B where x0 is smaller attribute of BB in x-axis (_min)
		float t1 = (max()[a] - r.origin()[a]) * invD; // tx1 = (x1-Ax)/B where x1 = _max
		
		if (invD < 0.0f) std::swap(t0, t1); // when direction is negative
		
		// make sure the t0 and t1 are in range (tmin, tmax) for the actual intersect
		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;

		if (tmax <= tmin) return false;
	}
	return true;
}

// Compound BBs
aabb surrounding_box(aabb box0, aabb box1) {
	// Get the smallest attribute
	vec3 small(
				ffmin(box0.min().x(), box1.min().x()),
				ffmin(box0.min().y(), box1.min().y()),
				ffmin(box0.min().z(), box1.min().z())
			  );
	// Get the biggest attribute
	vec3 big(
				ffmax(box0.max().x(), box1.max().x()),
				ffmax(box0.max().y(), box1.max().y()),
				ffmax(box0.max().z(), box1.max().z())
			);
	return aabb(small, big);
}

#endif
