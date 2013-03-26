#include "stb_image.h"
#include "stb_image_write.h"
#include "render/Sprite.hpp"
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <fstream>

// Algorithm from http://www.blackpawn.com/texts/lightmaps/default.html

struct StbiFreeDeleter {
	void operator()(void* p) {
		stbi_image_free(p);
	}
};

struct LoadedSprite {
	std::unique_ptr<uint8_t, StbiFreeDeleter> img_data;
	int width, height;
	std::string name;

	LoadedSprite() {}

	LoadedSprite(LoadedSprite&& o) :
		img_data(std::move(o.img_data)), width(o.width), height(o.height), name(std::move(o.name))
	{}

	LoadedSprite& operator=(LoadedSprite&& o) {
		img_data = std::move(o.img_data);
		width = o.width;
		height = o.height;
		name = std::move(o.name);
		return *this;
	}
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
	int spr_w = sprite->width + margin;
	int spr_h = sprite->height + margin;

	// Sprite doesn't fit
	if (spr_w > rect.w || spr_h > rect.h) {
		return false;
	}

	// Sprite fits exactly
	if (spr_w == rect.w || spr_h == rect.h) {
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
		const IntRect& rect = node.img_rectangle;

		const uint8_t* in_row = spr.img_data.get();
		uint8_t* out_row = img_data + (rect.y*img_width + rect.x)*4;
		for (int y = 0; y < spr.height; ++y) {
			std::copy_n(in_row, spr.width*4, out_row);
			in_row += spr.width*4;
			out_row += img_width*4;
		}

		info_file << spr.name << ',' << rect.x << ',' << rect.y << ',' << rect.w << ',' << rect.h << '\n';
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
	std::vector<LoadedSprite> sprite_data;
	for (auto& file : sprite_list) {
		LoadedSprite spr;
		spr.img_data.reset(stbi_load((path_prefix + file + ".png").c_str(), &spr.width, &spr.height, nullptr, 4));
		if (!spr.img_data) {
			std::cerr << "Couldn't load " << file << ": " <<stbi_failure_reason() << std::endl;
			return false;
		}
		spr.name = file;
		sprite_data.push_back(std::move(spr));
	}

	// Sort sprites by largest area
	std::sort(sprite_data.begin(), sprite_data.end(), [](const LoadedSprite& a, const LoadedSprite& b) {
		return (a.width * a.height) > (b.width * b.height);
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
