#pragma once
#include "Fixed.hpp"
#include <bitset>

typedef fixed24_8 PositionFixed;

namespace InputButtons {
	enum Enum {
		LEFT,
		RIGHT,
		THRUST,
		BRAKE,
		MAX
	};

	typedef std::bitset<InputButtons::MAX> Bitset;
}

static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;
