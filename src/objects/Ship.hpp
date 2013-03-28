#pragma once
#include "game_types.hpp"
#include "Rigidbody.hpp"
#include "Shield.hpp"
#include <bitset>

struct SpriteBuffer;
struct SpriteDb;
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

	Rigidbody rb;
	Shield shield;

	int shoot_cooldown;

	AnimationFlags::Bitset anim_flags;
	IntRect img_ship_body;
	IntRect img_ship_thrust;
	IntRect img_ship_brake;
	IntRect img_bullet;

	void init(const SpriteDb& sprite_db);
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update(InputButtons::Bitset& input, GameState& game_state);
};
