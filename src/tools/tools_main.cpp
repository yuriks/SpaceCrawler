#include "tools_main.hpp"
#include "atlas_packer.hpp"
#include <iostream>

int tools_main(int argc, const char* argv[]) {
	if (argc == 0) {
		std::cout << "Nothing to do." << std::endl;
	}

	while (argc > 0) {
		const char* command = argv[0];
		
		if (strcmp(command, "--pack-sprites") == 0) {
			if (argc < 3) {
				std::cerr << "Usage: --pack-sprites <sprites_dir> <output_file_without_extension>" << std::endl;
				return 1;
			}
			if (!processSpritePack(argv[1], argv[2])) {
				return 2;
			}
			argv += 3;
			argc -= 3;
		} else {
			std::cerr << "Unknown command " << command << std::endl;
			return 1;
		}
	}

	return 0;
}