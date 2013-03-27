#pragma once

#include "render/SpriteBuffer.hpp"
#include "render/SpriteDb.hpp"
#include <vector>

struct RenderState {
	SpriteBufferIndices sprite_buffer_indices;

	SpriteBuffer background_buffer;
	std::vector<IntRect> background_star_types;

	SpriteBuffer sprite_buffer;
	SpriteBuffer bullet_buffer;
	SpriteBuffer ui_buffer;

	SpriteDb sprite_db;
};
