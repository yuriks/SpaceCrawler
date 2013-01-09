#pragma once
#include "Rigidbody.hpp"

struct SpriteBuffer;
struct Camera;

struct Bullet {
	Rigidbody rb;
	float angle;

	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update();
};
