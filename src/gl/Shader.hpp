#pragma once

#include <algorithm>
#include "GL3/gl3w.h"
#include "util.hpp"

namespace gl {

struct Shader {
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

private:
	NONCOPYABLE(Shader);
};

} // namespace gl
