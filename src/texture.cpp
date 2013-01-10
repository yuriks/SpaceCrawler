#include "texture.hpp"

#include "stb_image.h"
#include "gl/gl_1_5.h"
#include "resources.hpp"
#include <memory>
#include <cassert>

gl::Texture loadTexture(int* out_width, int* out_height, const std::string& filename, bool premultiply) {
	gl::Texture main_texture;
	glGenTextures(1, &main_texture.name);

	glBindTexture(GL_TEXTURE_2D, main_texture.name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int width, height, comp;
	auto data = std::unique_ptr<unsigned char[], void(*)(void*)>(
		stbi_load(filename.c_str(), &width, &height, &comp, 4), &stbi_image_free);
	if (data == nullptr)
		return gl::Texture();

	if (premultiply) {
		unsigned int size = width * height;

		for (unsigned int i = 0; i < size; ++i) {
			unsigned char alpha = data[i*4 + 3];
			for (unsigned int j = 0; j < 3; ++j) {
				data[i*4 + j] = data[i*4 + j] * alpha / 255;
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

	*out_width = width;
	*out_height = height;
	return main_texture;
}

TextureInfo loadTexture(const std::string& filename, bool premultiply) {
	TextureInfo tex;
	tex.handle = loadTexture(&tex.width, &tex.height, data_path + filename, premultiply);
	assert(tex.handle.name != 0);
	return tex;
}
