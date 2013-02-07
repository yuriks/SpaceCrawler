#pragma once
#include "PhysParticle.hpp"

struct SpriteBuffer;
struct Camera;

struct Bullet {
	PhysParticle physp;
	float angle;
	int life;

	Bullet() : life(60*10) { }
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update();
};
