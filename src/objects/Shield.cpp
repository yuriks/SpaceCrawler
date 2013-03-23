#include "Shield.hpp"
#include <algorithm>
#include "render/Sprite.hpp"
#include "render/SpriteBuffer.hpp"

void Shield::init() {
	cur_level = max_level;
	cur_recharge_delay = 0;
	hit_anim_counter = 0;
}

void Shield::update() {
	if (hit_anim_counter > 0) {
		--hit_anim_counter;
	}

	if (cur_level < max_level) {
		if (--cur_recharge_delay < 0) {
			cur_level += recharge_step;
			cur_recharge_delay = recharge_interval;
		}
	}
}

void Shield::draw(SpriteBuffer& sprite_buffer, int x, int y) const {
	if (hit_anim_counter > 0) {
		Sprite shield_spr;
		shield_spr.img = img_shield_hit;
		int center_offset = shield_radius - img_shield_hit.w/2;
		shield_spr.x = x + static_cast<int>(center_offset * hit_direction.x);
		shield_spr.y = y + static_cast<int>(center_offset * hit_direction.y);
		shield_spr.color = shield_color;

		SpriteMatrix shield_matrix;
		shield_matrix.loadIdentity().rotate(hit_direction);

		sprite_buffer.append(shield_spr, shield_matrix);
	}
}

int Shield::getHit(int damage, vec2 rel_pos) {
	cur_recharge_delay = hit_recharge_delay;

	if (cur_level > 0) {
		hit_anim_counter = 2;
		hit_direction = normalized(rel_pos);
	}

	cur_level -= damage;
	if (cur_level < 0) {
		const int leak = -cur_level;
		cur_level = 0;
		return leak;
	} else {
		return 0;
	}
}