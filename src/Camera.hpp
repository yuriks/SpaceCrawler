#pragma once
#include "Position.hpp"
#include "vec2.hpp"

static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;

struct Camera {
	Position pos;

	inline vec2 transform(const Position& obj_pos) const {
		return (obj_pos - pos) + mvec2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	}
};