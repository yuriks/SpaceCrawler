#pragma once

#include <algorithm>
#include "GL3/gl3w.h"
#include "util.hpp"

namespace gl {

struct VertexArray {
	GLuint name;

	VertexArray()
		: name(0)
	{ }

	explicit VertexArray(GLuint name)
		: name(name)
	{ }

	VertexArray(VertexArray&& o)
		: name(o.name)
	{
		o.name = 0;
	}

	VertexArray& operator=(VertexArray&& o) {
		std::swap(name, o.name);
	}

	~VertexArray() {
		if (name != 0)
			glDeleteVertexArrays(1, &name);
	}

private:
	NONCOPYABLE(VertexArray);
};

} // namespace gl
