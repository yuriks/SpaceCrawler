#include "Bullet.hpp"

#include "render/SpriteBuffer.hpp"
#include "Camera.hpp"
#include <algorithm>
#undef min

static const IntRect img_bullet = {1, 10, 13, 3};

void Bullet::draw(SpriteBuffer& sprite_buffer, const Camera& camera) const {
	Sprite bullet_spr;
	bullet_spr.img = img_bullet;
	bullet_spr.setPos(camera.transform(physp.pos));
	uint8_t alpha = std::min(life * 32, 255);
	bullet_spr.color = makeColor(alpha, alpha, alpha, alpha);

	SpriteMatrix bullet_matrix;
	bullet_matrix.loadIdentity().rotate(orientation);

	sprite_buffer.append(bullet_spr, bullet_matrix);
}

void Bullet::update() {
	--life;
	physp.update();
}