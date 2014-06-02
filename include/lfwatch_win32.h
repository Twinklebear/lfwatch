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
enum Notify { CHANGE_FILE_NAME = FILE_NOTIFY_CHANGE_FILE_NAME,
	CHANGE_DIR_NAME = FILE_NOTIFY_CHANGE_DIR_NAME,
	CHANGE_ATTRIBUTES = FILE_NOTIFY_CHANGE_ATTRIBUTES,
	CHANGE_LAST_WRITE = FILE_NOTIFY_CHANGE_LAST_WRITE,
	CHANGE_LAST_ACCESS = FILE_NOTIFY_CHANGE_LAST_ACCESS,
};

struct WatchData {
	OVERLAPPED overlapped;
	HANDLE dir_handle;
	//Buffer to store file notification info in
	std::array<char, 4 * 1024> info_buf;
	std::string dir_name;
	uint32_t filter;
#ifdef NO_SDL
	Callback callback;

	WatchData(HANDLE handle, const std::string &dir, uint32_t filter, const Callback &cb);
#else
	WatchData(HANDLE handle, const std::string &dir, uint32_t filter);
#endif
};

class WatchWin32 {
	//Active watches { dir_name, watcher info }
	std::map<std::string, WatchData> watchers;
	//Would we need to do something special for copying?
	//clone the handles or something?
#ifndef NO_SDL
	//The SDL User event code for file events
	static uint32_t event_code;
#endif

public:
	WatchWin32();
	~WatchWin32();
	/**
	 * Start watching some directory for file changes
	 * Filters is a set of the notify flags or'd
	 * together to watch for
	 */
#ifdef NO_SDL
	void watch(const std::string &dir, uint32_t filters, const Callback &callback);
#else
	void watch(const std::string &dir, uint32_t filters);
#endif
	void remove(const std::string &dir);
	//Update watchers. I'd really like to put this on some background thread though
	void update();
#ifndef NO_SDL
	//Get the SDL user event code for events emitted by the watchers
	static uint32_t event();
#endif

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

