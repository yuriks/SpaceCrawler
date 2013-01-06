#pragma once

struct vec2 {
	float x, y;
};

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
