#pragma once
#include "PhysParticle.hpp"
#include "render/Sprite.hpp"

struct SpriteBuffer;
struct Camera;

struct Bullet {
	PhysParticle physp;
	vec2 orientation;
	int life;
	IntRect img;

	Bullet() : life(60*10) { }
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update();
};
