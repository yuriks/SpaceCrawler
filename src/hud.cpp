#include "hud.hpp"
#include "objects/GameState.hpp"
#include "RenderState.hpp"
#include "render/text.hpp"

FontInfo ui_font(' ', 8, 8, 0, 0, 16, 6);
const Color hud_color = {49, 209, 17, 0};

void initUiFont(const SpriteDb& sprite_db) {
	IntRect font_spr = sprite_db.lookup("ui_font");
	ui_font = FontInfo(' ', 8, 8, font_spr.x, font_spr.y, 16, 6);
}
