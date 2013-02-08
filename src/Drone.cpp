#include "Drone.hpp"

#include "SpriteBuffer.hpp"
#include "GameState.hpp"
#include "debug_sprite.hpp"
#include "Camera.hpp"
#include "text.hpp"
#include <array>

static const int STROBE_INTERVAL = 60*3;

static const IntRect img_body          = {34,  1, 24, 24};
static const IntRect img_strobe_yellow = {34, 26, 24, 24};
static const IntRect img_strobe_red    = {34, 51, 24, 24};
static const IntRect img_shield_hit    = {42, 94, 16, 32};

static const std::array<IntRect, 4> img_debris = {{
	{41, 76, 8, 8},
	{50, 76, 8, 8},
	{41, 85, 8, 8},
	{50, 85, 8, 8}
}};

void Drone::init(RandomGenerator& rng) {
	max_hull = 10;
	max_shield = 20;
	current_hull = max_hull;
	current_shield = max_shield;
	shield_recharge_delay = -1;

	rb.setOrientation(randRange(rng, 0.0f, DOUBLE_PI));
	rb.setAngularVel(randRange(rng, -DOUBLE_PI*0.003f, DOUBLE_PI*0.003f));
	anim_counter = randRange(rng, 0, STROBE_INTERVAL-1);
	hit_anim_counter = 0;
}

void Drone::draw(SpriteBuffer& sprite_buffer, SpriteBuffer& ui_buffer, const FontInfo& font, const Camera& camera) const {
	Sprite drone_spr;
	drone_spr.img = img_body;
	drone_spr.setPos(camera.transform(rb.pos));

	SpriteMatrix matrix;
	matrix.loadIdentity().rotate(rb.orientation);

	sprite_buffer.append(drone_spr, matrix);

	if (anim_flags.test(AnimationFlags::YELLOW_STROBE)) {
		drone_spr.img = img_strobe_yellow;
		sprite_buffer.append(drone_spr, matrix);
	}

	if (anim_flags.test(AnimationFlags::RED_STROBE)) {
		drone_spr.img = img_strobe_red;
		sprite_buffer.append(drone_spr, matrix);
	}

	if (hit_anim_counter > 0) {
		Sprite shield_spr;
		shield_spr.img = img_shield_hit;
		int center_offset = 16 - img_shield_hit.w/2;
		shield_spr.x = drone_spr.x + static_cast<int>(center_offset * hit_direction.x);
		shield_spr.y = drone_spr.y + static_cast<int>(center_offset * hit_direction.y);
		shield_spr.color = makeColor(0, 156, 255, 0);

		SpriteMatrix shield_matrix;
		shield_matrix.loadIdentity().rotate(hit_direction);

		sprite_buffer.append(shield_spr, shield_matrix);
	}

	static const Color ui_color = {{49, 209, 17, 0}};

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
	if (hit_anim_counter > 0) {
		--hit_anim_counter;
	}

	if (current_shield < max_shield) {
		if (--shield_recharge_delay == 0) {
			current_shield += 1;
			shield_recharge_delay = 10;
		}
	}

	rb.update();
}

void Drone::getHit(const int damage_amount, vec2 rel_pos) {
	shield_recharge_delay = 120;

	if (current_shield > 0) {
		hit_anim_counter = 2;
		hit_direction = normalized(rel_pos);
	}

	current_shield -= damage_amount;
	if (current_shield < 0) {
		current_hull -= -current_shield;
		current_shield = 0;
		if (current_hull < 0) {
			current_hull = 0;
		}
	}
}

void Drone::spawnDebris(std::vector<Debris>& debris_vec, RandomGenerator& rng) const {
	Debris d;
	d.rb.pos = rb.pos;
	d.life = 240;

	int num_debris = randRange(rng, 8, 32);
	for (int i = 0; i < num_debris; ++i) {
		float vel_magnitude = randRange(rng, 0.25f, 1.5f);
		d.rb.vel = vel_magnitude * complex_from_angle(randRange(rng, 0.0f, DOUBLE_PI));
		d.rb.setOrientation(randRange(rng, 0.0f, DOUBLE_PI));
		d.rb.setAngularVel(randRange(rng, -DOUBLE_PI*0.05f, DOUBLE_PI*0.05f));

		d.img = randElement(rng, img_debris);

		debris_vec.push_back(d);
	}
}
