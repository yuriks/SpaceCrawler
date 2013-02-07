#include "Ship.hpp"

#include "SpriteBuffer.hpp"
#include "GameState.hpp"
#include "Bullet.hpp"
#include "Camera.hpp"

static const IntRect img_ship_body   = {1,  1, 32, 24};
static const IntRect img_ship_thrust = {1, 26, 32, 24};
static const IntRect img_ship_brake  = {1, 51, 32, 24};

void Ship::init() {
	angle = 0;
	shoot_cooldown = 0;
}

void Ship::draw(SpriteBuffer& sprite_buffer, const Camera& camera) const {
	Sprite ship_spr;
	ship_spr.img = img_ship_body;
	ship_spr.setPos(camera.transform(physp.pos));

	SpriteMatrix matrix;
	matrix.loadIdentity().rotate(angle);

	sprite_buffer.append(ship_spr, matrix);

	if (anim_flags.test(AnimationFlags::THRUST_FORWARD)) {
		ship_spr.img = img_ship_thrust;
		sprite_buffer.append(ship_spr, matrix);
	}

	if (anim_flags.test(AnimationFlags::INERTIAL_BRAKE)) {
		ship_spr.img = img_ship_brake;
		sprite_buffer.append(ship_spr, matrix);
	}
}

void Ship::update(InputButtons::Bitset& input, GameState& game_state) {
	static const float TURNING_SPEED = radiansFromDegrees(5.0f);

	anim_flags.reset();

	if (input.test(InputButtons::LEFT)) {
		angle -= TURNING_SPEED;
		anim_flags.set(AnimationFlags::THRUST_CCW);
	}

	if (input.test(InputButtons::RIGHT)) {
		angle += TURNING_SPEED;
		anim_flags.set(AnimationFlags::THRUST_CW);
	}

	if (input.test(InputButtons::THRUST)) {
		vec2 accel = 0.05f * complex_from_angle(angle);
		physp.vel = physp.vel + accel;
		anim_flags.set(AnimationFlags::THRUST_FORWARD);
	} else if (input.test(InputButtons::BRAKE)) {
		physp.vel = physp.vel * 0.96f;
		anim_flags.set(AnimationFlags::INERTIAL_BRAKE);
	}

	if (input.test(InputButtons::SHOOT) && shoot_cooldown == 0) {
		Bullet bullet;
		bullet.rb.pos = physp.pos;
		bullet.rb.setOrientation(angle);
		bullet.rb.vel = physp.vel + complex_from_angle(angle) * 4.0f;

		game_state.bullets.push_back(bullet);
		shoot_cooldown = 5;
	}
	if (shoot_cooldown > 0) {
		--shoot_cooldown;
	}

	physp.update();
}
