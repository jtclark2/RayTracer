#pragma once

#include "rtweekend.h"

class camera {
public:
	// Inputs:
	//	lookfrom: The point at the center of the camera sensor
	//	lookat: A point directly normal from the center of the sensor 
	//		(we really only need a direction vector, but this is convenient for pointing the camera towards specific targets)
	//	vup (Vertical UP): A vector that determines the roll of the camera (though it won't move the plane of the sensor)
	//		This vector doesn't need to lie in the plane of the sensor - if it's out of plane, we'll project it
	camera(
		point3 lookfrom,
		point3 lookat,
		vec3 vup,	
		double vfov, // vertical field-of-view in degrees (eg: 90 degrees is total, 45 up / 45 down)
		double aspect_ratio,
		double aperture,
		double focus_dist
	) {
		auto theta = degrees_to_radians(vfov);
		auto focal_length = 1.0; // There's not really any reason to change this (It just cancels itself out, so I'll probably remove it)
		// camera 'sensor' height in vector space
		auto viewport_height = 2.0 * focal_length * tan(theta / 2);
		auto viewport_width = aspect_ratio * viewport_height;

		// Basis vectors  
		w = unit_vector(lookfrom - lookat)*focal_length; // normal to sensor array
		u = unit_vector(cross(vup, w)); // horizontal
		v = cross(w, u); // vertical: projection of vup

		origin = lookfrom; 
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;

		lens_radius = aperture / 2;
	}

	ray get_ray(double i, double j) const {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();

		return ray(
			origin + offset, 
			lower_left_corner + i*horizontal + j*vertical - origin - offset);
	}

private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	double lens_radius;
};