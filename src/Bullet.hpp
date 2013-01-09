#pragma once
#include "game_types.hpp"
#include "vec2.hpp"

struct SpriteBuffer;

struct Bullet {
	PositionFixed pos_x, pos_y;
	float angle;
	vec2 vel;

	void draw(SpriteBuffer& sprite_buffer) const;
	void update();
};
