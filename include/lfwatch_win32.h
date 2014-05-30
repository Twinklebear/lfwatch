#ifdef _WIN32
#ifndef LFWATCH_WIN32
#define LFWATCH_WIN32

#include <string>
#include <array>
#include <map>
#include <windows.h>

namespace lfw {
//Possible file events that we can notify about
enum Notify { CHANGE_FILE_NAME = FILE_NOTIFY_CHANGE_FILE_NAME,
	CHANGE_DIR_NAME = FILE_NOTIFY_CHANGE_DIR_NAME,
	CHANGE_ATTRIBUTES = FILE_NOTIFY_CHANGE_ATTRIBUTES,
	CHANGE_SIZE = FILE_NOTIFY_CHANGE_SIZE,
	CHANGE_LAST_WRITE = FILE_NOTIFY_CHANGE_LAST_WRITE,
	CHANGE_LAST_ACCESS = FILE_NOTIFY_CHANGE_LAST_ACCESS,
	CHANGE_CREATION = FILE_NOTIFY_CHANGE_CREATION,
	CHANGE_SECURITY = FILE_NOTIFY_CHANGE_SECURITY
};
struct WatchData {
	OVERLAPPED overlapped;
	HANDLE dir_handle;
	//Buffer to store file notification info in
	std::array<char, 16 * 1024> info_buf;
	std::string dir_name;
	bool watch_subtree;
	DWORD filter;

	WatchData(HANDLE handle, const std::string &dir, bool watch_subtree,
		DWORD filter);
};

class WatchWin32 {
	//Active watches { dir_name, watcher info }
	std::map<std::string, WatchData> watchers;
	//Would we need to do something special for copying?
	//clone the handles or something?

public:
	~WatchWin32();
	/**
	 * Start watching some directory for file changes
	 * watch_subtree should be true if we want to watch
	 * all subdirectories as well. Filters is a set of
	 * the notify flags or'd together to watch for
	 */
	void watch(const std::string &dir, bool watch_subtree, unsigned filters);
	void remove(const std::string &dir);
	//Update watchers. I'd really like to put this on some background thread though
	void update();
};
}

#endif
#endif

