/* C++ standard libraries */
#include <iostream> // cout
#include <fstream>  // file i/o

/* Other headers */
// Include a header once once within a project!
#include "float.h"

#include "../include/hittable/hittable_list.h"
#include "../include/hittable/sphere.h"
#include "../include/hittable/moving_sphere.h"
#include "../include/hittable/xy_rect.h"
#include "../include/hittable/yz_rect.h"
#include "../include/hittable/xz_rect.h"
#include "../include/hittable/flip_normals.h"
#include "../include/hittable/box.h"
#include "../include/hittable/translate.h"
#include "../include/hittable/rotate_y.h"
#include "../include/hittable/constant_medium.h"
#include "../include/hittable/bvh_node.h"

#include "../include/material/diffuse_light.h"
#include "../include/material/dielectric.h"
#include "../include/material/isotropic.h"
#include "../include/material/lambertian.h"
#include "../include/material/metal.h"

#include "../include/camera.h"
#include "../include/random.h"

#include "../include/texture/constant_texture.h"
#include "../include/texture/checker_texture.h"
#include "../include/texture/noise_texture.h"
#include "../include/texture/image_texture.h"

//#include "../include/pdf/cosine_pdf.h"
#include "../include/pdf/hittable_pdf.h"
#include "../include/pdf/mixture_pdf.h"

#include "../include/hammersley.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb/stb_image.h"


using namespace std;

inline vec3 de_nan(const vec3& c) {
	vec3 temp = c;
	if (!(temp[0] == temp[0])) temp[0] = 0;
	if (!(temp[1] == temp[1])) temp[1] = 0;
	if (!(temp[2] == temp[2])) temp[2] = 0;
	return temp;
}


/* Global variables */
bool texture_map;
bool use_ambient;
enum scene {
	random_s,
	moving_spheres_zoomin_s,
	two_spheres_s,
	two_perlin_spheres_s,
	image_texture_s,
	simple_light_s,
	cornell_box_s,
	cornell_smoke_s,
	final_s
};


/* Function prototypes */
hittable *get_world(scene s);
camera set_camera(scene s, int nx, int ny);
vec3 color(const ray& r, hittable *world, hittable *light_shape, int depth);

hittable *random_scene();
hittable *moving_spheres_zoomin();
hittable *two_spheres();
hittable *two_perlin_spheres();
hittable *image_textured_spheres();
hittable *simple_light();
hittable *cornell_box();
hittable *cornell_smoke();
hittable *final();

//void cornell_box(hittable **scene, camera **cam, float aspect);


/*
 * main
 *
 * Under src directory...
 * Compile: g++ -std=c++11 main.cpp vec3.cpp -o main
 * Run:	    ./main
 *
*/
int main(int argc, char * argv[]) {
	cout << "Rendering begins..." << endl;

	//if ( argc == 1 ) {
	//	cout << "Expected: ./main obj_file_name" << endl;
	//	return 0;
	//}

	// Choose from { random_s, moving_spheres_zoomin_s, two_spheres_s, two_perlin_spheres_s, image_texture_s, simple_light_s, cornell_box_s, cornell_smoke_s, final_s }
	scene s = cornell_box_s;

	if (s == image_texture_s || s == final_s) texture_map = true;
	else texture_map = false;

	if (s == simple_light_s || s == cornell_box_s || s == cornell_smoke_s || s == final_s) use_ambient = false;
	else use_ambient = true;

	int nx, ny;
	if (s == cornell_smoke_s || s == final_s) {
		nx = 300;
		ny = 300;
	} else if (s == cornell_box_s) {
		nx = 500;
		ny = 500;
	} else {
		nx = 500; //200;
		ny = 300; //100;
	}

	int ns = 1000;

	ofstream outfile;
	outfile.open ("../rendered_img/output.ppm");
	outfile << "P3\n" << nx << " " << ny << "\n255\n";

	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0); // step interval
	vec3 vertical(0.0, 2.0, 0.0);   // step interval
	vec3 origin(0.0, 0.0, 0.0);

	hittable *world = get_world(s);
	camera cam = set_camera(s, nx, ny);

	hittable *light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
	hittable *glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
	hittable *a[2];
	a[0] = light_shape;
	a[1] = glass_sphere;
	hittable_list hlist(a,2);

	hammersley * hm = new hammersley();
	double *hammersley_point;

	// Send a ray out of eye (0, 0, 0) from BL to UR corner
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			// Super sampling
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++) {
				//hammersley_point = hm->get_hammersley(s+1, 2, ns);
				//float u = float(i + hammersley_point[0]) / float(nx);
				//float v = float(j + hammersley_point[1]) / float(ny);
				
				// Offset by random_double value [0, 1)
				float u = float(i + random_double()) / float(nx);
				float v = float(j + random_double()) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				
				col += de_nan(color(r, world, light_shape, 0));
				//col += de_nan(color(r, world, &hlist, 0));
			}
			col /= float(ns); // average sum
			// gamma correction (brighter color)
			//col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
			col = vec3( 1.5 * sqrt(col[0]), 1.5 * sqrt(col[1]), 1.5 * sqrt(col[2]) );

			// Clamp color to [0, 1]
			for (int channel = 0; channel < 3; channel++)
				if (col[channel] > 1.0) col[channel] = 1.0;

			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}

	outfile.close();

	cout << "Path Tracer Completed!" << endl;
	return 0;
}

