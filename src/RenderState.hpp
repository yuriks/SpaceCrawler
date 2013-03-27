#pragma once

#include "render/SpriteBuffer.hpp"
#include "render/SpriteDb.hpp"

struct RenderState {
	SpriteBufferIndices sprite_buffer_indices;
	SpriteBuffer background_buffer;
	SpriteBuffer sprite_buffer;
	SpriteBuffer bullet_buffer;
	SpriteBuffer ui_buffer;

	SpriteDb sprite_db;
};
