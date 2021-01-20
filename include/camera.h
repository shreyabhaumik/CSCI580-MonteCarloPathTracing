#ifndef CAMERAH
#define CAMERAH

#include "ray.h"
#include "random.h"

vec3 random_in_unit_disk();

class camera {
	public:
		// Still stick to z=-1 plane
		camera(vec3 lookfrom, vec3 lookat, vec3 vup,
				float vfov, // vfov is top to bottom in degrees
				float aspect,
				float aperture, float focus_dist,
				float t0, float t1) {

			time0 = t0;
			time1 = t1;

			lens_radius = aperture / 2;
			float theta = vfov * M_PI / 180;
			float half_height = tan(theta / 2);
			float half_width = aspect * half_height;

			origin = lookfrom;
			w = unit_vector(lookfrom - lookat); // depth
            u = unit_vector(cross(vup, w)); // x direction
            v = cross(w, u); // y direction (upvector)

            lower_left_corner = origin
								- half_width * focus_dist * u
								- half_height * focus_dist * v
								- focus_dist * w;
			horizontal = 2 * half_width * focus_dist * u;
			vertical   = 2 * half_height * focus_dist * v;
		}

		ray get_ray(float s, float t) {
			// randomly get a ray starting point within a camera lens
			vec3 rd = lens_radius*random_in_unit_disk();
			vec3 offset = u * rd.x() + v * rd.y();
			// Get a ray at random time between time0 and time1 while opening shutter
			float time = time0 + random_double()*(time1-time0);

			// direction vector is
			// dir = (center->(s,t)) - (center->offset)
			return ray(origin + offset,
					   lower_left_corner + s*horizontal + t*vertical - origin - offset,
					   time);
		}

		vec3 origin;
		vec3 lower_left_corner;
		vec3 horizontal;
		vec3 vertical;
		vec3 u, v, w;
		float time0, time1;
		float lens_radius;
};

vec3 random_in_unit_disk() {
	vec3 p;

	do {
		// Randomly get (x,y,0) within [-1,1) until get a point inside of unit disk
		p = 2.0*vec3(random_double(),random_double(),0) - vec3(1,1,0);
	} while (dot(p,p) >= 1.0);

	return p;
}

#endif