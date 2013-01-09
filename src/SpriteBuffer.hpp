#pragma once
#include "GL3/gl3.h"
#include <vector>
#include <cstdint>
#include <array>
#include "graphics_init.hpp"
#include "texture.hpp"
#include "gl/Buffer.hpp"
#include "gl/VertexArray.hpp"
#include "vec2.hpp"

struct VertexData {
	GLfloat pos_x, pos_y;
	GLfloat tex_s, tex_t;
	std::array<GLubyte, 4> color;

	static void setupVertexAttribs();
};

typedef std::array<uint8_t, 4> Color;
inline Color makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	Color c = {{r, g, b, a}};
	return c;
}

struct Sprite {
	int x, y;
	int img_x, img_y;
	int img_h, img_w;
	Color color;

	Sprite() {
		const Color white = {255, 255, 255, 255};
		color = white;
	}

	void setImg(int x, int y, int w, int h) {
		img_x = x; img_y = y;
		img_w = w; img_h = h;
	}

	void setPos(int x_, int y_) {
		x = x_; y = y_;
	}

	void setPos(const vec2 v) {
		x = static_cast<int>(v.x);
		y = static_cast<int>(v.y);
	}
};

struct SpriteMatrix {
	GLfloat m[4]; // Row-major storage

	SpriteMatrix& loadIdentity();
	SpriteMatrix& multiply(const SpriteMatrix& l);
	SpriteMatrix& rotate(float degrees);
	SpriteMatrix& scale(float x, float y);
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
	gl::VertexArray vao;
	TextureInfo texture;

	SpriteBuffer();

	void clear();
	void append(const Sprite& spr);
	// Careful: spr position gives center of sprite, not top-left
	void append(const Sprite& spr, const SpriteMatrix& matrix);

	void draw(SpriteBufferIndices& indices) const;
};
