#ifdef __APPLE__

#ifndef LFWATCH_OSX
#define LFWATCH_OSX

#include <string>
#include <map>
#include <functional>
#include <CoreServices/CoreServices.h>
#include "events.h"

namespace lfw {
//Possible file events that we can notify about
enum Notify { FILE_CREATED = kFSEventStreamEventFlagItemCreated,
	FILE_REMOVED = kFSEventStreamEventFlagItemRemoved,
	CHANGE_FILE_NAME = kFSEventStreamEventFlagItemCreated,
	//Rename to change_last_modified?
	CHANGE_LAST_WRITE = kFSEventStreamEventFlagItemModified,
	CHANGE_ATTRIBUTES = kFSEventStreamEventFlagItemModified,
	CHANGE_LAST_ACCESS = kFSEventStreamEventFlagItemModified,
	//Not on OS X?
	CHANGE_DIR_NAME = 0
};

struct WatchData {
	FSEventStreamRef stream;
	std::string dir_name;
	uint32_t filter;
#ifdef NO_SDL
	Callback callback;

	WatchData(const std::string &dir, uint32_t filter, const Callback &cb);
#else
	WatchData(const std::string &dir, uint32_t filter);
#endif
	//Destructor to free? Could also do that method on Win32
};

class WatchOSX {
	//Active watches { dir_name, watcher info }
	std::map<std::string, WatchData> watchers;
#ifndef NO_SDL
	//The SDL User event code for file events
	static uint32_t event_code;
#endif

public:
	WatchOSX();
	~WatchOSX();
	/*
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
	//Update watchers, call this to get event information updated
	void update();
#ifndef NO_SDL
	//Get the SDL user event code for events emitted by the watchers
	static uint32_t event();
#endif

private:
	WatchOSX(const WatchOSX &w){
		//TODO: Rule of 3
	}
	WatchOSX& operator=(const WatchOSX &w){
		//TODO: Rule of 3
		return *this;
	}
};
}

#endif
#endif

