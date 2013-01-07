#pragma once

#include <random>
#include <algorithm>

#define NONCOPYABLE(cls) cls(const cls&); const cls& operator=(const cls&)

template <typename T>
T stepTowards(T initial, T target, T step) {
	if (initial < target) {
		initial += step;
		if (initial > target)
			return target;
		return initial;
	} else if (initial > target) {
		initial -= step;
		if (initial < target)
			return target;
		return initial;
	} else {
		return initial;
	}
}

template <typename T>
T clamp(T min, T val, T max) {
	if (val < min) return min;
	else if (val > max) return max;
	else return val;
}

template <typename C, typename F>
void remove_if(C& container, const F& predicate) {
	container.erase(std::remove_if(container.begin(), container.end(), predicate), container.end());
}

typedef std::mt19937 RandomGenerator;

inline int randRange(RandomGenerator& r, int min, int max) {
	return std::uniform_int_distribution<>(min, max)(r);
}

inline int randRange(RandomGenerator& r, int max) {
	return randRange(r, 0, max);
}

inline bool randBool(RandomGenerator& r) {
	return randRange(r, 1) == 1;
}

static const float DOUBLE_PI = 6.2831854f;
static const float DEG_TO_RAD = 0.017453292f;

inline float radiansFromDegrees(float degrees) {
	return degrees * DEG_TO_RAD;
}
