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
enum Notify {
	FILE_MODIFIED = kFSEventStreamEventFlagItemModified,
	FILE_CREATED = kFSEventStreamEventFlagItemCreated,
	FILE_REMOVED = kFSEventStreamEventFlagItemRemoved,
	//OS X uses same event flag for these so pick non-conflicting names
	FILE_RENAMED_OLD_NAME = 0x10000000,
	FILE_RENAMED_NEW_NAME = 0x20000000
};

struct WatchData {
	FSEventStreamRef stream;
	std::string dir_name;
	uint32_t filter;
	Callback callback;

	WatchData(const std::string &dir, uint32_t filter, const Callback &cb);
};

class WatchOSX {
	//Active watches { dir_name, watcher info }
	std::map<std::string, WatchData> watchers;

public:
	WatchOSX();
	WatchOSX(const WatchOSX &w) = delete;
	WatchOSX& operator=(const WatchOSX &w) = delete;
	~WatchOSX();
	/*
	 * Start watching some directory for file changes
	 * Filters is a set of the notify flags or'd
	 * together to watch for
	 */
	void watch(const std::string &dir, uint32_t filters, const Callback &callback);
	void remove(const std::string &dir);
	//Update watchers, call this to get event information updated
	void update();
};
}

#endif
#endif

