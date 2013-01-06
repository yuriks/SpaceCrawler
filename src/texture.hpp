#pragma once

#include "gl/Texture.hpp"

struct TextureInfo {
	gl::Texture handle;
	int width, height;

	TextureInfo() { }

	TextureInfo(TextureInfo&& o)
		: handle(std::move(o.handle)), width(o.width), height(o.height)
	{ }

	TextureInfo& operator =(TextureInfo&& o) {
		handle = std::move(o.handle);
		width = o.width;
		height = o.height;
		return *this;
	}

private:
	NONCOPYABLE(TextureInfo);
};

gl::Texture loadTexture(int* out_width, int* out_height, const char* filename, bool premultiply = true);
TextureInfo loadTexture(const char* filename, bool premultiply = true);
