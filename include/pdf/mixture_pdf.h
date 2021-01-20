#ifndef MIXTUREPDFH
#define MIXTUREPDFH

#include "pdf.h"

class mixture_pdf : public pdf {
	public:
		mixture_pdf(pdf *p0, pdf *p1) { p[0] = p0; p[1] = p1; }

		// Mixture of cosine and light sampling (50/50)
		virtual float value(const vec3& direction) const {
			return 0.5 * p[0]->value(direction) + 0.5 *p[1]->value(direction);
		}
		virtual vec3 generate() const {
			if (random_double() < 0.5) return p[0]->generate();
			else return p[1]->generate();
		}

		pdf *p[2];
};

#endif
