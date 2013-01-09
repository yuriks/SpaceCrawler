#include "Drone.hpp"

#include "SpriteBuffer.hpp"
#include "GameState.hpp"
#include "debug_sprite.hpp"

static const int STROBE_INTERVAL = 60*3;

void Drone::init(RandomGenerator& rng) {
	angle = randRange(rng, 0.0f, DOUBLE_PI);
	angle_rate = randRange(rng, -DOUBLE_PI*0.003f, DOUBLE_PI*0.003f);
	anim_counter = randRange(rng, 0, STROBE_INTERVAL-1);
}

void Drone::draw(SpriteBuffer& sprite_buffer) const {
	Sprite drone_spr;
	drone_spr.setImg(34, 1, 24, 24);
	drone_spr.setPos(pos_x.integer(), pos_y.integer());

	SpriteMatrix matrix;
	matrix.loadIdentity().rotate(angle);

	sprite_buffer.append(drone_spr, matrix);

	if (anim_flags.test(AnimationFlags::YELLOW_STROBE)) {
		drone_spr.img_y = 1+1*25;
		sprite_buffer.append(drone_spr, matrix);
	}

	if (anim_flags.test(AnimationFlags::RED_STROBE)) {
		drone_spr.img_y = 1+2*25;
		sprite_buffer.append(drone_spr, matrix);
	}
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

	angle += angle_rate;
}
