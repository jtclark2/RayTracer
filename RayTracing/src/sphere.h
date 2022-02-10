#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
	sphere() {}
	sphere(point3 cen, double r) : center(cen), radius(r) {};

	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
	point3 center;
	double radius;
};


bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	// Purpose: Check if/where an intersection occurs
	// I'm going to include a quick-ish derivation as well...
	// Equation for a sphere of radius r, centered at C: 
	//      (x-Cx)^2 + (y-Cy)^2 + (z-Cz)^2 = r2
	// Let vector P = (x,y,z)
	//      (Px-Cx)^2 + (Py-Cy)^2 + (Pz-Cz)^2 = r2
	// Simplify with the dot product (We'll use `.` for dot product, A*B = Ax*Bx + Ay.by + Az.Bz)
	//      (P-C).(P-C) = r^2
	// Substitute our slope/intercept form (or origin/dir, since that's how we've defined a ray)
	// for P = A + t*b 
	//      (origin+t*dir-C).(origin+t*dir-C)=r^2
	// Rearrange terms to get into quadratic form A*t^2 + b*t + c = 0:
	//      dir.dir*t^2 + 2*dir.(origin - Center)*t + (origin-center).(origin-center)-r^2 = 0
	// Quadratic formula: t = ( -b +/- sqrt(b^2 - 4ac) ) / (2ac)
	// We can implement from here, but a few slight simplifications (arguably) make the code cleaner
	//		- Replace dir.dir with length_squared(dir)
	//      - Let helf_b = b/2. Since there is a 2 in our expression, this cancels out nicely, and removes
	//      all the extra 2's and 4's from the math -> t = ( -half_b +/- sqrt(half_b^2 - ac) ) / (ac)
	// If the part under the sqrt (called the discriminant) > 0, then the solution is a real number, 
	// and the meaing the ray intersects the sphere. There are two solutions +/-,
	// representing the ray entering and exiting the sphere


	// Solve for discriminant (are there intercepts?)
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c; // the part of the quadratic formula under the sqrt()
		
	if (discriminant < 0) { // meaning sphere is hit
		return false;
	}

	// Find the roots/intercepts (one is where the ray hits first (enters) the sphere, and the other is the exit; we want the entry ray)
	// The solution that hits first, is the smallest value (as t increases, the ray advances, so lower t hits first).
	auto sqrtd = sqrt(discriminant);
	auto root = (-half_b - sqrtd) / a; 
	if (t_max < root || root < t_min) {	// Is the ENTERING ray intersection within the specified distance
		root = (-half_b + sqrtd) / a; // Exiting ray - not sure how we see this...maybe if we are inside a sphere?
		if (t_max < root || root < t_min) {	// Is the EXITING ray intersection within the specified distance
			return false;
		}
	}

	// rec is a hit_record; passed in by reference, so we don't need an explicit return
	rec.t = root; // intercept time
	rec.p = r.at(rec.t); // intercept point
	vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal); // normal (unit vector pointing straight out of surface

	return true;
}