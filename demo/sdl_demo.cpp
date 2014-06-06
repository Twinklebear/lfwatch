#include <iostream>
#include <SDL.h>
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
uint32_t event;
void push_event(const std::string &f){
	SDL_Event evt = { 0 };
	evt.type = event;
	evt.user.data1 = new std::string{f};
	SDL_PushEvent(&evt);
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
	event = SDL_RegisterEvents(1);
	lfw::Watcher watcher;
	watcher.watch(argv[1], lfw::Notify::FILE_MODIFIED,
		[](lfw::EventData e){
			std::cout << notify_string(e.event) << " event in "
				<< e.dir << " on file " << e.fname << "\n";
			push_event(e.fname);
		});

	watcher.watch(argv[2], lfw::Notify::FILE_CREATED | lfw::Notify::FILE_REMOVED
			| lfw::Notify::FILE_RENAMED_OLD_NAME | lfw::Notify::FILE_RENAMED_NEW_NAME,
		[](lfw::EventData e){
			std::cout << notify_string(e.event) << " event in "
				<< e.dir << " on file " << e.fname << "\n";
			push_event(e.fname);
		});

	for (int i = 0; i < 5; ++i){
		SDL_Event e;
		while (SDL_PollEvent(&e)){
			if (e.type == event){
				std::string *f = static_cast<std::string*>(e.user.data1);
				std::cout << "Got event on file: " << *f << std::endl;
				delete f;
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

