#pragma once
#include <cmath>

struct vec2 {
	float x, y;
};

static const vec2 vec2_0 = {0.0f, 0.0f};
static const vec2 vec2_1 = {1.0f, 1.0f};
static const vec2 vec2_x = {1.0f, 0.0f};
static const vec2 vec2_y = {0.0f, 1.0f};

inline vec2 mvec2(float x, float y) {
	vec2 tmp = {x, y};
	return tmp;
}

inline vec2 operator +(const vec2 a, const vec2 b) {
	return mvec2(a.x + b.x, a.y + b.y);
}

inline vec2 operator -(const vec2 a, const vec2 b) {
	return mvec2(a.x - b.x, a.y - b.y);
}

inline vec2 operator *(const float s, const vec2 v) {
	return mvec2(s*v.x, s*v.y);
}

inline vec2 operator *(const vec2 v, const float s) {
	return s * v;
}

inline vec2 operator /(const float s, const vec2 v) {
	return mvec2(s/v.x, s/v.y);
}

inline vec2 operator /(const vec2 v, const float s) {
	return mvec2(v.x/s, v.y/s);
}

inline vec2 operator -(const vec2 v) {
	return mvec2(-v.x, -v.y);
}

inline float dot(const vec2 a, const vec2 b) {
	return a.x*b.x + a.y*b.y;
}

inline float length_sqr(const vec2 v) {
	return dot(v, v);
}

inline vec2 normalized(const vec2 v) {
	return v / std::sqrt(length_sqr(v));
}

static const vec2 complex_1 = {1.0f, 0.0f};

inline vec2 complex_mul(const vec2 a, const vec2 b) {
	return mvec2(a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y);
}

inline vec2 complex_from_angle(float radians) {
	return mvec2(std::cos(radians), std::sin(radians));
}

inline vec2 complex_conjugate(const vec2 c) {
	return mvec2(c.x, -c.y);
}
