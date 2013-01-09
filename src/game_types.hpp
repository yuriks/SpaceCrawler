#pragma once
#include "Fixed.hpp"
#include <bitset>

struct InputButtons {
	enum Enum {
		LEFT,
		RIGHT,
		THRUST,
		BRAKE,
		SHOOT,
		MAX
	};

	typedef std::bitset<MAX> Bitset;
};

static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;
