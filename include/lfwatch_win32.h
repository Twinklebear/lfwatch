#ifdef _WIN32

#ifndef LFWATCH_WIN32
#define LFWATCH_WIN32

#include <string>
#include <array>
#include <map>
#include <windows.h>
#include "events.h"

namespace lfw {
//Possible file events that we can notify about
enum Notify {
	FILE_MODIFIED = 1,
	FILE_CREATED = 2,
	FILE_REMOVED = 4,
	FILE_RENAMED_OLD_NAME = 8,
	FILE_RENAMED_NEW_NAME = 16
};

struct WatchData {
	OVERLAPPED overlapped;
	HANDLE dir_handle;
	//Buffer to store file notification info in
	std::array<char, 4 * 1024> info_buf;
	std::string dir_name;
	uint32_t filter;
	Callback callback;

	WatchData(HANDLE handle, const std::string &dir, uint32_t filter, const Callback &cb);
};

class WatchWin32 {
	//Active watches { dir_name, watcher info }
	std::map<std::string, WatchData> watchers;
	//Would we need to do something special for copying?
	//clone the handles or something?

public:
	WatchWin32();
	~WatchWin32();
	/*
	 * Start watching some directory for file changes
	 * Filters is a set of the notify flags or'd
	 * together to watch for
	 */
	void watch(const std::string &dir, uint32_t filters, const Callback &callback);
	void remove(const std::string &dir);
	//Update watchers. I'd really like to put this on some background thread though
	void update();

private:
	WatchWin32(const WatchWin32 &w){
		//TODO: Rule of 3
	}
	WatchWin32& operator=(const WatchWin32 &w){
		//TODO: Rule of 3
		return *this;
	}
};
}

#endif
#endif

