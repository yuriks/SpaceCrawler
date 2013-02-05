#pragma once
#include "Rigidbody.hpp"
#include "util.hpp"
#include <bitset>

struct SpriteBuffer;
struct Camera;
struct FontInfo;

struct Drone {
	struct AnimationFlags {
		enum Enum {
			YELLOW_STROBE,
			RED_STROBE,
			MAX
		};

		typedef std::bitset<MAX> Bitset;
	};

	Rigidbody rb;
	float angle;
	float angle_rate;

	int current_hull;
	int max_hull;
	int current_shield;
	int max_shield;
	int shield_recharge_delay;

	int anim_counter;
	AnimationFlags::Bitset anim_flags;

	void init(RandomGenerator& rng);
	void draw(SpriteBuffer& sprite_buffer, SpriteBuffer& ui_buffer, const FontInfo& font, const Camera& camera) const;
	void update();
	void getHit(const int damage_amount);
};
