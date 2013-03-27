#include "stb_image.h"
#include "stb_image_write.h"
#include "render/Sprite.hpp"
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <fstream>
#include "util/csv.hpp"

// Algorithm from http://www.blackpawn.com/texts/lightmaps/default.html

struct StbiFreeDeleter {
	void operator()(void* p) {
		stbi_image_free(p);
	}
};

struct Image {
	std::unique_ptr<uint8_t, StbiFreeDeleter> data;
	int width, height;

	Image(const std::string& fname) :
		data(stbi_load(fname.c_str(), &width, &height, nullptr, 4))
	{}

	Image(Image&& o) :
		data(std::move(o.data)), width(o.width), height(o.height)
	{}

	Image& operator=(Image&& o) {
		data = std::move(o.data);
		width = o.width;
		height = o.height;
		return *this;
	}

	void copyRect(IntRect src_rect, uint8_t* dst, size_t dst_x, size_t dst_y, size_t dst_stride) const {
		const uint8_t* in_row = data.get();
		uint8_t* out_row = dst + dst_y*dst_stride + dst_x*4;
		for (int y = 0; y < src_rect.h; ++y) {
			std::copy_n(in_row, src_rect.w*4, out_row);
			in_row += width*4;
			out_row += dst_stride;
		}
	}
};

struct LoadedSprite {
	const Image* img;
	IntRect rect;
	std::string name;
};

struct TreeNode {
	std::unique_ptr<TreeNode> children[2];
	IntRect img_rectangle;
	const LoadedSprite* sprite;

	TreeNode(IntRect img_rectangle) : img_rectangle(img_rectangle), sprite(nullptr) {}
};

bool insertSprite(std::unique_ptr<TreeNode>& node, const LoadedSprite* sprite, int margin) {
	// Image leaf
	if (node->sprite != nullptr) {
		// Node occupied already
		return false;
	}

	// Non-leaf
	if (node->children[0]) {
		// If it has one children it must have both
		assert(node->children[1]);

		// Try to insert in a children
		return insertSprite(node->children[0], sprite, margin) || insertSprite(node->children[1], sprite, margin);
	}

	// Empty leaf
	IntRect& rect = node->img_rectangle;
	int spr_w = sprite->rect.w + margin;
	int spr_h = sprite->rect.h + margin;

	// Sprite doesn't fit
	if (spr_w > rect.w || spr_h > rect.h) {
		return false;
	}

	// Sprite fits exactly
	if (spr_w == rect.w && spr_h == rect.h) {
		node->sprite = sprite;
		return true;
	}

	// Else, cut node and insert
	int delta_w = rect.w - spr_w;
	int delta_h = rect.h - spr_h;

	IntRect rect_a = rect;
	IntRect rect_b = rect;
	if (delta_w > delta_h) {
		// Cut vertically
		rect_a.w  = spr_w;
		rect_b.x += spr_w;
		rect_b.w -= spr_w;
		assert(rect_a.w + rect_b.w == rect.w);

	} else {
		// Cut horizontally
		rect_a.h  = spr_h;
		rect_b.y += spr_h;
		rect_b.h -= spr_h;
		assert(rect_a.h + rect_b.h == rect.h);
	}

	node->children[0].reset(new TreeNode(rect_a));
	node->children[1].reset(new TreeNode(rect_b));
	return insertSprite(node->children[0], sprite, margin);
}

std::unique_ptr<TreeNode> packBoxes(const std::vector<LoadedSprite>& sprite_data, int width, int height, int margin) {
	std::unique_ptr<TreeNode> tree;
	{
		IntRect full_rect = {0, 0, width, height};
		tree.reset(new TreeNode(full_rect));
	}

	// Insert all sprites into tree
	for (auto& sprite : sprite_data) {
		if (!insertSprite(tree, &sprite, margin)) {
			// Failed to insert sprite
			return nullptr;
		}
	}

	return tree;
}

void writeNode(const TreeNode& node, uint8_t* img_data, int img_width, std::ostream& info_file) {
	if (node.sprite) {
		const LoadedSprite& spr = *node.sprite;
		const IntRect& dst_rect = node.img_rectangle;
		spr.img->copyRect(spr.rect, img_data, dst_rect.x, dst_rect.y, img_width*4);
		info_file << spr.name << ',' << dst_rect.x << ',' << dst_rect.y << ',' << dst_rect.w << ',' << dst_rect.h << '\n';
	} else if (node.children[0]) {
		assert(node.children[1]);
		writeNode(*node.children[0], img_data, img_width, info_file);
		writeNode(*node.children[1], img_data, img_width, info_file);
	}
}

