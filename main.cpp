#include <clocale>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstring>
#include <windows.h>
#include "lfwatch_win32.h"

int main(int argc, char **argv){
	if (argc < 3){
		std::cout << "Usage ./exe <dir1> <dir2>\n";
		return 1;
	}
	lfw::WatchWin32 watcher;
	watcher.watch(argv[1], false);
	watcher.watch(argv[2], false);
	for (int i = 0; i < 5; ++i){
		std::cout << "Sleeping\n";
		SleepEx(2000, true);
		std::cout << "Updating\n";
		watcher.update();
		if (i == 2){
			std::cout << "Removing watch\n";
			watcher.remove(argv[1]);
		}
	}
	return 0;
}

