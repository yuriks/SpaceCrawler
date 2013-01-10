#pragma once
#include "Rigidbody.hpp"

struct SpriteBuffer;
struct Camera;

struct Bullet {
	Rigidbody rb;
	float angle;
	int life;

	Bullet() : life(60*10) { }
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update();
};
