#pragma once
#include "util.hpp"
#include "game_types.hpp"
#include <vector>

#include "Ship.hpp"
#include "Drone.hpp"
#include "Bullet.hpp"
#include "Camera.hpp"
#include "Debris.hpp"

struct GameState {
	RandomGenerator rng;
	InputButtons::Bitset input;
	int mouse_x, mouse_y;
	Camera camera;

	double frametime_min, frametime_avg, frametime_max, fps;

	Ship player_ship;
	std::vector<Drone> drones;
	std::vector<Bullet> bullets;
	std::vector<Debris> debris;
};
