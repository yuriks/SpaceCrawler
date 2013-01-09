#include "Bullet.hpp"

#include "SpriteBuffer.hpp"

void Bullet::draw(SpriteBuffer& sprite_buffer) const {
	Sprite bullet_spr;
	bullet_spr.setImg(1, 10, 13, 3);
	bullet_spr.setPos(pos.x.integer(), pos.y.integer());

	SpriteMatrix bullet_matrix;
	bullet_matrix.loadIdentity().rotate(angle);

	sprite_buffer.append(bullet_spr, bullet_matrix);
}

void Bullet::update() {
	pos += vel;
}
