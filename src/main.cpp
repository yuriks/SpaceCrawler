#define GLFW_INCLUDE_GL3 1
#include <GL/glfw.h>
#include "GL3/gl3w.h"

#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include "util.hpp"
#include "Fixed.hpp"
#include "SpriteBuffer.hpp"
#include "vec2.hpp"
#include "graphics_init.hpp"
#include "texture.hpp"
#include "game_types.hpp"
#include "GameState.hpp"
#include "DrawState.hpp"
#include "debug_sprite.hpp"
#include "geometry.hpp"

struct FontInfo {
	char first_char;
	int img_x, img_y;
	int img_w, img_h;

	FontInfo(char first_char, int img_x, int img_y, int img_w, int img_h)
		: first_char(first_char), img_x(img_x), img_y(img_y), img_w(img_w), img_h(img_h)
	{ }
};

void drawText(int x, int y, const std::string& text, SpriteBuffer& buffer, const FontInfo& font)
{
	Sprite spr;
	spr.setPos(x, y);
	spr.setImg(font.img_x, font.img_y, font.img_w, font.img_h);

	for (char c : text) {
		spr.img_x = font.img_x + (c - font.first_char) * font.img_w;
		buffer.append(spr);
		spr.x += font.img_w;
	}
}

void drawScene(const GameState& game_state, DrawState& draw_state) {
	/* Draw scene */
	draw_state.sprite_buffer.clear();
	draw_state.bullet_buffer.clear();

	for (const Drone& drone : game_state.drones) {
		drone.draw(draw_state.sprite_buffer);
	}
	for (const Bullet& bullet : game_state.bullets) {
		bullet.draw(draw_state.bullet_buffer);
	}
	game_state.player_ship.draw(draw_state.sprite_buffer);

	/* Submit sprites */
	// More superfluous drawcalls to change the GPU into high-performance mode? Sure, why not.
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < 1000; ++i) {
		draw_state.sprite_buffer.draw(draw_state.sprite_buffer_indices);
	}

	glClear(GL_COLOR_BUFFER_BIT);
	draw_state.sprite_buffer.draw(draw_state.sprite_buffer_indices);
	draw_state.bullet_buffer.draw(draw_state.sprite_buffer_indices);
}

void updateScene(GameState& game_state) {
	InputButtons::Bitset input;
	input.set(InputButtons::LEFT, glfwGetKey(GLFW_KEY_LEFT) == GL_TRUE);
	input.set(InputButtons::RIGHT, glfwGetKey(GLFW_KEY_RIGHT) == GL_TRUE);
	input.set(InputButtons::THRUST, glfwGetKey(GLFW_KEY_UP) == GL_TRUE);
	input.set(InputButtons::BRAKE, glfwGetKey(GLFW_KEY_DOWN) == GL_TRUE);
	input.set(InputButtons::SHOOT, glfwGetKey('X') == GL_TRUE);

	game_state.player_ship.update(input, game_state);
	for (Bullet& bullet : game_state.bullets) {
		bullet.update();
	}
	for (Drone& drone : game_state.drones) {
		drone.update();
	}

	// Test bullet-drone collision
	for (Drone& drone : game_state.drones) {
		std::vector<Bullet>& bullets = game_state.bullets;
		for (auto i = bullets.begin(); i < bullets.end();) {
			vec2 rel_pos = drone.pos - i->pos;
			if (collideCircleRectangle(rel_pos, 8.0f, mvec2(13.0f / 2, 3.0f / 2), i->angle)) {
				i = bullets.erase(i);
			} else {
				++i;
			}
		}
	}
}

int main() {
	if (!initWindow(WINDOW_WIDTH, WINDOW_HEIGHT)) {
		std::cerr << "Failed to initialize window.\n";
		return 1;
	}

	{

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	gl::ShaderProgram shader_program = loadShaderProgram();
	glUseProgram(shader_program.name);

	GLuint u_view_matrix_location = glGetUniformLocation(shader_program.name, "u_view_matrix");
	GLfloat view_matrix[9] = {
		2.0f/WINDOW_WIDTH,  0.0f,              -1.0f,
		0.0f,              -2.0/WINDOW_HEIGHT,  1.0f,
		0.0f,               0.0f,               1.0f
	};
	glUniformMatrix3fv(u_view_matrix_location, 1, GL_TRUE, view_matrix);

	GLuint u_texture_location = glGetUniformLocation(shader_program.name, "u_texture");
	glUniform1i(u_texture_location, 0);

	CHECK_GL_ERROR;

	glActiveTexture(GL_TEXTURE0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	CHECK_GL_ERROR;

	initDebugSprites();

	DrawState draw_state;
	CHECK_GL_ERROR;
	draw_state.sprite_buffer.texture = loadTexture("ships.png");
	draw_state.bullet_buffer.texture = loadTexture("bullets.png");

	CHECK_GL_ERROR;

	///////////////////////////
	// Initialize game state //
	///////////////////////////
	GameState game_state;
	RandomGenerator& rng = game_state.rng;
	rng.seed(1235);

	{
		Ship& ship = game_state.player_ship;
		ship.init();
		ship.pos = mPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	}
	for (Drone& drone : game_state.drones) {
		drone.init(rng);
		drone.pos = mPosition(randRange(rng, 64, WINDOW_WIDTH - 64 - 1), randRange(rng, 64, WINDOW_HEIGHT - 64 - 1));
	}

	////////////////////
	// Main game loop //
	////////////////////
	bool running = true;
	double update_time = 0.0;
	static const double TIMESTEP = 1.0 / 60.0;
	double last_time = glfwGetTime();
	while (running) {
		double cur_time = glfwGetTime();
		update_time += cur_time - last_time;
		last_time = cur_time;

		while (update_time > 0.0) {
			updateScene(game_state);
			update_time -= TIMESTEP;
		}
		drawScene(game_state, draw_state);
		drawDebugSprites(draw_state.sprite_buffer_indices);

		glfwSwapBuffers();
		running = running && glfwGetWindowParam(GLFW_OPENED);
		running = running && glfwGetKey(GLFW_KEY_ESC) == GL_FALSE;

		CHECK_GL_ERROR;
	}

	deinitDebugSprites();

	}

	glfwCloseWindow();
	glfwTerminate();
}
