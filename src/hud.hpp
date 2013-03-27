#pragma once

struct FontInfo;
struct Color;
struct SpriteDb;

extern FontInfo ui_font;
extern const Color hud_color;

void initUiFont(const SpriteDb& sprite_db);
