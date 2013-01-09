#pragma once
#include "util.hpp"
#include "game_types.hpp"
#include <array>
#include <vector>

#include "Ship.hpp"
#include "Drone.hpp"
#include "Bullet.hpp"

struct GameState {
	RandomGenerator rng;
	InputButtons::Bitset input;

	Ship player_ship;
	std::array<Drone, 8> drones;
	std::vector<Bullet> bullets;
};
