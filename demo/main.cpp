#include <iostream>
#include <functional>
#include "lfwatch.h"

std::string notify_string(unsigned mask){
	std::string msg;
	if (mask & lfw::Notify::CHANGE_FILE_NAME){
		msg += "CHANGE_FILE_NAME";
	}
	if (mask & lfw::Notify::CHANGE_DIR_NAME){
		msg += "CHANGE_DIR_NAME";
	}
	if (mask & lfw::Notify::CHANGE_ATTRIBUTES){
		msg += "CHANGE_ATTRIBUTES";
	}
	if (mask & lfw::Notify::CHANGE_LAST_WRITE){
		msg += "CHANGE_LAST_WRITE";
	}
	if (mask & lfw::Notify::CHANGE_LAST_ACCESS){
		msg += "CHANGE_LAST_ACCESS";
	}
	return msg;
}

int main(int argc, char **argv){
	if (argc < 3){
		std::cout << "Usage ./exe <dir1> <dir2>\n";
		return 1;
	}
	lfw::Watcher watcher;
	watcher.watch(argv[1], lfw::Notify::CHANGE_LAST_WRITE,
		[](const std::string &dir, const std::string &fname, unsigned mask){
			std::cout << notify_string(mask) << " event in " << dir
				<< " on file " << fname << "\n";
		});

	watcher.watch(argv[2], lfw::Notify::CHANGE_FILE_NAME,
		[](const std::string &dir, const std::string &fname, unsigned mask){
			std::cout << notify_string(mask) << " event in " << dir
				<< " on file " << fname << "\n";
		});

	for (int i = 0; i < 5; ++i){
		std::cout << "Press enter to update watches > ";
		std::cin.ignore();
		std::cout << "Updating\n";
		watcher.update();
		if (i == 2){
			std::cout << "Removing watch\n";
			watcher.remove(argv[1]);
		}
	}
	return 0;
}