bool writeAtlas(const TreeNode& tree, const std::string& atlas_filename, const std::string& info_filename) {
	int img_width = tree.img_rectangle.w;
	int img_height = tree.img_rectangle.h;

	std::ofstream info_file(info_filename);
	if (!info_file) {
		std::cerr << "Error opening " << info_filename << " for writing." << std::endl;
		return false;
	}

	std::vector<uint8_t> img_data(img_width * img_height * 4, 0);
	writeNode(tree, img_data.data(), img_width, info_file);

	return stbi_write_png(atlas_filename.c_str(), img_width, img_height, 4, img_data.data(), 0) != 0;
}

bool packSprites(
	const std::string& path_prefix,
	const std::vector<std::string>& sprite_list,
	const std::string& atlas_filename,
	const std::string& info_filename,
	int margin)
{
	if (sprite_list.empty()) {
		std::cerr << "No sprites given." << std::endl;
		return false;
	}

	// Load all sprites
	std::vector<Image> image_data;
	image_data.reserve(sprite_list.size());
	std::vector<LoadedSprite> sprite_data;
	sprite_data.reserve(sprite_data.size()); // A good minimum bound

	for (auto& line : sprite_list) {
		std::string::size_type cur_pos = 0;

		std::string id = getNextCsvField(line, cur_pos);

		{
			Image img(path_prefix + id + ".png");
			if (!img.data) {
				std::cerr << "Couldn't load " << id << ".png: " <<stbi_failure_reason() << std::endl;
				return false;
			}
			image_data.push_back(std::move(img));
		}

		LoadedSprite spr;
		IntRect& r = spr.rect;
		{
			IntRect spr_rect = {0, 0, image_data.back().width, image_data.back().height};
			r = spr_rect;
		}
		spr.img = &image_data.back();

		if (cur_pos == std::string::npos) {
			spr.name = id;
			sprite_data.push_back(std::move(spr));
		} else {
			// Read as spritesheet
			int i = 1;
			int sprsheet_w = r.w;
			int sprsheet_h = r.h;
			r.w = std::stoi(getNextCsvField(line, cur_pos));
			r.h = std::stoi(getNextCsvField(line, cur_pos));

			for (r.y = 0; r.y+r.h <= sprsheet_h; r.y += r.h) {
				for (r.x = 0; r.x+r.w <= sprsheet_w; r.x += r.w) {
					spr.name = id + std::to_string(i++);
					sprite_data.push_back(spr);
				}
			}
		}
	}

	// Sort sprites by largest area
	std::sort(sprite_data.begin(), sprite_data.end(), [](const LoadedSprite& a, const LoadedSprite& b) {
		return (a.rect.w * a.rect.h) > (b.rect.w * b.rect.h);
	});

	int best_width;
	int best_height;
	std::unique_ptr<TreeNode> best_tree;

	static const unsigned int MAX_OUTPUT_SIZE = 4096;

	// Try all combos of sizes
	for (int width = 8; width <= MAX_OUTPUT_SIZE; width *= 2) {
		for (int height = 8; height <= MAX_OUTPUT_SIZE; height *= 2) {
			if (best_tree) {
				unsigned int area = (unsigned int)width * (unsigned int)height;
				unsigned int best_area = (unsigned int)best_width * (unsigned int)best_height;
				int rectangleness = std::abs(width - height);
				int best_rectangleness = std::abs(best_width - best_height);
				// Reject if larger or less square than current best
				if (area > best_area || (area == best_area && rectangleness >= best_rectangleness)) {
					continue;
				}
			}

			std::unique_ptr<TreeNode> packed_tree = packBoxes(sprite_data, width, height, margin);
			if (packed_tree) {
				best_width = width;
				best_height = height;
				best_tree = std::move(packed_tree);
			}
		}
	}

	if (!best_tree) {
		std::cerr << "Sprites didn't fit within " << MAX_OUTPUT_SIZE << 'x' << MAX_OUTPUT_SIZE << " image." << std::endl;
		return false;
	}

	writeAtlas(*best_tree, atlas_filename, info_filename);

	return true;
}

bool processSpritePack(const std::string& path_prefix, const std::string& out_file) {
	std::ifstream f(path_prefix + "sprite_list.txt");
	if (!f) {
		std::cerr << "Unable to open " + path_prefix + "sprite_list.txt" << std::endl;
		return false;
	}

	std::vector<std::string> sprite_list;
	std::string line;
	while (std::getline(f, line)) {
		sprite_list.push_back(std::move(line));
	}

	return packSprites(path_prefix, sprite_list, out_file + ".png", out_file + ".csv", 1);
}
