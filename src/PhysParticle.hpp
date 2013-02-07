#pragma once
#include "Position.hpp"
#include "vec2.hpp"

struct PhysParticle {
	Position pos;
	vec2 vel;

	inline PhysParticle()
		: vel(mvec2(0.0f, 0.0f))
	{
	}

	inline void update() {
		pos += vel;
	}
};
