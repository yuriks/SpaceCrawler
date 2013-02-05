#pragma once
#include "util.hpp"
#include "game_types.hpp"
#include <array>
#include <vector>

#include "Ship.hpp"
#include "Drone.hpp"
#include "Bullet.hpp"
#include "Camera.hpp"

struct GameState {
	RandomGenerator rng;
	InputButtons::Bitset input;
	Camera camera;

	double frametime_min, frametime_avg, frametime_max, fps;

	Ship player_ship;
	std::array<Drone, 24> drones;
	std::vector<Bullet> bullets;
};
