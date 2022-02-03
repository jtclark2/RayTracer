/******************************************************************************
Trevor's thoughts:
- viewport: This is effectively the eye/wall we project the image on. You can
kind of think of it as the image, except that it has a position of project,
rather than being associated with a pixel. main loops through pixel values to
find the viewport value at each pixel index.
******************************************************************************/

#include <iostream>

#include "ray.h"
#include "color.h"
#include "vec3.h"


bool hit_sphere(const point3& center, double radius, const ray& r) {
	// Solve for t
	// dir^2*t^2 + 2*dir*(pt - Center) + dot(pt-center, pt-center)-r^2 = 0
	// Quadratic formula: t = -b +/- sqrt(b^2 - 4ac) / (2ac)
	// Simplify a bit further...we don't need the solution, just whether or
	// not there is one. If the value in the sqrt > 0, then the answers are
	// real. 
	// Technically == 0 also gives us a value, but it's right on the edge, whic
	// isn't a concern, since that's negligably small with float/double anyways

	vec3 oc = r.origin() - center;
	auto a = dot(r.direction(), r.direction());
	auto b = 2.0 * dot(oc, r.direction());
	auto c = dot(oc, oc) - radius*radius;
	auto discriminant = b*b - 4*a*c;
	//std::cerr << "origin: " << r.origin() << " center: " << center <<  "ray at z=-1: " << r.<<  disc: " << discriminant << "\n";
	return (discriminant > 0);
}

color ray_color(const ray& r) {
	if (hit_sphere(point3(0, 0, -1), 0.5, r)) { // sphere at0,0,-1; radius=0.5, and we pass the ray to check for intersection
		return color(1, 0, 0); // red sphere
	}

	// Create background/horizon (blue to white fade)
	vec3 unit_direction = unit_vector(r.direction());
	// ensure 0-1, since direction magnitudes range: -1 to 1
	auto t = 0.5*(unit_direction.y() + 1.0);
	// Linear Interpolation (LERP) between white(1,1,1), and blue(0.5,0.7,1.0)
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main() {

	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	//const int image_width = 256;
	//const int image_height = 256;

	// Camera
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	// Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j  << " of " << image_height << " " << std::flush;
		for (int i = 0; i < image_width; ++i) {
			auto u = double(i) / (image_width - 1); 
			auto v = double(j) / (image_height - 1); // TODO: minor optimization: move this to outer loop
			// This was just a sample image, creating a blended rainbow color picker table
			//color pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);

			// This could be optimized (most these multiplications only apply to 1 element of the vec3
			// Rays come from:
			//	r.origin : local is also called origin - basically the center of our camera/projection surface):
			//	r.direction: from lower_left (most negative point in both x,y FOV, then iterate through the entire FOV,
			//		spanning the entire vertical and horizontal.
			//		- I think we're assuming dist to target is 1, which is why this works...there are some issues with
			//		that, but there's lots of tutorial to go
			ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			color pixel_color = ray_color(r);
			write_color(std::cout, pixel_color);

			// Commented version does not use color or vec3 libraries
			//auto r = double(i) / (image_width - 1);
			//auto g = double(j) / (image_height - 1);S
			//auto b = 0.25;
			//int ir = static_cast<int>(255.999 * r);
			//int ig = static_cast<int>(255.999 * g);
			//int ib = static_cast<int>(255.999 * b);
			//std::cout << ir << ' ' << ig << ' ' << ib << '\n';
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
