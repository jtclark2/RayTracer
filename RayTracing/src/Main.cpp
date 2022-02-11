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
#include "material.h"

/*
As a physics major, I'm amazed how well this simple model is working, despite a lot of assumptions, and missing elemnents.
This author's done a great job so far, so I'm sure we're getting to at least some of these, but here's a few
thoughts for improvement, just in case:
1) We have no light sources. This was confusing to me at first. Rather than light, we just return the background when we
don't hit anything. I guess this is kind of like a foggy day, when looking any direction is just the same brightness, 
with no apparent source/direction to the light. It's basically all secondary scattering...Kind of a cool work around.
2) ray tracing is expensive: Why not turn down the sample count, record you first contact object, creating a
	mask, and smoothing within that mask...
	- OK, for a "perfect" render, this model is simpler, and more accurate, but the CPU time is getting a
	bit crazy (maybe not too bad with shaders, but I imagine you would want to process as many objects as
	possible, and some large/realistic scenes (I'm talking modern triangles, not just spheres) could have
	many objects, and potentially high frame rates
		- You could blur on secondary objects too, but I'm not sure that would really be needed
		- Not sure if it's worth it, but you could also blur in a scaled space...I don't have my intuition,
		so I'm just throwing ideas around, but converting to RMS / log / exp space and then blurring might be interesting.
		The main effect would be how much you let anomalous pixels pull their surroundings, rather than getting 
		pulled in.
		- Also, why used random rays? Would sub-pixel rays for interpolated contributions make more sense?
			- You could get more even distribution
			- You could re-uses a weighted contribution of each ray on the 4 pixels whose centers it would be between
3) I'm sure we'll get to more reflective surfaces...curious how we'll approach those
	- If we got really deep, we might address the changes to light as reflections approach 90 degrees off of the normal
		- The light becomes polarized (which probably doesn't matter in 99% of simulations, though lots of 
		modern glass/windows will never look quite right)
		- I'm pretty sure it also loses saturation. I understand why it wouldn't be in this tutorial, but 
		a bright room, with direct sunlight should get hotspots, and change in color on reflective surfaces
4) I peaked ahead, so I know we'll be dealing with transparent objects
	- Will they be pure transparent models, or include translucent scattered (which could be done with pure ray
	tracer, or more computationally efficient with a skin depth model)

*/
color ray_color(const ray& r, const hittable& world, int depth) {
	if (depth <= 0)
		return  color(0, 0, 0);

	hit_record rec;

	// limit lower end of range to avoid floating a reflected ray hitting the spot it reflected off of...
	// This is a bug that can occur because of floating point precision. You start the ray where the last one collided,
	// but that could be +/-0.0000000000000000001 (or however many 0's). Then the ray could technically be inside the sphere
	// when it's created. This happens a lot, causing a speckling problem, called "shadow acne". This impact is way bigger than
	// I expected. Without this fix, repeated reflection were slowing the render down a ton (I think most rays would reflect once or twice,
	// but must have ended up reflecting max_depth times because of this). Also, the "acne" is very pronounced. It looked very noisy. 
	// I'm very glad the tutorial pointed this out, because it would have taken me forever to find this one!
	if (world.hit(r, 0.001, infinity, rec)) { 
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);

		//// Lambertian reflection off of diffuse surfaces (2 options with very similar effects...to my eye at least)
		//// Option 1: using this for now...seems closest to my understanding after reading the wiki
		//point3 target = rec.p + rec.normal + random_unit_vector(); // random ray coming off of target pointing towards random point in the unit sphere
		//// Option 2: Very similar render (I can't tell the difference (look up Lambertian Diffuse)
		////point3 target = rec.p + random_in_hemisphere(rec.normal);
		//return GAMMA * ray_color(ray(rec.p, target - rec.p), world, depth - 1);

		//vec3 N = unit_vector(r.at(hit) - vec3(0, 0, -1));
		//return 0.5*color(N.x() + 1, N.y() + 1, N.z() + 1); // (x+1)*.5 shifts -1 -> 1 distribution to 0 -> 1 

		//return 0.5 *  (rec.normal + color(1, 1, 1)); // still just a representation of the normal (not a real physics based reflection)
		
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

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
	//auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	//auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));


	// Image
	const auto aspect_ratio = 16.0 / 9.0; // TODO: we seem to have this hardcoded here and in the camera
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 10;
	const int max_depth = 10;

	// Camera
	camera cam;

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
				pixel_color += ray_color(r, world, max_depth);
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
