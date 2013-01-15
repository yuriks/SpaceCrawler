#include "text.hpp"
#include "SpriteBuffer.hpp"

int measureTextWidth(const std::string& text, const FontInfo& font) {
	return text.length() * font.img_w;
}

void drawText(int x, int y, const std::string& text, SpriteBuffer& buffer, const FontInfo& font) {
	Sprite spr;
	spr.setPos(x, y);
	spr.setImg(font.img_x, font.img_y, font.img_w, font.img_h);

	for (char c : text) {
		spr.img_x = font.img_x + (c - font.first_char) * font.img_w;
		buffer.append(spr);
		spr.x += font.img_w;
	}
}

void drawText(int x, int y, const std::string& text, SpriteBuffer& buffer, const FontInfo& font, TextAlignment alignment) {
	switch (alignment) {
	case TextAlignment::left:
		drawText(x, y, text, buffer, font);
		break;
	case TextAlignment::right:
		drawText(x - measureTextWidth(text, font), y, text, buffer, font);
		break;
	case TextAlignment::center:
		// The weird position dance is so that it rounds down instead of up.
		drawText((2*x - measureTextWidth(text, font)) / 2, y, text, buffer, font);
		break;
	}
}
