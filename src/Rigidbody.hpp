#pragma once
#include "PhysParticle.hpp"
#include "vec2.hpp"

struct Rigidbody : PhysParticle {
	float angle;
	float angle_rate;

	inline Rigidbody()
		: angle_rate(0.0f)
	{ }

	inline void update() {
		PhysParticle::update();
		angle += angle_rate;
	}
};
