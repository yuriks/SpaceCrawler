#include "Bullet.hpp"

#include "SpriteBuffer.hpp"
#include "Camera.hpp"

void Bullet::draw(SpriteBuffer& sprite_buffer, const Camera& camera) const {
	Sprite bullet_spr;
	bullet_spr.setImg(1, 10, 13, 3);
	bullet_spr.setPos(camera.transform(rb.pos));

	SpriteMatrix bullet_matrix;
	bullet_matrix.loadIdentity().rotate(angle);

	sprite_buffer.append(bullet_spr, bullet_matrix);
}

void Bullet::update() {
	rb.update();
}
