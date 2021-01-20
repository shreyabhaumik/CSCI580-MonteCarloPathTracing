#ifndef CHECKERTEXTUREH
#define CHECKERTEXTUREH

#include "texture.h"

class checker_texture : public texture {
	public:
		checker_texture() {}
		checker_texture(texture *t0, texture *t1): even(t0), odd(t1) {}

		virtual vec3 value(float u, float v, const vec3& p) const {
			// Value of checker is independent from u, v
			// Only depends on the intersection position
			//float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
			float sines = sin(0.1*p.x())*sin(0.1*p.y())*sin(0.05*p.z());
			if (sines < 0) return odd->value(u, v, p);
			else return even->value(u, v, p);
		}

		texture *odd;
		texture *even;
};

#endif
