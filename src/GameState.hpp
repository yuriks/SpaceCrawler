#pragma once
#include "util.hpp"
#include "game_types.hpp"

#include "Ship.hpp"

struct GameState {
	RandomGenerator rng;
	InputButtons::Bitset input;

	Ship player_ship;
};
