#pragma once

#include <algorithm>
#include "GL3/gl3w.h"
#include "util.hpp"

namespace gl {

struct ShaderProgram {
	GLuint name;

	ShaderProgram()
		: name(0)
	{ }

	explicit ShaderProgram(GLuint name)
		: name(name)
	{ }

	ShaderProgram(ShaderProgram&& o)
		: name(o.name)
	{
		o.name = 0;
	}

	ShaderProgram& operator=(ShaderProgram&& o) {
		std::swap(name, o.name);
	}

	~ShaderProgram() {
		if (name != 0)
			glDeleteProgram(name);
	}

private:
	NONCOPYABLE(ShaderProgram);
};

} // namespace gl
