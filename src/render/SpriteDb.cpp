#include "SpriteDb.hpp"
#include <fstream>
#include <cassert>
#include "util/resources.hpp"

static std::string getNextCsvField(const std::string& str, std::string::size_type& cur_pos) {
	assert(cur_pos != std::string::npos);

	std::string::size_type start_pos = cur_pos;
	cur_pos = str.find(',', cur_pos);

	std::string::size_type field_len = std::string::npos;
	if (cur_pos != std::string::npos) {
		field_len = cur_pos - start_pos;
		++cur_pos; // Skip past ','
	}

	return str.substr(start_pos, field_len);
}

void SpriteDb::loadFromCsv(const std::string& filename) {
	std::fstream f(data_path + filename);
	assert(f); // TODO: ERROR_CHECK

	std::string line;
	while (std::getline(f, line)) {
		std::string::size_type pos = 0;

		std::string id = getNextCsvField(line, pos);

		IntRect r;
		r.x = std::stoi(getNextCsvField(line, pos));
		r.y = std::stoi(getNextCsvField(line, pos));
		r.w = std::stoi(getNextCsvField(line, pos));
		r.h = std::stoi(getNextCsvField(line, pos));

		sprite_db.insert(std::make_pair(id, r));
	}
}