#pragma once

#include <cstdint>
#include "util/vec2.hpp"
#include <vector>
struct RenderState;

void drawStarfield(RenderState& draw_state, const uint32_t starfield_seed, const vec2 offset, const std::vector<IntRect>& star_types);
