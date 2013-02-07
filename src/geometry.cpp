#include "geometry.hpp"

#include <cmath>

bool collideCircleAARectangle(vec2 cp, float cr, vec2 r) {
	// Adapted from http://stackoverflow.com/a/402010
	vec2 circDist = {std::abs(cp.x) - r.x, std::abs(cp.y) - r.y};

	if (circDist.x > cr || circDist.y > cr) {
		return false;
	}

	if (circDist.x <= 0.0f || circDist.y <= 0.0f) {
		return true;
	}

	return length_sqr(circDist) <= cr*cr;
}

bool collideCircleRectangle(vec2 circle_position, float circle_radius, vec2 rect_size, vec2 rect_orientation) {
	vec2 rotated_circle = complex_mul(circle_position, complex_conjugate(rect_orientation));
	return collideCircleAARectangle(rotated_circle, circle_radius, rect_size);
}
