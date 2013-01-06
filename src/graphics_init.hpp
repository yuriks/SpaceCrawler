#pragma once

#include <cassert>
#include "GL3/gl3w.h"
#include "gl/Shader.hpp"
#include "gl/ShaderProgram.hpp"

#define CHECK_GL_ERROR assert(glGetError() == GL_NO_ERROR)
#ifdef _DEBUG
#define CHECK_GL_ERROR_PARANOID assert(glGetError() == GL_NO_ERROR)
#else
#define CHECK_GL_ERROR_PARANOID
#endif

gl::Shader loadShader(const char* shader_src, GLenum shader_type);
gl::ShaderProgram loadShaderProgram();
bool initWindow(int width, int height);
