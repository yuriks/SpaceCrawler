#include "graphics_init.hpp"

#include "stb_image.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <memory>

gl::Texture loadTexture(int* out_width, int* out_height, const char* filename, bool premultiply) {
	gl::Texture main_texture;
	glGenTextures(1, &main_texture.name);

	glBindTexture(GL_TEXTURE_2D, main_texture.name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int width, height, comp;
	auto data = std::unique_ptr<unsigned char[], void(*)(void*)>(
		stbi_load(filename, &width, &height, &comp, 4), &stbi_image_free);
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

gl::Shader loadShader(const char* shader_src, GLenum shader_type) {
	gl::Shader shader(glCreateShader(shader_type));
	glShaderSource(shader.name, 1, &shader_src, nullptr);
	glCompileShader(shader.name);

	GLint compile_result;
	glGetShaderiv(shader.name, GL_COMPILE_STATUS, &compile_result);

	if (compile_result == GL_FALSE) {
		GLint log_size;
		glGetShaderiv(shader.name, GL_INFO_LOG_LENGTH, &log_size);

		std::vector<char> log_buf(log_size);
		glGetShaderInfoLog(shader.name, log_buf.size(), nullptr, log_buf.data());

		std::cerr << "Error compiling shader:\n";
		std::cerr << log_buf.data() << std::flush;

		return gl::Shader();
	}

	return shader;
}

std::string loadTextFile(const std::string& filename) {
	std::string ret;
	
	std::ifstream f(filename);
	if (f) {
		f.seekg(0, std::ios::end);
		ret.resize(static_cast<size_t>(f.tellg()));
		f.seekg(0);
		f.read(&ret[0], ret.size());
	}

	return ret;
}

gl::ShaderProgram loadShaderProgram() {
	std::string vertex_shader_src = loadTextFile("vertex_shader.glsl");
	assert(!vertex_shader_src.empty());
	std::string fragment_shader_src = loadTextFile("fragment_shader.glsl");
	assert(!fragment_shader_src.empty());

	gl::Shader vertex_shader(loadShader(vertex_shader_src.c_str(), GL_VERTEX_SHADER));
	assert(vertex_shader.name != 0);
	gl::Shader fragment_shader(loadShader(fragment_shader_src.c_str(), GL_FRAGMENT_SHADER));
	assert(fragment_shader.name != 0);

	gl::ShaderProgram program(glCreateProgram());
	glAttachShader(program.name, vertex_shader.name);
	glAttachShader(program.name, fragment_shader.name);
	glLinkProgram(program.name);

	GLint link_result;
	glGetProgramiv(program.name, GL_LINK_STATUS, &link_result);

	if (link_result == GL_FALSE) {
		GLint log_size;
		glGetProgramiv(program.name, GL_INFO_LOG_LENGTH, &log_size);

		std::vector<char> log_buf(log_size);
		glGetProgramInfoLog(program.name, log_buf.size(), nullptr, log_buf.data());

		std::cerr << "Error linking shader program:\n";
		std::cerr << log_buf.data() << std::flush;

		return gl::ShaderProgram();
	}

	return program;
}

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	if ((type != GL_DEBUG_TYPE_PERFORMANCE_ARB && type != GL_DEBUG_TYPE_OTHER_ARB) || severity != GL_DEBUG_SEVERITY_LOW_ARB)
		std::cerr << message << std::endl;
	if ((type != GL_DEBUG_TYPE_PERFORMANCE_ARB && type != GL_DEBUG_TYPE_OTHER_ARB) || severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		__debugbreak(); // Breakpoint
}

bool initWindow(int width, int height) {
	if (!glfwInit()) {
		std::cerr << "Couldn't init GLFW.\n";
		return false;
	}

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	if (!glfwOpenWindow(width, height, 8, 8, 8, 0, 0, 8, GLFW_WINDOW)) {
		std::cerr << "Couldn't open window.\n";
		return false;
	}

	if (gl3wInit()) {
		std::cerr << "Couldn't init gl3w.\n";
		return false;
	}

	if (!gl3wIsSupported(3, 3)) {
		std::cerr << "OpenGL 3.3 not supported.\n";
		return false;
	}

	glfwSwapInterval(1);
	glViewport(0, 0, width, height);

	/*
	if (glDebugMessageCallbackARB) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
		glDebugMessageCallbackARB(debug_callback, 0);
	}
	*/

	return true;
}
