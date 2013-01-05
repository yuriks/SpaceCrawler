#pragma

#define GLFW_INCLUDE_GL3 1
#include <GL/glfw.h>
#include "GL3/gl3w.h"

#include <cassert>
#include "gl/Texture.hpp"
#include "gl/Shader.hpp"
#include "gl/ShaderProgram.hpp"

#define CHECK_GL_ERROR assert(glGetError() == GL_NO_ERROR)

gl::Texture loadTexture(int* out_width, int* out_height, const char* filename, bool premultiply = true);
gl::Shader loadShader(const char* shader_src, GLenum shader_type);
gl::ShaderProgram loadShaderProgram();
bool initWindow(int width, int height);
