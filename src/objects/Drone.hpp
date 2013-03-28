#pragma once
#include "Rigidbody.hpp"
#include "util/util.hpp"
#include "Shield.hpp"
#include <bitset>
#include <vector>

struct SpriteBuffer;
struct SpriteDb;
struct Camera;
struct FontInfo;
struct Debris;

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
	Shield shield;

	int current_hull;
	int max_hull;

	int anim_counter;
	AnimationFlags::Bitset anim_flags;
	IntRect img_body;
	IntRect img_strobe_yellow;
	IntRect img_strobe_red;
	std::vector<IntRect> img_debris;

	void init(RandomGenerator& rng, const SpriteDb& sprite_db);
	void draw(SpriteBuffer& sprite_buffer, SpriteBuffer& ui_buffer, const Camera& camera) const;
	void update();
	void getHit(const int damage_amount, vec2 rel_pos);
	void spawnDebris(std::vector<Debris>& debris_vec, RandomGenerator& rng) const;
};
