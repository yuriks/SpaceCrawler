#pragma once

#include "SpriteBuffer.hpp"

struct DrawState {
	SpriteBufferIndices sprite_buffer_indices;
	SpriteBuffer sprite_buffer;
	SpriteBuffer bullet_buffer;
};
