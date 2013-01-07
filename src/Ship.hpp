#pragma once
#include "game_types.hpp"
#include "vec2.hpp"

struct SpriteBuffer;

struct Ship {
	PositionFixed pos_x, pos_y;
	vec2 vel;
	float angle;
	vec2 accel;

	void draw(SpriteBuffer& sprite_buffer) const;
	void update(InputButtons::Bitset& input);
};