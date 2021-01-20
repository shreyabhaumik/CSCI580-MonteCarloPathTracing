#ifndef BVHNODEH
#define BVHNODEH

#include "hittable.h"

int box_x_compare (const void * a, const void * b);
int box_y_compare (const void * a, const void * b);
int box_z_compare (const void * a, const void * b);

// Bounding Volume Hierarchy
class bvh_node : public hittable {
	public:
		bvh_node() {}
		// Construction is more like kd-tree
		bvh_node(hittable **l, int n, float time0, float time1);

		virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;

		hittable *left;
		hittable *right;
		aabb box;
};

// Defin constructor here
bvh_node::bvh_node(hittable **l, int n, float time0, float time1) {
	// Randomly pick an axis to sort about or divide in
	int axis = int(3*random_double());

	if (axis == 0) qsort(l, n, sizeof(hittable *), box_x_compare);      // x-axis
	else if (axis == 1) qsort(l, n, sizeof(hittable *), box_y_compare); // y-axis
	else qsort(l, n, sizeof(hittable *), box_z_compare);                // z-axis

	if (n == 1) {
		left = right = l[0];
	}
	else if (n == 2) {
		left = l[0];
		right = l[1];
	}
	else { // where there are more than 3 hittable objects in the list
		left = new bvh_node(l, n/2, time0, time1); // left node is the fitst half after sorting the node
		right = new bvh_node(l + n/2, n - n/2, time0, time1); // left node is the latter half after sorting the node
	}

	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) ||
		!right->bounding_box(time0, time1, box_right)) {
		std::cerr << "no bounding box in bvh_node constructor\n";
	}

	box = surrounding_box(box_left, box_right);
}

bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	if (box.hit(r, t_min, t_max)) {
		hit_record left_rec, right_rec;
		bool hit_left = left->hit(r, t_min, t_max, left_rec);
		bool hit_right = right->hit(r, t_min, t_max, right_rec);

		if (hit_left && hit_right) {
			// Only consider the first hit
			if (left_rec.t < right_rec.t) rec = left_rec;
			else rec = right_rec;
			return true;
		}
		else if (hit_left) {
			rec = left_rec;
			return true;
		}
		else if (hit_right) {
			rec = right_rec;
			return true;
		}
		else return false;
	}
	else return false;
}

bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
	// bouding box is the box for the node itself
	b = box;
	return true;
}

// Takes void pointers which you cast
int box_x_compare (const void * p1, const void * p2) {
	hittable *p1h = *(hittable**)p1;
	hittable *p2h = *(hittable**)p2;

	aabb box_left, box_right;
	if (!p1h->bounding_box(0,0, box_left) || !p2h->bounding_box(0,0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	// Meaning of return values for qsort
	// <0 : The element pointed by p1 goes before the element pointed by p2
	// 0  : The element pointed by p1 is equivalent to the element pointed by p2
	// >0 : The element pointed by p1 goes after the element pointed by p2
	if (box_left.min().x() - box_right.min().x() < 0.0) return -1;
	else return 1;
}

int box_y_compare (const void * p1, const void * p2) {
	hittable *p1h = *(hittable**)p1;
	hittable *p2h = *(hittable**)p2;

	aabb box_left, box_right;
	if (!p1h->bounding_box(0,0, box_left) || !p2h->bounding_box(0,0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	if (box_left.min().y() - box_right.min().y() < 0.0) return -1;
	else return 1;
}

int box_z_compare (const void * p1, const void * p2) {
	hittable *p1h = *(hittable**)p1;
	hittable *p2h = *(hittable**)p2;

	aabb box_left, box_right;
	if (!p1h->bounding_box(0,0, box_left) || !p2h->bounding_box(0,0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	if (box_left.min().z() - box_right.min().z() < 0.0) return -1;
	else return 1;
}

#endif
