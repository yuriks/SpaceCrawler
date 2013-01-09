#pragma once
#include "Position.hpp"
#include "vec2.hpp"

struct Rigidbody {
	Position pos;
	vec2 vel;

	inline Rigidbody()
		: vel(mvec2(0.0f, 0.0f))
	{
	}

	inline void update() {
		pos += vel;
	}
};
