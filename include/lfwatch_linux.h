#ifdef __linux

#ifndef LFWATCH_LINUX
#define LFWATCH_LINUX

#include <string>
#include <map>
#include <functional>
#include <sys/inotify.h>
#include "events.h"

namespace lfw {
//Possible events we can notify about
enum Notify {
	//Should we listen for this event type? there's some differences
	//in windows/linux rename event handling need to test
	CHANGE_FILE_NAME = IN_MOVE,
	CHANGE_DIR_NAME = IN_MOVE_SELF,
	CHANGE_ATTRIBUTES = IN_ATTRIB,
	CHANGE_LAST_WRITE = IN_CLOSE_WRITE,
	CHANGE_LAST_ACCESS = IN_ACCESS,
};

struct WatchData {
	int watch_descriptor;
	std::string dir_name;
	uint32_t filter;
#ifdef NO_SDL
	Callback callback;
#endif

#ifdef NO_SDL
	WatchData(int wd, const std::string &dir, uint32_t filter, const Callback &cb);
#else
	WatchData(int wd, const std::string &dir, uint32_t filter);
#endif
};

class WatchLinux {
	//Active watches { watch descriptor, dir name }
	//Assuming that people will get more updates than they'll
	//call remove, using this map layout will give fast dir name
	//lookups by id but slow name lookups to remove watches
	std::map<int, WatchData> watchers;
	int notify_fd;

#ifndef NO_SDL
	//The SDL User event code for file events
	static uint32_t event_code;
#endif

public:
	WatchLinux();
	~WatchLinux();
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
	WatchLinux(const WatchLinux &w){
		//TODO Rule of 3
	}
	WatchLinux& operator=(const WatchLinux &w){
		//TODO Rule of 3
		return *this;
	}
	//Run through all inotify events in the buffer and emit them
	void emit_events(const char *buf, int len);
};
}

#endif
#endif

