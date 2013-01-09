#pragma once
#include "Rigidbody.hpp"
#include "util.hpp"
#include <bitset>

struct SpriteBuffer;
struct Camera;

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

	int anim_counter;
	AnimationFlags::Bitset anim_flags;

	void init(RandomGenerator& rng);
	void draw(SpriteBuffer& sprite_buffer, const Camera& camera) const;
	void update();
};
