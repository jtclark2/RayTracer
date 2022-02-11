/*
This file is just filled with small inline utility functions.
*/


#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.;
}

// An alternate implementation was provided...I don't see any advantage, 
// but know that there are options, in case this profiles really slow, or presents problems
inline double random_double() {
	// Returns a random real in [0,1).
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	// returns a random real in [min, max).
	return min + (max - min)*random_double();
}

inline double clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}