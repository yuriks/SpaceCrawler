#pragma once

#include <algorithm>
#include <boost/noncopyable.hpp>

#include "GL3/gl3w.h"

namespace gl {

struct Shader : private boost::noncopyable {
	GLuint name;

	Shader()
		: name(0)
	{ }

	explicit Shader(GLuint name)
		: name(name)
	{ }

	Shader(Shader&& o)
		: name(o.name)
	{
		o.name = 0;
	}

	Shader& operator=(Shader&& o) {
		std::swap(name, o.name);
	}

	~Shader() {
		if (name != 0)
			glDeleteShader(name);
	}
};

} // namespace gl
