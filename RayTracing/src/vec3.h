/********************************
My thoughts: I'm sticking to the script pretty closely so far, just changing 
little syntax things, like pragma once instead of ifndef. I want to log 
questions/comments so I can come back to understand/optimize later:
- why public e[3]?
- how does he distinguish betwee calls the operator[] method? Does the 
compiler recognize the need for const in context?
- I'm assuming that optimization at compile time makes the /= operator just as
fast as if it were written explicitly, but consider trying it.
- How much (if at all) would this speed up if I switched doubles to floats? Obviously
it should take 1/2 the memory, and in theory if I'm efficient about it, it 
should also take around 1/2 the CPU time.
- I'm assuming inline definitions are all used to speed up the program by 
avoiding calls...might be fun to explore how big an impact that makes.
********************************/


#pragma once

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
public:
	vec3() : e{ 0, 0, 0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return  e[i];  }

	vec3& operator+=(const vec3 &v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(const double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(const double t) {
		return *this *= 1 / t;
	}

	double length() const {
		return sqrt(length_squared());
	}

	bool near_zero() const {
		// Return true if the vector is close to zero in every dimension
		const auto small = 1e-8; // Threhsold for a small number
		return (fabs(e[0]) < small) && (fabs(e[1]) < small) && (fabs(e[2]) < small);
	}

	// This is actually redundant, since we define dot down below, which is a more general form
	// Leaving for consistency
	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

public:
	double e[3];

};

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;


// vec3 Utility Functions

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
	return t * v; // uses definition above
}


// TODO: Figure out why this one isn't passed as ref...
// Initially I made it a ref, since everything else was, but it throws an error
// When I look back at the tutorial, this one is not a reference...why not???
inline vec3 operator/(vec3 v, double t) {
	return (1 / t) * v;
}

// Not including...makes sense from a broadcasting perspective, but 
// we generally shouldn't be dividing a double by a vec3
 //inline vec3 operator/(double t, vec3 &v) {
 //	return vec3(t / v.e[0], t / v.e[1], t / v.e[2])
 //}

inline double dot(const vec3 &u, const vec3 &v) {
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}




vec3 random_in_unit_sphere() {
	while (true) {
		vec3 pt = vec3::random(-1, 1);
		if (pt.length_squared() < 1)
			return pt;
	}
}

vec3 random_unit_vector() { // with a very unique/particular distribution
	return unit_vector(random_in_unit_sphere());
}

vec3 random_in_hemisphere(const vec3& normal) {
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;
}


vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	// This method basically computes snell's law, though a little re-arranging has been done
	// I'm going to use n = eta (even though technically eta is a greek letter that just looks like an n)
	// Snell's Law: n*sin(theta)=n'*sin(theta') 
	// Recall: |a|*|b|*cos(theta) = dot(a, b)
	// Use some trig properties to swap the sin for cos, and re-arrange with algrebra...you can end up with
	// expression for both components of the refracted ray:
	// r_out_perp is perpendicular to the normal
	// r_out_parallel is parallel to the normal
	//
	// Inputs:
	//	- etai_over_etat can be thought of as: eta I / eta T, the ratio of the two indexes of refraction
	//  - n: The normal to the surface of interaction
	//  - uv: direction vector of the the incoming ray
	//
	// Return:
	//  - refracted ray
	// where eta is the greek letter that looks like n, used in snell's law of refaction
	auto cos_theta = fmin(dot(-uv, n), 1.0);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;

}

vec3 random_in_unit_disk() {
	while (true) {
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}