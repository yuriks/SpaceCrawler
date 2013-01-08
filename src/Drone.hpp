#pragma once
#include "game_types.hpp"
#include "vec2.hpp"
#include "util.hpp"
#include <bitset>

struct SpriteBuffer;

struct Drone {
	struct AnimationFlags {
		enum Enum {
			YELLOW_STROBE,
			RED_STROBE,
			MAX
		};

		typedef std::bitset<MAX> Bitset;
	};

	PositionFixed pos_x, pos_y;
	float angle;
	float angle_rate;

	int anim_counter;
	AnimationFlags::Bitset anim_flags;

	void init(RandomGenerator& rng);
	void draw(SpriteBuffer& sprite_buffer) const;
	void update();
};
