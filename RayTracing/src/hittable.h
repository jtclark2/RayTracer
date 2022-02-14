#pragma once

#include "rtweekend.h"
#include "ray.h"

class material;

// We can choose a convention to use to define our normal (there are multiple valid options)
// This is important any time a surface has different behavior for rays hitting from different
// sides. eg: 
// - different texture on one side vs. the other
// - see-thru from the inside, so we don't make the whole scene invisible
// - transparent materials: light bends in different directions for enter vs. exit

// We'll use convention 2

// Convetion 1: Normal always points out of the sphere
//	Advantages: When we need to deterimine behavior (which side of the surface to use),
//  we can figure it out based on the geometry: bool sign = dot product(ray, normal) > 0

// Convetion 2: Normal points against the ray
//	Disadvantage: We need to store the orientation (bool outward_facing)
//	Advantages: We calculate the side impacted when we detect the intersect, and record immediately
//  If I want to extend this to process triangles/surfaces, then there won't be a clear in/out, so 
//  this more explicit convention might be nice...though either way could work
struct hit_record {
	point3 p;
	vec3 normal;
	shared_ptr<material> mat_ptr;
	double t;
	bool front_face;

	// The general concept here is setting which side a surface is being hit...
	// However, the concept of 'outward' is specific to a sphere.
	// I'm not sure if that makes this correct where it is, or more of a 
	// sphere-specific method...I'll leave it until I add triangles
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};