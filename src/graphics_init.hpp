#pragma once

#include <cassert>
#include "gl/gl_1_5.h"

#define CHECK_GL_ERROR assert(glGetError() == GL_NO_ERROR)
#ifdef _DEBUG
#define CHECK_GL_ERROR_PARANOID assert(glGetError() == GL_NO_ERROR)
#else
#define CHECK_GL_ERROR_PARANOID
#endif

bool initWindow(int width, int height);
