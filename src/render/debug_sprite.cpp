#include "debug_sprite.hpp"
#include "SpriteBuffer.hpp"
#include "texture.hpp"
#include <memory>

std::unique_ptr<SpriteBuffer> debug_buffer;

void debugPoint(int x, int y) {
	Sprite spr;
	spr.color = makeColor(255, 0, 0, 255);
	spr.setImg(2, 2, 4, 4);
	spr.setPos(x - 2, y - 2);

	debug_buffer->append(spr);
}

void initDebugSprites() {
	debug_buffer.reset(new SpriteBuffer);
	debug_buffer->texture = loadTexture("debug.png");
}

void deinitDebugSprites() {
	debug_buffer.reset(nullptr);
}

void drawDebugSprites(SpriteBufferIndices& indices) {
	debug_buffer->draw(indices);
	debug_buffer->clear();
}