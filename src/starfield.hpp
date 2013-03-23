#pragma once

#include <cstdint>
#include "util/vec2.hpp"
struct RenderState;

void drawStarfield(RenderState& draw_state, const uint32_t starfield_seed, const vec2 offset);
