#pragma once

#include <string>

struct SpriteBuffer;

struct FontInfo {
	char first_char;
	int img_x, img_y;
	int img_w, img_h;

	FontInfo(char first_char, int img_x, int img_y, int img_w, int img_h)
		: first_char(first_char), img_x(img_x), img_y(img_y), img_w(img_w), img_h(img_h)
	{ }
};

enum class TextAlignment {
	left, right, center
};

int measureTextWidth(const std::string& text, const FontInfo& font);
void drawText(int x, int y, const std::string& text, SpriteBuffer& buffer, const FontInfo& font);
void drawText(int x, int y, const std::string& text, SpriteBuffer& buffer, const FontInfo& font, TextAlignment alignment);
