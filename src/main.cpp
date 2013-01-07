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

// Splits vector vel into components parallel and perpendicular to the normal
// of the plane n.
void splitVector(vec2 vel, vec2 n, vec2* out_par, vec2* out_perp) {
	vec2 par = dot(vel, n) * n;
	*out_par = par;
	*out_perp = vel - par;
}

#if 0
void collideBallWithBall(Gem& a, Gem& b) {
	vec2 dv = {(a.pos_x - b.pos_x).toFloat(), (a.pos_y - b.pos_y).toFloat()};
	float d_sqr = length_sqr(dv);

	if (d_sqr < (2*Gem::RADIUS)*(2*Gem::RADIUS)) {
		fixed16_16 rel_vel_x = a.vel_x - b.vel_x;
		fixed16_16 rel_vel_y = a.vel_y - b.vel_y;
		vec2 rel_vel = {rel_vel_x.toFloat(), rel_vel_y.toFloat()};
		float rel_speed_sqr = length_sqr(rel_vel);
		
		if (rel_speed_sqr >= Gem::MERGE_SPEED*Gem::MERGE_SPEED) {
			fixed32_0 two(2);
			a.pos_x = (a.pos_x + b.pos_x) / two;
			a.pos_y = (a.pos_y + b.pos_y) / two;

			a.vel_x = a.vel_x + b.vel_x;
			a.vel_y = a.vel_y + b.vel_y;

			a.score_value += b.score_value;
			a.score_value += a.score_value / 2;

			b.pos_x = -9999;
			b.pos_y = 9999;
			b.vel_x = b.vel_y = 0;
			b.score_value = 0;
		} else {
			float d = std::sqrt(d_sqr);
			float sz = Gem::RADIUS - d / 2.0f;

			vec2 normal = dv / d;
			fixed24_8 push_back_x(sz * normal.x);
			fixed24_8 push_back_y(sz * normal.y);

			a.pos_x += push_back_x;
			a.pos_y += push_back_y;
			b.pos_x -= push_back_x;
			b.pos_y -= push_back_y;

			vec2 a_par, a_perp;
			vec2 b_par, b_perp;

			vec2 a_vel = {a.vel_x.toFloat(), a.vel_y.toFloat()};
			vec2 b_vel = {b.vel_x.toFloat(), b.vel_y.toFloat()};
			splitVector(a_vel, normal, &a_par, &a_perp);
			splitVector(b_vel, -normal, &b_par, &b_perp);

			static const float friction = 1.0f;
			static const float bounce = 0.9f;

			float A = (1.0f + bounce) / 2.0f;
			float B = (1.0f - bounce) / 2.0f;

			a_vel = A*b_par + B*a_par + friction*a_perp;
			b_vel = A*a_par + B*b_par + friction*b_perp;

			a.vel_x = fixed16_16(a_vel.x);
			a.vel_y = fixed16_16(a_vel.y);

			b.vel_x = fixed16_16(b_vel.x);
			b.vel_y = fixed16_16(b_vel.y);
		}
	}
}
#endif

// Returns the nearest point in line segment a-b to point p.
vec2 pointLineSegmentNearestPoint(vec2 p, vec2 a, vec2 b) {
	// Taken from http://stackoverflow.com/a/1501725
	const float l2 = length_sqr(b - a);
	if (l2 == 0.0f) {
		return a;
	}

	const float t = dot(p - a, b - a) / l2;
	if (t < 0.0f) {
		return a;
	} else if (t > 1.0f) {
		return b;
	} else {
		return a + t * (b - a);
	}
}

