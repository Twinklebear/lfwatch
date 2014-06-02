#include <iostream>
#include <SDL.h>
#include "lfwatch.h"

std::string notify_string(unsigned mask){
	std::string msg;
	if (mask & lfw::Notify::CHANGE_FILE_NAME){
		msg += "CHANGE_FILE_NAME ";
	}
	if (mask & lfw::Notify::CHANGE_DIR_NAME){
		msg += "CHANGE_DIR_NAME ";
	}
	if (mask & lfw::Notify::CHANGE_ATTRIBUTES){
		msg += "CHANGE_ATTRIBUTES ";
	}
	if (mask & lfw::Notify::CHANGE_LAST_WRITE){
		msg += "CHANGE_LAST_WRITE ";
	}
	if (mask & lfw::Notify::CHANGE_LAST_ACCESS){
		msg += "CHANGE_LAST_ACCESS ";
	}
	return msg;
}

int main(int argc, char **argv){
	if (argc < 3){
		std::cout << "Usage ./exe <dir1> <dir2>\n";
		return 1;
	}
	if (SDL_Init(SDL_INIT_EVENTS) != 0){
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	}
	lfw::Watcher watcher;
	watcher.watch(argv[1], lfw::Notify::CHANGE_LAST_WRITE);
	watcher.watch(argv[2], lfw::Notify::CHANGE_FILE_NAME);

	for (int i = 0; i < 5; ++i){
		SDL_Event e;
		while (SDL_PollEvent(&e)){
			if (e.type == lfw::Watcher::event()){
				lfw::EventData *data = static_cast<lfw::EventData*>(e.user.data1);
				std::cout << notify_string(data->event)
					<< " event in " << data->dir
					<< " on file: " << data->fname << "\n";
				delete data;
			}
		}
		std::cout << "Press enter to update watchers > ";
		std::cin.ignore();
		std::cout << "Updating\n";
		watcher.update();
		if (i == 2){
			std::cout << "Removing watch on " << argv[1] << "\n";
			watcher.remove(argv[1]);
		}
	}
	return 0;
}

