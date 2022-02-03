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

	// This is actually redundant, since we define dot down below, which is a more general form
	// Leaving for consistency
	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
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

inline vec3 operator/(vec3 &v, double t) {
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