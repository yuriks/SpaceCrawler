#include "Ship.hpp"

#include "SpriteBuffer.hpp"
#include "GameState.hpp"
#include "Bullet.hpp"

void Ship::init() {
	angle = 0;
	vel = mvec2(0.0f, 0.0f);
	shoot_cooldown = 0;
}

void Ship::draw(SpriteBuffer& sprite_buffer) const {
	Sprite ship_spr;
	ship_spr.setImg(1, 1, 32, 24);
	ship_spr.setPos(pos_x.integer(), pos_y.integer());

	SpriteMatrix matrix;
	matrix.loadIdentity().rotate(angle);

	sprite_buffer.append(ship_spr, matrix);

	if (anim_flags.test(AnimationFlags::THRUST_FORWARD)) {
		ship_spr.setImg(1, 1+1*25, 32, 24);
		sprite_buffer.append(ship_spr, matrix);
	}

	if (anim_flags.test(AnimationFlags::INERTIAL_BRAKE)) {
		ship_spr.setImg(1, 1+2*25, 32, 24);
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
		vel = vel + accel;
		anim_flags.set(AnimationFlags::THRUST_FORWARD);
	} else if (input.test(InputButtons::BRAKE)) {
		vel = vel * 0.96f;
		anim_flags.set(AnimationFlags::INERTIAL_BRAKE);
	}

	if (input.test(InputButtons::SHOOT) && shoot_cooldown == 0) {
		Bullet bullet;
		bullet.pos_x = pos_x;
		bullet.pos_y = pos_y;
		bullet.angle = angle;
		bullet.vel = vel + complex_from_angle(angle) * 4.0f;

		game_state.bullets.push_back(bullet);
		shoot_cooldown = 5;
	}
	if (shoot_cooldown > 0) {
		--shoot_cooldown;
	}

	pos_x += PositionFixed(vel.x);
	pos_y += PositionFixed(vel.y);
}
