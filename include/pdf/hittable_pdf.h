#ifndef HITTABLEPDFH
#define HITTABLEPDFH

#include "pdf.h"

// Sample directly from lights
// the hittable obj is the light itself
class hittable_pdf : public pdf {
	public:
		hittable_pdf(hittable *p, const vec3& origin) : ptr(p), o(origin) {}

		virtual float value(const vec3& direction) const {
			return ptr->pdf_value(o, direction);
		}

		virtual vec3 generate() const {
			return ptr->random(o);
		}

		vec3 o;
		hittable *ptr;
};

#endif
