#ifndef LAMBERTIANH
#define LAMBERTIANH

#include "material.h"
#include "../onb.h"

class lambertian : public material {
	public:
		lambertian(texture *a, bool texture_map=false) : albedo(a), image_texture(texture_map) {}

		virtual bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const {
			srec.is_specular = false;
			srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
			srec.pdf_ptr = new cosine_pdf(hrec.normal);
			return true;
		}

		float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
			float cosine = dot(rec.normal, unit_vector(scattered.direction()));
			if (cosine < 0) return 0;
			return cosine / M_PI;
		}

		bool image_texture;
		texture *albedo;
};

#endif
