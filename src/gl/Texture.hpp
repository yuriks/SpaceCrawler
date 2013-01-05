#pragma once

#include <algorithm>
#include <boost/noncopyable.hpp>

#include "GL3/gl3w.h"

namespace gl {

struct Texture : private boost::noncopyable {
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
	}

	~Texture() {
		if (name != 0)
			glDeleteTextures(1, &name);
	}
};

} // namespace gl
