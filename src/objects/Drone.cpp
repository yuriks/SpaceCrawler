#include "Drone.hpp"

#include "render/SpriteBuffer.hpp"
#include "GameState.hpp"
#include "render/debug_sprite.hpp"
#include "Camera.hpp"
#include "render/text.hpp"
#include "hud.hpp"
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
	current_hull = max_hull;

	shield.max_level = 20;
	shield.hit_recharge_delay = 120;
	shield.recharge_interval = 10;
	shield.img_shield_hit = img_shield_hit;
	shield.init();

	rb.setOrientation(randRange(rng, 0.0f, DOUBLE_PI));
	rb.setAngularVel(randRange(rng, -DOUBLE_PI*0.003f, DOUBLE_PI*0.003f));
	anim_counter = randRange(rng, 0, STROBE_INTERVAL-1);
}

void Drone::draw(SpriteBuffer& sprite_buffer, SpriteBuffer& ui_buffer, const Camera& camera) const {
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

	shield.draw(sprite_buffer, drone_spr.x, drone_spr.y);

	drawString(drone_spr.x + 16, drone_spr.y - 16 - ui_font.char_h,
		"HULL: " + std::to_string(current_hull) + "/" + std::to_string(max_hull), ui_buffer, ui_font, hud_color);
	drawString(drone_spr.x + 16, drone_spr.y - 16,
		"SHLD: " + std::to_string(shield.cur_level) + "/" + std::to_string(shield.max_level), ui_buffer, ui_font, hud_color);
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

	shield.update();
	rb.update();
}

void Drone::getHit(const int damage_amount, vec2 rel_pos) {
	const int shield_leak = shield.getHit(damage_amount, rel_pos);
	if (shield_leak > 0) {
		current_hull -= shield_leak;
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
