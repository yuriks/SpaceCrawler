#pragma once

#include <unordered_map>
#include <string>
#include "Sprite.hpp"

struct SpriteDb {
	std::unordered_map<std::string, IntRect> sprite_db;

	IntRect lookup(const std::string& id) const { return sprite_db.at(id); }
	void loadFromCsv(const std::string& filename);
};