vec3 color(const ray& r, hittable *world, hittable *light_shape, int depth) {
	hit_record hrec;

	if (world->hit(r, 0.001, MAXFLOAT, hrec)) {
		scatter_record srec;
		vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
		
		if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
			if (srec.is_specular) {
				// On specular surface, color is only collected from the reflected direction
				return srec.attenuation * color(srec.specular_ray, world, light_shape, depth+1);
			} else {
				hittable_pdf plight(light_shape, hrec.p);
				mixture_pdf p(&plight, srec.pdf_ptr);
				ray scattered = ray(hrec.p, p.generate(), r.time());
				float pdf_val = p.value(scattered.direction());
				return emitted
					+ srec.attenuation * hrec.mat_ptr->scattering_pdf(r, hrec, scattered)
										* color(scattered, world, light_shape, depth+1)
										/ pdf_val;
			}
		}
		else return emitted;	
	} else {
		if (use_ambient) {
			// Sky color is a linear interpolation b/w while & blue
			vec3 unit_direction = unit_vector(r.direction());

			// t is a mapped y from [-1, 1] to [0, 1]
			// more accurately, t = 0.5 * (sqrt(2)*unit_direction.y() + 1.0)
			float t = 0.5 * (unit_direction.y() + 1.0);

			return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
		} else return vec3(0,0,0);
	}
}

hittable *get_world(scene s) {
	switch(s)
	{
		case random_s:
			return random_scene();
		case moving_spheres_zoomin_s:
			return moving_spheres_zoomin();
		case two_spheres_s:
			return two_spheres();
		case two_perlin_spheres_s:
			return two_perlin_spheres();
		case image_texture_s:
			return image_textured_spheres();
		case simple_light_s:
			return simple_light();
		case cornell_box_s:
			return cornell_box();
		case cornell_smoke_s:
			return cornell_smoke();
		case final_s:
			return final();
		default:
			return two_spheres();
	}
}

