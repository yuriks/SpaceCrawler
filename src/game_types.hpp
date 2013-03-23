#pragma once
#include "util/Fixed.hpp"
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
