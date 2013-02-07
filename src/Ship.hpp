#pragma once
#include "game_types.hpp"
#include "PhysParticle.hpp"
#include <bitset>

struct SpriteBuffer;
struct GameState;
struct Camera;

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

	PhysParticle physp;
	float angle;

	int shoot_cooldown;

	AnimationFlags::Bitset anim_flags;

	void init();
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update(InputButtons::Bitset& input, GameState& game_state);
};
