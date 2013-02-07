#pragma once
#include "PhysParticle.hpp"
#include "vec2.hpp"

struct Rigidbody : PhysParticle {
	vec2 orientation;
	vec2 angular_vel;

	inline Rigidbody()
		: angular_vel(complex_1)
	{ }

	inline void setOrientation(float radians) {
		orientation = complex_from_angle(radians);
	}

	inline void setAngularVel(float radians_per_frame) {
		angular_vel = complex_from_angle(radians_per_frame);
	}

	inline void update() {
		PhysParticle::update();
		orientation = complex_mul(orientation, angular_vel);
	}
};
