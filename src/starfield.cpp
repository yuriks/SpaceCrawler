#include "RenderState.hpp"
#include <limits>
#include "util/util.hpp"
#include <array>
#include "render/SpriteBuffer.hpp"
#include "objects/Camera.hpp"

static const int starfield_sector_size = 512/2;
static const int max_stars_per_sector = 32;

static void drawStarfieldSector(
	RenderState& draw_state, const uint32_t starfield_seed, const std::vector<IntRect>& star_types,
	int16_t sector_x, int16_t sector_y,
	int sector_x_off, int sector_y_off)
{
	uint32_t sector_xu = sector_x - std::numeric_limits<int16_t>::min() + 1;
	uint32_t sector_yu = sector_y - std::numeric_limits<int16_t>::min() + 1;

	uint32_t sector_seed = ((sector_xu << 16) | sector_yu) ^ starfield_seed;

	RandomGenerator star_rng(sector_seed);

	int num_stars = randRange(star_rng, max_stars_per_sector);

	Sprite star_spr;
	for (int i = 0; i < num_stars; ++i) {
		star_spr.img = star_types[randRange(star_rng, star_types.size() - 1)];
		star_spr.x = randRange(star_rng, starfield_sector_size) + randRange(star_rng, starfield_sector_size - 1) + sector_x_off - star_spr.img.w / 2;
		star_spr.y = randRange(star_rng, starfield_sector_size) + randRange(star_rng, starfield_sector_size - 1) + sector_y_off - star_spr.img.h / 2;

		draw_state.background_buffer.append(star_spr);
	}
}

void drawStarfield(RenderState& draw_state, const uint32_t starfield_seed, const vec2 offset, const std::vector<IntRect>& star_types) {
	int16_t sector_x =  static_cast<int>(offset.x) / starfield_sector_size;
	int sector_x_off = -static_cast<int>(offset.x) % starfield_sector_size;
	int16_t sector_y =  static_cast<int>(offset.y) / starfield_sector_size;
	int sector_y_off = -static_cast<int>(offset.y) % starfield_sector_size;

	static const int sectors_per_width = intDivRoundUp(WINDOW_WIDTH, starfield_sector_size) + 3;
	static const int sectors_per_height = intDivRoundUp(WINDOW_HEIGHT, starfield_sector_size) + 3;

	for (int x = -2; x < sectors_per_width - 1; ++x) {
		for (int y = -2; y < sectors_per_height - 1; ++y) {
			drawStarfieldSector(draw_state, starfield_seed, star_types,
					sector_x + x, sector_y + y,
					sector_x_off + starfield_sector_size * x, sector_y_off + starfield_sector_size * y);
		}
	}
}
