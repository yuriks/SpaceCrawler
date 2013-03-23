#pragma once

#include "gl/gl_1_5.h"
#include <vector>
#include <array>
#include "graphics_init.hpp"
#include "texture.hpp"
#include "gl/Buffer.hpp"
#include "Sprite.hpp"
#include "util/vec2.hpp"

struct VertexData {
	GLfloat pos_x, pos_y;
	GLfloat tex_s, tex_t;
	std::array<GLubyte, 4> color;

	static void setupVertexAttribs();
};

struct SpriteMatrix {
	GLfloat m[4]; // Row-major storage

	SpriteMatrix& loadIdentity();
	SpriteMatrix& multiply(const SpriteMatrix& l);
	SpriteMatrix& rotate(vec2 complex);
	SpriteMatrix& rotate(float radians) { return rotate(complex_from_angle(radians)); }
	SpriteMatrix& scale(float x, float y);
	SpriteMatrix& scale(float s) { return scale(s, s); }
	SpriteMatrix& shear(float x, float y);

	void transform(float* x, float* y);
};

struct SpriteBufferIndices {
	std::vector<GLushort> indices;
	unsigned int index_count;

	gl::Buffer ibo;

	SpriteBufferIndices();
	void update(unsigned int sprite_count);
};

struct SpriteBuffer {
	std::vector<VertexData> vertices;

	unsigned int sprite_count;
	gl::Buffer vbo;
	TextureInfo texture;

	SpriteBuffer();

	void clear();
	void append(const Sprite& spr);
	// Careful: spr position gives center of sprite, not top-left
	void append(const Sprite& spr, const SpriteMatrix& matrix);

	void draw(SpriteBufferIndices& indices) const;
};
