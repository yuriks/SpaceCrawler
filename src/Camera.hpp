#pragma once
#include "Position.hpp"
#include "vec2.hpp"

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;

struct Camera {
	Position pos;

	inline vec2 transform(const Position& obj_pos) const {
		return (obj_pos - pos) + mvec2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	}

	inline Position inverse_transform(const vec2 screen_pos) const {
		return pos + (screen_pos - mvec2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
	}
};