#include <iostream>
#include "lfwatch.h"

int main(int argc, char **argv){
	if (argc < 3){
		std::cout << "Usage ./exe <dir1> <dir2>\n";
		return 1;
	}
	lfw::Watcher watcher;
	watcher.watch(argv[1], lfw::Notify::CHANGE_LAST_WRITE);
	watcher.watch(argv[2], lfw::Notify::CHANGE_FILE_NAME);
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

