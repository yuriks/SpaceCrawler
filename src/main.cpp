#include "gl/gl_1_5.h"
#include <GL/glfw.h>

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
#include "RenderState.hpp"
#include "debug_sprite.hpp"
#include "geometry.hpp"

void drawScene(const GameState& game_state, RenderState& draw_state) {
	/* Draw scene */
	draw_state.sprite_buffer.clear();
	draw_state.bullet_buffer.clear();

	for (const Drone& drone : game_state.drones) {
		drone.draw(draw_state.sprite_buffer, game_state.camera);
	}
	for (const Bullet& bullet : game_state.bullets) {
		bullet.draw(draw_state.bullet_buffer, game_state.camera);
	}
	game_state.player_ship.draw(draw_state.sprite_buffer, game_state.camera);

	/* Submit sprites */
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

	// Test bullet-drone collision and bullet lifetime
	remove_if(game_state.bullets, [&](const Bullet& bullet) -> bool {
		if (bullet.life == 0)
			return true;
		for (const Drone& drone : game_state.drones) {
			vec2 rel_pos = drone.rb.pos - bullet.rb.pos;
			if (collideCircleRectangle(rel_pos, 8.0f, mvec2(13.0f / 2, 3.0f / 2), bullet.angle)) {
				return true;
			}
		}
		return false;
	});

	game_state.camera.pos = game_state.player_ship.rb.pos;
}

int main() {
	if (!initWindow(WINDOW_WIDTH, WINDOW_HEIGHT)) {
		std::cerr << "Failed to initialize window.\n";
		return 1;
	}

	{

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	GLfloat view_matrix[16] = {
		2.0f/WINDOW_WIDTH,  0.0f,               0.0f, -1.0f,
		0.0f,              -2.0/WINDOW_HEIGHT,  0.0f, 1.0f,
		0.0f,               0.0f,               1.0f, 0.0f,
		0.0f,               0.0f,               0.0f, 1.0f
	};
	glLoadTransposeMatrixf(view_matrix);

	CHECK_GL_ERROR;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	CHECK_GL_ERROR;

	initDebugSprites();

	RenderState draw_state;
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
		ship.rb.pos = mPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	}
	for (Drone& drone : game_state.drones) {
		drone.init(rng);
		drone.rb.pos = mPosition(randRange(rng, 64, WINDOW_WIDTH - 64 - 1), randRange(rng, 64, WINDOW_HEIGHT - 64 - 1));
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
