#pragma once
#include "Rigidbody.hpp"
#include "render/Sprite.hpp"

struct SpriteBuffer;
struct Camera;

struct Debris {
	Rigidbody rb;
	IntRect img;
	int life;

	void update();
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
};
