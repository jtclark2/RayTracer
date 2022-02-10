/******************************************************************************
Trevor's thoughts:
- viewport: This is effectively the eye/wall we project the image on. You can
kind of think of it as the image, except that it has a position of project,
rather than being associated with a pixel. main loops through pixel values to
find the viewport value at each pixel index.


	Scene:
	-------------------------------------------------------
		sphere							|
			sphere						|
		sphere				*			|(origin)
										|
				sphere					|

	-------------------------------------------------------

	My ascii art isn't great, so use your imagination:
		- The Sphere's are populating the scene. Rays will interact with them.
		- The focal point is the '*'. This is located at the origin, which all rays pass through.
		- The wall of `|` represent the image & camera. These are two representations of the same 2D window into the scene.
		  The main difference is that camera is a 2D window living in the 3D vector space of the scene. The image is the
		  a necessary abstraction for us to view that scene in pixels. It lives exclusively in a 2D pixel space and it's
		  only connection to the 3D scene is via the camera.
			- The wall is located `focal_length` from the focal point.
				- Currently, this is purely along the Z axis, we could translate the camera pretty easily by changing the definition of 
				ray, which is based on lower_left_corner. To change the orientation would be a little more involved, but still doable. 
				 *Maybe I'll work that out if I have time at the end*
			- Camera lives in vector space with dimensions of viewport_height, viewport_width (width is not shown in my 2D ascii art)
			  We're using coordinates in which increase from left -> right / bottom -> top.
			  Therefore, lower_left_corner is at (-viewport_width/2, -viewport_height/2, -focal_length)
			- Image is then found by looping through discrete locations on the camera, which represent the pixels. At each location,
			we create a ray, and send it out towards the focal point, checking for collisions along the way. If we hit a sphere, then
			we return the appropriate color.
				- Don't light rays travel to a camera, not out of it? In real life, yes. However, that would create so many rays, and
				we're only interested in the ones that hit the camera. So yes, we are doing it backwards, but and it's running the physics
				in reverse. Rather than asking "Which photons hit the sensor here?". We are asking "Given that this photon hit the sensor,
				what did it's journey look like?"
				- I'll add more about appropriate colors later, since it's a function of illumination, reflection, and a bunch of lighting
				stuff I haven't gotten to yet 
	Image is a grid of pixels.
******************************************************************************/

#include <iostream>

#include "rtweekend.h"
//#include "ray.h"
//#include "vec3.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"


color ray_color(const ray& r, const hittable& world) {
	hit_record rec;
	//auto hit = hit_sphere(point3(0, 0, -1), 0.5, r);
	if (world.hit(r, 0, infinity, rec)) { // sphere at0,0,-1; radius=0.5, and we pass the ray to check for intersection
		//vec3 N = unit_vector(r.at(hit) - vec3(0, 0, -1));
		//return 0.5*color(N.x() + 1, N.y() + 1, N.z() + 1); // (x+1)*.5 shifts -1 -> 1 distribution to 0 -> 1 
		return 0.5 *  (rec.normal + color(1, 1, 1)); // still just a representation of the normal (not a real physics based reflection)
		//return color(1, 0, 0); // red sphere
	}

	// Create background/horizon (blue to white fade)
	vec3 unit_direction = unit_vector(r.direction());
	// ensure 0-1, since direction magnitudes range: -1 to 1
	auto hit = 0.5*(unit_direction.y() + 1.0);
	// Linear Interpolation (LERP) between white(1,1,1), and blue(0.5,0.7,1.0)
	return (1.0 - hit) * color(1.0, 1.0, 1.0) + hit * color(0.5, 0.7, 1.0);
}

int main() {


	// World
	hittable_list world; // all objects that rays can interact with in the scene (visible stuff)
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

	// Image
	const auto aspect_ratio = 16.0 / 9.0; // TODO: we seem to have this hardcoded here and in the camera
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 100; // Seems like a lot...we're going to slow down 100x

	// Camera
	camera cam;
	//auto viewport_height = 2.0;
	//auto viewport_width = aspect_ratio * viewport_height;
	//auto focal_length = 1.0;

	//auto origin = point3(0, 0, 0); // location of camera (which is also where rays come from)
	//auto horizontal = vec3(viewport_width, 0, 0);
	//auto vertical = vec3(0, viewport_height, 0);
	////auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
	//auto lower_left_corner = origin - vec3(viewport_width/2, viewport_height/2, focal_length);

	// Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	// Loop through all pixels in the image
	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j  << " of " << image_height << " " << std::flush;
		for (int i = 0; i < image_width; ++i) {
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto v = (j + random_double()) / (image_height - 1);
				auto u = (i + random_double()) / (image_width - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}

	std::cerr << "\nDone.\n";
}

// Ctrl+shft+B to compile solution
// Shift F7 to build current project
// ctrl+F7 to compile current file
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
