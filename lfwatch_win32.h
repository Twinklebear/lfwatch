#ifdef _WIN32
#ifndef LFWATCH_WIN32
#define LFWATCH_WIN32

#include <string>
#include <array>
#include <map>
#include <windows.h>

namespace lfw {
struct WatchData {
	OVERLAPPED overlapped;
	HANDLE dir_handle;
	//Buffer to store file notification info in
	std::array<char, 16 * 1024> info_buf;
	std::string dir_name;
	bool watch_subtree;

	WatchData(HANDLE handle, const std::string &dir, bool watch_subtree);
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
	 * all subdirectories as well
	 */
	void watch_dir(const std::string &dir, bool watch_subtree);
	void remove_watch(const std::string &dir);
	//Update watchers. I'd really like to put this on some background thread though
	void update();

private:
	void cancel(WatchData &watch);
};
}

#endif
#endif