camera set_camera(scene s, int nx, int ny) {
	vec3 lookfrom, lookat;
	float dist_to_focus, aperture, vfov;

	switch(s)
	{
		case random_s:
		case two_spheres_s:
		case two_perlin_spheres_s:
		case image_texture_s:
		case simple_light_s:
			lookfrom = vec3(13,2,3);
			lookat = vec3(0,0,0);
			dist_to_focus = 10.0;
			aperture = 0.0;
			vfov = 20.0;
			return camera(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
		case moving_spheres_zoomin_s:
			lookfrom = vec3(3,3,2);
			lookat = vec3(0,0,-1);
			dist_to_focus = (lookfrom-lookat).length();
			aperture = 0.0;
			vfov = 20.0;
			return camera(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
		case cornell_box_s:
		case cornell_smoke_s:
		case final_s:
			lookfrom = vec3(278, 278, -800);
			lookat = vec3(278,278,0);
			dist_to_focus = 10.0;
			aperture = 0.0;
			vfov = 40.0;
			return camera(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
		default:
			lookfrom = vec3(13,2,3);
			lookat = vec3(0,0,0);
			dist_to_focus = 10.0;
			aperture = 0.0;
			vfov = 20.0;
			return camera(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
	}
}

hittable *random_scene() {
	int n = 8; // Only use mulptiole of 4
	int arr_size = pow(4, n/4)+4;
	hittable **list = new hittable*[arr_size];

	// Checker ground
	texture *checker = new checker_texture(
    						new constant_texture(vec3(0.2, 0.3, 0.1)),
    						new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(checker));

	// Plane ground
	//list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
	
	int i = 1;
	for (int a = -n/4; a < n/4; a++) {
		for (int b = -n/4; b < n/4; b++) {
			float choose_mat = random_double();
			vec3 center(a+2.5*random_double(),0.2,b+2.5*random_double());

			if ((center-vec3(4,0.2,0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					list[i++] = new moving_sphere(
						center,
						center+vec3(0, 0.5*random_double(), 0),
						0.0, 1.0, 0.2,
						new lambertian(new constant_texture(
							vec3(random_double()*random_double(),
								random_double()*random_double(),
								random_double()*random_double())
						))
					);
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(center, 0.2,
							new metal(vec3(0.5*(1 + random_double()),
										   0.5*(1 + random_double()),
										   0.5*(1 + random_double())),
									  0.5*random_double()));
				}
				else {  // glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hittable_list(list,i);
}

hittable *moving_spheres_zoomin() {
	hittable **list = new hittable*[4];
	list[0] = new moving_sphere(vec3(0,0,-1), vec3(0,0,-1)+vec3(0, 0.5*random_double(), 0), 0.0, 1.0, 0.2,
									new lambertian(new constant_texture(
											vec3(random_double()*random_double(),
											random_double()*random_double(),
											random_double()*random_double())
										))
					);
	list[1] = new moving_sphere(vec3(1,0,-1), vec3(1,0,-1)+vec3(0, 0.5*random_double(), 0), 0.0, 1.0, 0.2,
									new lambertian(new constant_texture(
											vec3(random_double()*random_double(),
											random_double()*random_double(),
											random_double()*random_double())
										))
					);
	list[2] = new moving_sphere(vec3(-1,0,-1), vec3(-1,0,-1)+vec3(0, 0.5*random_double(), 0), 0.0, 1.0, 0.2,
									new lambertian(new constant_texture(
											vec3(random_double()*random_double(),
											random_double()*random_double(),
											random_double()*random_double())
										))
					);
	list[3] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
	return new hittable_list(list, 4);
}

hittable *two_spheres() {
	texture *checker = new checker_texture(
		new constant_texture(vec3(0.2, 0.3, 0.1)),
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);

	hittable **list = new hittable*[2];
	list[0] = new sphere(vec3(0,-10, 0), 10, new lambertian(checker));
	list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));

	return new hittable_list(list, 2);
}

hittable *two_perlin_spheres() {
	texture *pertext = new noise_texture(4);
	hittable **list = new hittable*[2];
	list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	return new hittable_list(list, 2);
}

hittable *image_textured_spheres() {
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("../texture_img/earthmap.jpg", &nx, &ny, &nn, 0);
	material *mat = new lambertian(new image_texture(tex_data, nx, ny), texture_map);

	texture *pertext = new noise_texture(4);
	hittable **list = new hittable*[2];
	list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, mat);
	return new hittable_list(list, 2);
}

hittable *simple_light() {
	texture *pertext = new noise_texture(4);
	hittable **list = new hittable*[4];
	list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4,4,4))));
    list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4,4,4))));
	return new hittable_list(list,4);
}

