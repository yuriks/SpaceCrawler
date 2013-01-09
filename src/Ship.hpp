#pragma once
#include "game_types.hpp"
#include "vec2.hpp"
#include "Position.hpp"
#include <bitset>

struct SpriteBuffer;
struct GameState;

struct Ship {
	struct AnimationFlags {
		enum Enum {
			THRUST_FORWARD,
			THRUST_CW,
			THRUST_CCW,
			INERTIAL_BRAKE,
			MAX
		};

		typedef std::bitset<MAX> Bitset;
	};

	Position pos;
	vec2 vel;
	float angle;

	int shoot_cooldown;

	AnimationFlags::Bitset anim_flags;

	void init();
	void draw(SpriteBuffer& sprite_buffer) const;
	void update(InputButtons::Bitset& input, GameState& game_state);
};
