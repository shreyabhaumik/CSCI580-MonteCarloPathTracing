#ifndef METALH
#define METALH

#include "material.h"

class metal : public material {
	public:
		metal(const vec3& a, float f) : albedo(a) {
			if (f < 1) fuzz = f; else fuzz = 1;
		}

		virtual bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const {
			vec3 reflected = reflect(unit_vector(r_in.direction()), hrec.normal);
			srec.specular_ray = ray(hrec.p, reflected+fuzz*random_in_unit_sphere());
			srec.attenuation = albedo;
			srec.is_specular = true;
			srec.pdf_ptr = 0; // On metal surface, the light only comes from reflected direction. So don't use cosine pdf
			return true;
		}

		vec3 albedo;
		float fuzz;
};

#endif
