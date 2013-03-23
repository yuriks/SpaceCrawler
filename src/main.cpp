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
#include <sstream>
#include <ios>
#include <iomanip>
#include <numeric>
#include "util/util.hpp"
#include "util/Fixed.hpp"
#include "render/SpriteBuffer.hpp"
#include "util/vec2.hpp"
#include "render/graphics_init.hpp"
#include "render/texture.hpp"
#include "game_types.hpp"
#include "objects/GameState.hpp"
#include "RenderState.hpp"
#include "render/debug_sprite.hpp"
#include "util/geometry.hpp"
#include "render/text.hpp"
#include "starfield.hpp"
#include "hud.hpp"

std::string formatFrametimeFloat(double x) {
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(3) << x;
	return ss.str();
}

void drawScene(const GameState& game_state, RenderState& draw_state) {
	/* Draw scene */
	draw_state.background_buffer.clear();
	draw_state.sprite_buffer.clear();
	draw_state.bullet_buffer.clear();
	draw_state.ui_buffer.clear();

	static const float starfield_parallax = 1.0f / 32.0f;
	for (int i = 0; i < 4; ++i) {
		vec2 starfield_pos = -game_state.camera.transform(mPosition(0, 0)) * starfield_parallax * (float)i;
		drawStarfield(draw_state, i*10, starfield_pos);
	}

	for (const Drone& drone : game_state.drones) {
		drone.draw(draw_state.sprite_buffer, draw_state.ui_buffer, game_state.camera);
	}
	for (const Debris& debris : game_state.debris) {
		debris.draw(draw_state.sprite_buffer, game_state.camera);
	}
	for (const Bullet& bullet : game_state.bullets) {
		bullet.draw(draw_state.bullet_buffer, game_state.camera);
	}
	game_state.player_ship.draw(draw_state.sprite_buffer, game_state.camera);

	/* Draw FPS */
	{
		const std::string fps_text = "FPS: " + formatFrametimeFloat(game_state.fps);
		const std::string min_text = "MIN: " + formatFrametimeFloat(game_state.frametime_min * 1000.0f);
		const std::string avg_text = "AVG: " + formatFrametimeFloat(game_state.frametime_avg * 1000.0f);
		const std::string max_text = "MAX: " + formatFrametimeFloat(game_state.frametime_max * 1000.0f);

		drawString(WINDOW_WIDTH, 0*8, fps_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
		drawString(WINDOW_WIDTH, 1*8, min_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
		drawString(WINDOW_WIDTH, 2*8, avg_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
		drawString(WINDOW_WIDTH, 3*8, max_text, draw_state.ui_buffer, ui_font, TextAlignment::right, color_white);
	}

	Sprite mouse_cursor;
	mouse_cursor.setImg(10, 48, 11, 11);
	mouse_cursor.color = hud_color;
	mouse_cursor.setPos(game_state.mouse_x - 5, game_state.mouse_y - 5);
	draw_state.ui_buffer.append(mouse_cursor);

	/* Submit sprites */
	glClear(GL_COLOR_BUFFER_BIT);
	draw_state.background_buffer.draw(draw_state.sprite_buffer_indices);
	draw_state.sprite_buffer.draw(draw_state.sprite_buffer_indices);
	draw_state.bullet_buffer.draw(draw_state.sprite_buffer_indices);
	draw_state.ui_buffer.draw(draw_state.sprite_buffer_indices);
}

void updateScene(GameState& game_state) {
	InputButtons::Bitset& input = game_state.input;
	input.set(InputButtons::LEFT, glfwGetKey(GLFW_KEY_LEFT) == GL_TRUE);
	input.set(InputButtons::RIGHT, glfwGetKey(GLFW_KEY_RIGHT) == GL_TRUE);
	input.set(InputButtons::THRUST, glfwGetKey(GLFW_KEY_UP) || glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT));
	input.set(InputButtons::BRAKE, glfwGetKey(GLFW_KEY_DOWN) || glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT));
	input.set(InputButtons::SHOOT, glfwGetKey('X') || glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE));
	glfwGetMousePos(&game_state.mouse_x, &game_state.mouse_y);
	game_state.mouse_x = clamp(0, game_state.mouse_x, WINDOW_WIDTH-1);
	game_state.mouse_y = clamp(0, game_state.mouse_y, WINDOW_HEIGHT-1);
	glfwSetMousePos(game_state.mouse_x, game_state.mouse_y);


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
		for (Drone& drone : game_state.drones) {
			vec2 rel_pos = drone.rb.pos - bullet.physp.pos;
			float drone_radius = drone.shield.cur_level > 0 ? drone.shield.shield_radius : 8.0f;
			if (collideCircleRectangle(rel_pos, drone_radius, mvec2(13.0f / 2, 3.0f / 2), bullet.orientation)) {
				drone.getHit(1, -rel_pos);
				return true;
			}
		}
		return false;
	});

	// Kill drones with no health
	remove_if(game_state.drones, [&](const Drone& drone) -> bool {
		if (drone.current_hull > 0)
			return false;

		drone.spawnDebris(game_state.debris, game_state.rng);
		return true;
	});

	for (Debris& debris : game_state.debris) {
		debris.update();
	}
	remove_if(game_state.debris, [&](const Debris& debris) -> bool {
		return debris.life == 0;
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
	draw_state.background_buffer.texture = loadTexture("background.png");
	draw_state.sprite_buffer.texture = loadTexture("ships.png");
	draw_state.bullet_buffer.texture = loadTexture("bullets.png");
	draw_state.ui_buffer.texture = loadTexture("ui.png");

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

	game_state.drones.resize(24);
	for (Drone& drone : game_state.drones) {
		drone.init(rng);
		drone.rb.pos = mPosition(
			randRange(rng, -WINDOW_WIDTH,  2 * WINDOW_WIDTH  - 1),
			randRange(rng, -WINDOW_HEIGHT, 2 * WINDOW_HEIGHT - 1));
	}

	////////////////////
	// Main game loop //
	////////////////////
	std::array<double, 60> frametimes;
	unsigned int frametimes_pos = 0;
	frametimes.fill(1.0 / 60.0);

	bool running = true;
	double update_time = 0.0;
	static const double TIMESTEP = 1.0 / 60.0;
	double last_time = glfwGetTime();
	while (running) {
		double cur_time = glfwGetTime();
		const double frame_time = cur_time - last_time;
		update_time += frame_time;
		last_time = cur_time;

		frametimes[frametimes_pos] = frame_time;
		if (++frametimes_pos >= frametimes.size()) frametimes_pos = 0;

		while (update_time > 0.0) {
			updateScene(game_state);
			update_time -= TIMESTEP;
		}

		auto frametimes_minmax = std::minmax_element(frametimes.cbegin(), frametimes.cend());
		game_state.frametime_min = *frametimes_minmax.first;
		game_state.frametime_max = *frametimes_minmax.second;
		game_state.frametime_avg = std::accumulate(frametimes.cbegin(), frametimes.cend(), 0.0) / frametimes.size();
		game_state.fps = 1.f / game_state.frametime_avg;

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