hittable *cornell_box() {
	hittable **list = new hittable*[11];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *yellow = new lambertian(new constant_texture(vec3(1.0, 0.96, 0.31)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

	material *turbtext = new lambertian(new noise_texture(0.1));
	material *pertext = new lambertian(new noise_texture_perlin(0.1));

	// Warm light
	//material *light = new diffuse_light(new constant_texture(vec3(16.86 + 5.0, 8.76+2.0 + 5.0, 3.2+0.5 + 5.0)));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	//list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));


	////////////// Last shot /////////////////
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("../texture_img/thankyou.jpg", &nx, &ny, &nn, 0);
	material *img_mat = new lambertian(new image_texture(tex_data, nx, ny), texture_map);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, img_mat));

	tex_data = stbi_load("../texture_img/bunny.jpg", &nx, &ny, &nn, 0);
	img_mat = new lambertian(new image_texture(tex_data, nx, ny), texture_map);
	list[i++] = new translate(
					new rotate_y(new box(vec3(0,0,0), vec3(120,120,120), img_mat), -18),
					vec3(130,0,200));


	// Two boxes in the room
	/*
	// Small box
	list[i++] = new translate(
					new rotate_y(new box(vec3(0,0,0), vec3(165,165,165), white), -18),
					vec3(130,0,65));
	// Tall box
	list[i++] = new translate(
					new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15),
					vec3(265,0,295));
	*/


	////////////// All textures /////////////////
	/*
	material *turbtext = new lambertian(new noise_texture(0.1));
	material *pertext = new lambertian(new noise_texture_perlin(0.1));

	// Checker tall box
	texture *checker = new checker_texture(
		new constant_texture(vec3(0.2, 0.3, 0.1)),
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
	list[i++] = new translate(
					new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), new lambertian(checker)),  15),
					vec3(265,0,295));

	// Image textured medium box
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("../texture_img/trojans_flip.png", &nx, &ny, &nn, 0);
	material *img_mat = new lambertian(new image_texture(tex_data, nx, ny), texture_map);
	list[i++] = new translate(
					new rotate_y(new box(vec3(0,0,0), vec3(165,165,165), img_mat), -18),
					vec3(130,0,65));

	// Noises on sphere
	list[i++] = new sphere(vec3(380, 0, 180), 100, pertext);
	//list[i++] = new rotate_y(new sphere(vec3(280, 40, 150), 40, turbtext), 15);

	// Turbulence floor
	list[i++] = new xz_rect(0, 555, 0, 555, 0, turbtext);
	*/

	// Solid small box
	/*
	list[i++] = new translate(
					new rotate_y(new box(vec3(0,0,0), vec3(70,70,70), yellow), 5),
					vec3(200,165,120));
	*/

	/*
	////////////// glass sphere //////////////
	material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190),90 , glass);
    */


    ////////////// Metal box tall //////////////
    /*
	material *aluminum = new metal(vec3(0.8, 0.85, 0.88), 0.0);
	list[i++] = new translate(
					new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), aluminum),  15),
					vec3(265,0,295));
	*/



	////////// Last image //////////
	/*
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("../texture_img/thankyou.png", &nx, &ny, &nn, 0);
	material *img_mat = new lambertian(new image_texture(tex_data, nx, ny), texture_map);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, img_mat));

	tex_data = stbi_load("../texture_img/tommy.jpg", &nx, &ny, &nn, 0);
	img_mat = new lambertian(new image_texture(tex_data, nx, ny), texture_map);
	list[i++] = new translate(
					new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), img_mat),  15),
					vec3(265,0,295));
	*/

	return new hittable_list(list,i);
}

hittable *cornell_smoke() {
	hittable **list = new hittable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	hittable *b1 = new translate(
						new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
						vec3(130,0,65));
	hittable *b2 = new translate(
						new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15),
						vec3(265,0,295));

	list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
    list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

	return new hittable_list(list,i);
}

hittable *final() {
	int nb = 20;
	hittable **list = new hittable*[30];
	hittable **boxlist = new hittable*[10000];
	hittable **boxlist2 = new hittable*[10000];
	material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian( new constant_texture(vec3(0.48, 0.83, 0.53)));

	int b = 0;
	for (int i = 0; i < nb; i++) {
		for (int j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i*w;
			float z0 = -1000 + j*w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100*(random_double()+0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground);
		}
	}
	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)));
	list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
	vec3 center(400, 400, 200);
	list[l++] = new moving_sphere(center, center+vec3(30, 0, 0),
								0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));
	hittable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("../texture_img/earthmap.jpg", &nx, &ny, &nn, 0);
	material *emat =  new lambertian(new image_texture(tex_data, nx, ny), texture_map);
	list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
	texture *pertext = new noise_texture(0.1);
	list[l++] =  new sphere(vec3(220, 280, 300), 80, new lambertian( pertext ));

	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxlist2[j] = new sphere(vec3(165*random_double(), 165*random_double(), 165*random_double()), 10, white);
	}
	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100,270,395));

	return new hittable_list(list,l);
}

/*
void cornell_box(hittable **scene, camera **cam, float aspect) {
	int i = 0;
	hittable **list = new hittable*[8];

	material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
	material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
	material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
	material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	list[i++] = new translate(new rotate_y(
		new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130,0,65));
	list[i++] = new translate(new rotate_y(
		new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));

	*scene = new hittable_list(list, i);
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0,1,0),
					vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}
*/




