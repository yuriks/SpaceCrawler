#pragma once
#include "PhysParticle.hpp"

struct SpriteBuffer;
struct Camera;

struct Bullet {
	PhysParticle physp;
	vec2 orientation;
	int life;

	Bullet() : life(60*10) { }
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update();
};
