#pragma once

#include <algorithm>
#include "GL3/gl3w.h"
#include "util.hpp"

namespace gl {

struct Texture {
	GLuint name;

	Texture()
		: name(0)
	{ }

	explicit Texture(GLuint name)
		: name(name)
	{ }

	Texture(Texture&& o)
		: name(o.name)
	{
		o.name = 0;
	}

	Texture& operator=(Texture&& o) {
		std::swap(name, o.name);
		return *this;
	}

	~Texture() {
		if (name != 0)
			glDeleteTextures(1, &name);
	}

private:
	NONCOPYABLE(Texture);
};

} // namespace gl
