#include "Drone.hpp"

#include "SpriteBuffer.hpp"
#include "GameState.hpp"
#include "debug_sprite.hpp"
#include "Camera.hpp"
#include "text.hpp"

static const int STROBE_INTERVAL = 60*3;

void Drone::init(RandomGenerator& rng) {
	max_hull = 10;
	max_shield = 20;
	current_hull = max_hull;
	current_shield = max_shield;
	shield_recharge_delay = -1;

	angle = randRange(rng, 0.0f, DOUBLE_PI);
	angle_rate = randRange(rng, -DOUBLE_PI*0.003f, DOUBLE_PI*0.003f);
	anim_counter = randRange(rng, 0, STROBE_INTERVAL-1);
}

void Drone::draw(SpriteBuffer& sprite_buffer, SpriteBuffer& ui_buffer, const FontInfo& font, const Camera& camera) const {
	Sprite drone_spr;
	drone_spr.setImg(34, 1, 24, 24);
	drone_spr.setPos(camera.transform(rb.pos));

	SpriteMatrix matrix;
	matrix.loadIdentity().rotate(angle);

	sprite_buffer.append(drone_spr, matrix);

	if (anim_flags.test(AnimationFlags::YELLOW_STROBE)) {
		drone_spr.img.y = 1+1*25;
		sprite_buffer.append(drone_spr, matrix);
	}

	if (anim_flags.test(AnimationFlags::RED_STROBE)) {
		drone_spr.img.y = 1+2*25;
		sprite_buffer.append(drone_spr, matrix);
	}

	static const Color ui_color = {{17, 133, 209, 0}};

	drawString(drone_spr.x + 16, drone_spr.y - 16 - font.char_h,
		"HULL: " + std::to_string(current_hull) + "/" + std::to_string(max_hull), ui_buffer, font, ui_color);
	drawString(drone_spr.x + 16, drone_spr.y - 16,
		"SHLD: " + std::to_string(current_shield) + "/" + std::to_string(max_shield), ui_buffer, font, ui_color);
}

void Drone::update() {
	static const int STROBE_DURATION = 3;
	anim_flags.reset();
	if (anim_counter >= 0 && anim_counter < 0 + STROBE_DURATION) {
		anim_flags.set(AnimationFlags::YELLOW_STROBE);
	}
	if (anim_counter >= 15 && anim_counter < 15 + STROBE_DURATION) {
		anim_flags.set(AnimationFlags::RED_STROBE);
	}

	if (++anim_counter == STROBE_INTERVAL) {
		anim_counter = 0;
	}

	if (current_shield < max_shield) {
		if (--shield_recharge_delay == 0) {
			current_shield += 1;
			shield_recharge_delay = 10;
		}
	}

	angle += angle_rate;
	rb.update();
}

void Drone::getHit(const int damage_amount) {
	shield_recharge_delay = 120;

	current_shield -= damage_amount;
	if (current_shield < 0) {
		current_hull -= -current_shield;
		current_shield = 0;
		if (current_hull < 0) {
			current_hull = 0;
		}
	}
}
