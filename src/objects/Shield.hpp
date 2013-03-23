#pragma once
#include "util/vec2.hpp"
#include "render/Sprite.hpp"

struct SpriteBuffer;

struct Shield {
	int cur_level;
	int cur_recharge_delay;
	int hit_anim_counter;
	vec2 hit_direction;

	// Shield parameters
	int max_level;
	int hit_recharge_delay;
	int recharge_step;
	int recharge_interval;
	IntRect img_shield_hit;
	Color shield_color;
	int shield_radius;

	Shield()
		: max_level(0),
		hit_recharge_delay(0),
		recharge_step(1),
		recharge_interval(0),
		shield_color(makeColor(0, 156, 255, 0)),
		shield_radius(16)
	{}

	void init();
	void update();
	void draw(SpriteBuffer& sprite_buffer, int x, int y) const;
	// Damages shield. Returns amount of leaked damage.
	int getHit(int damage, vec2 rel_pos);
};