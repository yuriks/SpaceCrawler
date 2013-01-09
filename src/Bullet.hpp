#pragma once
#include "Position.hpp"
#include "vec2.hpp"

struct SpriteBuffer;

struct Bullet {
	Position pos;
	float angle;
	vec2 vel;

	void draw(SpriteBuffer& sprite_buffer) const;
	void update();
};
