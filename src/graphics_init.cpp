#include "graphics_init.hpp"

#include "gl/gl_1_5.h"
#include <GL/glfw.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include "resources.hpp"

bool initWindow(int width, int height) {
	if (!glfwInit()) {
		std::cerr << "Couldn't init GLFW.\n";
		return false;
	}

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);
	//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	if (!glfwOpenWindow(width, height, 8, 8, 8, 0, 0, 8, GLFW_WINDOW)) {
		std::cerr << "Couldn't open window.\n";
		return false;
	}

	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED) {
		std::cerr << "Couldn't init OpenGL functions.\n";
		return false;
	}

	if (ogl_IsVersionGEQ(1, 5)) {
		std::cerr << "OpenGL 1.5 not supported.\n";
		return false;
	}

	glfwSwapInterval(1);
	glViewport(0, 0, width, height);

	return true;
}
