#include "Bullet.hpp"

#include "render/SpriteBuffer.hpp"
#include "Camera.hpp"
#undef min
#include <algorithm>

void Bullet::draw(SpriteBuffer& sprite_buffer, const Camera& camera) const {
	Sprite bullet_spr;
	bullet_spr.img = img;
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