#if 0
void collideBallWithPaddle(Gem& ball, const Paddle& paddle) {
	SpriteMatrix matrix = paddle.getSpriteMatrix();

	// Left sphere
	vec2 left = {-24, 0};
	// Right sphere
	vec2 right = {24, 0};

	matrix.transform(&left.x, &left.y);
	matrix.transform(&right.x, &right.y);

	static const int PADDLE_RADIUS = 8;

	fixed24_8 rel_ball_x = ball.pos_x - paddle.pos_x;
	fixed24_8 rel_ball_y = ball.pos_y - paddle.pos_y;
	vec2 rel_ball = {rel_ball_x.toFloat(), rel_ball_y.toFloat()};

	vec2 nearest_point = pointLineSegmentNearestPoint(rel_ball, left, right);
	vec2 penetration = rel_ball - nearest_point;
	float d_sqr = length_sqr(penetration);
	float r = PADDLE_RADIUS + Gem::RADIUS;
	if (d_sqr < r*r) {
		vec2 vel = {ball.vel_x.toFloat(), ball.vel_y.toFloat()};
		int score_addition = static_cast<int>(ball.score_value * (ball.vel_y.toFloat() / 128.f));
		ball.score_value = std::min(ball.score_value + std::max(score_addition, 0), Gem::MAX_VALUE);

		float d = std::sqrt(d_sqr);
		float sz = r - d;

		vec2 normal = penetration / d;
		fixed24_8 push_back_x(sz * normal.x);
		fixed24_8 push_back_y(sz * normal.y);

		ball.pos_x += push_back_x;
		ball.pos_y += push_back_y;

		vec2 par, perp;
		splitVector(vel, normal, &par, &perp);
		vel = perp - par;

		ball.vel_x = fixed16_16(vel.x);
		ball.vel_y = fixed16_16(vel.y);
	}
}
#endif

void hsvToRgb(float h, float s, float v, float* out_r, float* out_g, float* out_b) {
	// Taken from http://www.cs.rit.edu/~ncs/color/t_convert.html

	assert(h >= 0.0f && h < 360.f);
	assert(s >= 0.0f && s <= 1.0f);
	assert(v >= 0.0f && v <= 1.0f);

	h /= 60.0f;
	int i = static_cast<int>(h);
	float f = h - i;
	float p = v*(1.0f - s);
	float q = v*(1.0f - s*f);
	float t = v*(1.0f - s*(1.0f - f));

#define CASE(i, r, g, b) case i: *out_r = r; *out_g = g; *out_b = b; break
	switch (i) {
		CASE(0, v, t, p);
		CASE(1, q, v, p);
		CASE(2, p, v, t);
		CASE(3, p, q, v);
		CASE(4, t, p, v);
		CASE(5, v, p, q);
	default: assert(false);
	}
#undef CASE
}

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

	game_state.player_ship.draw(draw_state.sprite_buffer);

	/* Submit sprites */
	// More superfluous drawcalls to change the GPU into high-performance mode? Sure, why not.
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < 1000; ++i) {
		draw_state.sprite_buffer.draw(draw_state.sprite_buffer_indices);
	}

	glClear(GL_COLOR_BUFFER_BIT);
	draw_state.sprite_buffer.draw(draw_state.sprite_buffer_indices);
}

void updateScene(GameState& game_state) {
	InputButtons::Bitset input;
	input.set(InputButtons::LEFT, glfwGetKey(GLFW_KEY_LEFT) == GL_TRUE);
	input.set(InputButtons::RIGHT, glfwGetKey(GLFW_KEY_RIGHT) == GL_TRUE);
	input.set(InputButtons::THRUST, glfwGetKey(GLFW_KEY_UP) == GL_TRUE);
	input.set(InputButtons::BRAKE, glfwGetKey(GLFW_KEY_DOWN) == GL_TRUE);

	game_state.player_ship.update(input);
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
	{
		SpriteBuffer& sprite_buffer = draw_state.sprite_buffer;
		sprite_buffer.texture = loadTexture("player-ship.png");
	}

	CHECK_GL_ERROR;

	///////////////////////////
	// Initialize game state //
	///////////////////////////
	GameState game_state;
	RandomGenerator& rng = game_state.rng;
	rng.seed(123);

	{
		Ship& ship = game_state.player_ship;
		ship.angle = 0;
		ship.pos_x = WINDOW_WIDTH / 2;
		ship.pos_y = WINDOW_HEIGHT / 2;
		ship.vel = mvec2(0.0f, 0.0f);
	}

	////////////////////
	// Main game loop //
	////////////////////
	bool running = true;
	while (running) {
		updateScene(game_state);
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
