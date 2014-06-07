#ifdef __linux

#ifndef LFWATCH_LINUX
#define LFWATCH_LINUX

#include <string>
#include <map>
#include <sys/inotify.h>
#include "events.h"

namespace lfw {
//Possible events we can notify about
enum Notify {
	FILE_MODIFIED = IN_MODIFY,
	FILE_CREATED = IN_CREATE,
	FILE_REMOVED = IN_DELETE,
	FILE_RENAMED_OLD_NAME = IN_MOVED_FROM,
	FILE_RENAMED_NEW_NAME = IN_MOVED_TO
};

struct WatchData {
	int watch_descriptor;
	std::string dir_name;
	uint32_t filter;
	Callback callback;

	WatchData(int wd, const std::string &dir, uint32_t filter, const Callback &cb);
};

class WatchLinux {
	//Active watches { watch descriptor, dir name }
	//Assuming that people will get more updates than they'll
	//call remove, using this map layout will give fast dir name
	//lookups by id but slow name lookups to remove watches
	std::map<int, WatchData> watchers;
	int notify_fd;

public:
	WatchLinux();
	WatchLinux(const WatchLinux &w) = delete;
	WatchLinux& operator=(const WatchLinux &w) = delete;
	~WatchLinux();
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
	//Run through all inotify events in the buffer and emit them
	void emit_events(const char *buf, int len);
	//Look up a watcher by name and return the iterator to it
	std::map<int, WatchData>::iterator find_dir(const std::string &dir);
};
}

#endif
#endif

