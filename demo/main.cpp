#include <iostream>
#include <functional>
#include "lfwatch.h"

std::string notify_string(uint32_t mask){
	std::string msg;
	if (mask & lfw::Notify::FILE_MODIFIED){
		msg += "FILE_MODIFIED ";
	}
	if (mask & lfw::Notify::FILE_CREATED){
		msg += "FILE_CREATED ";
	}
	if (mask & lfw::Notify::FILE_REMOVED){
		msg += "FILE_REMOVED ";
	}
	if (mask & lfw::Notify::FILE_RENAMED_OLD_NAME){
		msg += "FILE_RENAMED_OLD_NAME";
	}
	if (mask & lfw::Notify::FILE_RENAMED_NEW_NAME){
		msg += "FILE_RENAMED_NEW_NAME";
	}
	return msg;
}

int main(int argc, char **argv){
	if (argc < 3){
		std::cout << "Usage ./exe <dir1> <dir2>\n";
		return 1;
	}
	lfw::Watcher watcher;
	watcher.watch(argv[1], lfw::Notify::FILE_MODIFIED,
		[](lfw::EventData e){
			std::cout << notify_string(e.event) << " event in "
				<< e.dir << " on file " << e.fname << "\n";
		});

	watcher.watch(argv[2], lfw::Notify::FILE_CREATED | lfw::Notify::FILE_REMOVED
			| lfw::Notify::FILE_RENAMED_OLD_NAME | lfw::Notify::FILE_RENAMED_NEW_NAME,
		[](lfw::EventData e){
			std::cout << notify_string(e.event) << " event in "
				<< e.dir << " on file " << e.fname << "\n";
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

