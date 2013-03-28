#include "Ship.hpp"

#include "render/SpriteBuffer.hpp"
#include "GameState.hpp"
#include "Bullet.hpp"
#include "Camera.hpp"
#include "render/SpriteDb.hpp"

void Ship::init(const SpriteDb& sprite_db) {
	rb.orientation = vec2_x;
	shoot_cooldown = 0;

	shield.max_level = 50;
	shield.hit_recharge_delay = 10;
	shield.img_shield_hit = sprite_db.lookup("shield24_hit"); // TODO create radius 32 shield sprite
	shield.shield_radius = 32;
	shield.init();

	img_ship_body   = sprite_db.lookup("player_ship1");
	img_ship_thrust = sprite_db.lookup("player_ship2");
	img_ship_brake  = sprite_db.lookup("player_ship3");
	img_bullet      = sprite_db.lookup("bullet_pulse_a");
}

void Ship::draw(SpriteBuffer& sprite_buffer, const Camera& camera) const {
	Sprite ship_spr;
	ship_spr.img = img_ship_body;
	ship_spr.setPos(camera.transform(rb.pos));

	SpriteMatrix matrix;
	matrix.loadIdentity().rotate(rb.orientation);

	sprite_buffer.append(ship_spr, matrix);

	if (anim_flags.test(AnimationFlags::THRUST_FORWARD)) {
		ship_spr.img = img_ship_thrust;
		sprite_buffer.append(ship_spr, matrix);
	}

	if (anim_flags.test(AnimationFlags::INERTIAL_BRAKE)) {
		ship_spr.img = img_ship_brake;
		sprite_buffer.append(ship_spr, matrix);
	}

	shield.draw(sprite_buffer, ship_spr.x, ship_spr.y);
}

void Ship::update(InputButtons::Bitset& input, GameState& game_state) {
	static const float TURNING_SPEED = radiansFromDegrees(5.0f);
	static const vec2 turning_vel = complex_from_angle(TURNING_SPEED);

	anim_flags.reset();

	if (input.test(InputButtons::LEFT)) {
		rb.angular_vel = complex_conjugate(turning_vel);
		anim_flags.set(AnimationFlags::THRUST_CCW);
	}

	if (input.test(InputButtons::RIGHT)) {
		rb.angular_vel = turning_vel;
		anim_flags.set(AnimationFlags::THRUST_CW);
	}

	if (input.test(InputButtons::LEFT) == input.test(InputButtons::RIGHT)) {
		rb.angular_vel = complex_1;
		Position mouse_pos = game_state.camera.inverse_transform(
			mvec2(static_cast<float>(game_state.mouse_x), static_cast<float>(game_state.mouse_y)));
		vec2 mouse_orientation = normalized(mouse_pos - rb.pos);
		rb.orientation = rotateTowards(rb.orientation, mouse_orientation, TURNING_SPEED);
	}

	if (input.test(InputButtons::BRAKE)) {
		rb.vel = 0.96f * rb.vel;
		anim_flags.set(AnimationFlags::INERTIAL_BRAKE);
	} else if (input.test(InputButtons::THRUST)) {
		vec2 accel = 0.05f * rb.orientation;
		rb.vel = rb.vel + accel;
		anim_flags.set(AnimationFlags::THRUST_FORWARD);
	}

	if (input.test(InputButtons::SHOOT) && shoot_cooldown == 0) {
		Bullet bullet;
		bullet.physp.pos = rb.pos;
		bullet.orientation = rb.orientation;
		bullet.physp.vel = rb.vel + 4.0f * rb.orientation;
		bullet.img = img_bullet;

		game_state.bullets.push_back(bullet);
		shoot_cooldown = 5;
	}
	if (shoot_cooldown > 0) {
		--shoot_cooldown;
	}

	shield.update();
	rb.update();
}
