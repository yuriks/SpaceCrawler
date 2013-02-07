#pragma once
#include "vec2.hpp"

// Collides circle with axis-aligned rectangle centered on origin
bool collideCircleAARectangle(vec2 circle_position, float circle_radius, vec2 rect_halfsize);
// Collides circle with rotated rectangle centered on origin
bool collideCircleRectangle(vec2 circle_position, float circle_radius, vec2 rect_halfsize, vec2 rect_orientation);
