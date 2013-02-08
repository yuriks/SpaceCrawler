#include "Debris.hpp"

#include "SpriteBuffer.hpp"
#include "Camera.hpp"

void Debris::update() {
	--life;
	rb.update();
}

void Debris::draw(SpriteBuffer& sprite_buffer, const Camera& camera) const {
	Sprite spr;
	spr.img = img;
	spr.setPos(camera.transform(rb.pos));
	uint8_t alpha = std::min(life * 32, 255);
	spr.color = makeColor(alpha, alpha, alpha, alpha);

	SpriteMatrix bullet_matrix;
	bullet_matrix.loadIdentity().rotate(rb.orientation);

	sprite_buffer.append(spr, bullet_matrix);
}
