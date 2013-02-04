#pragma once

#include "SpriteBuffer.hpp"

struct RenderState {
	SpriteBufferIndices sprite_buffer_indices;
	SpriteBuffer background_buffer;
	SpriteBuffer sprite_buffer;
	SpriteBuffer bullet_buffer;
	SpriteBuffer ui_buffer;
};
